























#if defined(TINYTEST_LOCAL)
#include "tinytest_local.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if !defined(NO_FORKING)

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#if defined(__APPLE__) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#if (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060 && __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1070)


#define FORK_BREAKS_GCOV
#include <vproc.h>
#endif
#endif

#endif

#if !defined(__GNUC__)
#define __attribute__(x)
#endif

#include "tinytest.h"
#include "tinytest_macros.h"

#define LONGEST_TEST_NAME 16384

static int in_tinytest_main = 0;
static int n_ok = 0;
static int n_bad = 0;
static int n_skipped = 0;

static int opt_forked = 0;
static int opt_nofork = 0;
static int opt_verbosity = 1;
const char *verbosity_flag = "";

const struct testlist_alias_t *cfg_aliases=NULL;

enum outcome { SKIP=2, OK=1, FAIL=0 };
static enum outcome cur_test_outcome = 0;
const char *cur_test_prefix = NULL;

const char *cur_test_name = NULL;

#if defined(_WIN32)

static char commandname[MAX_PATH+1];
#endif

static void usage(struct testgroup_t *groups, int list_groups)
__attribute__((noreturn));
static int process_test_option(struct testgroup_t *groups, const char *test);

static enum outcome
testcase_run_bare_(const struct testcase_t *testcase)
{
void *env = NULL;
int outcome;
if (testcase->setup) {
env = testcase->setup->setup_fn(testcase);
if (!env)
return FAIL;
else if (env == (void*)TT_SKIP)
return SKIP;
}

cur_test_outcome = OK;
testcase->fn(env);
outcome = cur_test_outcome;

if (testcase->setup) {
if (testcase->setup->cleanup_fn(testcase, env) == 0)
outcome = FAIL;
}

return outcome;
}

#define MAGIC_EXITCODE 42

#if !defined(NO_FORKING)

static enum outcome
testcase_run_forked_(const struct testgroup_t *group,
const struct testcase_t *testcase)
{
#if defined(_WIN32)








int ok;
char buffer[LONGEST_TEST_NAME+256];
STARTUPINFOA si;
PROCESS_INFORMATION info;
DWORD exitcode;

if (!in_tinytest_main) {
printf("\nERROR. On Windows, testcase_run_forked_ must be"
" called from within tinytest_main.\n");
abort();
}
if (opt_verbosity>0)
printf("[forking] ");

snprintf(buffer, sizeof(buffer), "%s --RUNNING-FORKED %s %s%s",
commandname, verbosity_flag, group->prefix, testcase->name);

memset(&si, 0, sizeof(si));
memset(&info, 0, sizeof(info));
si.cb = sizeof(si);

ok = CreateProcessA(commandname, buffer, NULL, NULL, 0,
0, NULL, NULL, &si, &info);
if (!ok) {
printf("CreateProcess failed!\n");
return 0;
}
WaitForSingleObject(info.hProcess, INFINITE);
GetExitCodeProcess(info.hProcess, &exitcode);
CloseHandle(info.hProcess);
CloseHandle(info.hThread);
if (exitcode == 0)
return OK;
else if (exitcode == MAGIC_EXITCODE)
return SKIP;
else
return FAIL;
#else
int outcome_pipe[2];
pid_t pid;
(void)group;

if (pipe(outcome_pipe))
perror("opening pipe");

if (opt_verbosity>0)
printf("[forking] ");
pid = fork();
#if defined(FORK_BREAKS_GCOV)
vproc_transaction_begin(0);
#endif
if (!pid) {

int test_r, write_r;
char b[1];
close(outcome_pipe[0]);
test_r = testcase_run_bare_(testcase);
assert(0<=(int)test_r && (int)test_r<=2);
b[0] = "NYS"[test_r];
write_r = (int)write(outcome_pipe[1], b, 1);
if (write_r != 1) {
perror("write outcome to pipe");
exit(1);
}
exit(0);
return FAIL;
} else {

int status, r;
char b[1];


close(outcome_pipe[1]);
r = (int)read(outcome_pipe[0], b, 1);
if (r == 0) {
printf("[Lost connection!] ");
return 0;
} else if (r != 1) {
perror("read outcome from pipe");
}
waitpid(pid, &status, 0);
close(outcome_pipe[0]);
return b[0]=='Y' ? OK : (b[0]=='S' ? SKIP : FAIL);
}
#endif
}

