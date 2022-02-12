#include "mt19937-64.h"
#include "server.h"
#include "rdb.h"
#include <stdarg.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
void createSharedObjects(void);
void rdbLoadProgressCallback(rio *r, const void *buf, size_t len);
int rdbCheckMode = 0;
struct {
rio *rio;
robj *key;
int key_type;
unsigned long keys;
unsigned long expires;
unsigned long already_expired;
int doing;
int error_set;
char error[1024];
} rdbstate;
#define RDB_CHECK_DOING_START 0
#define RDB_CHECK_DOING_READ_TYPE 1
#define RDB_CHECK_DOING_READ_EXPIRE 2
#define RDB_CHECK_DOING_READ_KEY 3
#define RDB_CHECK_DOING_READ_OBJECT_VALUE 4
#define RDB_CHECK_DOING_CHECK_SUM 5
#define RDB_CHECK_DOING_READ_LEN 6
#define RDB_CHECK_DOING_READ_AUX 7
#define RDB_CHECK_DOING_READ_MODULE_AUX 8
char *rdb_check_doing_string[] = {
"start",
"read-type",
"read-expire",
"read-key",
"read-object-value",
"check-sum",
"read-len",
"read-aux",
"read-module-aux"
};
char *rdb_type_string[] = {
"string",
"list-linked",
"set-hashtable",
"zset-v1",
"hash-hashtable",
"zset-v2",
"module-value",
"","",
"hash-zipmap",
"list-ziplist",
"set-intset",
"zset-ziplist",
"hash-ziplist",
"quicklist",
"stream",
"hash-listpack",
"zset-listpack",
"quicklist-v2"
};
void rdbShowGenericInfo(void) {
printf("[info] %lu keys read\n", rdbstate.keys);
printf("[info] %lu expires\n", rdbstate.expires);
printf("[info] %lu already expired\n", rdbstate.already_expired);
}
void rdbCheckError(const char *fmt, ...) {
char msg[1024];
va_list ap;
va_start(ap, fmt);
vsnprintf(msg, sizeof(msg), fmt, ap);
va_end(ap);
printf("--- RDB ERROR DETECTED ---\n");
printf("[offset %llu] %s\n",
(unsigned long long) (rdbstate.rio ?
rdbstate.rio->processed_bytes : 0), msg);
printf("[additional info] While doing: %s\n",
rdb_check_doing_string[rdbstate.doing]);
if (rdbstate.key)
printf("[additional info] Reading key '%s'\n",
(char*)rdbstate.key->ptr);
if (rdbstate.key_type != -1)
printf("[additional info] Reading type %d (%s)\n",
rdbstate.key_type,
((unsigned)rdbstate.key_type <
sizeof(rdb_type_string)/sizeof(char*)) ?
rdb_type_string[rdbstate.key_type] : "unknown");
rdbShowGenericInfo();
}
void rdbCheckInfo(const char *fmt, ...) {
char msg[1024];
va_list ap;
va_start(ap, fmt);
vsnprintf(msg, sizeof(msg), fmt, ap);
va_end(ap);
printf("[offset %llu] %s\n",
(unsigned long long) (rdbstate.rio ?
rdbstate.rio->processed_bytes : 0), msg);
}
void rdbCheckSetError(const char *fmt, ...) {
va_list ap;
va_start(ap, fmt);
vsnprintf(rdbstate.error, sizeof(rdbstate.error), fmt, ap);
va_end(ap);
rdbstate.error_set = 1;
}
void rdbCheckHandleCrash(int sig, siginfo_t *info, void *secret) {
UNUSED(sig);
UNUSED(info);
UNUSED(secret);
rdbCheckError("Server crash checking the specified RDB file!");
exit(1);
}
void rdbCheckSetupSignals(void) {
struct sigaction act;
sigemptyset(&act.sa_mask);
act.sa_flags = SA_NODEFER | SA_RESETHAND | SA_SIGINFO;
act.sa_sigaction = rdbCheckHandleCrash;
sigaction(SIGSEGV, &act, NULL);
sigaction(SIGBUS, &act, NULL);
sigaction(SIGFPE, &act, NULL);
sigaction(SIGILL, &act, NULL);
sigaction(SIGABRT, &act, NULL);
}
int redis_check_rdb(char *rdbfilename, FILE *fp) {
uint64_t dbid;
int selected_dbid = -1;
int type, rdbver;
char buf[1024];
long long expiretime, now = mstime();
static rio rdb;
struct stat sb;
int closefile = (fp == NULL);
if (fp == NULL && (fp = fopen(rdbfilename,"r")) == NULL) return 1;
if (fstat(fileno(fp), &sb) == -1)
sb.st_size = 0;
startLoadingFile(sb.st_size, rdbfilename, RDBFLAGS_NONE);
rioInitWithFile(&rdb,fp);
rdbstate.rio = &rdb;
rdb.update_cksum = rdbLoadProgressCallback;
if (rioRead(&rdb,buf,9) == 0) goto eoferr;
buf[9] = '\0';
if (memcmp(buf,"REDIS",5) != 0) {
rdbCheckError("Wrong signature trying to load DB from file");
goto err;
}
rdbver = atoi(buf+5);
if (rdbver < 1 || rdbver > RDB_VERSION) {
rdbCheckError("Can't handle RDB format version %d",rdbver);
goto err;
}
expiretime = -1;
while(1) {
robj *key, *val;
rdbstate.doing = RDB_CHECK_DOING_READ_TYPE;
if ((type = rdbLoadType(&rdb)) == -1) goto eoferr;
if (type == RDB_OPCODE_EXPIRETIME) {
rdbstate.doing = RDB_CHECK_DOING_READ_EXPIRE;
expiretime = rdbLoadTime(&rdb);
expiretime *= 1000;
if (rioGetReadError(&rdb)) goto eoferr;
continue;
} else if (type == RDB_OPCODE_EXPIRETIME_MS) {
rdbstate.doing = RDB_CHECK_DOING_READ_EXPIRE;
expiretime = rdbLoadMillisecondTime(&rdb, rdbver);
if (rioGetReadError(&rdb)) goto eoferr;
continue;
} else if (type == RDB_OPCODE_FREQ) {
uint8_t byte;
if (rioRead(&rdb,&byte,1) == 0) goto eoferr;
continue;
} else if (type == RDB_OPCODE_IDLE) {
if (rdbLoadLen(&rdb,NULL) == RDB_LENERR) goto eoferr;
continue;
} else if (type == RDB_OPCODE_EOF) {
break;
} else if (type == RDB_OPCODE_SELECTDB) {
rdbstate.doing = RDB_CHECK_DOING_READ_LEN;
if ((dbid = rdbLoadLen(&rdb,NULL)) == RDB_LENERR)
goto eoferr;
rdbCheckInfo("Selecting DB ID %llu", (unsigned long long)dbid);
selected_dbid = dbid;
continue;
} else if (type == RDB_OPCODE_RESIZEDB) {
uint64_t db_size, expires_size;
rdbstate.doing = RDB_CHECK_DOING_READ_LEN;
if ((db_size = rdbLoadLen(&rdb,NULL)) == RDB_LENERR)
goto eoferr;
if ((expires_size = rdbLoadLen(&rdb,NULL)) == RDB_LENERR)
goto eoferr;
continue;
} else if (type == RDB_OPCODE_AUX) {
robj *auxkey, *auxval;
rdbstate.doing = RDB_CHECK_DOING_READ_AUX;
if ((auxkey = rdbLoadStringObject(&rdb)) == NULL) goto eoferr;
if ((auxval = rdbLoadStringObject(&rdb)) == NULL) goto eoferr;
rdbCheckInfo("AUX FIELD %s = '%s'",
(char*)auxkey->ptr, (char*)auxval->ptr);
decrRefCount(auxkey);
decrRefCount(auxval);
continue;
} else if (type == RDB_OPCODE_MODULE_AUX) {
uint64_t moduleid, when_opcode, when;
rdbstate.doing = RDB_CHECK_DOING_READ_MODULE_AUX;
if ((moduleid = rdbLoadLen(&rdb,NULL)) == RDB_LENERR) goto eoferr;
if ((when_opcode = rdbLoadLen(&rdb,NULL)) == RDB_LENERR) goto eoferr;
if ((when = rdbLoadLen(&rdb,NULL)) == RDB_LENERR) goto eoferr;
char name[10];
moduleTypeNameByID(name,moduleid);
rdbCheckInfo("MODULE AUX for: %s", name);
robj *o = rdbLoadCheckModuleValue(&rdb,name);
decrRefCount(o);
continue;
} else if (type == RDB_OPCODE_FUNCTION) {
sds err = NULL;
if (rdbFunctionLoad(&rdb, rdbver, NULL, 0, &err) != C_OK) {
rdbCheckError("Failed loading library, %s", err);
sdsfree(err);
goto err;
}
continue;
} else {
if (!rdbIsObjectType(type)) {
rdbCheckError("Invalid object type: %d", type);
goto err;
}
rdbstate.key_type = type;
}
rdbstate.doing = RDB_CHECK_DOING_READ_KEY;
if ((key = rdbLoadStringObject(&rdb)) == NULL) goto eoferr;
rdbstate.key = key;
rdbstate.keys++;
rdbstate.doing = RDB_CHECK_DOING_READ_OBJECT_VALUE;
if ((val = rdbLoadObject(type,&rdb,key->ptr,selected_dbid,NULL)) == NULL) goto eoferr;
if (expiretime != -1 && expiretime < now)
rdbstate.already_expired++;
if (expiretime != -1) rdbstate.expires++;
rdbstate.key = NULL;
decrRefCount(key);
decrRefCount(val);
rdbstate.key_type = -1;
expiretime = -1;
}
if (rdbver >= 5 && server.rdb_checksum) {
uint64_t cksum, expected = rdb.cksum;
rdbstate.doing = RDB_CHECK_DOING_CHECK_SUM;
if (rioRead(&rdb,&cksum,8) == 0) goto eoferr;
memrev64ifbe(&cksum);
if (cksum == 0) {
rdbCheckInfo("RDB file was saved with checksum disabled: no check performed.");
} else if (cksum != expected) {
rdbCheckError("RDB CRC error");
goto err;
} else {
rdbCheckInfo("Checksum OK");
}
}
if (closefile) fclose(fp);
stopLoading(1);
return 0;
eoferr:
if (rdbstate.error_set) {
rdbCheckError(rdbstate.error);
} else {
rdbCheckError("Unexpected EOF reading RDB file");
}
err:
if (closefile) fclose(fp);
stopLoading(0);
return 1;
}
static sds checkRdbVersion(void) {
sds version;
version = sdscatprintf(sdsempty(), "%s", REDIS_VERSION);
if (strtoll(redisGitSHA1(),NULL,16)) {
version = sdscatprintf(version, " (git:%s", redisGitSHA1());
if (strtoll(redisGitDirty(),NULL,10))
version = sdscatprintf(version, "-dirty");
version = sdscat(version, ")");
}
return version;
}
int redis_check_rdb_main(int argc, char **argv, FILE *fp) {
struct timeval tv;
if (argc != 2 && fp == NULL) {
fprintf(stderr, "Usage: %s <rdb-file-name>\n", argv[0]);
exit(1);
} else if (!strcmp(argv[1],"-v") || !strcmp(argv[1], "--version")) {
sds version = checkRdbVersion();
printf("redis-check-rdb %s\n", version);
sdsfree(version);
exit(0);
}
gettimeofday(&tv, NULL);
init_genrand64(((long long) tv.tv_sec * 1000000 + tv.tv_usec) ^ getpid());
if (shared.integers[0] == NULL)
createSharedObjects();
server.loading_process_events_interval_bytes = 0;
server.sanitize_dump_payload = SANITIZE_DUMP_YES;
rdbCheckMode = 1;
rdbCheckInfo("Checking RDB file %s", argv[1]);
rdbCheckSetupSignals();
int retval = redis_check_rdb(argv[1],fp);
if (retval == 0) {
rdbCheckInfo("\\o/ RDB looks OK! \\o/");
rdbShowGenericInfo();
}
if (fp) return (retval == 0) ? C_OK : C_ERR;
exit(retval);
}
