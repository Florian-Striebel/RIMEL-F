#include "test/jemalloc_test.h"
#if !defined(_WIN32)
#include <sys/wait.h>
#endif
#if !defined(_WIN32)
static void
wait_for_child_exit(int pid) {
int status;
while (true) {
if (waitpid(pid, &status, 0) == -1) {
test_fail("Unexpected waitpid() failure.");
}
if (WIFSIGNALED(status)) {
test_fail("Unexpected child termination due to "
"signal %d", WTERMSIG(status));
break;
}
if (WIFEXITED(status)) {
if (WEXITSTATUS(status) != 0) {
test_fail("Unexpected child exit value %d",
WEXITSTATUS(status));
}
break;
}
}
}
#endif
TEST_BEGIN(test_fork) {
#if !defined(_WIN32)
void *p;
pid_t pid;
unsigned arena_ind;
size_t sz = sizeof(unsigned);
assert_d_eq(mallctl("arenas.create", (void *)&arena_ind, &sz, NULL, 0),
0, "Unexpected mallctl() failure");
unsigned old_arena_ind;
sz = sizeof(old_arena_ind);
assert_d_eq(mallctl("thread.arena", (void *)&old_arena_ind, &sz,
(void *)&arena_ind, sizeof(arena_ind)), 0,
"Unexpected mallctl() failure");
p = malloc(1);
assert_ptr_not_null(p, "Unexpected malloc() failure");
pid = fork();
free(p);
p = malloc(64);
assert_ptr_not_null(p, "Unexpected malloc() failure");
free(p);
if (pid == -1) {
test_fail("Unexpected fork() failure");
} else if (pid == 0) {
_exit(0);
} else {
wait_for_child_exit(pid);
}
#else
test_skip("fork(2) is irrelevant to Windows");
#endif
}
TEST_END
#if !defined(_WIN32)
static void *
do_fork_thd(void *arg) {
malloc(1);
int pid = fork();
if (pid == -1) {
test_fail("Unexpected fork() failure");
} else if (pid == 0) {
char *args[] = {"true", NULL};
execvp(args[0], args);
test_fail("Exec failed");
} else {
wait_for_child_exit(pid);
}
return NULL;
}
#endif
#if !defined(_WIN32)
static void
do_test_fork_multithreaded() {
thd_t child;
thd_create(&child, do_fork_thd, NULL);
do_fork_thd(NULL);
thd_join(child, NULL);
}
#endif
TEST_BEGIN(test_fork_multithreaded) {
#if !defined(_WIN32)
for (int i = 0; i < 100; i++) {
int pid = fork();
if (pid == -1) {
test_fail("Unexpected fork() failure,");
} else if (pid == 0) {
do_test_fork_multithreaded();
_exit(0);
} else {
wait_for_child_exit(pid);
}
}
#else
test_skip("fork(2) is irrelevant to Windows");
#endif
}
TEST_END
int
main(void) {
return test_no_reentrancy(
test_fork,
test_fork_multithreaded);
}
