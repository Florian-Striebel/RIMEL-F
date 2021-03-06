





























#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <dtrace.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#if defined(illumos)
#include <alloca.h>
#endif
#include <libgen.h>
#if defined(illumos)
#include <libproc.h>
#endif
#if defined(__FreeBSD__)
#include <locale.h>
#include <spawn.h>
#endif

typedef struct dtrace_cmd {
void (*dc_func)(struct dtrace_cmd *);
dtrace_probespec_t dc_spec;
char *dc_arg;
const char *dc_name;
const char *dc_desc;
dtrace_prog_t *dc_prog;
char dc_ofile[PATH_MAX];
} dtrace_cmd_t;

#define DMODE_VERS 0
#define DMODE_EXEC 1
#define DMODE_ANON 2
#define DMODE_LINK 3
#define DMODE_LIST 4
#define DMODE_HEADER 5

#define E_SUCCESS 0
#define E_ERROR 1
#define E_USAGE 2

static const char DTRACE_OPTSTR[] =
"3:6:aAb:Bc:CD:ef:FGhHi:I:lL:m:n:o:p:P:qs:SU:vVwx:X:Z";

static char **g_argv;
static int g_argc;
static char **g_objv;
static int g_objc;
static dtrace_cmd_t *g_cmdv;
static int g_cmdc;
static struct ps_prochandle **g_psv;
static int g_psc;
static int g_pslive;
static char *g_pname;
static int g_quiet;
static int g_flowindent;
static int g_intr;
static int g_impatient;
static int g_newline;
#if defined(__FreeBSD__)
static int g_siginfo;
#endif
static int g_total;
static int g_cflags;
static int g_oflags;
static int g_verbose;
static int g_exec = 1;
static int g_mode = DMODE_EXEC;
static int g_status = E_SUCCESS;
static int g_grabanon = 0;
static const char *g_ofile = NULL;
static FILE *g_ofp;
static dtrace_hdl_t *g_dtp;
#if defined(illumos)
static char *g_etcfile = "/etc/system";
static const char *g_etcbegin = "* vvvv Added by DTrace";
static const char *g_etcend = "* ^^^^ Added by DTrace";

static const char *g_etc[] = {
"*",
"* The following forceload directives were added by dtrace(1M) to allow for",
"* tracing during boot. If these directives are removed, the system will",
"* continue to function, but tracing will not occur during boot as desired.",
"* To remove these directives (and this block comment) automatically, run",
"* \"dtrace -A\" without additional arguments. See the \"Anonymous Tracing\"",
"* chapter of the Solaris Dynamic Tracing Guide for details.",
"*",
NULL };
#endif

static int
usage(FILE *fp)
{
static const char predact[] = "[[ predicate ] action ]";

(void) fprintf(fp, "Usage: %s [-32|-64] [-aACeFGhHlqSvVwZ] "
"[-b bufsz] [-c cmd] [-D name[=def]]\n\t[-I path] [-L path] "
"[-o output] [-p pid] [-s script] [-U name]\n\t"
"[-x opt[=val]] [-X a|c|s|t]\n\n"
"\t[-P provider %s]\n"
"\t[-m [ provider: ] module %s]\n"
"\t[-f [[ provider: ] module: ] func %s]\n"
"\t[-n [[[ provider: ] module: ] func: ] name %s]\n"
"\t[-i probe-id %s] [ args ... ]\n\n", g_pname,
predact, predact, predact, predact, predact);

(void) fprintf(fp, "\tpredicate -> '/' D-expression '/'\n");
(void) fprintf(fp, "\t action -> '{' D-statements '}'\n");

(void) fprintf(fp, "\n"
"\t-32 generate 32-bit D programs and ELF files\n"
"\t-64 generate 64-bit D programs and ELF files\n\n"
"\t-a claim anonymous tracing state\n"
"\t-A generate driver.conf(4) directives for anonymous tracing\n"
"\t-b set trace buffer size\n"
"\t-c run specified command and exit upon its completion\n"
"\t-C run cpp(1) preprocessor on script files\n"
"\t-D define symbol when invoking preprocessor\n"
"\t-e exit after compiling request but prior to enabling probes\n"
"\t-f enable or list probes matching the specified function name\n"
"\t-F coalesce trace output by function\n"
"\t-G generate an ELF file containing embedded dtrace program\n"
"\t-h generate a header file with definitions for static probes\n"
"\t-H print included files when invoking preprocessor\n"
"\t-i enable or list probes matching the specified probe id\n"
"\t-I add include directory to preprocessor search path\n"
"\t-l list probes matching specified criteria\n"
"\t-L add library directory to library search path\n"
"\t-m enable or list probes matching the specified module name\n"
"\t-n enable or list probes matching the specified probe name\n"
"\t-o set output file\n"
"\t-p grab specified process-ID and cache its symbol tables\n"
"\t-P enable or list probes matching the specified provider name\n"
"\t-q set quiet mode (only output explicitly traced data)\n"
"\t-s enable or list probes according to the specified D script\n"
"\t-S print D compiler intermediate code\n"
"\t-U undefine symbol when invoking preprocessor\n"
"\t-v set verbose mode (report stability attributes, arguments)\n"
"\t-V report DTrace API version\n"
"\t-w permit destructive actions\n"
"\t-x enable or modify compiler and tracing options\n"
"\t-X specify ISO C conformance settings for preprocessor\n"
"\t-Z permit probe descriptions that match zero probes\n");

return (E_USAGE);
}

static void
verror(const char *fmt, va_list ap)
{
int error = errno;

(void) fprintf(stderr, "%s: ", g_pname);
(void) vfprintf(stderr, fmt, ap);

if (fmt[strlen(fmt) - 1] != '\n')
(void) fprintf(stderr, ": %s\n", strerror(error));
}


static void
fatal(const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
verror(fmt, ap);
va_end(ap);





if (g_dtp)
dtrace_close(g_dtp);

exit(E_ERROR);
}


static void
dfatal(const char *fmt, ...)
{
#if !defined(illumos) && defined(NEED_ERRLOC)
char *p_errfile = NULL;
int errline = 0;
#endif
va_list ap;

va_start(ap, fmt);

(void) fprintf(stderr, "%s: ", g_pname);
if (fmt != NULL)
(void) vfprintf(stderr, fmt, ap);

va_end(ap);

if (fmt != NULL && fmt[strlen(fmt) - 1] != '\n') {
(void) fprintf(stderr, ": %s\n",
dtrace_errmsg(g_dtp, dtrace_errno(g_dtp)));
} else if (fmt == NULL) {
(void) fprintf(stderr, "%s\n",
dtrace_errmsg(g_dtp, dtrace_errno(g_dtp)));
}
#if !defined(illumos) && defined(NEED_ERRLOC)
dt_get_errloc(g_dtp, &p_errfile, &errline);
if (p_errfile != NULL)
printf("File '%s', line %d\n", p_errfile, errline);
#endif





dtrace_close(g_dtp);

exit(E_ERROR);
}


