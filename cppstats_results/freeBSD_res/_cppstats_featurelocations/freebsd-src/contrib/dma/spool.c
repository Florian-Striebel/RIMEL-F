


































#include "dfcompat.h"

#include <sys/file.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <ctype.h>
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <syslog.h>

#include "dma.h"




















int
newspoolf(struct queue *queue)
{
char fn[PATH_MAX+1];
struct stat st;
struct stritem *t;
int fd;

if (snprintf(fn, sizeof(fn), "%s/%s", config.spooldir, "tmp_XXXXXXXXXX") <= 0)
return (-1);

fd = mkstemp(fn);
if (fd < 0)
return (-1);

if (fchmod(fd, 0660) < 0)
goto fail;
if (flock(fd, LOCK_EX) == -1)
goto fail;
queue->tmpf = strdup(fn);
if (queue->tmpf == NULL)
goto fail;




if (fstat(fd, &st) != 0)
goto fail;
if (asprintf(&queue->id, "%"PRIxMAX, (uintmax_t)st.st_ino) < 0)
goto fail;

queue->mailf = fdopen(fd, "r+");
if (queue->mailf == NULL)
goto fail;

t = malloc(sizeof(*t));
if (t != NULL) {
t->str = queue->tmpf;
SLIST_INSERT_HEAD(&tmpfs, t, next);
}
return (0);

fail:
if (queue->mailf != NULL)
fclose(queue->mailf);
close(fd);
unlink(fn);
return (-1);
}

static int
writequeuef(struct qitem *it)
{
int error;
int queuefd;

queuefd = open_locked(it->queuefn, O_CREAT|O_EXCL|O_RDWR, 0660);
if (queuefd == -1)
return (-1);
if (fchmod(queuefd, 0660) < 0)
return (-1);
it->queuef = fdopen(queuefd, "w+");
if (it->queuef == NULL)
return (-1);

error = fprintf(it->queuef,
"ID: %s\n"
"Sender: %s\n"
"Recipient: %s\n",
it->queueid,
it->sender,
it->addr);

if (error <= 0)
return (-1);

if (fflush(it->queuef) != 0 || fsync(fileno(it->queuef)) != 0)
return (-1);

return (0);
}

static struct qitem *
readqueuef(struct queue *queue, char *queuefn)
{
char line[1000];
struct queue itmqueue;
FILE *queuef = NULL;
char *s;
char *queueid = NULL, *sender = NULL, *addr = NULL;
struct qitem *it = NULL;

bzero(&itmqueue, sizeof(itmqueue));
LIST_INIT(&itmqueue.queue);

queuef = fopen(queuefn, "r");
if (queuef == NULL)
goto out;

while (!feof(queuef)) {
if (fgets(line, sizeof(line), queuef) == NULL || line[0] == 0)
break;
line[strlen(line) - 1] = 0;

s = strchr(line, ':');
if (s == NULL)
goto malformed;
*s = 0;

s++;
while (isspace(*s))
s++;

s = strdup(s);
if (s == NULL)
goto malformed;

if (strcmp(line, "ID") == 0) {
queueid = s;
} else if (strcmp(line, "Sender") == 0) {
sender = s;
} else if (strcmp(line, "Recipient") == 0) {
addr = s;
} else {
syslog(LOG_DEBUG, "ignoring unknown queue info `%s' in `%s'",
line, queuefn);
free(s);
}
}

if (queueid == NULL || sender == NULL || addr == NULL ||
*queueid == 0 || *addr == 0) {
malformed:
errno = EINVAL;
syslog(LOG_ERR, "malformed queue file `%s'", queuefn);
goto out;
}

if (add_recp(&itmqueue, addr, 0) != 0)
goto out;

it = LIST_FIRST(&itmqueue.queue);
it->sender = sender; sender = NULL;
it->queueid = queueid; queueid = NULL;
it->queuefn = queuefn; queuefn = NULL;
LIST_INSERT_HEAD(&queue->queue, it, next);

out:
if (sender != NULL)
free(sender);
if (queueid != NULL)
free(queueid);
if (addr != NULL)
free(addr);
if (queuef != NULL)
fclose(queuef);

return (it);
}

