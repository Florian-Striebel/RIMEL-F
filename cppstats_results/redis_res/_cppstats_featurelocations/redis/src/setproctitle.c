


























#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <errno.h>

#if !defined(HAVE_SETPROCTITLE)
#if (defined __NetBSD__ || defined __FreeBSD__ || defined __OpenBSD__ || defined __DragonFly__)
#define HAVE_SETPROCTITLE 1
#else
#define HAVE_SETPROCTITLE 0
#endif
#endif


#if !HAVE_SETPROCTITLE
#if (defined __linux || defined __APPLE__)

#if defined(__GLIBC__)
#define HAVE_CLEARENV
#endif

extern char **environ;

static struct {

const char *arg0;


char *base, *end;


char *nul;

_Bool reset;
int error;
} SPT;


#if !defined(SPT_MIN)
#define SPT_MIN(a, b) (((a) < (b))? (a) : (b))
#endif

static inline size_t spt_min(size_t a, size_t b) {
return SPT_MIN(a, b);
}






int spt_clearenv(void) {
#if defined(HAVE_CLEARENV)
return clearenv();
#else
extern char **environ;
static char **tmp;

if (!(tmp = malloc(sizeof *tmp)))
return errno;

tmp[0] = NULL;
environ = tmp;

return 0;
#endif
}


static int spt_copyenv(int envc, char *oldenv[]) {
extern char **environ;
char **envcopy = NULL;
char *eq;
int i, error;
int envsize;

if (environ != oldenv)
return 0;




envsize = (envc + 1) * sizeof(char *);
envcopy = malloc(envsize);
if (!envcopy)
return ENOMEM;
memcpy(envcopy, oldenv, envsize);




if ((error = spt_clearenv())) {
environ = oldenv;
free(envcopy);
return error;
}


for (i = 0; envcopy[i]; i++) {
if (!(eq = strchr(envcopy[i], '=')))
continue;

*eq = '\0';
error = (0 != setenv(envcopy[i], eq + 1, 1))? errno : 0;
*eq = '=';


if (error) {
#if defined(HAVE_CLEARENV)




environ = envcopy;
#else
free(envcopy);
free(environ);
environ = oldenv;
#endif
return error;
}
}

free(envcopy);
return 0;
}


static int spt_copyargs(int argc, char *argv[]) {
char *tmp;
int i;

for (i = 1; i < argc || (i >= argc && argv[i]); i++) {
if (!argv[i])
continue;

if (!(tmp = strdup(argv[i])))
return errno;

argv[i] = tmp;
}

return 0;
}











void spt_init(int argc, char *argv[]) {
char **envp = environ;
char *base, *end, *nul, *tmp;
int i, error, envc;

if (!(base = argv[0]))
return;


nul = &base[strlen(base)];
end = nul + 1;






for (i = 0; i < argc || (i >= argc && argv[i]); i++) {
if (!argv[i] || argv[i] < end)
continue;

if (end >= argv[i] && end <= argv[i] + strlen(argv[i]))
end = argv[i] + strlen(argv[i]) + 1;
}




for (i = 0; envp[i]; i++) {
if (envp[i] < end)
continue;

if (end >= envp[i] && end <= envp[i] + strlen(envp[i]))
end = envp[i] + strlen(envp[i]) + 1;
}
envc = i;





if (!(SPT.arg0 = strdup(argv[0])))
goto syerr;

#if __linux__
if (!(tmp = strdup(program_invocation_name)))
goto syerr;

program_invocation_name = tmp;

if (!(tmp = strdup(program_invocation_short_name)))
goto syerr;

program_invocation_short_name = tmp;
#elif __APPLE__
if (!(tmp = strdup(getprogname())))
goto syerr;

setprogname(tmp);
#endif


if ((error = spt_copyenv(envc, envp)))
goto error;

if ((error = spt_copyargs(argc, argv)))
goto error;

SPT.nul = nul;
SPT.base = base;
SPT.end = end;

return;
syerr:
error = errno;
error:
SPT.error = error;
}


#if !defined(SPT_MAXTITLE)
#define SPT_MAXTITLE 255
#endif

void setproctitle(const char *fmt, ...) {
char buf[SPT_MAXTITLE + 1];
va_list ap;
char *nul;
int len, error;

if (!SPT.base)
return;

if (fmt) {
va_start(ap, fmt);
len = vsnprintf(buf, sizeof buf, fmt, ap);
va_end(ap);
} else {
len = snprintf(buf, sizeof buf, "%s", SPT.arg0);
}

if (len <= 0)
{ error = errno; goto error; }

if (!SPT.reset) {
memset(SPT.base, 0, SPT.end - SPT.base);
SPT.reset = 1;
} else {
memset(SPT.base, 0, spt_min(sizeof buf, SPT.end - SPT.base));
}

len = spt_min(len, spt_min(sizeof buf, SPT.end - SPT.base) - 1);
memcpy(SPT.base, buf, len);
nul = &SPT.base[len];

if (nul < SPT.nul) {
*SPT.nul = '.';
} else if (nul == SPT.nul && &nul[1] < SPT.end) {
*SPT.nul = ' ';
*++nul = '\0';
}

return;
error:
SPT.error = error;
}


#endif
#endif

#if defined(SETPROCTITLE_TEST_MAIN)
int main(int argc, char *argv[]) {
spt_init(argc, argv);

printf("SPT.arg0: [%p] '%s'\n", SPT.arg0, SPT.arg0);
printf("SPT.base: [%p] '%s'\n", SPT.base, SPT.base);
printf("SPT.end: [%p] (%d bytes after base)'\n", SPT.end, (int) (SPT.end - SPT.base));
return 0;
}
#endif
