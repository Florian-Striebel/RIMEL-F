


































#if 0
#if !defined(lint)
static char const copyright[] =
"@(#) Copyright (c) 1992, 1993\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#if !defined(lint)
static char sccsid[] = "@(#)pax.c 8.2 (Berkeley) 4/18/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <paths.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pax.h"
#include "extern.h"
static int gen_init(void);








int act = DEFOP;
FSUB *frmt = NULL;
int cflag;
int cwdfd;
int dflag;
int iflag;
int kflag;
int lflag;
int nflag;
int tflag;
int uflag;
int vflag;
int Dflag;
int Hflag;
int Lflag;
int Oflag;
int Xflag;
int Yflag;
int Zflag;
int vfpart;
int patime = 1;
int pmtime = 1;
int nodirs;
int pmode;
int pids;
int rmleadslash = 0;
int exit_val;
int docrc;
char *dirptr;
const char *argv0;
sigset_t s_mask;
FILE *listf;
char *tempfile;
char *tempbase;

























































































































int
main(int argc, char *argv[])
{
const char *tmpdir;
size_t tdlen;

(void) setlocale(LC_ALL, "");
listf = stderr;



cwdfd = open(".", O_RDONLY | O_CLOEXEC);
if (cwdfd < 0) {
syswarn(0, errno, "Can't open current working directory.");
return(exit_val);
}




if ((tmpdir = getenv("TMPDIR")) == NULL || *tmpdir == '\0')
tmpdir = _PATH_TMP;
tdlen = strlen(tmpdir);
while (tdlen > 0 && tmpdir[tdlen - 1] == '/')
tdlen--;
tempfile = malloc(tdlen + 1 + sizeof(_TFILE_BASE));
if (tempfile == NULL) {
paxwarn(1, "Cannot allocate memory for temp file name.");
return(exit_val);
}
if (tdlen)
memcpy(tempfile, tmpdir, tdlen);
tempbase = tempfile + tdlen;
*tempbase++ = '/';




options(argc, argv);
if ((gen_init() < 0) || (tty_init() < 0))
return(exit_val);




switch (act) {
case EXTRACT:
extract();
break;
case ARCHIVE:
archive();
break;
case APPND:
if (gzip_program != NULL)
err(1, "can not gzip while appending");
append();
break;
case COPY:
copy();
break;
default:
case LIST:
list();
break;
}
return(exit_val);
}










void
sig_cleanup(int which_sig)
{





vflag = vfpart = 1;
if (which_sig == SIGXCPU)
paxwarn(0, "Cpu time limit reached, cleaning up.");
else
paxwarn(0, "Signal caught, cleaning up.");

ar_close();
proc_dir();
if (tflag)
atdir_end();
exit(1);
}






static int
setup_sig(int sig, const struct sigaction *n_hand)
{
struct sigaction o_hand;

if (sigaction(sig, NULL, &o_hand) < 0)
return (-1);

if (o_hand.sa_handler == SIG_IGN)
return (0);

return (sigaction(sig, n_hand, NULL));
}







static int
gen_init(void)
{
struct rlimit reslimit;
struct sigaction n_hand;





if (getrlimit(RLIMIT_DATA , &reslimit) == 0){
reslimit.rlim_cur = reslimit.rlim_max;
(void)setrlimit(RLIMIT_DATA , &reslimit);
}





if (getrlimit(RLIMIT_FSIZE , &reslimit) == 0){
reslimit.rlim_cur = reslimit.rlim_max;
(void)setrlimit(RLIMIT_FSIZE , &reslimit);
}




if (getrlimit(RLIMIT_STACK , &reslimit) == 0){
reslimit.rlim_cur = reslimit.rlim_max;
(void)setrlimit(RLIMIT_STACK , &reslimit);
}




if (getrlimit(RLIMIT_RSS , &reslimit) == 0){
reslimit.rlim_cur = reslimit.rlim_max;
(void)setrlimit(RLIMIT_RSS , &reslimit);
}







if ((sigemptyset(&s_mask) < 0) || (sigaddset(&s_mask, SIGTERM) < 0) ||
(sigaddset(&s_mask,SIGINT) < 0)||(sigaddset(&s_mask,SIGHUP) < 0) ||
(sigaddset(&s_mask,SIGPIPE) < 0)||(sigaddset(&s_mask,SIGQUIT)<0) ||
(sigaddset(&s_mask,SIGXCPU) < 0)||(sigaddset(&s_mask,SIGXFSZ)<0)) {
paxwarn(1, "Unable to set up signal mask");
return(-1);
}
memset(&n_hand, 0, sizeof n_hand);
n_hand.sa_mask = s_mask;
n_hand.sa_flags = 0;
n_hand.sa_handler = sig_cleanup;

if (setup_sig(SIGHUP, &n_hand) ||
setup_sig(SIGTERM, &n_hand) ||
setup_sig(SIGINT, &n_hand) ||
setup_sig(SIGQUIT, &n_hand) ||
setup_sig(SIGXCPU, &n_hand))
goto out;

n_hand.sa_handler = SIG_IGN;
if ((sigaction(SIGPIPE, &n_hand, NULL) < 0) ||
(sigaction(SIGXFSZ, &n_hand, NULL) < 0))
goto out;
return(0);

out:
syswarn(1, errno, "Unable to set up signal handler");
return(-1);
}