static void
error(const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
verror(fmt, ap);
va_end(ap);
}


static void
notice(const char *fmt, ...)
{
va_list ap;

if (g_quiet)
return;

va_start(ap, fmt);
verror(fmt, ap);
va_end(ap);
}


static void
oprintf(const char *fmt, ...)
{
va_list ap;
int n;

if (g_ofp == NULL)
return;

va_start(ap, fmt);
n = vfprintf(g_ofp, fmt, ap);
va_end(ap);

if (n < 0) {
if (errno != EINTR) {
fatal("failed to write to %s",
g_ofile ? g_ofile : "<stdout>");
}
clearerr(g_ofp);
}
}

static char **
make_argv(char *s)
{
const char *ws = "\f\n\r\t\v ";
char **argv = malloc(sizeof (char *) * (strlen(s) / 2 + 1));
int argc = 0;
char *p = s;

if (argv == NULL)
return (NULL);

for (p = strtok(s, ws); p != NULL; p = strtok(NULL, ws))
argv[argc++] = p;

if (argc == 0)
argv[argc++] = s;

argv[argc] = NULL;
return (argv);
}

static void
dof_prune(const char *fname)
{
struct stat sbuf;
size_t sz, i, j, mark, len;
char *buf;
int msg = 0, fd;

if ((fd = open(fname, O_RDONLY)) == -1) {



if (errno != ENOENT)
fatal("failed to open %s", fname);
return;
}

if (fstat(fd, &sbuf) == -1)
fatal("failed to fstat %s", fname);

if ((buf = malloc((sz = sbuf.st_size) + 1)) == NULL)
fatal("failed to allocate memory for %s", fname);

if (read(fd, buf, sz) != sz)
fatal("failed to read %s", fname);

buf[sz] = '\0';
(void) close(fd);

if ((fd = open(fname, O_WRONLY | O_TRUNC)) == -1)
fatal("failed to open %s for writing", fname);

len = strlen("dof-data-");

for (mark = 0, i = 0; i < sz; i++) {
if (strncmp(&buf[i], "dof-data-", len) != 0)
continue;




if (i != 0 && buf[i - 1] != '\n')
continue;

if (msg++ == 0) {
error("cleaned up old anonymous "
"enabling in %s\n", fname);
}




if (i != mark) {
if (write(fd, &buf[mark], i - mark) != i - mark)
fatal("failed to write to %s", fname);
}




for (j = i; j < sz && buf[j] != '\n'; j++)
continue;




if ((mark = j + 1) >= sz)
break;

i = j;
}

if (mark < sz) {
if (write(fd, &buf[mark], sz - mark) != sz - mark)
fatal("failed to write to %s", fname);
}

(void) close(fd);
free(buf);
}

#if defined(__FreeBSD__)





static void
bootdof_add(void)
{
char * const nbargv[] = {
"nextboot", "-a",
"-e", "dtraceall_load=\"YES\"",
"-e", "dtrace_dof_load=\"YES\"",
"-e", "dtrace_dof_name=\"/boot/dtrace.dof\"",
"-e", "dtrace_dof_type=\"dtrace_dof\"",
NULL,
};
pid_t child;
int err, status;

err = posix_spawnp(&child, "nextboot", NULL, NULL, nbargv,
NULL);
if (err != 0) {
error("failed to execute nextboot: %s", strerror(err));
exit(E_ERROR);
}

if (waitpid(child, &status, 0) != child)
fatal("waiting for nextboot");
if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
error("nextboot returned with status %d", status);
exit(E_ERROR);
}
}
#else
static void
etcsystem_prune(void)
{
struct stat sbuf;
size_t sz;
char *buf, *start, *end;
int fd;
char *fname = g_etcfile, *tmpname;

if ((fd = open(fname, O_RDONLY)) == -1)
fatal("failed to open %s", fname);

if (fstat(fd, &sbuf) == -1)
fatal("failed to fstat %s", fname);

if ((buf = malloc((sz = sbuf.st_size) + 1)) == NULL)
fatal("failed to allocate memory for %s", fname);

if (read(fd, buf, sz) != sz)
fatal("failed to read %s", fname);

buf[sz] = '\0';
(void) close(fd);

if ((start = strstr(buf, g_etcbegin)) == NULL)
goto out;

if (strlen(buf) != sz) {
fatal("embedded nul byte in %s; manual repair of %s "
"required\n", fname, fname);
}

if (strstr(start + 1, g_etcbegin) != NULL) {
fatal("multiple start sentinels in %s; manual repair of %s "
"required\n", fname, fname);
}

if ((end = strstr(buf, g_etcend)) == NULL) {
fatal("missing end sentinel in %s; manual repair of %s "
"required\n", fname, fname);
}

if (start > end) {
fatal("end sentinel preceeds start sentinel in %s; manual "
"repair of %s required\n", fname, fname);
}

end += strlen(g_etcend) + 1;
bcopy(end, start, strlen(end) + 1);

tmpname = alloca(sz = strlen(fname) + 80);
(void) snprintf(tmpname, sz, "%s.dtrace.%d", fname, getpid());

if ((fd = open(tmpname,
O_WRONLY | O_CREAT | O_EXCL, sbuf.st_mode)) == -1)
fatal("failed to create %s", tmpname);

if (write(fd, buf, strlen(buf)) < strlen(buf)) {
(void) unlink(tmpname);
fatal("failed to write to %s", tmpname);
}

(void) close(fd);

if (chown(tmpname, sbuf.st_uid, sbuf.st_gid) != 0) {
(void) unlink(tmpname);
fatal("failed to chown(2) %s to uid %d, gid %d", tmpname,
(int)sbuf.st_uid, (int)sbuf.st_gid);
}

if (rename(tmpname, fname) == -1)
fatal("rename of %s to %s failed", tmpname, fname);

error("cleaned up forceload directives in %s\n", fname);
out:
free(buf);
}

static void
etcsystem_add(void)
{
const char *mods[20];
int nmods, line;

if ((g_ofp = fopen(g_ofile = g_etcfile, "a")) == NULL)
fatal("failed to open output file '%s'", g_ofile);

oprintf("%s\n", g_etcbegin);

for (line = 0; g_etc[line] != NULL; line++)
oprintf("%s\n", g_etc[line]);

nmods = dtrace_provider_modules(g_dtp, mods,
sizeof (mods) / sizeof (char *) - 1);

if (nmods >= sizeof (mods) / sizeof (char *))
fatal("unexpectedly large number of modules!");

mods[nmods++] = "dtrace";

for (line = 0; line < nmods; line++)
oprintf("forceload: drv/%s\n", mods[line]);

oprintf("%s\n", g_etcend);

if (fclose(g_ofp) == EOF)
fatal("failed to close output file '%s'", g_ofile);

error("added forceload directives to %s\n", g_ofile);
}
#endif