int
linkspool(struct queue *queue)
{
struct stat st;
struct qitem *it;

if (fflush(queue->mailf) != 0 || fsync(fileno(queue->mailf)) != 0)
goto delfiles;

syslog(LOG_INFO, "new mail from user=%s uid=%d envelope_from=<%s>",
username, getuid(), queue->sender);

LIST_FOREACH(it, &queue->queue, next) {
if (asprintf(&it->queueid, "%s.%"PRIxPTR, queue->id, (uintptr_t)it) <= 0)
goto delfiles;
if (asprintf(&it->queuefn, "%s/Q%s", config.spooldir, it->queueid) <= 0)
goto delfiles;
if (asprintf(&it->mailfn, "%s/M%s", config.spooldir, it->queueid) <= 0)
goto delfiles;


if (stat(it->queuefn, &st) == 0 || stat(it->mailfn, &st) == 0)
goto delfiles;

if (writequeuef(it) != 0)
goto delfiles;

if (link(queue->tmpf, it->mailfn) != 0)
goto delfiles;
}

LIST_FOREACH(it, &queue->queue, next) {
syslog(LOG_INFO, "mail to=<%s> queued as %s",
it->addr, it->queueid);
}

unlink(queue->tmpf);
return (0);

delfiles:
LIST_FOREACH(it, &queue->queue, next) {
unlink(it->mailfn);
unlink(it->queuefn);
}
return (-1);
}

int
load_queue(struct queue *queue)
{
struct stat sb;
struct qitem *it;
DIR *spooldir;
struct dirent *de;
char *queuefn;
char *mailfn;

bzero(queue, sizeof(*queue));
LIST_INIT(&queue->queue);

spooldir = opendir(config.spooldir);
if (spooldir == NULL)
err(EX_NOINPUT, "reading queue");

while ((de = readdir(spooldir)) != NULL) {
queuefn = NULL;
mailfn = NULL;


if (de->d_name[0] != 'Q')
continue;
if (asprintf(&queuefn, "%s/Q%s", config.spooldir, de->d_name + 1) < 0)
goto fail;
if (asprintf(&mailfn, "%s/M%s", config.spooldir, de->d_name + 1) < 0)
goto fail;






if (stat(queuefn, &sb) != 0)
goto skip_item;
if (!S_ISREG(sb.st_mode)) {
errno = EINVAL;
goto skip_item;
}

if (stat(mailfn, &sb) != 0)
goto skip_item;

it = readqueuef(queue, queuefn);
if (it == NULL)
goto skip_item;

it->mailfn = mailfn;
continue;

skip_item:
syslog(LOG_INFO, "could not pick up queue file: `%s'/`%s': %m", queuefn, mailfn);
if (queuefn != NULL)
free(queuefn);
if (mailfn != NULL)
free(mailfn);
}
closedir(spooldir);
return (0);

fail:
return (-1);
}

void
delqueue(struct qitem *it)
{
unlink(it->mailfn);
unlink(it->queuefn);
if (it->queuef != NULL)
fclose(it->queuef);
if (it->mailf != NULL)
fclose(it->mailf);
free(it);
}

int
acquirespool(struct qitem *it)
{
int queuefd;

if (it->queuef == NULL) {
queuefd = open_locked(it->queuefn, O_RDWR|O_NONBLOCK);
if (queuefd < 0)
goto fail;
it->queuef = fdopen(queuefd, "r+");
if (it->queuef == NULL)
goto fail;
}

if (it->mailf == NULL) {
it->mailf = fopen(it->mailfn, "r");
if (it->mailf == NULL)
goto fail;
}

return (0);

fail:
if (errno == EWOULDBLOCK)
return (1);
syslog(LOG_INFO, "could not acquire queue file: %m");
return (-1);
}

void
dropspool(struct queue *queue, struct qitem *keep)
{
struct qitem *it;

LIST_FOREACH(it, &queue->queue, next) {
if (it == keep)
continue;

if (it->queuef != NULL)
fclose(it->queuef);
if (it->mailf != NULL)
fclose(it->mailf);
}
}

int
flushqueue_since(unsigned int period)
{
struct stat st;
struct timeval now;
char *flushfn = NULL;

if (asprintf(&flushfn, "%s/%s", config.spooldir, SPOOL_FLUSHFILE) < 0)
return (0);
if (stat(flushfn, &st) < 0) {
free(flushfn);
return (0);
}
free(flushfn);
flushfn = NULL;
if (gettimeofday(&now, 0) != 0)
return (0);


if (st.st_mtim.tv_sec + (int)period >= now.tv_sec)
return (1);
else
return (0);
}

int
flushqueue_signal(void)
{
char *flushfn = NULL;
int fd;

if (asprintf(&flushfn, "%s/%s", config.spooldir, SPOOL_FLUSHFILE) < 0)
return (-1);
fd = open(flushfn, O_CREAT|O_WRONLY|O_TRUNC, 0660);
free(flushfn);
if (fd < 0) {
syslog(LOG_ERR, "could not open flush file: %m");
return (-1);
}
close(fd);
return (0);
}
