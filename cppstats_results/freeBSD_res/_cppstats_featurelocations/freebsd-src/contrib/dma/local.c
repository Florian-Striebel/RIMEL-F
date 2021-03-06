


































#include <sys/types.h>
#include <sys/wait.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <paths.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "dma.h"

static int
create_mbox(const char *name)
{
struct sigaction sa, osa;
pid_t child, waitchild;
int status;
int i;
long maxfd;
int e;
int r = -1;




bzero(&sa, sizeof(sa));
sa.sa_handler = SIG_DFL;
sigaction(SIGCHLD, &sa, &osa);

do_timeout(100, 0);

child = fork();
switch (child) {
case 0:

maxfd = sysconf(_SC_OPEN_MAX);
if (maxfd == -1)
maxfd = 1024;

for (i = 3; i <= maxfd; ++i)
close(i);

execl(LIBEXEC_PATH "/dma-mbox-create", "dma-mbox-create", name, (char *)NULL);
syslog(LOG_ERR, "cannot execute "LIBEXEC_PATH"/dma-mbox-create: %m");
exit(EX_SOFTWARE);

default:

waitchild = waitpid(child, &status, 0);

e = errno;

do_timeout(0, 0);

if (waitchild == -1 && e == EINTR) {
syslog(LOG_ERR, "hung child while creating mbox `%s': %m", name);
break;
}

if (waitchild == -1) {
syslog(LOG_ERR, "child disappeared while creating mbox `%s': %m", name);
break;
}

if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
syslog(LOG_ERR, "error creating mbox `%s'", name);
break;
}


r = 0;
break;

case -1:

syslog(LOG_ERR, "error creating mbox");
break;
}

sigaction(SIGCHLD, &osa, NULL);

return (r);
}

int
deliver_local(struct qitem *it)
{
char fn[PATH_MAX+1];
char line[1000];
const char *sender;
const char *newline = "\n";
size_t linelen;
int tries = 0;
int mbox;
int error;
int hadnl = 0;
off_t mboxlen;
time_t now = time(NULL);

error = snprintf(fn, sizeof(fn), "%s/%s", _PATH_MAILDIR, it->addr);
if (error < 0 || (size_t)error >= sizeof(fn)) {
syslog(LOG_NOTICE, "local delivery deferred: %m");
return (1);
}

retry:

do_timeout(100, 0);


mbox = open_locked(fn, O_WRONLY|O_APPEND);
if (mbox < 0) {
int e = errno;

do_timeout(0, 0);

switch (e) {
case EACCES:
case ENOENT:




if (tries > 0 || create_mbox(it->addr) != 0) {
syslog(LOG_ERR, "local delivery deferred: can not create `%s'", fn);
return (1);
}
++tries;
goto retry;

case EINTR:
syslog(LOG_NOTICE, "local delivery deferred: can not lock `%s'", fn);
break;

default:
syslog(LOG_NOTICE, "local delivery deferred: can not open `%s': %m", fn);
break;
}
return (1);
}
do_timeout(0, 0);

mboxlen = lseek(mbox, 0, SEEK_END);


if (mboxlen == 0)
newline = "";


sender = it->sender;
if (strcmp(sender, "") == 0)
sender = "MAILER-DAEMON";

if (fseek(it->mailf, 0, SEEK_SET) != 0) {
syslog(LOG_NOTICE, "local delivery deferred: can not seek: %m");
goto out;
}

error = snprintf(line, sizeof(line), "%sFrom %s %s", newline, sender, ctime(&now));
if (error < 0 || (size_t)error >= sizeof(line)) {
syslog(LOG_NOTICE, "local delivery deferred: can not write header: %m");
goto out;
}
if (write(mbox, line, error) != error)
goto wrerror;

while (!feof(it->mailf)) {
if (fgets(line, sizeof(line), it->mailf) == NULL)
break;
linelen = strlen(line);
if (linelen == 0 || line[linelen - 1] != '\n') {
syslog(LOG_CRIT, "local delivery failed: corrupted queue file");
snprintf(errmsg, sizeof(errmsg), "corrupted queue file");
error = -1;
goto chop;
}










if ((!MBOX_STRICT || hadnl) &&
strncmp(&line[strspn(line, ">")], "From ", 5) == 0) {
const char *gt = ">";

if (write(mbox, gt, 1) != 1)
goto wrerror;
hadnl = 0;
} else if (strcmp(line, "\n") == 0) {
hadnl = 1;
} else {
hadnl = 0;
}
if ((size_t)write(mbox, line, linelen) != linelen)
goto wrerror;
}
close(mbox);
return (0);

wrerror:
syslog(LOG_ERR, "local delivery failed: write error: %m");
error = 1;
chop:
if (ftruncate(mbox, mboxlen) != 0)
syslog(LOG_WARNING, "error recovering mbox `%s': %m", fn);
out:
close(mbox);
return (error);
}