static void
print_probe_info(const dtrace_probeinfo_t *p)
{
char buf[BUFSIZ];
char *user;
int i;

oprintf("\n\tProbe Description Attributes\n");

oprintf("\t\tIdentifier Names: %s\n",
dtrace_stability_name(p->dtp_attr.dtat_name));
oprintf("\t\tData Semantics: %s\n",
dtrace_stability_name(p->dtp_attr.dtat_data));
oprintf("\t\tDependency Class: %s\n",
dtrace_class_name(p->dtp_attr.dtat_class));

oprintf("\n\tArgument Attributes\n");

oprintf("\t\tIdentifier Names: %s\n",
dtrace_stability_name(p->dtp_arga.dtat_name));
oprintf("\t\tData Semantics: %s\n",
dtrace_stability_name(p->dtp_arga.dtat_data));
oprintf("\t\tDependency Class: %s\n",
dtrace_class_name(p->dtp_arga.dtat_class));

oprintf("\n\tArgument Types\n");

for (i = 0; i < p->dtp_argc; i++) {
if (p->dtp_argv[i].dtt_flags & DTT_FL_USER)
user = "userland ";
else
user = "";
if (ctf_type_name(p->dtp_argv[i].dtt_ctfp,
p->dtp_argv[i].dtt_type, buf, sizeof (buf)) == NULL)
(void) strlcpy(buf, "(unknown)", sizeof (buf));
oprintf("\t\targs[%d]: %s%s\n", i, user, buf);
}

if (p->dtp_argc == 0)
oprintf("\t\tNone\n");

oprintf("\n");
}


static int
info_stmt(dtrace_hdl_t *dtp, dtrace_prog_t *pgp,
dtrace_stmtdesc_t *stp, dtrace_ecbdesc_t **last)
{
dtrace_ecbdesc_t *edp = stp->dtsd_ecbdesc;
dtrace_probedesc_t *pdp = &edp->dted_probe;
dtrace_probeinfo_t p;

if (edp == *last)
return (0);

oprintf("\n%s:%s:%s:%s\n",
pdp->dtpd_provider, pdp->dtpd_mod, pdp->dtpd_func, pdp->dtpd_name);

if (dtrace_probe_info(dtp, pdp, &p) == 0)
print_probe_info(&p);

*last = edp;
return (0);
}






static void
exec_prog(const dtrace_cmd_t *dcp)
{
dtrace_ecbdesc_t *last = NULL;
dtrace_proginfo_t dpi;

if (!g_exec) {
dtrace_program_info(g_dtp, dcp->dc_prog, &dpi);
} else if (dtrace_program_exec(g_dtp, dcp->dc_prog, &dpi) == -1) {
dfatal("failed to enable '%s'", dcp->dc_name);
} else {
notice("%s '%s' matched %u probe%s\n",
dcp->dc_desc, dcp->dc_name,
dpi.dpi_matches, dpi.dpi_matches == 1 ? "" : "s");
}

if (g_verbose) {
oprintf("\nStability attributes for %s %s:\n",
dcp->dc_desc, dcp->dc_name);

oprintf("\n\tMinimum Probe Description Attributes\n");
oprintf("\t\tIdentifier Names: %s\n",
dtrace_stability_name(dpi.dpi_descattr.dtat_name));
oprintf("\t\tData Semantics: %s\n",
dtrace_stability_name(dpi.dpi_descattr.dtat_data));
oprintf("\t\tDependency Class: %s\n",
dtrace_class_name(dpi.dpi_descattr.dtat_class));

oprintf("\n\tMinimum Statement Attributes\n");

oprintf("\t\tIdentifier Names: %s\n",
dtrace_stability_name(dpi.dpi_stmtattr.dtat_name));
oprintf("\t\tData Semantics: %s\n",
dtrace_stability_name(dpi.dpi_stmtattr.dtat_data));
oprintf("\t\tDependency Class: %s\n",
dtrace_class_name(dpi.dpi_stmtattr.dtat_class));

if (!g_exec) {
(void) dtrace_stmt_iter(g_dtp, dcp->dc_prog,
(dtrace_stmt_f *)info_stmt, &last);
} else
oprintf("\n");
}

g_total += dpi.dpi_matches;
}





static void
anon_prog(const dtrace_cmd_t *dcp, dof_hdr_t *dof, int n)
{
const uchar_t *p, *q;

if (dof == NULL)
dfatal("failed to create DOF image for '%s'", dcp->dc_name);

p = (uchar_t *)dof;
q = p + dof->dofh_filesz;

#if defined(__FreeBSD__)




oprintf("dof-data-%d=", n);

while (p < q)
oprintf("%02x", *p++);

oprintf("\n");
#else
oprintf("dof-data-%d=0x%x", n, *p++);

while (p < q)
oprintf(",0x%x", *p++);

oprintf(";\n");
#endif

dtrace_dof_destroy(g_dtp, dof);
}








static void
link_prog(dtrace_cmd_t *dcp)
{
char *p;

if (g_cmdc == 1 && g_ofile != NULL) {
(void) strlcpy(dcp->dc_ofile, g_ofile, sizeof (dcp->dc_ofile));
} else if ((p = strrchr(dcp->dc_arg, '.')) != NULL &&
strcmp(p, ".d") == 0) {
p[0] = '\0';
(void) snprintf(dcp->dc_ofile, sizeof (dcp->dc_ofile),
"%s.o", basename(dcp->dc_arg));
} else if (g_cmdc > 1) {
(void) snprintf(dcp->dc_ofile, sizeof (dcp->dc_ofile),
"d.out.%td", dcp - g_cmdv);
} else {
(void) snprintf(dcp->dc_ofile, sizeof (dcp->dc_ofile),
"d.out");
}

if (dtrace_program_link(g_dtp, dcp->dc_prog, DTRACE_D_PROBES,
dcp->dc_ofile, g_objc, g_objv) != 0)
dfatal("failed to link %s %s", dcp->dc_desc, dcp->dc_name);
}


static int
list_probe(dtrace_hdl_t *dtp, const dtrace_probedesc_t *pdp, void *arg)
{
dtrace_probeinfo_t p;

oprintf("%5d %10s %17s %33s %s\n", pdp->dtpd_id,
pdp->dtpd_provider, pdp->dtpd_mod, pdp->dtpd_func, pdp->dtpd_name);

if (g_verbose && dtrace_probe_info(dtp, pdp, &p) == 0)
print_probe_info(&p);

if (g_intr != 0)
return (1);

return (0);
}


