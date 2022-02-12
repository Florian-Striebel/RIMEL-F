#include "server.h"
#include "bio.h"
#include "rio.h"
#include "functions.h"
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/param.h>
void freeClientArgv(client *c);
off_t getAppendOnlyFileSize(sds filename);
off_t getBaseAndIncrAppendOnlyFilesSize(aofManifest *am);
int getBaseAndIncrAppendOnlyFilesNum(aofManifest *am);
int aofFileExist(char *filename);
int rewriteAppendOnlyFile(char *filename);
#define BASE_FILE_SUFFIX ".base"
#define INCR_FILE_SUFFIX ".incr"
#define RDB_FORMAT_SUFFIX ".rdb"
#define AOF_FORMAT_SUFFIX ".aof"
#define MANIFEST_NAME_SUFFIX ".manifest"
#define MANIFEST_TEMP_NAME_PREFIX "temp_"
#define AOF_MANIFEST_KEY_FILE_NAME "file"
#define AOF_MANIFEST_KEY_FILE_SEQ "seq"
#define AOF_MANIFEST_KEY_FILE_TYPE "type"
aofInfo *aofInfoCreate(void) {
return zcalloc(sizeof(aofInfo));
}
void aofInfoFree(aofInfo *ai) {
serverAssert(ai != NULL);
if (ai->file_name) sdsfree(ai->file_name);
zfree(ai);
}
aofInfo *aofInfoDup(aofInfo *orig) {
serverAssert(orig != NULL);
aofInfo *ai = aofInfoCreate();
ai->file_name = sdsdup(orig->file_name);
ai->file_seq = orig->file_seq;
ai->file_type = orig->file_type;
return ai;
}
sds aofInfoFormat(sds buf, aofInfo *ai) {
sds filename_repr = NULL;
if (sdsneedsrepr(ai->file_name))
filename_repr = sdscatrepr(sdsempty(), ai->file_name, sdslen(ai->file_name));
sds ret = sdscatprintf(buf, "%s %s %s %lld %s %c\n",
AOF_MANIFEST_KEY_FILE_NAME, filename_repr ? filename_repr : ai->file_name,
AOF_MANIFEST_KEY_FILE_SEQ, ai->file_seq,
AOF_MANIFEST_KEY_FILE_TYPE, ai->file_type);
sdsfree(filename_repr);
return ret;
}
void aofListFree(void *item) {
aofInfo *ai = (aofInfo *)item;
aofInfoFree(ai);
}
void *aofListDup(void *item) {
return aofInfoDup(item);
}
aofManifest *aofManifestCreate(void) {
aofManifest *am = zcalloc(sizeof(aofManifest));
am->incr_aof_list = listCreate();
am->history_aof_list = listCreate();
listSetFreeMethod(am->incr_aof_list, aofListFree);
listSetDupMethod(am->incr_aof_list, aofListDup);
listSetFreeMethod(am->history_aof_list, aofListFree);
listSetDupMethod(am->history_aof_list, aofListDup);
return am;
}
void aofManifestFree(aofManifest *am) {
if (am->base_aof_info) aofInfoFree(am->base_aof_info);
if (am->incr_aof_list) listRelease(am->incr_aof_list);
if (am->history_aof_list) listRelease(am->history_aof_list);
zfree(am);
}
sds getAofManifestFileName() {
return sdscatprintf(sdsempty(), "%s%s", server.aof_filename,
MANIFEST_NAME_SUFFIX);
}
sds getTempAofManifestFileName() {
return sdscatprintf(sdsempty(), "%s%s%s", MANIFEST_TEMP_NAME_PREFIX,
server.aof_filename, MANIFEST_NAME_SUFFIX);
}
sds getAofManifestAsString(aofManifest *am) {
serverAssert(am != NULL);
sds buf = sdsempty();
listNode *ln;
listIter li;
if (am->base_aof_info) {
buf = aofInfoFormat(buf, am->base_aof_info);
}
listRewind(am->history_aof_list, &li);
while ((ln = listNext(&li)) != NULL) {
aofInfo *ai = (aofInfo*)ln->value;
buf = aofInfoFormat(buf, ai);
}
listRewind(am->incr_aof_list, &li);
while ((ln = listNext(&li)) != NULL) {
aofInfo *ai = (aofInfo*)ln->value;
buf = aofInfoFormat(buf, ai);
}
return buf;
}
#define MANIFEST_MAX_LINE 1024
void aofLoadManifestFromDisk(void) {
const char *err = NULL;
long long maxseq = 0;
server.aof_manifest = aofManifestCreate();
if (!dirExists(server.aof_dirname)) {
serverLog(LL_NOTICE, "The AOF directory %s doesn't exist", server.aof_dirname);
return;
}
sds am_name = getAofManifestFileName();
sds am_filepath = makePath(server.aof_dirname, am_name);
if (!fileExist(am_filepath)) {
serverLog(LL_NOTICE, "The AOF manifest file %s doesn't exist", am_name);
sdsfree(am_name);
sdsfree(am_filepath);
return;
}
FILE *fp = fopen(am_filepath, "r");
if (fp == NULL) {
serverLog(LL_WARNING, "Fatal error: can't open the AOF manifest "
"file %s for reading: %s", am_name, strerror(errno));
exit(1);
}
sdsfree(am_name);
sdsfree(am_filepath);
char buf[MANIFEST_MAX_LINE+1];
sds *argv = NULL;
int argc;
aofInfo *ai = NULL;
sds line = NULL;
int linenum = 0;
while (1) {
if (fgets(buf, MANIFEST_MAX_LINE+1, fp) == NULL) {
if (feof(fp)) {
if (linenum == 0) {
err = "Found an empty AOF manifest";
goto loaderr;
} else {
break;
}
} else {
err = "Read AOF manifest failed";
goto loaderr;
}
}
linenum++;
if (buf[0] == '#') continue;
if (strchr(buf, '\n') == NULL) {
err = "The AOF manifest file contains too long line";
goto loaderr;
}
line = sdstrim(sdsnew(buf), " \t\r\n");
if (!sdslen(line)) {
err = "The AOF manifest file is invalid format";
goto loaderr;
}
argv = sdssplitargs(line, &argc);
if (argv == NULL || argc < 6 || (argc % 2)) {
err = "The AOF manifest file is invalid format";
goto loaderr;
}
ai = aofInfoCreate();
for (int i = 0; i < argc; i += 2) {
if (!strcasecmp(argv[i], AOF_MANIFEST_KEY_FILE_NAME)) {
ai->file_name = sdsnew(argv[i+1]);
if (!pathIsBaseName(ai->file_name)) {
err = "File can't be a path, just a filename";
goto loaderr;
}
} else if (!strcasecmp(argv[i], AOF_MANIFEST_KEY_FILE_SEQ)) {
ai->file_seq = atoll(argv[i+1]);
} else if (!strcasecmp(argv[i], AOF_MANIFEST_KEY_FILE_TYPE)) {
ai->file_type = (argv[i+1])[0];
}
}
if (!ai->file_name || !ai->file_seq || !ai->file_type) {
err = "The AOF manifest file is invalid format";
goto loaderr;
}
sdsfreesplitres(argv, argc);
argv = NULL;
if (ai->file_type == AOF_FILE_TYPE_BASE) {
if (server.aof_manifest->base_aof_info) {
err = "Found duplicate base file information";
goto loaderr;
}
server.aof_manifest->base_aof_info = ai;
server.aof_manifest->curr_base_file_seq = ai->file_seq;
} else if (ai->file_type == AOF_FILE_TYPE_HIST) {
listAddNodeTail(server.aof_manifest->history_aof_list, ai);
} else if (ai->file_type == AOF_FILE_TYPE_INCR) {
if (ai->file_seq <= maxseq) {
err = "Found a non-monotonic sequence number";
goto loaderr;
}
listAddNodeTail(server.aof_manifest->incr_aof_list, ai);
server.aof_manifest->curr_incr_file_seq = ai->file_seq;
maxseq = ai->file_seq;
} else {
err = "Unknown AOF file type";
goto loaderr;
}
sdsfree(line);
line = NULL;
ai = NULL;
}
fclose(fp);
return;
loaderr:
if (argv) sdsfreesplitres(argv, argc);
if (ai) aofInfoFree(ai);
serverLog(LL_WARNING, "\n*** FATAL AOF MANIFEST FILE ERROR ***\n");
if (line) {
serverLog(LL_WARNING, "Reading the manifest file, at line %d\n", linenum);
serverLog(LL_WARNING, ">>> '%s'\n", line);
}
serverLog(LL_WARNING, "%s\n", err);
exit(1);
}
aofManifest *aofManifestDup(aofManifest *orig) {
serverAssert(orig != NULL);
aofManifest *am = zcalloc(sizeof(aofManifest));
am->curr_base_file_seq = orig->curr_base_file_seq;
am->curr_incr_file_seq = orig->curr_incr_file_seq;
am->dirty = orig->dirty;
if (orig->base_aof_info) {
am->base_aof_info = aofInfoDup(orig->base_aof_info);
}
am->incr_aof_list = listDup(orig->incr_aof_list);
am->history_aof_list = listDup(orig->history_aof_list);
serverAssert(am->incr_aof_list != NULL);
serverAssert(am->history_aof_list != NULL);
return am;
}
void aofManifestFreeAndUpdate(aofManifest *am) {
serverAssert(am != NULL);
if (server.aof_manifest) aofManifestFree(server.aof_manifest);
server.aof_manifest = am;
}
sds getNewBaseFileNameAndMarkPreAsHistory(aofManifest *am) {
serverAssert(am != NULL);
if (am->base_aof_info) {
serverAssert(am->base_aof_info->file_type == AOF_FILE_TYPE_BASE);
am->base_aof_info->file_type = AOF_FILE_TYPE_HIST;
listAddNodeHead(am->history_aof_list, am->base_aof_info);
}
char *format_suffix = server.aof_use_rdb_preamble ?
RDB_FORMAT_SUFFIX:AOF_FORMAT_SUFFIX;
aofInfo *ai = aofInfoCreate();
ai->file_name = sdscatprintf(sdsempty(), "%s.%lld%s%s", server.aof_filename,
++am->curr_base_file_seq, BASE_FILE_SUFFIX, format_suffix);
ai->file_seq = am->curr_base_file_seq;
ai->file_type = AOF_FILE_TYPE_BASE;
am->base_aof_info = ai;
am->dirty = 1;
return am->base_aof_info->file_name;
}
sds getNewIncrAofName(aofManifest *am) {
aofInfo *ai = aofInfoCreate();
ai->file_type = AOF_FILE_TYPE_INCR;
ai->file_name = sdscatprintf(sdsempty(), "%s.%lld%s%s", server.aof_filename,
++am->curr_incr_file_seq, INCR_FILE_SUFFIX, AOF_FORMAT_SUFFIX);
ai->file_seq = am->curr_incr_file_seq;
listAddNodeTail(am->incr_aof_list, ai);
am->dirty = 1;
return ai->file_name;
}
sds getLastIncrAofName(aofManifest *am) {
serverAssert(am != NULL);
if (!listLength(am->incr_aof_list)) {
return getNewIncrAofName(am);
}
listNode *lastnode = listIndex(am->incr_aof_list, -1);
aofInfo *ai = listNodeValue(lastnode);
return ai->file_name;
}
void markRewrittenIncrAofAsHistory(aofManifest *am) {
serverAssert(am != NULL);
if (!listLength(am->incr_aof_list)) {
return;
}
listNode *ln;
listIter li;
listRewindTail(am->incr_aof_list, &li);
if (server.aof_fd != -1) {
ln = listNext(&li);
serverAssert(ln != NULL);
}
while ((ln = listNext(&li)) != NULL) {
aofInfo *ai = (aofInfo*)ln->value;
serverAssert(ai->file_type == AOF_FILE_TYPE_INCR);
aofInfo *hai = aofInfoDup(ai);
hai->file_type = AOF_FILE_TYPE_HIST;
listAddNodeHead(am->history_aof_list, hai);
listDelNode(am->incr_aof_list, ln);
}
am->dirty = 1;
}
int writeAofManifestFile(sds buf) {
int ret = C_OK;
ssize_t nwritten;
int len;
sds am_name = getAofManifestFileName();
sds am_filepath = makePath(server.aof_dirname, am_name);
sds tmp_am_name = getTempAofManifestFileName();
sds tmp_am_filepath = makePath(server.aof_dirname, tmp_am_name);
int fd = open(tmp_am_filepath, O_WRONLY|O_TRUNC|O_CREAT, 0644);
if (fd == -1) {
serverLog(LL_WARNING, "Can't open the AOF manifest file %s: %s",
tmp_am_name, strerror(errno));
ret = C_ERR;
goto cleanup;
}
len = sdslen(buf);
while(len) {
nwritten = write(fd, buf, len);
if (nwritten < 0) {
if (errno == EINTR) continue;
serverLog(LL_WARNING, "Error trying to write the temporary AOF manifest file %s: %s",
tmp_am_name, strerror(errno));
ret = C_ERR;
goto cleanup;
}
len -= nwritten;
buf += nwritten;
}
if (redis_fsync(fd) == -1) {
serverLog(LL_WARNING, "Fail to fsync the temp AOF file %s: %s.",
tmp_am_name, strerror(errno));
ret = C_ERR;
goto cleanup;
}
if (rename(tmp_am_filepath, am_filepath) != 0) {
serverLog(LL_WARNING,
"Error trying to rename the temporary AOF manifest file %s into %s: %s",
tmp_am_name, am_name, strerror(errno));
ret = C_ERR;
}
cleanup:
if (fd != -1) close(fd);
sdsfree(am_name);
sdsfree(am_filepath);
sdsfree(tmp_am_name);
sdsfree(tmp_am_filepath);
return ret;
}
int persistAofManifest(aofManifest *am) {
if (am->dirty == 0) {
return C_OK;
}
sds amstr = getAofManifestAsString(am);
int ret = writeAofManifestFile(amstr);
sdsfree(amstr);
if (ret == C_OK) am->dirty = 0;
return ret;
}
void aofUpgradePrepare(aofManifest *am) {
serverAssert(!aofFileExist(server.aof_filename));
if (dirCreateIfMissing(server.aof_dirname) == -1) {
serverLog(LL_WARNING, "Can't open or create append-only dir %s: %s",
server.aof_dirname, strerror(errno));
exit(1);
}
if (am->base_aof_info) aofInfoFree(am->base_aof_info);
aofInfo *ai = aofInfoCreate();
ai->file_name = sdsnew(server.aof_filename);
ai->file_seq = 1;
ai->file_type = AOF_FILE_TYPE_BASE;
am->base_aof_info = ai;
am->curr_base_file_seq = 1;
am->dirty = 1;
if (persistAofManifest(am) != C_OK) {
exit(1);
}
sds aof_filepath = makePath(server.aof_dirname, server.aof_filename);
if (rename(server.aof_filename, aof_filepath) == -1) {
serverLog(LL_WARNING,
"Error trying to move the old AOF file %s into dir %s: %s",
server.aof_filename,
server.aof_dirname,
strerror(errno));
sdsfree(aof_filepath);
exit(1);;
}
sdsfree(aof_filepath);
serverLog(LL_NOTICE, "Successfully migrated an old-style AOF file (%s) into the AOF directory (%s).",
server.aof_filename, server.aof_dirname);
}
int aofDelHistoryFiles(void) {
if (server.aof_manifest == NULL ||
server.aof_disable_auto_gc == 1 ||
!listLength(server.aof_manifest->history_aof_list))
{
return C_OK;
}
listNode *ln;
listIter li;
listRewind(server.aof_manifest->history_aof_list, &li);
while ((ln = listNext(&li)) != NULL) {
aofInfo *ai = (aofInfo*)ln->value;
serverAssert(ai->file_type == AOF_FILE_TYPE_HIST);
serverLog(LL_NOTICE, "Removing the history file %s in the background", ai->file_name);
sds aof_filepath = makePath(server.aof_dirname, ai->file_name);
bg_unlink(aof_filepath);
sdsfree(aof_filepath);
listDelNode(server.aof_manifest->history_aof_list, ln);
}
server.aof_manifest->dirty = 1;
return persistAofManifest(server.aof_manifest);
}
void aofOpenIfNeededOnServerStart(void) {
if (server.aof_state != AOF_ON) {
return;
}
serverAssert(server.aof_manifest != NULL);
serverAssert(server.aof_fd == -1);
if (dirCreateIfMissing(server.aof_dirname) == -1) {
serverLog(LL_WARNING, "Can't open or create append-only dir %s: %s",
server.aof_dirname, strerror(errno));
exit(1);
}
if (!server.aof_manifest->base_aof_info &&
!listLength(server.aof_manifest->incr_aof_list))
{
sds base_name = getNewBaseFileNameAndMarkPreAsHistory(server.aof_manifest);
sds base_filepath = makePath(server.aof_dirname, base_name);
if (rewriteAppendOnlyFile(base_filepath) != C_OK) {
exit(1);
}
sdsfree(base_filepath);
}
sds aof_name = getLastIncrAofName(server.aof_manifest);
sds aof_filepath = makePath(server.aof_dirname, aof_name);
server.aof_fd = open(aof_filepath, O_WRONLY|O_APPEND|O_CREAT, 0644);
sdsfree(aof_filepath);
if (server.aof_fd == -1) {
serverLog(LL_WARNING, "Can't open the append-only file %s: %s",
aof_name, strerror(errno));
exit(1);
}
int ret = persistAofManifest(server.aof_manifest);
if (ret != C_OK) {
exit(1);
}
server.aof_last_incr_size = getAppendOnlyFileSize(aof_name);
}
int aofFileExist(char *filename) {
sds file_path = makePath(server.aof_dirname, filename);
int ret = fileExist(file_path);
sdsfree(file_path);
return ret;
}
int openNewIncrAofForAppend(void) {
serverAssert(server.aof_manifest != NULL);
int newfd;
if (server.aof_state == AOF_OFF) return C_OK;
aofManifest *temp_am = aofManifestDup(server.aof_manifest);
sds new_aof_name = getNewIncrAofName(temp_am);
sds new_aof_filepath = makePath(server.aof_dirname, new_aof_name);
newfd = open(new_aof_filepath, O_WRONLY|O_TRUNC|O_CREAT, 0644);
sdsfree(new_aof_filepath);
if (newfd == -1) {
serverLog(LL_WARNING, "Can't open the append-only file %s: %s",
new_aof_name, strerror(errno));
aofManifestFree(temp_am);
return C_ERR;
}
int ret = persistAofManifest(temp_am);
if (ret == C_ERR) {
close(newfd);
aofManifestFree(temp_am);
return C_ERR;
}
if (server.aof_fd != -1) bioCreateCloseJob(server.aof_fd);
server.aof_fd = newfd;
server.aof_last_incr_size = 0;
aofManifestFreeAndUpdate(temp_am);
return C_OK;
}
#define AOF_REWRITE_LIMITE_THRESHOLD 3
#define AOF_REWRITE_LIMITE_NAX_MINUTES 60
int aofRewriteLimited(void) {
int limit = 0;
static int limit_deley_minutes = 0;
static time_t next_rewrite_time = 0;
unsigned long incr_aof_num = listLength(server.aof_manifest->incr_aof_list);
if (incr_aof_num >= AOF_REWRITE_LIMITE_THRESHOLD) {
if (server.unixtime < next_rewrite_time) {
limit = 1;
} else {
if (limit_deley_minutes == 0) {
limit = 1;
limit_deley_minutes = 1;
} else {
limit_deley_minutes *= 2;
}
if (limit_deley_minutes > AOF_REWRITE_LIMITE_NAX_MINUTES) {
limit_deley_minutes = AOF_REWRITE_LIMITE_NAX_MINUTES;
}
next_rewrite_time = server.unixtime + limit_deley_minutes * 60;
serverLog(LL_WARNING,
"Background AOF rewrite has repeatedly failed %ld times and triggered the limit, will retry in %d minutes",
incr_aof_num, limit_deley_minutes);
}
} else {
limit_deley_minutes = 0;
next_rewrite_time = 0;
}
return limit;
}
int aofFsyncInProgress(void) {
return bioPendingJobsOfType(BIO_AOF_FSYNC) != 0;
}
void aof_background_fsync(int fd) {
bioCreateFsyncJob(fd);
}
void killAppendOnlyChild(void) {
int statloc;
if (server.child_type != CHILD_TYPE_AOF) return;
serverLog(LL_NOTICE,"Killing running AOF rewrite child: %ld",
(long) server.child_pid);
if (kill(server.child_pid,SIGUSR1) != -1) {
while(waitpid(-1, &statloc, 0) != server.child_pid);
}
aofRemoveTempFile(server.child_pid);
resetChildState();
server.aof_rewrite_time_start = -1;
}
void stopAppendOnly(void) {
serverAssert(server.aof_state != AOF_OFF);
flushAppendOnlyFile(1);
if (redis_fsync(server.aof_fd) == -1) {
serverLog(LL_WARNING,"Fail to fsync the AOF file: %s",strerror(errno));
} else {
server.aof_fsync_offset = server.aof_current_size;
server.aof_last_fsync = server.unixtime;
}
close(server.aof_fd);
server.aof_fd = -1;
server.aof_selected_db = -1;
server.aof_state = AOF_OFF;
server.aof_rewrite_scheduled = 0;
server.aof_last_incr_size = 0;
killAppendOnlyChild();
sdsfree(server.aof_buf);
server.aof_buf = sdsempty();
}
int startAppendOnly(void) {
serverAssert(server.aof_state == AOF_OFF);
server.aof_state = AOF_WAIT_REWRITE;
if (hasActiveChildProcess() && server.child_type != CHILD_TYPE_AOF) {
server.aof_rewrite_scheduled = 1;
serverLog(LL_WARNING,"AOF was enabled but there is already another background operation. An AOF background was scheduled to start when possible.");
} else if (server.in_exec){
server.aof_rewrite_scheduled = 1;
serverLog(LL_WARNING,"AOF was enabled during a transaction. An AOF background was scheduled to start when possible.");
} else {
if (server.child_type == CHILD_TYPE_AOF) {
serverLog(LL_WARNING,"AOF was enabled but there is already an AOF rewriting in background. Stopping background AOF and starting a rewrite now.");
killAppendOnlyChild();
}
if (rewriteAppendOnlyFileBackground() == C_ERR) {
server.aof_state = AOF_OFF;
serverLog(LL_WARNING,"Redis needs to enable the AOF but can't trigger a background AOF rewrite operation. Check the above logs for more info about the error.");
return C_ERR;
}
}
server.aof_last_fsync = server.unixtime;
int aof_bio_fsync_status;
atomicGet(server.aof_bio_fsync_status, aof_bio_fsync_status);
if (aof_bio_fsync_status == C_ERR) {
serverLog(LL_WARNING,
"AOF reopen, just ignore the AOF fsync error in bio job");
atomicSet(server.aof_bio_fsync_status,C_OK);
}
if (server.aof_last_write_status == C_ERR) {
serverLog(LL_WARNING,"AOF reopen, just ignore the last error.");
server.aof_last_write_status = C_OK;
}
return C_OK;
}
ssize_t aofWrite(int fd, const char *buf, size_t len) {
ssize_t nwritten = 0, totwritten = 0;
while(len) {
nwritten = write(fd, buf, len);
if (nwritten < 0) {
if (errno == EINTR) continue;
return totwritten ? totwritten : -1;
}
len -= nwritten;
buf += nwritten;
totwritten += nwritten;
}
return totwritten;
}
#define AOF_WRITE_LOG_ERROR_RATE 30
void flushAppendOnlyFile(int force) {
ssize_t nwritten;
int sync_in_progress = 0;
mstime_t latency;
if (sdslen(server.aof_buf) == 0) {
if (server.aof_fsync == AOF_FSYNC_EVERYSEC &&
server.aof_fsync_offset != server.aof_current_size &&
server.unixtime > server.aof_last_fsync &&
!(sync_in_progress = aofFsyncInProgress())) {
goto try_fsync;
} else {
return;
}
}
if (server.aof_fsync == AOF_FSYNC_EVERYSEC)
sync_in_progress = aofFsyncInProgress();
if (server.aof_fsync == AOF_FSYNC_EVERYSEC && !force) {
if (sync_in_progress) {
if (server.aof_flush_postponed_start == 0) {
server.aof_flush_postponed_start = server.unixtime;
return;
} else if (server.unixtime - server.aof_flush_postponed_start < 2) {
return;
}
server.aof_delayed_fsync++;
serverLog(LL_NOTICE,"Asynchronous AOF fsync is taking too long (disk is busy?). Writing the AOF buffer without waiting for fsync to complete, this may slow down Redis.");
}
}
if (server.aof_flush_sleep && sdslen(server.aof_buf)) {
usleep(server.aof_flush_sleep);
}
latencyStartMonitor(latency);
nwritten = aofWrite(server.aof_fd,server.aof_buf,sdslen(server.aof_buf));
latencyEndMonitor(latency);
if (sync_in_progress) {
latencyAddSampleIfNeeded("aof-write-pending-fsync",latency);
} else if (hasActiveChildProcess()) {
latencyAddSampleIfNeeded("aof-write-active-child",latency);
} else {
latencyAddSampleIfNeeded("aof-write-alone",latency);
}
latencyAddSampleIfNeeded("aof-write",latency);
server.aof_flush_postponed_start = 0;
if (nwritten != (ssize_t)sdslen(server.aof_buf)) {
static time_t last_write_error_log = 0;
int can_log = 0;
if ((server.unixtime - last_write_error_log) > AOF_WRITE_LOG_ERROR_RATE) {
can_log = 1;
last_write_error_log = server.unixtime;
}
if (nwritten == -1) {
if (can_log) {
serverLog(LL_WARNING,"Error writing to the AOF file: %s",
strerror(errno));
server.aof_last_write_errno = errno;
}
} else {
if (can_log) {
serverLog(LL_WARNING,"Short write while writing to "
"the AOF file: (nwritten=%lld, "
"expected=%lld)",
(long long)nwritten,
(long long)sdslen(server.aof_buf));
}
if (ftruncate(server.aof_fd, server.aof_last_incr_size) == -1) {
if (can_log) {
serverLog(LL_WARNING, "Could not remove short write "
"from the append-only file. Redis may refuse "
"to load the AOF the next time it starts. "
"ftruncate: %s", strerror(errno));
}
} else {
nwritten = -1;
}
server.aof_last_write_errno = ENOSPC;
}
if (server.aof_fsync == AOF_FSYNC_ALWAYS) {
serverLog(LL_WARNING,"Can't recover from AOF write error when the AOF fsync policy is 'always'. Exiting...");
exit(1);
} else {
server.aof_last_write_status = C_ERR;
if (nwritten > 0) {
server.aof_current_size += nwritten;
server.aof_last_incr_size += nwritten;
sdsrange(server.aof_buf,nwritten,-1);
}
return;
}
} else {
if (server.aof_last_write_status == C_ERR) {
serverLog(LL_WARNING,
"AOF write error looks solved, Redis can write again.");
server.aof_last_write_status = C_OK;
}
}
server.aof_current_size += nwritten;
server.aof_last_incr_size += nwritten;
if ((sdslen(server.aof_buf)+sdsavail(server.aof_buf)) < 4000) {
sdsclear(server.aof_buf);
} else {
sdsfree(server.aof_buf);
server.aof_buf = sdsempty();
}
try_fsync:
if (server.aof_no_fsync_on_rewrite && hasActiveChildProcess())
return;
if (server.aof_fsync == AOF_FSYNC_ALWAYS) {
latencyStartMonitor(latency);
if (redis_fsync(server.aof_fd) == -1) {
serverLog(LL_WARNING,"Can't persist AOF for fsync error when the "
"AOF fsync policy is 'always': %s. Exiting...", strerror(errno));
exit(1);
}
latencyEndMonitor(latency);
latencyAddSampleIfNeeded("aof-fsync-always",latency);
server.aof_fsync_offset = server.aof_current_size;
server.aof_last_fsync = server.unixtime;
} else if ((server.aof_fsync == AOF_FSYNC_EVERYSEC &&
server.unixtime > server.aof_last_fsync)) {
if (!sync_in_progress) {
aof_background_fsync(server.aof_fd);
server.aof_fsync_offset = server.aof_current_size;
}
server.aof_last_fsync = server.unixtime;
}
}
sds catAppendOnlyGenericCommand(sds dst, int argc, robj **argv) {
char buf[32];
int len, j;
robj *o;
buf[0] = '*';
len = 1+ll2string(buf+1,sizeof(buf)-1,argc);
buf[len++] = '\r';
buf[len++] = '\n';
dst = sdscatlen(dst,buf,len);
for (j = 0; j < argc; j++) {
o = getDecodedObject(argv[j]);
buf[0] = '$';
len = 1+ll2string(buf+1,sizeof(buf)-1,sdslen(o->ptr));
buf[len++] = '\r';
buf[len++] = '\n';
dst = sdscatlen(dst,buf,len);
dst = sdscatlen(dst,o->ptr,sdslen(o->ptr));
dst = sdscatlen(dst,"\r\n",2);
decrRefCount(o);
}
return dst;
}
sds genAofTimestampAnnotationIfNeeded(int force) {
sds ts = NULL;
if (force || server.aof_cur_timestamp < server.unixtime) {
server.aof_cur_timestamp = force ? time(NULL) : server.unixtime;
ts = sdscatfmt(sdsempty(), "#TS:%I\r\n", server.aof_cur_timestamp);
serverAssert(sdslen(ts) <= AOF_ANNOTATION_LINE_MAX_LEN);
}
return ts;
}
void feedAppendOnlyFile(int dictid, robj **argv, int argc) {
sds buf = sdsempty();
serverAssert(dictid >= 0 && dictid < server.dbnum);
if (server.aof_timestamp_enabled) {
sds ts = genAofTimestampAnnotationIfNeeded(0);
if (ts != NULL) {
buf = sdscatsds(buf, ts);
sdsfree(ts);
}
}
if (dictid != server.aof_selected_db) {
char seldb[64];
snprintf(seldb,sizeof(seldb),"%d",dictid);
buf = sdscatprintf(buf,"*2\r\n$6\r\nSELECT\r\n$%lu\r\n%s\r\n",
(unsigned long)strlen(seldb),seldb);
server.aof_selected_db = dictid;
}
buf = catAppendOnlyGenericCommand(buf,argc,argv);
if (server.aof_state == AOF_ON ||
server.child_type == CHILD_TYPE_AOF)
{
server.aof_buf = sdscatlen(server.aof_buf, buf, sdslen(buf));
}
sdsfree(buf);
}
struct client *createAOFClient(void) {
struct client *c = createClient(NULL);
c->id = CLIENT_ID_AOF;
c->flags = CLIENT_DENY_BLOCKING;
c->replstate = SLAVE_STATE_WAIT_BGSAVE_START;
return c;
}
int loadSingleAppendOnlyFile(char *filename) {
struct client *fakeClient;
struct redis_stat sb;
int old_aof_state = server.aof_state;
long loops = 0;
off_t valid_up_to = 0;
off_t valid_before_multi = 0;
off_t last_progress_report_size = 0;
int ret = C_OK;
sds aof_filepath = makePath(server.aof_dirname, filename);
FILE *fp = fopen(aof_filepath, "r");
if (fp == NULL) {
int en = errno;
if (redis_stat(aof_filepath, &sb) == 0 || errno != ENOENT) {
serverLog(LL_WARNING,"Fatal error: can't open the append log file %s for reading: %s", filename, strerror(en));
sdsfree(aof_filepath);
return AOF_OPEN_ERR;
} else {
serverLog(LL_WARNING,"The append log file %s doesn't exist: %s", filename, strerror(errno));
sdsfree(aof_filepath);
return AOF_NOT_EXIST;
}
}
if (fp && redis_fstat(fileno(fp),&sb) != -1 && sb.st_size == 0) {
fclose(fp);
sdsfree(aof_filepath);
return AOF_EMPTY;
}
server.aof_state = AOF_OFF;
client *old_client = server.current_client;
fakeClient = server.current_client = createAOFClient();
char sig[5];
if (fread(sig,1,5,fp) != 5 || memcmp(sig,"REDIS",5) != 0) {
if (fseek(fp,0,SEEK_SET) == -1) goto readerr;
} else {
rio rdb;
serverLog(LL_NOTICE,"Reading RDB preamble from AOF file...");
if (fseek(fp,0,SEEK_SET) == -1) goto readerr;
rioInitWithFile(&rdb,fp);
if (rdbLoadRio(&rdb,RDBFLAGS_AOF_PREAMBLE,NULL) != C_OK) {
serverLog(LL_WARNING,"Error reading the RDB preamble of the AOF file %s, AOF loading aborted", filename);
goto readerr;
} else {
loadingAbsProgress(ftello(fp));
last_progress_report_size = ftello(fp);
serverLog(LL_NOTICE,"Reading the remaining AOF tail...");
}
}
while(1) {
int argc, j;
unsigned long len;
robj **argv;
char buf[AOF_ANNOTATION_LINE_MAX_LEN];
sds argsds;
struct redisCommand *cmd;
if (!(loops++ % 1024)) {
off_t progress_delta = ftello(fp) - last_progress_report_size;
loadingIncrProgress(progress_delta);
last_progress_report_size += progress_delta;
processEventsWhileBlocked();
processModuleLoadingProgressEvent(1);
}
if (fgets(buf,sizeof(buf),fp) == NULL) {
if (feof(fp)) {
break;
} else {
goto readerr;
}
}
if (buf[0] == '#') continue;
if (buf[0] != '*') goto fmterr;
if (buf[1] == '\0') goto readerr;
argc = atoi(buf+1);
if (argc < 1) goto fmterr;
if ((size_t)argc > SIZE_MAX / sizeof(robj*)) goto fmterr;
argv = zmalloc(sizeof(robj*)*argc);
fakeClient->argc = argc;
fakeClient->argv = argv;
fakeClient->argv_len = argc;
for (j = 0; j < argc; j++) {
char *readres = fgets(buf,sizeof(buf),fp);
if (readres == NULL || buf[0] != '$') {
fakeClient->argc = j;
freeClientArgv(fakeClient);
if (readres == NULL)
goto readerr;
else
goto fmterr;
}
len = strtol(buf+1,NULL,10);
argsds = sdsnewlen(SDS_NOINIT,len);
if (len && fread(argsds,len,1,fp) == 0) {
sdsfree(argsds);
fakeClient->argc = j;
freeClientArgv(fakeClient);
goto readerr;
}
argv[j] = createObject(OBJ_STRING,argsds);
if (fread(buf,2,1,fp) == 0) {
fakeClient->argc = j+1;
freeClientArgv(fakeClient);
goto readerr;
}
}
cmd = lookupCommand(argv,argc);
if (!cmd) {
serverLog(LL_WARNING,
"Unknown command '%s' reading the append only file %s",
(char*)argv[0]->ptr, filename);
freeClientArgv(fakeClient);
ret = AOF_FAILED;
goto cleanup;
}
if (cmd->proc == multiCommand) valid_before_multi = valid_up_to;
fakeClient->cmd = fakeClient->lastcmd = cmd;
if (fakeClient->flags & CLIENT_MULTI &&
fakeClient->cmd->proc != execCommand)
{
queueMultiCommand(fakeClient);
} else {
cmd->proc(fakeClient);
}
serverAssert(fakeClient->bufpos == 0 &&
listLength(fakeClient->reply) == 0);
serverAssert((fakeClient->flags & CLIENT_BLOCKED) == 0);
freeClientArgv(fakeClient);
if (server.aof_load_truncated) valid_up_to = ftello(fp);
if (server.key_load_delay)
debugDelay(server.key_load_delay);
}
if (fakeClient->flags & CLIENT_MULTI) {
serverLog(LL_WARNING,
"Revert incomplete MULTI/EXEC transaction in AOF file %s", filename);
valid_up_to = valid_before_multi;
goto uxeof;
}
loaded_ok:
loadingIncrProgress(ftello(fp) - last_progress_report_size);
server.aof_state = old_aof_state;
goto cleanup;
readerr:
if (!feof(fp)) {
serverLog(LL_WARNING,"Unrecoverable error reading the append only file %s: %s", filename, strerror(errno));
ret = AOF_FAILED;
goto cleanup;
}
uxeof:
if (server.aof_load_truncated) {
serverLog(LL_WARNING,"!!! Warning: short read while loading the AOF file %s!!!", filename);
serverLog(LL_WARNING,"!!! Truncating the AOF %s at offset %llu !!!",
filename, (unsigned long long) valid_up_to);
if (valid_up_to == -1 || truncate(aof_filepath,valid_up_to) == -1) {
if (valid_up_to == -1) {
serverLog(LL_WARNING,"Last valid command offset is invalid");
} else {
serverLog(LL_WARNING,"Error truncating the AOF file %s: %s",
filename, strerror(errno));
}
} else {
if (server.aof_fd != -1 && lseek(server.aof_fd,0,SEEK_END) == -1) {
serverLog(LL_WARNING,"Can't seek the end of the AOF file %s: %s",
filename, strerror(errno));
} else {
serverLog(LL_WARNING,
"AOF %s loaded anyway because aof-load-truncated is enabled", filename);
ret = AOF_TRUNCATED;
goto loaded_ok;
}
}
}
serverLog(LL_WARNING,"Unexpected end of file reading the append only file %s. You can: \
1) Make a backup of your AOF file, then use ./redis-check-aof --fix <filename>. \
2) Alternatively you can set the 'aof-load-truncated' configuration option to yes and restart the server.", filename);
ret = AOF_FAILED;
goto cleanup;
fmterr:
serverLog(LL_WARNING,"Bad file format reading the append only file %s: \
make a backup of your AOF file, then use ./redis-check-aof --fix <filename>", filename);
ret = AOF_FAILED;
cleanup:
if (fakeClient) freeClient(fakeClient);
server.current_client = old_client;
fclose(fp);
sdsfree(aof_filepath);
return ret;
}
int loadAppendOnlyFiles(aofManifest *am) {
serverAssert(am != NULL);
int ret = C_OK;
long long start;
off_t total_size = 0;
sds aof_name;
int total_num, aof_num = 0, last_file;
if (fileExist(server.aof_filename)) {
if (!dirExists(server.aof_dirname) ||
(am->base_aof_info == NULL && listLength(am->incr_aof_list) == 0) ||
(am->base_aof_info != NULL && listLength(am->incr_aof_list) == 0 &&
!strcmp(am->base_aof_info->file_name, server.aof_filename) && !aofFileExist(server.aof_filename)))
{
aofUpgradePrepare(am);
}
}
if (am->base_aof_info == NULL && listLength(am->incr_aof_list) == 0) {
return AOF_NOT_EXIST;
}
total_num = getBaseAndIncrAppendOnlyFilesNum(am);
serverAssert(total_num > 0);
total_size = getBaseAndIncrAppendOnlyFilesSize(am);
startLoading(total_size, RDBFLAGS_AOF_PREAMBLE, 0);
if (am->base_aof_info) {
serverAssert(am->base_aof_info->file_type == AOF_FILE_TYPE_BASE);
aof_name = (char*)am->base_aof_info->file_name;
updateLoadingFileName(aof_name);
last_file = ++aof_num == total_num;
start = ustime();
ret = loadSingleAppendOnlyFile(aof_name);
if (ret == AOF_OK || (ret == AOF_TRUNCATED && last_file)) {
serverLog(LL_NOTICE, "DB loaded from base file %s: %.3f seconds",
aof_name, (float)(ustime()-start)/1000000);
}
if (ret == AOF_NOT_EXIST || (ret == AOF_TRUNCATED && !last_file)) {
ret = AOF_FAILED;
}
if (ret == AOF_OPEN_ERR || ret == AOF_FAILED) {
goto cleanup;
}
}
if (listLength(am->incr_aof_list)) {
listNode *ln;
listIter li;
listRewind(am->incr_aof_list, &li);
while ((ln = listNext(&li)) != NULL) {
aofInfo *ai = (aofInfo*)ln->value;
serverAssert(ai->file_type == AOF_FILE_TYPE_INCR);
aof_name = (char*)ai->file_name;
updateLoadingFileName(aof_name);
last_file = ++aof_num == total_num;
start = ustime();
ret = loadSingleAppendOnlyFile(aof_name);
if (ret == AOF_OK || (ret == AOF_TRUNCATED && last_file)) {
serverLog(LL_NOTICE, "DB loaded from incr file %s: %.3f seconds",
aof_name, (float)(ustime()-start)/1000000);
}
if (ret == AOF_NOT_EXIST || (ret == AOF_TRUNCATED && !last_file)) {
ret = AOF_FAILED;
}
if (ret == AOF_OPEN_ERR || ret == AOF_FAILED) {
goto cleanup;
}
}
}
server.aof_current_size = total_size;
server.aof_rewrite_base_size = server.aof_current_size;
server.aof_fsync_offset = server.aof_current_size;
cleanup:
stopLoading(ret == AOF_OK);
return ret;
}
int rioWriteBulkObject(rio *r, robj *obj) {
if (obj->encoding == OBJ_ENCODING_INT) {
return rioWriteBulkLongLong(r,(long)obj->ptr);
} else if (sdsEncodedObject(obj)) {
return rioWriteBulkString(r,obj->ptr,sdslen(obj->ptr));
} else {
serverPanic("Unknown string encoding");
}
}
int rewriteListObject(rio *r, robj *key, robj *o) {
long long count = 0, items = listTypeLength(o);
if (o->encoding == OBJ_ENCODING_QUICKLIST) {
quicklist *list = o->ptr;
quicklistIter *li = quicklistGetIterator(list, AL_START_HEAD);
quicklistEntry entry;
while (quicklistNext(li,&entry)) {
if (count == 0) {
int cmd_items = (items > AOF_REWRITE_ITEMS_PER_CMD) ?
AOF_REWRITE_ITEMS_PER_CMD : items;
if (!rioWriteBulkCount(r,'*',2+cmd_items) ||
!rioWriteBulkString(r,"RPUSH",5) ||
!rioWriteBulkObject(r,key))
{
quicklistReleaseIterator(li);
return 0;
}
}
if (entry.value) {
if (!rioWriteBulkString(r,(char*)entry.value,entry.sz)) {
quicklistReleaseIterator(li);
return 0;
}
} else {
if (!rioWriteBulkLongLong(r,entry.longval)) {
quicklistReleaseIterator(li);
return 0;
}
}
if (++count == AOF_REWRITE_ITEMS_PER_CMD) count = 0;
items--;
}
quicklistReleaseIterator(li);
} else {
serverPanic("Unknown list encoding");
}
return 1;
}
int rewriteSetObject(rio *r, robj *key, robj *o) {
long long count = 0, items = setTypeSize(o);
if (o->encoding == OBJ_ENCODING_INTSET) {
int ii = 0;
int64_t llval;
while(intsetGet(o->ptr,ii++,&llval)) {
if (count == 0) {
int cmd_items = (items > AOF_REWRITE_ITEMS_PER_CMD) ?
AOF_REWRITE_ITEMS_PER_CMD : items;
if (!rioWriteBulkCount(r,'*',2+cmd_items) ||
!rioWriteBulkString(r,"SADD",4) ||
!rioWriteBulkObject(r,key))
{
return 0;
}
}
if (!rioWriteBulkLongLong(r,llval)) return 0;
if (++count == AOF_REWRITE_ITEMS_PER_CMD) count = 0;
items--;
}
} else if (o->encoding == OBJ_ENCODING_HT) {
dictIterator *di = dictGetIterator(o->ptr);
dictEntry *de;
while((de = dictNext(di)) != NULL) {
sds ele = dictGetKey(de);
if (count == 0) {
int cmd_items = (items > AOF_REWRITE_ITEMS_PER_CMD) ?
AOF_REWRITE_ITEMS_PER_CMD : items;
if (!rioWriteBulkCount(r,'*',2+cmd_items) ||
!rioWriteBulkString(r,"SADD",4) ||
!rioWriteBulkObject(r,key))
{
dictReleaseIterator(di);
return 0;
}
}
if (!rioWriteBulkString(r,ele,sdslen(ele))) {
dictReleaseIterator(di);
return 0;
}
if (++count == AOF_REWRITE_ITEMS_PER_CMD) count = 0;
items--;
}
dictReleaseIterator(di);
} else {
serverPanic("Unknown set encoding");
}
return 1;
}
int rewriteSortedSetObject(rio *r, robj *key, robj *o) {
long long count = 0, items = zsetLength(o);
if (o->encoding == OBJ_ENCODING_LISTPACK) {
unsigned char *zl = o->ptr;
unsigned char *eptr, *sptr;
unsigned char *vstr;
unsigned int vlen;
long long vll;
double score;
eptr = lpSeek(zl,0);
serverAssert(eptr != NULL);
sptr = lpNext(zl,eptr);
serverAssert(sptr != NULL);
while (eptr != NULL) {
vstr = lpGetValue(eptr,&vlen,&vll);
score = zzlGetScore(sptr);
if (count == 0) {
int cmd_items = (items > AOF_REWRITE_ITEMS_PER_CMD) ?
AOF_REWRITE_ITEMS_PER_CMD : items;
if (!rioWriteBulkCount(r,'*',2+cmd_items*2) ||
!rioWriteBulkString(r,"ZADD",4) ||
!rioWriteBulkObject(r,key))
{
return 0;
}
}
if (!rioWriteBulkDouble(r,score)) return 0;
if (vstr != NULL) {
if (!rioWriteBulkString(r,(char*)vstr,vlen)) return 0;
} else {
if (!rioWriteBulkLongLong(r,vll)) return 0;
}
zzlNext(zl,&eptr,&sptr);
if (++count == AOF_REWRITE_ITEMS_PER_CMD) count = 0;
items--;
}
} else if (o->encoding == OBJ_ENCODING_SKIPLIST) {
zset *zs = o->ptr;
dictIterator *di = dictGetIterator(zs->dict);
dictEntry *de;
while((de = dictNext(di)) != NULL) {
sds ele = dictGetKey(de);
double *score = dictGetVal(de);
if (count == 0) {
int cmd_items = (items > AOF_REWRITE_ITEMS_PER_CMD) ?
AOF_REWRITE_ITEMS_PER_CMD : items;
if (!rioWriteBulkCount(r,'*',2+cmd_items*2) ||
!rioWriteBulkString(r,"ZADD",4) ||
!rioWriteBulkObject(r,key))
{
dictReleaseIterator(di);
return 0;
}
}
if (!rioWriteBulkDouble(r,*score) ||
!rioWriteBulkString(r,ele,sdslen(ele)))
{
dictReleaseIterator(di);
return 0;
}
if (++count == AOF_REWRITE_ITEMS_PER_CMD) count = 0;
items--;
}
dictReleaseIterator(di);
} else {
serverPanic("Unknown sorted zset encoding");
}
return 1;
}
static int rioWriteHashIteratorCursor(rio *r, hashTypeIterator *hi, int what) {
if (hi->encoding == OBJ_ENCODING_LISTPACK) {
unsigned char *vstr = NULL;
unsigned int vlen = UINT_MAX;
long long vll = LLONG_MAX;
hashTypeCurrentFromListpack(hi, what, &vstr, &vlen, &vll);
if (vstr)
return rioWriteBulkString(r, (char*)vstr, vlen);
else
return rioWriteBulkLongLong(r, vll);
} else if (hi->encoding == OBJ_ENCODING_HT) {
sds value = hashTypeCurrentFromHashTable(hi, what);
return rioWriteBulkString(r, value, sdslen(value));
}
serverPanic("Unknown hash encoding");
return 0;
}
int rewriteHashObject(rio *r, robj *key, robj *o) {
hashTypeIterator *hi;
long long count = 0, items = hashTypeLength(o);
hi = hashTypeInitIterator(o);
while (hashTypeNext(hi) != C_ERR) {
if (count == 0) {
int cmd_items = (items > AOF_REWRITE_ITEMS_PER_CMD) ?
AOF_REWRITE_ITEMS_PER_CMD : items;
if (!rioWriteBulkCount(r,'*',2+cmd_items*2) ||
!rioWriteBulkString(r,"HMSET",5) ||
!rioWriteBulkObject(r,key))
{
hashTypeReleaseIterator(hi);
return 0;
}
}
if (!rioWriteHashIteratorCursor(r, hi, OBJ_HASH_KEY) ||
!rioWriteHashIteratorCursor(r, hi, OBJ_HASH_VALUE))
{
hashTypeReleaseIterator(hi);
return 0;
}
if (++count == AOF_REWRITE_ITEMS_PER_CMD) count = 0;
items--;
}
hashTypeReleaseIterator(hi);
return 1;
}
int rioWriteBulkStreamID(rio *r,streamID *id) {
int retval;
sds replyid = sdscatfmt(sdsempty(),"%U-%U",id->ms,id->seq);
retval = rioWriteBulkString(r,replyid,sdslen(replyid));
sdsfree(replyid);
return retval;
}
int rioWriteStreamPendingEntry(rio *r, robj *key, const char *groupname, size_t groupname_len, streamConsumer *consumer, unsigned char *rawid, streamNACK *nack) {
streamID id;
streamDecodeID(rawid,&id);
if (rioWriteBulkCount(r,'*',12) == 0) return 0;
if (rioWriteBulkString(r,"XCLAIM",6) == 0) return 0;
if (rioWriteBulkObject(r,key) == 0) return 0;
if (rioWriteBulkString(r,groupname,groupname_len) == 0) return 0;
if (rioWriteBulkString(r,consumer->name,sdslen(consumer->name)) == 0) return 0;
if (rioWriteBulkString(r,"0",1) == 0) return 0;
if (rioWriteBulkStreamID(r,&id) == 0) return 0;
if (rioWriteBulkString(r,"TIME",4) == 0) return 0;
if (rioWriteBulkLongLong(r,nack->delivery_time) == 0) return 0;
if (rioWriteBulkString(r,"RETRYCOUNT",10) == 0) return 0;
if (rioWriteBulkLongLong(r,nack->delivery_count) == 0) return 0;
if (rioWriteBulkString(r,"JUSTID",6) == 0) return 0;
if (rioWriteBulkString(r,"FORCE",5) == 0) return 0;
return 1;
}
int rioWriteStreamEmptyConsumer(rio *r, robj *key, const char *groupname, size_t groupname_len, streamConsumer *consumer) {
if (rioWriteBulkCount(r,'*',5) == 0) return 0;
if (rioWriteBulkString(r,"XGROUP",6) == 0) return 0;
if (rioWriteBulkString(r,"CREATECONSUMER",14) == 0) return 0;
if (rioWriteBulkObject(r,key) == 0) return 0;
if (rioWriteBulkString(r,groupname,groupname_len) == 0) return 0;
if (rioWriteBulkString(r,consumer->name,sdslen(consumer->name)) == 0) return 0;
return 1;
}
int rewriteStreamObject(rio *r, robj *key, robj *o) {
stream *s = o->ptr;
streamIterator si;
streamIteratorStart(&si,s,NULL,NULL,0);
streamID id;
int64_t numfields;
if (s->length) {
while(streamIteratorGetID(&si,&id,&numfields)) {
if (!rioWriteBulkCount(r,'*',3+numfields*2) ||
!rioWriteBulkString(r,"XADD",4) ||
!rioWriteBulkObject(r,key) ||
!rioWriteBulkStreamID(r,&id))
{
streamIteratorStop(&si);
return 0;
}
while(numfields--) {
unsigned char *field, *value;
int64_t field_len, value_len;
streamIteratorGetField(&si,&field,&value,&field_len,&value_len);
if (!rioWriteBulkString(r,(char*)field,field_len) ||
!rioWriteBulkString(r,(char*)value,value_len))
{
streamIteratorStop(&si);
return 0;
}
}
}
} else {
id.ms = 0; id.seq = 1;
if (!rioWriteBulkCount(r,'*',7) ||
!rioWriteBulkString(r,"XADD",4) ||
!rioWriteBulkObject(r,key) ||
!rioWriteBulkString(r,"MAXLEN",6) ||
!rioWriteBulkString(r,"0",1) ||
!rioWriteBulkStreamID(r,&id) ||
!rioWriteBulkString(r,"x",1) ||
!rioWriteBulkString(r,"y",1))
{
streamIteratorStop(&si);
return 0;
}
}
if (!rioWriteBulkCount(r,'*',3) ||
!rioWriteBulkString(r,"XSETID",6) ||
!rioWriteBulkObject(r,key) ||
!rioWriteBulkStreamID(r,&s->last_id))
{
streamIteratorStop(&si);
return 0;
}
if (s->cgroups) {
raxIterator ri;
raxStart(&ri,s->cgroups);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
streamCG *group = ri.data;
if (!rioWriteBulkCount(r,'*',5) ||
!rioWriteBulkString(r,"XGROUP",6) ||
!rioWriteBulkString(r,"CREATE",6) ||
!rioWriteBulkObject(r,key) ||
!rioWriteBulkString(r,(char*)ri.key,ri.key_len) ||
!rioWriteBulkStreamID(r,&group->last_id))
{
raxStop(&ri);
streamIteratorStop(&si);
return 0;
}
raxIterator ri_cons;
raxStart(&ri_cons,group->consumers);
raxSeek(&ri_cons,"^",NULL,0);
while(raxNext(&ri_cons)) {
streamConsumer *consumer = ri_cons.data;
if (raxSize(consumer->pel) == 0) {
if (rioWriteStreamEmptyConsumer(r,key,(char*)ri.key,
ri.key_len,consumer) == 0)
{
raxStop(&ri_cons);
raxStop(&ri);
streamIteratorStop(&si);
return 0;
}
continue;
}
raxIterator ri_pel;
raxStart(&ri_pel,consumer->pel);
raxSeek(&ri_pel,"^",NULL,0);
while(raxNext(&ri_pel)) {
streamNACK *nack = ri_pel.data;
if (rioWriteStreamPendingEntry(r,key,(char*)ri.key,
ri.key_len,consumer,
ri_pel.key,nack) == 0)
{
raxStop(&ri_pel);
raxStop(&ri_cons);
raxStop(&ri);
streamIteratorStop(&si);
return 0;
}
}
raxStop(&ri_pel);
}
raxStop(&ri_cons);
}
raxStop(&ri);
}
streamIteratorStop(&si);
return 1;
}
int rewriteModuleObject(rio *r, robj *key, robj *o, int dbid) {
RedisModuleIO io;
moduleValue *mv = o->ptr;
moduleType *mt = mv->type;
moduleInitIOContext(io,mt,r,key,dbid);
mt->aof_rewrite(&io,key,mv->value);
if (io.ctx) {
moduleFreeContext(io.ctx);
zfree(io.ctx);
}
return io.error ? 0 : 1;
}
static int rewriteFunctions(rio *aof) {
dict *functions = functionsLibGet();
dictIterator *iter = dictGetIterator(functions);
dictEntry *entry = NULL;
while ((entry = dictNext(iter))) {
functionLibInfo *li = dictGetVal(entry);
if (li->desc) {
if (rioWrite(aof, "*7\r\n", 4) == 0) goto werr;
} else {
if (rioWrite(aof, "*5\r\n", 4) == 0) goto werr;
}
char fucntion_load[] = "$8\r\nFUNCTION\r\n$4\r\nLOAD\r\n";
if (rioWrite(aof, fucntion_load, sizeof(fucntion_load) - 1) == 0) goto werr;
if (rioWriteBulkString(aof, li->ei->name, sdslen(li->ei->name)) == 0) goto werr;
if (rioWriteBulkString(aof, li->name, sdslen(li->name)) == 0) goto werr;
if (li->desc) {
if (rioWriteBulkString(aof, "description", 11) == 0) goto werr;
if (rioWriteBulkString(aof, li->desc, sdslen(li->desc)) == 0) goto werr;
}
if (rioWriteBulkString(aof, li->code, sdslen(li->code)) == 0) goto werr;
}
dictReleaseIterator(iter);
return 1;
werr:
dictReleaseIterator(iter);
return 0;
}
int rewriteAppendOnlyFileRio(rio *aof) {
dictIterator *di = NULL;
dictEntry *de;
int j;
long key_count = 0;
long long updated_time = 0;
if (server.aof_timestamp_enabled) {
sds ts = genAofTimestampAnnotationIfNeeded(1);
if (rioWrite(aof,ts,sdslen(ts)) == 0) { sdsfree(ts); goto werr; }
sdsfree(ts);
}
if (rewriteFunctions(aof) == 0) goto werr;
for (j = 0; j < server.dbnum; j++) {
char selectcmd[] = "*2\r\n$6\r\nSELECT\r\n";
redisDb *db = server.db+j;
dict *d = db->dict;
if (dictSize(d) == 0) continue;
di = dictGetSafeIterator(d);
if (rioWrite(aof,selectcmd,sizeof(selectcmd)-1) == 0) goto werr;
if (rioWriteBulkLongLong(aof,j) == 0) goto werr;
while((de = dictNext(di)) != NULL) {
sds keystr;
robj key, *o;
long long expiretime;
size_t aof_bytes_before_key = aof->processed_bytes;
keystr = dictGetKey(de);
o = dictGetVal(de);
initStaticStringObject(key,keystr);
expiretime = getExpire(db,&key);
if (o->type == OBJ_STRING) {
char cmd[]="*3\r\n$3\r\nSET\r\n";
if (rioWrite(aof,cmd,sizeof(cmd)-1) == 0) goto werr;
if (rioWriteBulkObject(aof,&key) == 0) goto werr;
if (rioWriteBulkObject(aof,o) == 0) goto werr;
} else if (o->type == OBJ_LIST) {
if (rewriteListObject(aof,&key,o) == 0) goto werr;
} else if (o->type == OBJ_SET) {
if (rewriteSetObject(aof,&key,o) == 0) goto werr;
} else if (o->type == OBJ_ZSET) {
if (rewriteSortedSetObject(aof,&key,o) == 0) goto werr;
} else if (o->type == OBJ_HASH) {
if (rewriteHashObject(aof,&key,o) == 0) goto werr;
} else if (o->type == OBJ_STREAM) {
if (rewriteStreamObject(aof,&key,o) == 0) goto werr;
} else if (o->type == OBJ_MODULE) {
if (rewriteModuleObject(aof,&key,o,j) == 0) goto werr;
} else {
serverPanic("Unknown object type");
}
size_t dump_size = aof->processed_bytes - aof_bytes_before_key;
if (server.in_fork_child) dismissObject(o, dump_size);
if (expiretime != -1) {
char cmd[]="*3\r\n$9\r\nPEXPIREAT\r\n";
if (rioWrite(aof,cmd,sizeof(cmd)-1) == 0) goto werr;
if (rioWriteBulkObject(aof,&key) == 0) goto werr;
if (rioWriteBulkLongLong(aof,expiretime) == 0) goto werr;
}
if ((key_count++ & 1023) == 0) {
long long now = mstime();
if (now - updated_time >= 1000) {
sendChildInfo(CHILD_INFO_TYPE_CURRENT_INFO, key_count, "AOF rewrite");
updated_time = now;
}
}
if (server.rdb_key_save_delay)
debugDelay(server.rdb_key_save_delay);
}
dictReleaseIterator(di);
di = NULL;
}
return C_OK;
werr:
if (di) dictReleaseIterator(di);
return C_ERR;
}
int rewriteAppendOnlyFile(char *filename) {
rio aof;
FILE *fp = NULL;
char tmpfile[256];
snprintf(tmpfile,256,"temp-rewriteaof-%d.aof", (int) getpid());
fp = fopen(tmpfile,"w");
if (!fp) {
serverLog(LL_WARNING, "Opening the temp file for AOF rewrite in rewriteAppendOnlyFile(): %s", strerror(errno));
return C_ERR;
}
rioInitWithFile(&aof,fp);
if (server.aof_rewrite_incremental_fsync)
rioSetAutoSync(&aof,REDIS_AUTOSYNC_BYTES);
startSaving(RDBFLAGS_AOF_PREAMBLE);
if (server.aof_use_rdb_preamble) {
int error;
if (rdbSaveRio(SLAVE_REQ_NONE,&aof,&error,RDBFLAGS_AOF_PREAMBLE,NULL) == C_ERR) {
errno = error;
goto werr;
}
} else {
if (rewriteAppendOnlyFileRio(&aof) == C_ERR) goto werr;
}
if (fflush(fp)) goto werr;
if (fsync(fileno(fp))) goto werr;
if (fclose(fp)) { fp = NULL; goto werr; }
fp = NULL;
if (rename(tmpfile,filename) == -1) {
serverLog(LL_WARNING,"Error moving temp append only file on the final destination: %s", strerror(errno));
unlink(tmpfile);
stopSaving(0);
return C_ERR;
}
serverLog(LL_NOTICE,"SYNC append only file rewrite performed");
stopSaving(1);
return C_OK;
werr:
serverLog(LL_WARNING,"Write error writing append only file on disk: %s", strerror(errno));
if (fp) fclose(fp);
unlink(tmpfile);
stopSaving(0);
return C_ERR;
}
int rewriteAppendOnlyFileBackground(void) {
pid_t childpid;
if (hasActiveChildProcess()) return C_ERR;
if (dirCreateIfMissing(server.aof_dirname) == -1) {
serverLog(LL_WARNING, "Can't open or create append-only dir %s: %s",
server.aof_dirname, strerror(errno));
return C_ERR;
}
server.aof_selected_db = -1;
flushAppendOnlyFile(1);
if (openNewIncrAofForAppend() != C_OK) return C_ERR;
if ((childpid = redisFork(CHILD_TYPE_AOF)) == 0) {
char tmpfile[256];
redisSetProcTitle("redis-aof-rewrite");
redisSetCpuAffinity(server.aof_rewrite_cpulist);
snprintf(tmpfile,256,"temp-rewriteaof-bg-%d.aof", (int) getpid());
if (rewriteAppendOnlyFile(tmpfile) == C_OK) {
sendChildCowInfo(CHILD_INFO_TYPE_AOF_COW_SIZE, "AOF rewrite");
exitFromChild(0);
} else {
exitFromChild(1);
}
} else {
if (childpid == -1) {
server.aof_lastbgrewrite_status = C_ERR;
serverLog(LL_WARNING,
"Can't rewrite append only file in background: fork: %s",
strerror(errno));
return C_ERR;
}
serverLog(LL_NOTICE,
"Background append only file rewriting started by pid %ld",(long) childpid);
server.aof_rewrite_scheduled = 0;
server.aof_rewrite_time_start = time(NULL);
return C_OK;
}
return C_OK;
}
void bgrewriteaofCommand(client *c) {
if (server.child_type == CHILD_TYPE_AOF) {
addReplyError(c,"Background append only file rewriting already in progress");
} else if (hasActiveChildProcess() || server.in_exec) {
server.aof_rewrite_scheduled = 1;
addReplyStatus(c,"Background append only file rewriting scheduled");
} else if (rewriteAppendOnlyFileBackground() == C_OK) {
addReplyStatus(c,"Background append only file rewriting started");
} else {
addReplyError(c,"Can't execute an AOF background rewriting. "
"Please check the server logs for more information.");
}
}
void aofRemoveTempFile(pid_t childpid) {
char tmpfile[256];
snprintf(tmpfile,256,"temp-rewriteaof-bg-%d.aof", (int) childpid);
bg_unlink(tmpfile);
snprintf(tmpfile,256,"temp-rewriteaof-%d.aof", (int) childpid);
bg_unlink(tmpfile);
}
off_t getAppendOnlyFileSize(sds filename) {
struct redis_stat sb;
off_t size;
mstime_t latency;
sds aof_filepath = makePath(server.aof_dirname, filename);
latencyStartMonitor(latency);
if (redis_stat(aof_filepath, &sb) == -1) {
serverLog(LL_WARNING, "Unable to obtain the AOF file %s length. stat: %s",
filename, strerror(errno));
size = 0;
} else {
size = sb.st_size;
}
latencyEndMonitor(latency);
latencyAddSampleIfNeeded("aof-fstat", latency);
sdsfree(aof_filepath);
return size;
}
off_t getBaseAndIncrAppendOnlyFilesSize(aofManifest *am) {
off_t size = 0;
listNode *ln;
listIter li;
if (am->base_aof_info) {
serverAssert(am->base_aof_info->file_type == AOF_FILE_TYPE_BASE);
size += getAppendOnlyFileSize(am->base_aof_info->file_name);
}
listRewind(am->incr_aof_list, &li);
while ((ln = listNext(&li)) != NULL) {
aofInfo *ai = (aofInfo*)ln->value;
serverAssert(ai->file_type == AOF_FILE_TYPE_INCR);
size += getAppendOnlyFileSize(ai->file_name);
}
return size;
}
int getBaseAndIncrAppendOnlyFilesNum(aofManifest *am) {
int num = 0;
if (am->base_aof_info) num++;
if (am->incr_aof_list) num += listLength(am->incr_aof_list);
return num;
}
void backgroundRewriteDoneHandler(int exitcode, int bysignal) {
if (!bysignal && exitcode == 0) {
char tmpfile[256];
long long now = ustime();
sds new_base_filename;
aofManifest *temp_am;
mstime_t latency;
serverLog(LL_NOTICE,
"Background AOF rewrite terminated with success");
snprintf(tmpfile, 256, "temp-rewriteaof-bg-%d.aof",
(int)server.child_pid);
serverAssert(server.aof_manifest != NULL);
temp_am = aofManifestDup(server.aof_manifest);
new_base_filename = getNewBaseFileNameAndMarkPreAsHistory(temp_am);
serverAssert(new_base_filename != NULL);
sds new_base_filepath = makePath(server.aof_dirname, new_base_filename);
latencyStartMonitor(latency);
if (rename(tmpfile, new_base_filepath) == -1) {
serverLog(LL_WARNING,
"Error trying to rename the temporary AOF file %s into %s: %s",
tmpfile,
new_base_filename,
strerror(errno));
aofManifestFree(temp_am);
sdsfree(new_base_filepath);
goto cleanup;
}
latencyEndMonitor(latency);
latencyAddSampleIfNeeded("aof-rename", latency);
markRewrittenIncrAofAsHistory(temp_am);
if (persistAofManifest(temp_am) == C_ERR) {
bg_unlink(new_base_filepath);
aofManifestFree(temp_am);
sdsfree(new_base_filepath);
goto cleanup;
}
sdsfree(new_base_filepath);
aofManifestFreeAndUpdate(temp_am);
if (server.aof_fd != -1) {
server.aof_selected_db = -1;
server.aof_current_size = getAppendOnlyFileSize(new_base_filename) + server.aof_last_incr_size;
server.aof_rewrite_base_size = server.aof_current_size;
server.aof_fsync_offset = server.aof_current_size;
server.aof_last_fsync = server.unixtime;
}
aofDelHistoryFiles();
server.aof_lastbgrewrite_status = C_OK;
serverLog(LL_NOTICE, "Background AOF rewrite finished successfully");
if (server.aof_state == AOF_WAIT_REWRITE)
server.aof_state = AOF_ON;
serverLog(LL_VERBOSE,
"Background AOF rewrite signal handler took %lldus", ustime()-now);
} else if (!bysignal && exitcode != 0) {
server.aof_lastbgrewrite_status = C_ERR;
serverLog(LL_WARNING,
"Background AOF rewrite terminated with error");
} else {
if (bysignal != SIGUSR1)
server.aof_lastbgrewrite_status = C_ERR;
serverLog(LL_WARNING,
"Background AOF rewrite terminated by signal %d", bysignal);
}
cleanup:
aofRemoveTempFile(server.child_pid);
server.aof_rewrite_time_last = time(NULL)-server.aof_rewrite_time_start;
server.aof_rewrite_time_start = -1;
if (server.aof_state == AOF_WAIT_REWRITE)
server.aof_rewrite_scheduled = 1;
}