#endif

int
testcase_run_one(const struct testgroup_t *group,
const struct testcase_t *testcase)
{
enum outcome outcome;

if (testcase->flags & (TT_SKIP|TT_OFF_BY_DEFAULT)) {
if (opt_verbosity>0)
printf("%s%s: %s\n",
group->prefix, testcase->name,
(testcase->flags & TT_SKIP) ? "SKIPPED" : "DISABLED");
++n_skipped;
return SKIP;
}

if (opt_verbosity>0 && !opt_forked) {
printf("%s%s: ", group->prefix, testcase->name);
} else {
if (opt_verbosity==0) printf(".");
cur_test_prefix = group->prefix;
cur_test_name = testcase->name;
}

#if !defined(NO_FORKING)
if ((testcase->flags & TT_FORK) && !(opt_forked||opt_nofork)) {
outcome = testcase_run_forked_(group, testcase);
} else {
#else
{
#endif
outcome = testcase_run_bare_(testcase);
}

if (outcome == OK) {
++n_ok;
if (opt_verbosity>0 && !opt_forked)
puts(opt_verbosity==1?"OK":"");
} else if (outcome == SKIP) {
++n_skipped;
if (opt_verbosity>0 && !opt_forked)
puts("SKIPPED");
} else {
++n_bad;
if (!opt_forked)
printf("\n [%s FAILED]\n", testcase->name);
}

if (opt_forked) {
exit(outcome==OK ? 0 : (outcome==SKIP?MAGIC_EXITCODE : 1));
return 1;
} else {
return (int)outcome;
}
}

int
tinytest_set_flag_(struct testgroup_t *groups, const char *arg, int set, unsigned long flag)
{
int i, j;
size_t length = LONGEST_TEST_NAME;
char fullname[LONGEST_TEST_NAME];
int found=0;
if (strstr(arg, ".."))
length = strstr(arg,"..")-arg;
for (i=0; groups[i].prefix; ++i) {
for (j=0; groups[i].cases[j].name; ++j) {
struct testcase_t *testcase = &groups[i].cases[j];
snprintf(fullname, sizeof(fullname), "%s%s",
groups[i].prefix, testcase->name);
if (!flag) {
printf(" %s", fullname);
if (testcase->flags & TT_OFF_BY_DEFAULT)
puts(" (Off by default)");
else if (testcase->flags & TT_SKIP)
puts(" (DISABLED)");
else
puts("");
}
if (!strncmp(fullname, arg, length)) {
if (set)
testcase->flags |= flag;
else
testcase->flags &= ~flag;
++found;
}
}
}
return found;
}

static void
usage(struct testgroup_t *groups, int list_groups)
{
puts("Options are: [--verbose|--quiet|--terse] [--no-fork]");
puts(" Specify tests by name, or using a prefix ending with '..'");
puts(" To skip a test, prefix its name with a colon.");
puts(" To enable a disabled test, prefix its name with a plus.");
puts(" Use --list-tests for a list of tests.");
if (list_groups) {
puts("Known tests are:");
tinytest_set_flag_(groups, "..", 1, 0);
}
exit(0);
}

static int
process_test_alias(struct testgroup_t *groups, const char *test)
{
int i, j, n, r;
for (i=0; cfg_aliases && cfg_aliases[i].name; ++i) {
if (!strcmp(cfg_aliases[i].name, test)) {
n = 0;
for (j = 0; cfg_aliases[i].tests[j]; ++j) {
r = process_test_option(groups, cfg_aliases[i].tests[j]);
if (r<0)
return -1;
n += r;
}
return n;
}
}
printf("No such test alias as @%s!",test);
return -1;
}

static int
process_test_option(struct testgroup_t *groups, const char *test)
{
int flag = TT_ENABLED_;
int n = 0;
if (test[0] == '@') {
return process_test_alias(groups, test + 1);
} else if (test[0] == ':') {
++test;
flag = TT_SKIP;
} else if (test[0] == '+') {
++test;
++n;
if (!tinytest_set_flag_(groups, test, 0, TT_OFF_BY_DEFAULT)) {
printf("No such test as %s!\n", test);
return -1;
}
} else {
++n;
}
if (!tinytest_set_flag_(groups, test, 1, flag)) {
printf("No such test as %s!\n", test);
return -1;
}
return n;
}

void
tinytest_set_aliases(const struct testlist_alias_t *aliases)
{
cfg_aliases = aliases;
}

int
tinytest_main(int c, const char **v, struct testgroup_t *groups)
{
int i, j, n=0;

#if defined(_WIN32)
const char *sp = strrchr(v[0], '.');
const char *extension = "";
if (!sp || stricmp(sp, ".exe"))
extension = ".exe";
snprintf(commandname, sizeof(commandname), "%s%s", v[0], extension);
commandname[MAX_PATH]='\0';
#endif
for (i=1; i<c; ++i) {
if (v[i][0] == '-') {
if (!strcmp(v[i], "--RUNNING-FORKED")) {
opt_forked = 1;
} else if (!strcmp(v[i], "--no-fork")) {
opt_nofork = 1;
} else if (!strcmp(v[i], "--quiet")) {
opt_verbosity = -1;
verbosity_flag = "--quiet";
} else if (!strcmp(v[i], "--verbose")) {
opt_verbosity = 2;
verbosity_flag = "--verbose";
} else if (!strcmp(v[i], "--terse")) {
opt_verbosity = 0;
verbosity_flag = "--terse";
} else if (!strcmp(v[i], "--help")) {
usage(groups, 0);
} else if (!strcmp(v[i], "--list-tests")) {
usage(groups, 1);
} else {
printf("Unknown option %s. Try --help\n",v[i]);
return -1;
}
} else {
int r = process_test_option(groups, v[i]);
if (r<0)
return -1;
n += r;
}
}
if (!n)
tinytest_set_flag_(groups, "..", 1, TT_ENABLED_);

#if defined(_IONBF)
setvbuf(stdout, NULL, _IONBF, 0);
#endif

++in_tinytest_main;
for (i=0; groups[i].prefix; ++i)
for (j=0; groups[i].cases[j].name; ++j)
if (groups[i].cases[j].flags & TT_ENABLED_)
testcase_run_one(&groups[i],
&groups[i].cases[j]);

--in_tinytest_main;

if (opt_verbosity==0)
puts("");

if (n_bad)
printf("%d/%d TESTS FAILED. (%d skipped)\n", n_bad,
n_bad+n_ok,n_skipped);
else if (opt_verbosity >= 1)
printf("%d tests ok. (%d skipped)\n", n_ok, n_skipped);

return (n_bad == 0) ? 0 : 1;
}

int
tinytest_get_verbosity_(void)
{
return opt_verbosity;
}

void
tinytest_set_test_failed_(void)
{
if (opt_verbosity <= 0 && cur_test_name) {
if (opt_verbosity==0) puts("");
printf("%s%s: ", cur_test_prefix, cur_test_name);
cur_test_name = NULL;
}
cur_test_outcome = 0;
}

void
tinytest_set_test_skipped_(void)
{
if (cur_test_outcome==OK)
cur_test_outcome = SKIP;
}

char *
tinytest_format_hex_(const void *val_, unsigned long len)
{
const unsigned char *val = val_;
char *result, *cp;
size_t i;

if (!val)
return strdup("null");
if (!(result = malloc(len*2+1)))
return strdup("<allocation failure>");
cp = result;
for (i=0;i<len;++i) {
*cp++ = "0123456789ABCDEF"[val[i] >> 4];
*cp++ = "0123456789ABCDEF"[val[i] & 0x0f];
}
*cp = 0;
return result;
}