static int
list_stmt(dtrace_hdl_t *dtp, dtrace_prog_t *pgp,
dtrace_stmtdesc_t *stp, dtrace_ecbdesc_t **last)
{
dtrace_ecbdesc_t *edp = stp->dtsd_ecbdesc;

if (edp == *last)
return (0);

if (dtrace_probe_iter(g_dtp, &edp->dted_probe, list_probe, NULL) != 0) {
error("failed to match %s:%s:%s:%s: %s\n",
edp->dted_probe.dtpd_provider, edp->dted_probe.dtpd_mod,
edp->dted_probe.dtpd_func, edp->dted_probe.dtpd_name,
dtrace_errmsg(dtp, dtrace_errno(dtp)));
}

*last = edp;
return (0);
}





static void
list_prog(const dtrace_cmd_t *dcp)
{
dtrace_ecbdesc_t *last = NULL;

(void) dtrace_stmt_iter(g_dtp, dcp->dc_prog,
(dtrace_stmt_f *)list_stmt, &last);
}

static void
compile_file(dtrace_cmd_t *dcp)
{
char *arg0;
FILE *fp;

if ((fp = fopen(dcp->dc_arg, "r")) == NULL)
fatal("failed to open %s", dcp->dc_arg);

arg0 = g_argv[0];
g_argv[0] = dcp->dc_arg;

if ((dcp->dc_prog = dtrace_program_fcompile(g_dtp, fp,
g_cflags, g_argc, g_argv)) == NULL)
dfatal("failed to compile script %s", dcp->dc_arg);

g_argv[0] = arg0;
(void) fclose(fp);

dcp->dc_desc = "script";
dcp->dc_name = dcp->dc_arg;
}

static void
compile_str(dtrace_cmd_t *dcp)
{
char *p;

if ((dcp->dc_prog = dtrace_program_strcompile(g_dtp, dcp->dc_arg,
dcp->dc_spec, g_cflags | DTRACE_C_PSPEC, g_argc, g_argv)) == NULL)
dfatal("invalid probe specifier %s", dcp->dc_arg);

if ((p = strpbrk(dcp->dc_arg, "{/;")) != NULL)
*p = '\0';

dcp->dc_desc = "description";
dcp->dc_name = dcp->dc_arg;
}


static void
prochandler(struct ps_prochandle *P, const char *msg, void *arg)
{
#if defined(illumos)
const psinfo_t *prp = Ppsinfo(P);
int pid = Pstatus(P)->pr_pid;
char name[SIG2STR_MAX];
#else
int wstatus = proc_getwstat(P);
int pid = proc_getpid(P);
#endif

if (msg != NULL) {
notice("pid %d: %s\n", pid, msg);
return;
}

#if defined(illumos)
switch (Pstate(P)) {
#else
switch (proc_state(P)) {
#endif
case PS_UNDEAD:
#if defined(illumos)








if (prp != NULL && WIFSIGNALED(prp->pr_wstat)) {
notice("pid %d terminated by %s\n", pid,
proc_signame(WTERMSIG(prp->pr_wstat),
name, sizeof (name)));
#else
if (WIFSIGNALED(wstatus)) {
notice("pid %d terminated by %d\n", pid,
WTERMSIG(wstatus));
#endif
#if defined(illumos)
} else if (prp != NULL && WEXITSTATUS(prp->pr_wstat) != 0) {
notice("pid %d exited with status %d\n",
pid, WEXITSTATUS(prp->pr_wstat));
#else
} else if (WEXITSTATUS(wstatus) != 0) {
notice("pid %d exited with status %d\n",
pid, WEXITSTATUS(wstatus));
#endif
} else {
notice("pid %d has exited\n", pid);
}
g_pslive--;
break;

case PS_LOST:
notice("pid %d exec'd a set-id or unobservable program\n", pid);
g_pslive--;
break;
}
}


static int
errhandler(const dtrace_errdata_t *data, void *arg)
{
error(data->dteda_msg);
return (DTRACE_HANDLE_OK);
}


static int
drophandler(const dtrace_dropdata_t *data, void *arg)
{
error(data->dtdda_msg);
return (DTRACE_HANDLE_OK);
}


static int
setopthandler(const dtrace_setoptdata_t *data, void *arg)
{
if (strcmp(data->dtsda_option, "quiet") == 0)
g_quiet = data->dtsda_newval != DTRACEOPT_UNSET;

if (strcmp(data->dtsda_option, "flowindent") == 0)
g_flowindent = data->dtsda_newval != DTRACEOPT_UNSET;

return (DTRACE_HANDLE_OK);
}

#define BUFDUMPHDR(hdr) (void) printf("%s: %s%s\n", g_pname, hdr, strlen(hdr) > 0 ? ":" : "");


#define BUFDUMPSTR(ptr, field) (void) printf("%s: %20s => ", g_pname, #field); if ((ptr)->field != NULL) { const char *c = (ptr)->field; (void) printf("\""); do { if (*c == '\n') { (void) printf("\\n"); continue; } (void) printf("%c", *c); } while (*c++ != '\0'); (void) printf("\"\n"); } else { (void) printf("<NULL>\n"); }

















#define BUFDUMPASSTR(ptr, field, str) (void) printf("%s: %20s => %s\n", g_pname, #field, str);


#define BUFDUMP(ptr, field) (void) printf("%s: %20s => %lld\n", g_pname, #field, (long long)(ptr)->field);



#define BUFDUMPPTR(ptr, field) (void) printf("%s: %20s => %s\n", g_pname, #field, (ptr)->field != NULL ? "<non-NULL>" : "<NULL>");




static int
bufhandler(const dtrace_bufdata_t *bufdata, void *arg)
{
const dtrace_aggdata_t *agg = bufdata->dtbda_aggdata;
const dtrace_recdesc_t *rec = bufdata->dtbda_recdesc;
const dtrace_probedesc_t *pd;
uint32_t flags = bufdata->dtbda_flags;
char buf[512], *c = buf, *end = c + sizeof (buf);
int i, printed;

struct {
const char *name;
uint32_t value;
} flagnames[] = {
{ "AGGVAL", DTRACE_BUFDATA_AGGVAL },
{ "AGGKEY", DTRACE_BUFDATA_AGGKEY },
{ "AGGFORMAT", DTRACE_BUFDATA_AGGFORMAT },
{ "AGGLAST", DTRACE_BUFDATA_AGGLAST },
{ "???", UINT32_MAX },
{ NULL }
};

if (bufdata->dtbda_probe != NULL) {
pd = bufdata->dtbda_probe->dtpda_pdesc;
} else if (agg != NULL) {
pd = agg->dtada_pdesc;
} else {
pd = NULL;
}

BUFDUMPHDR(">>> Called buffer handler");
BUFDUMPHDR("");

BUFDUMPHDR(" dtrace_bufdata");
BUFDUMPSTR(bufdata, dtbda_buffered);
BUFDUMPPTR(bufdata, dtbda_probe);
BUFDUMPPTR(bufdata, dtbda_aggdata);
BUFDUMPPTR(bufdata, dtbda_recdesc);

(void) snprintf(c, end - c, "0x%x ", bufdata->dtbda_flags);
c += strlen(c);

for (i = 0, printed = 0; flagnames[i].name != NULL; i++) {
if (!(flags & flagnames[i].value))
continue;

(void) snprintf(c, end - c,
"%s%s", printed++ ? " | " : "(", flagnames[i].name);
c += strlen(c);
flags &= ~flagnames[i].value;
}

if (printed)
(void) snprintf(c, end - c, ")");

BUFDUMPASSTR(bufdata, dtbda_flags, buf);
BUFDUMPHDR("");

if (pd != NULL) {
BUFDUMPHDR(" dtrace_probedesc");
BUFDUMPSTR(pd, dtpd_provider);
BUFDUMPSTR(pd, dtpd_mod);
BUFDUMPSTR(pd, dtpd_func);
BUFDUMPSTR(pd, dtpd_name);
BUFDUMPHDR("");
}

if (rec != NULL) {
BUFDUMPHDR(" dtrace_recdesc");
BUFDUMP(rec, dtrd_action);
BUFDUMP(rec, dtrd_size);

if (agg != NULL) {
uint8_t *data;
int lim = rec->dtrd_size;

(void) sprintf(buf, "%d (data: ", rec->dtrd_offset);
c = buf + strlen(buf);

if (lim > sizeof (uint64_t))
lim = sizeof (uint64_t);

data = (uint8_t *)agg->dtada_data + rec->dtrd_offset;

for (i = 0; i < lim; i++) {
(void) snprintf(c, end - c, "%s%02x",
i == 0 ? "" : " ", *data++);
c += strlen(c);
}

(void) snprintf(c, end - c,
"%s)", lim < rec->dtrd_size ? " ..." : "");
BUFDUMPASSTR(rec, dtrd_offset, buf);
} else {
BUFDUMP(rec, dtrd_offset);
}

BUFDUMPHDR("");
}

if (agg != NULL) {
dtrace_aggdesc_t *desc = agg->dtada_desc;

BUFDUMPHDR(" dtrace_aggdesc");
BUFDUMPSTR(desc, dtagd_name);
BUFDUMP(desc, dtagd_varid);
BUFDUMP(desc, dtagd_id);
BUFDUMP(desc, dtagd_nrecs);
BUFDUMPHDR("");
}

return (DTRACE_HANDLE_OK);
}


static int
chewrec(const dtrace_probedata_t *data, const dtrace_recdesc_t *rec, void *arg)
{
dtrace_actkind_t act;
uintptr_t addr;

if (rec == NULL) {




if (!g_quiet)
oprintf("\n");

return (DTRACE_CONSUME_NEXT);
}

act = rec->dtrd_action;
addr = (uintptr_t)data->dtpda_data;

if (act == DTRACEACT_EXIT) {
g_status = *((uint32_t *)addr);
return (DTRACE_CONSUME_NEXT);
}

return (DTRACE_CONSUME_THIS);
}


static int
chew(const dtrace_probedata_t *data, void *arg)
{
dtrace_probedesc_t *pd = data->dtpda_pdesc;
processorid_t cpu = data->dtpda_cpu;
static int heading;

if (g_impatient) {
g_newline = 0;
return (DTRACE_CONSUME_ABORT);
}

if (heading == 0) {
if (!g_flowindent) {
if (!g_quiet) {
oprintf("%3s %6s %32s\n",
"CPU", "ID", "FUNCTION:NAME");
}
} else {
oprintf("%3s %-41s\n", "CPU", "FUNCTION");
}
heading = 1;
}

if (!g_flowindent) {
if (!g_quiet) {
char name[DTRACE_FUNCNAMELEN + DTRACE_NAMELEN + 2];

(void) snprintf(name, sizeof (name), "%s:%s",
pd->dtpd_func, pd->dtpd_name);

oprintf("%3d %6d %32s ", cpu, pd->dtpd_id, name);
}
} else {
int indent = data->dtpda_indent;
char *name;
size_t len;

if (data->dtpda_flow == DTRACEFLOW_NONE) {
len = indent + DTRACE_FUNCNAMELEN + DTRACE_NAMELEN + 5;
name = alloca(len);
(void) snprintf(name, len, "%*s%s%s:%s", indent, "",
data->dtpda_prefix, pd->dtpd_func,
pd->dtpd_name);
} else {
len = indent + DTRACE_FUNCNAMELEN + 5;
name = alloca(len);
(void) snprintf(name, len, "%*s%s%s", indent, "",
data->dtpda_prefix, pd->dtpd_func);
}

oprintf("%3d %-41s ", cpu, name);
}

return (DTRACE_CONSUME_THIS);
}

static void
go(void)
{
int i;

struct {
char *name;
char *optname;
dtrace_optval_t val;
} bufs[] = {
{ "buffer size", "bufsize" },
{ "aggregation size", "aggsize" },
{ "speculation size", "specsize" },
{ "dynamic variable size", "dynvarsize" },
{ NULL }
}, rates[] = {
{ "cleaning rate", "cleanrate" },
{ "status rate", "statusrate" },
{ NULL }
};

for (i = 0; bufs[i].name != NULL; i++) {
if (dtrace_getopt(g_dtp, bufs[i].optname, &bufs[i].val) == -1)
fatal("couldn't get option %s", bufs[i].optname);
}

for (i = 0; rates[i].name != NULL; i++) {
if (dtrace_getopt(g_dtp, rates[i].optname, &rates[i].val) == -1)
fatal("couldn't get option %s", rates[i].optname);
}

if (dtrace_go(g_dtp) == -1)
dfatal("could not enable tracing");

for (i = 0; bufs[i].name != NULL; i++) {
dtrace_optval_t j = 0, mul = 10;
dtrace_optval_t nsize;

if (bufs[i].val == DTRACEOPT_UNSET)
continue;

(void) dtrace_getopt(g_dtp, bufs[i].optname, &nsize);

if (nsize == DTRACEOPT_UNSET || nsize == 0)
continue;

if (nsize >= bufs[i].val - sizeof (uint64_t))
continue;

for (; (INT64_C(1) << mul) <= nsize; j++, mul += 10)
continue;

if (!(nsize & ((INT64_C(1) << (mul - 10)) - 1))) {
error("%s lowered to %lld%c\n", bufs[i].name,
(long long)nsize >> (mul - 10), " kmgtpe"[j]);
} else {
error("%s lowered to %lld bytes\n", bufs[i].name,
(long long)nsize);
}
}

for (i = 0; rates[i].name != NULL; i++) {
dtrace_optval_t nval;
char *dir;

if (rates[i].val == DTRACEOPT_UNSET)
continue;

(void) dtrace_getopt(g_dtp, rates[i].optname, &nval);

if (nval == DTRACEOPT_UNSET || nval == 0)
continue;

if (rates[i].val == nval)
continue;

dir = nval > rates[i].val ? "reduced" : "increased";

if (nval <= NANOSEC && (NANOSEC % nval) == 0) {
error("%s %s to %lld hz\n", rates[i].name, dir,
(long long)NANOSEC / (long long)nval);
continue;
}

if ((nval % NANOSEC) == 0) {
error("%s %s to once every %lld seconds\n",
rates[i].name, dir,
(long long)nval / (long long)NANOSEC);
continue;
}

error("%s %s to once every %lld nanoseconds\n",
rates[i].name, dir, (long long)nval);
}
}


static void
intr(int signo)
{
if (!g_intr)
g_newline = 1;

if (g_intr++)
g_impatient = 1;
}

#if defined(__FreeBSD__)
static void
siginfo(int signo __unused)
{

g_siginfo++;
g_newline = 1;
}
#endif

static void
installsighands(void)
{
struct sigaction act, oact;

(void) sigemptyset(&act.sa_mask);
act.sa_flags = 0;
act.sa_handler = intr;

if (sigaction(SIGINT, NULL, &oact) == 0 && oact.sa_handler != SIG_IGN)
(void) sigaction(SIGINT, &act, NULL);

if (sigaction(SIGTERM, NULL, &oact) == 0 && oact.sa_handler != SIG_IGN)
(void) sigaction(SIGTERM, &act, NULL);

#if defined(__FreeBSD__)
if (sigaction(SIGPIPE, NULL, &oact) == 0 && oact.sa_handler != SIG_IGN)
(void) sigaction(SIGPIPE, &act, NULL);

if (sigaction(SIGUSR1, NULL, &oact) == 0 && oact.sa_handler != SIG_IGN)
(void) sigaction(SIGUSR1, &act, NULL);

act.sa_handler = siginfo;
if (sigaction(SIGINFO, NULL, &oact) == 0 && oact.sa_handler != SIG_IGN)
(void) sigaction(SIGINFO, &act, NULL);
#endif
}

int
main(int argc, char *argv[])
{
dtrace_bufdesc_t buf;
dtrace_status_t status[2];
dtrace_optval_t opt;
dtrace_cmd_t *dcp;

g_ofp = stdout;
int done = 0, mode = 0;
int err, i, c;
char *p, **v;
struct ps_prochandle *P;
pid_t pid;

#if defined(__FreeBSD__)

(void) setlocale(LC_NUMERIC, "");


(void) setlocale(LC_TIME, "");
#endif

g_pname = basename(argv[0]);

if (argc == 1)
return (usage(stderr));

if ((g_argv = malloc(sizeof (char *) * argc)) == NULL ||
(g_cmdv = malloc(sizeof (dtrace_cmd_t) * argc)) == NULL ||
(g_psv = malloc(sizeof (struct ps_prochandle *) * argc)) == NULL)
fatal("failed to allocate memory for arguments");

g_argv[g_argc++] = argv[0];
argv[0] = g_pname;

bzero(status, sizeof (status));
bzero(&buf, sizeof (buf));







for (optind = 1; optind < argc; optind++) {
while ((c = getopt(argc, argv, DTRACE_OPTSTR)) != -1) {
switch (c) {
case '3':
if (strcmp(optarg, "2") != 0) {
(void) fprintf(stderr,
"%s: illegal option -- 3%s\n",
argv[0], optarg);
return (usage(stderr));
}
g_oflags &= ~DTRACE_O_LP64;
g_oflags |= DTRACE_O_ILP32;
break;

case '6':
if (strcmp(optarg, "4") != 0) {
(void) fprintf(stderr,
"%s: illegal option -- 6%s\n",
argv[0], optarg);
return (usage(stderr));
}
g_oflags &= ~DTRACE_O_ILP32;
g_oflags |= DTRACE_O_LP64;
break;

case 'a':
g_grabanon++;
break;

case 'A':
g_mode = DMODE_ANON;
g_exec = 0;
mode++;
break;

case 'e':
g_exec = 0;
done = 1;
break;

case 'h':
g_mode = DMODE_HEADER;
g_oflags |= DTRACE_O_NODEV;
g_cflags |= DTRACE_C_ZDEFS;
g_exec = 0;
mode++;
break;

case 'G':
g_mode = DMODE_LINK;
g_oflags |= DTRACE_O_NODEV;
g_cflags |= DTRACE_C_ZDEFS;
g_exec = 0;
mode++;
break;

case 'l':
g_mode = DMODE_LIST;
g_cflags |= DTRACE_C_ZDEFS;
mode++;
break;

case 'V':
g_mode = DMODE_VERS;
mode++;
break;

default:
if (strchr(DTRACE_OPTSTR, c) == NULL)
return (usage(stderr));
}
}

if (optind < argc)
g_argv[g_argc++] = argv[optind];
}

if (mode > 1) {
(void) fprintf(stderr, "%s: only one of the [-AGhlV] options "
"can be specified at a time\n", g_pname);
return (E_USAGE);
}

if (g_mode == DMODE_VERS)
return (printf("%s: %s\n", g_pname, _dtrace_version) <= 0);







if (g_mode == DMODE_LINK &&
(g_oflags & (DTRACE_O_ILP32 | DTRACE_O_LP64)) == 0 &&
elf_version(EV_CURRENT) != EV_NONE) {
int fd;
Elf *elf;
GElf_Ehdr ehdr;

for (i = 1; i < g_argc; i++) {
if ((fd = open64(g_argv[i], O_RDONLY)) == -1)
break;

if ((elf = elf_begin(fd, ELF_C_READ, NULL)) == NULL) {
(void) close(fd);
break;
}

if (elf_kind(elf) != ELF_K_ELF ||
gelf_getehdr(elf, &ehdr) == NULL) {
(void) close(fd);
(void) elf_end(elf);
break;
}

(void) close(fd);
(void) elf_end(elf);

if (ehdr.e_ident[EI_CLASS] == ELFCLASS64) {
if (g_oflags & DTRACE_O_ILP32) {
fatal("can't mix 32-bit and 64-bit "
"object files\n");
}
g_oflags |= DTRACE_O_LP64;
} else if (ehdr.e_ident[EI_CLASS] == ELFCLASS32) {
if (g_oflags & DTRACE_O_LP64) {
fatal("can't mix 32-bit and 64-bit "
"object files\n");
}
g_oflags |= DTRACE_O_ILP32;
} else {
break;
}
}
}





while ((g_dtp = dtrace_open(DTRACE_VERSION, g_oflags, &err)) == NULL) {
if (!(g_oflags & DTRACE_O_NODEV) && !g_exec && !g_grabanon) {
g_oflags |= DTRACE_O_NODEV;
continue;
}

fatal("failed to initialize dtrace: %s\n",
dtrace_errmsg(NULL, err));
}

#if defined(__i386__)

(void) dtrace_setopt(g_dtp, "bufsize", "12m");
(void) dtrace_setopt(g_dtp, "aggsize", "12m");
#else
(void) dtrace_setopt(g_dtp, "bufsize", "4m");
(void) dtrace_setopt(g_dtp, "aggsize", "4m");
#endif
(void) dtrace_setopt(g_dtp, "temporal", "yes");







if (g_mode == DMODE_LINK) {
(void) dtrace_setopt(g_dtp, "linkmode", "dynamic");
(void) dtrace_setopt(g_dtp, "unodefs", NULL);





g_objc = g_argc - 1;
g_objv = g_argv + 1;




g_argc = 1;
} else if (g_mode == DMODE_ANON)
(void) dtrace_setopt(g_dtp, "linkmode", "primary");







for (optind = 1; optind < argc; optind++) {
while ((c = getopt(argc, argv, DTRACE_OPTSTR)) != -1) {
switch (c) {
case 'a':
if (dtrace_setopt(g_dtp, "grabanon", 0) != 0)
dfatal("failed to set -a");
break;

case 'b':
if (dtrace_setopt(g_dtp,
"bufsize", optarg) != 0)
dfatal("failed to set -b %s", optarg);
break;

case 'B':
g_ofp = NULL;
break;

case 'C':
g_cflags |= DTRACE_C_CPP;
break;

case 'D':
if (dtrace_setopt(g_dtp, "define", optarg) != 0)
dfatal("failed to set -D %s", optarg);
break;

case 'f':
dcp = &g_cmdv[g_cmdc++];
dcp->dc_func = compile_str;
dcp->dc_spec = DTRACE_PROBESPEC_FUNC;
dcp->dc_arg = optarg;
break;

case 'F':
if (dtrace_setopt(g_dtp, "flowindent", 0) != 0)
dfatal("failed to set -F");
break;

case 'H':
if (dtrace_setopt(g_dtp, "cpphdrs", 0) != 0)
dfatal("failed to set -H");
break;

case 'i':
dcp = &g_cmdv[g_cmdc++];
dcp->dc_func = compile_str;
dcp->dc_spec = DTRACE_PROBESPEC_NAME;
dcp->dc_arg = optarg;
break;

case 'I':
if (dtrace_setopt(g_dtp, "incdir", optarg) != 0)
dfatal("failed to set -I %s", optarg);
break;

case 'L':
if (dtrace_setopt(g_dtp, "libdir", optarg) != 0)
dfatal("failed to set -L %s", optarg);
break;

case 'm':
dcp = &g_cmdv[g_cmdc++];
dcp->dc_func = compile_str;
dcp->dc_spec = DTRACE_PROBESPEC_MOD;
dcp->dc_arg = optarg;
break;

case 'n':
dcp = &g_cmdv[g_cmdc++];
dcp->dc_func = compile_str;
dcp->dc_spec = DTRACE_PROBESPEC_NAME;
dcp->dc_arg = optarg;
break;

case 'P':
dcp = &g_cmdv[g_cmdc++];
dcp->dc_func = compile_str;
dcp->dc_spec = DTRACE_PROBESPEC_PROVIDER;
dcp->dc_arg = optarg;
break;

case 'q':
if (dtrace_setopt(g_dtp, "quiet", 0) != 0)
dfatal("failed to set -q");
break;

case 'o':
g_ofile = optarg;
break;

case 's':
dcp = &g_cmdv[g_cmdc++];
dcp->dc_func = compile_file;
dcp->dc_spec = DTRACE_PROBESPEC_NONE;
dcp->dc_arg = optarg;
break;

case 'S':
g_cflags |= DTRACE_C_DIFV;
break;

case 'U':
if (dtrace_setopt(g_dtp, "undef", optarg) != 0)
dfatal("failed to set -U %s", optarg);
break;

case 'v':
g_verbose++;
break;

case 'w':
if (dtrace_setopt(g_dtp, "destructive", 0) != 0)
dfatal("failed to set -w");
break;

case 'x':
if ((p = strchr(optarg, '=')) != NULL)
*p++ = '\0';

if (dtrace_setopt(g_dtp, optarg, p) != 0)
dfatal("failed to set -x %s", optarg);
break;

case 'X':
if (dtrace_setopt(g_dtp, "stdc", optarg) != 0)
dfatal("failed to set -X %s", optarg);
break;

case 'Z':
g_cflags |= DTRACE_C_ZDEFS;
break;

default:
if (strchr(DTRACE_OPTSTR, c) == NULL)
return (usage(stderr));
}
}
}

if (g_ofp == NULL && g_mode != DMODE_EXEC) {
(void) fprintf(stderr, "%s: -B not valid in combination"
" with [-AGl] options\n", g_pname);
return (E_USAGE);
}

if (g_ofp == NULL && g_ofile != NULL) {
(void) fprintf(stderr, "%s: -B not valid in combination"
" with -o option\n", g_pname);
return (E_USAGE);
}






for (optind = 1; optind < argc; optind++) {
while ((c = getopt(argc, argv, DTRACE_OPTSTR)) != -1) {
switch (c) {
case 'c':
if ((v = make_argv(optarg)) == NULL)
fatal("failed to allocate memory");

P = dtrace_proc_create(g_dtp, v[0], v, NULL, NULL);
if (P == NULL)
dfatal(NULL);

g_psv[g_psc++] = P;
free(v);
break;

case 'p':
errno = 0;
pid = strtol(optarg, &p, 10);

if (errno != 0 || p == optarg || p[0] != '\0')
fatal("invalid pid: %s\n", optarg);

P = dtrace_proc_grab(g_dtp, pid, 0);
if (P == NULL)
dfatal(NULL);

g_psv[g_psc++] = P;
break;
}
}
}





for (i = 0; i < g_cmdc; i++)
g_cmdv[i].dc_func(&g_cmdv[i]);

if (g_mode != DMODE_LIST) {
if (dtrace_handle_err(g_dtp, &errhandler, NULL) == -1)
dfatal("failed to establish error handler");

if (dtrace_handle_drop(g_dtp, &drophandler, NULL) == -1)
dfatal("failed to establish drop handler");

if (dtrace_handle_proc(g_dtp, &prochandler, NULL) == -1)
dfatal("failed to establish proc handler");

if (dtrace_handle_setopt(g_dtp, &setopthandler, NULL) == -1)
dfatal("failed to establish setopt handler");

if (g_ofp == NULL &&
dtrace_handle_buffered(g_dtp, &bufhandler, NULL) == -1)
dfatal("failed to establish buffered handler");
}

(void) dtrace_getopt(g_dtp, "flowindent", &opt);
g_flowindent = opt != DTRACEOPT_UNSET;

(void) dtrace_getopt(g_dtp, "grabanon", &opt);
g_grabanon = opt != DTRACEOPT_UNSET;

(void) dtrace_getopt(g_dtp, "quiet", &opt);
g_quiet = opt != DTRACEOPT_UNSET;








switch (g_mode) {
case DMODE_EXEC:
if (g_ofile != NULL && (g_ofp = fopen(g_ofile, "a")) == NULL)
fatal("failed to open output file '%s'", g_ofile);

for (i = 0; i < g_cmdc; i++)
exec_prog(&g_cmdv[i]);

if (done && !g_grabanon) {
dtrace_close(g_dtp);
return (g_status);
}
break;

case DMODE_ANON:
if (g_ofile == NULL)
#if defined(illumos)
g_ofile = "/kernel/drv/dtrace.conf";
#else




g_ofile = "/boot/dtrace.dof";
#endif

dof_prune(g_ofile);
#if defined(illumos)
etcsystem_prune();
#endif

if (g_cmdc == 0) {
dtrace_close(g_dtp);
return (g_status);
}

if ((g_ofp = fopen(g_ofile, "a")) == NULL)
fatal("failed to open output file '%s'", g_ofile);

for (i = 0; i < g_cmdc; i++) {
anon_prog(&g_cmdv[i],
dtrace_dof_create(g_dtp, g_cmdv[i].dc_prog, 0), i);
}





anon_prog(NULL, dtrace_geterr_dof(g_dtp), i++);
anon_prog(NULL, dtrace_getopt_dof(g_dtp), i++);

if (fclose(g_ofp) == EOF)
fatal("failed to close output file '%s'", g_ofile);






error("saved anonymous enabling in %s\n", g_ofile);

#if defined(__FreeBSD__)
bootdof_add();
#else
etcsystem_add();
error("run update_drv(1M) or reboot to enable changes\n");
#endif

dtrace_close(g_dtp);
return (g_status);

case DMODE_LINK:
if (g_cmdc == 0) {
(void) fprintf(stderr, "%s: -G requires one or more "
"scripts or enabling options\n", g_pname);
dtrace_close(g_dtp);
return (E_USAGE);
}

for (i = 0; i < g_cmdc; i++)
link_prog(&g_cmdv[i]);

if (g_cmdc > 1 && g_ofile != NULL) {
char **objv = alloca(g_cmdc * sizeof (char *));

for (i = 0; i < g_cmdc; i++)
objv[i] = g_cmdv[i].dc_ofile;

if (dtrace_program_link(g_dtp, NULL, DTRACE_D_PROBES,
g_ofile, g_cmdc, objv) != 0)
dfatal(NULL);
}

dtrace_close(g_dtp);
return (g_status);

case DMODE_LIST:
if (g_ofile != NULL && (g_ofp = fopen(g_ofile, "a")) == NULL)
fatal("failed to open output file '%s'", g_ofile);

installsighands();

oprintf("%5s %10s %17s %33s %s\n",
"ID", "PROVIDER", "MODULE", "FUNCTION", "NAME");

for (i = 0; i < g_cmdc; i++)
list_prog(&g_cmdv[i]);

if (g_cmdc == 0)
(void) dtrace_probe_iter(g_dtp, NULL, list_probe, NULL);

dtrace_close(g_dtp);
return (g_status);

case DMODE_HEADER:
if (g_cmdc == 0) {
(void) fprintf(stderr, "%s: -h requires one or more "
"scripts or enabling options\n", g_pname);
dtrace_close(g_dtp);
return (E_USAGE);
}

if (g_ofile == NULL) {
char *p;

if (g_cmdc > 1) {
(void) fprintf(stderr, "%s: -h requires an "
"output file if multiple scripts are "
"specified\n", g_pname);
dtrace_close(g_dtp);
return (E_USAGE);
}

if ((p = strrchr(g_cmdv[0].dc_arg, '.')) == NULL ||
strcmp(p, ".d") != 0) {
(void) fprintf(stderr, "%s: -h requires an "
"output file if no scripts are "
"specified\n", g_pname);
dtrace_close(g_dtp);
return (E_USAGE);
}

p[0] = '\0';
g_ofile = p = g_cmdv[0].dc_ofile;
(void) snprintf(p, sizeof (g_cmdv[0].dc_ofile),
"%s.h", basename(g_cmdv[0].dc_arg));
}

if ((g_ofp = fopen(g_ofile, "w")) == NULL)
fatal("failed to open header file '%s'", g_ofile);

oprintf("/*\n * Generated by dtrace(1M).\n */\n\n");

if (dtrace_program_header(g_dtp, g_ofp, g_ofile) != 0 ||
fclose(g_ofp) == EOF)
dfatal("failed to create header file %s", g_ofile);

dtrace_close(g_dtp);
return (g_status);
}





if (g_total == 0 && !g_grabanon && !(g_cflags & DTRACE_C_ZDEFS))
dfatal("no probes %s\n", g_cmdc ? "matched" : "specified");





go();

(void) dtrace_getopt(g_dtp, "flowindent", &opt);
g_flowindent = opt != DTRACEOPT_UNSET;

(void) dtrace_getopt(g_dtp, "grabanon", &opt);
g_grabanon = opt != DTRACEOPT_UNSET;

(void) dtrace_getopt(g_dtp, "quiet", &opt);
g_quiet = opt != DTRACEOPT_UNSET;

(void) dtrace_getopt(g_dtp, "destructive", &opt);
if (opt != DTRACEOPT_UNSET)
notice("allowing destructive actions\n");

installsighands();






for (i = 0; i < g_psc; i++)
dtrace_proc_continue(g_dtp, g_psv[i]);

g_pslive = g_psc;

do {
if (!g_intr && !done)
dtrace_sleep(g_dtp);

#if defined(__FreeBSD__)
if (g_siginfo) {
(void)dtrace_aggregate_print(g_dtp, g_ofp, NULL);
g_siginfo = 0;
}
#endif

if (g_newline) {





oprintf("\n");
g_newline = 0;
}

if (done || g_intr || (g_psc != 0 && g_pslive == 0)) {
done = 1;
if (dtrace_stop(g_dtp) == -1)
dfatal("couldn't stop tracing");
}

switch (dtrace_work(g_dtp, g_ofp, chew, chewrec, NULL)) {
case DTRACE_WORKSTATUS_DONE:
done = 1;
break;
case DTRACE_WORKSTATUS_OKAY:
break;
default:
if (!g_impatient && dtrace_errno(g_dtp) != EINTR)
dfatal("processing aborted");
}

if (g_ofp != NULL && fflush(g_ofp) == EOF)
clearerr(g_ofp);
} while (!done);

oprintf("\n");

if (!g_impatient) {
if (dtrace_aggregate_print(g_dtp, g_ofp, NULL) == -1 &&
dtrace_errno(g_dtp) != EINTR)
dfatal("failed to print aggregations");
}

dtrace_close(g_dtp);
return (g_status);
}
