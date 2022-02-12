
#if !defined(CAPSICUM_TEST_H)
#define CAPSICUM_TEST_H

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>

#include <ios>
#include <ostream>
#include <string>

#include "gtest/gtest.h"

extern bool verbose;
extern std::string tmpdir;
extern bool tmpdir_on_tmpfs;
extern bool force_mt;
extern bool force_nofork;
extern uid_t other_uid;

static inline void *WaitingThreadFn(void *) {

while (true) {
usleep(10000);
pthread_testcancel();
}
return NULL;
}



template <typename T, typename Function>
void MaybeRunWithThread(T *self, Function fn) {
pthread_t subthread;
if (force_mt) {
pthread_create(&subthread, NULL, WaitingThreadFn, NULL);
}
(self->*fn)();
if (force_mt) {
pthread_cancel(subthread);
pthread_join(subthread, NULL);
}
}
template <typename Function>
void MaybeRunWithThread(Function fn) {
pthread_t subthread;
if (force_mt) {
pthread_create(&subthread, NULL, WaitingThreadFn, NULL);
}
(fn)();
if (force_mt) {
pthread_cancel(subthread);
pthread_join(subthread, NULL);
}
}




const char *TmpFile(const char *pathname);






#define _RUN_FORKED(INNERCODE, TESTCASENAME, TESTNAME) pid_t pid = force_nofork ? 0 : fork(); if (pid == 0) { INNERCODE; if (!force_nofork) { exit(HasFailure()); } } else if (pid > 0) { int rc, status; int remaining_us = 30000000; while (remaining_us > 0) { status = 0; rc = waitpid(pid, &status, WNOHANG); if (rc != 0) break; remaining_us -= 10000; usleep(10000); } if (remaining_us <= 0) { fprintf(stderr, "Warning: killing unresponsive test " "%s.%s (pid %d)\n", TESTCASENAME, TESTNAME, pid); kill(pid, SIGKILL); ADD_FAILURE() << "Test hung"; } else if (rc < 0) { fprintf(stderr, "Warning: waitpid error %s (%d)\n", strerror(errno), errno); ADD_FAILURE() << "Failed to wait for child"; } else { int rc = WIFEXITED(status) ? WEXITSTATUS(status) : -1; EXPECT_EQ(0, rc); } }































#define _RUN_FORKED_MEM(THIS, TESTFN, TESTCASENAME, TESTNAME) _RUN_FORKED(MaybeRunWithThread(THIS, &TESTFN), TESTCASENAME, TESTNAME);

#define _RUN_FORKED_FN(TESTFN, TESTCASENAME, TESTNAME) _RUN_FORKED(MaybeRunWithThread(&TESTFN), TESTCASENAME, TESTNAME);




#define FORK_TEST_ON(test_case_name, test_name, test_file) static void test_case_name##_##test_name##_ForkTest(); TEST(test_case_name, test_name ##Forked) { _RUN_FORKED_FN(test_case_name##_##test_name##_ForkTest, #test_case_name, #test_name); const char *filename = test_file; if (filename) unlink(filename); } static void test_case_name##_##test_name##_ForkTest()









#define FORK_TEST(test_case_name, test_name) FORK_TEST_ON(test_case_name, test_name, NULL)



#define ICLASS_NAME(test_case_name, test_name) Forked##test_case_name##_##test_name
#define FORK_TEST_F(test_case_name, test_name) class ICLASS_NAME(test_case_name, test_name) : public test_case_name { public: ICLASS_NAME(test_case_name, test_name)() {} void InnerTestBody(); }; TEST_F(ICLASS_NAME(test_case_name, test_name), _) { _RUN_FORKED_MEM(this, ICLASS_NAME(test_case_name, test_name)::InnerTestBody, #test_case_name, #test_name); } void ICLASS_NAME(test_case_name, test_name)::InnerTestBody()













#define EXPECT_OK(v) EXPECT_LE(0, v) << " errno " << errno << " " << strerror(errno)


#define EXPECT_SYSCALL_FAIL(E, C) do { SCOPED_TRACE(#C); EXPECT_GT(0, C); EXPECT_EQ(E, errno) << "expected '" << strerror(E) << "' but got '" << strerror(errno) << "'"; } while (0)








#define EXPECT_SYSCALL_FAIL_NOT(E, C) do { EXPECT_GT(0, C); EXPECT_NE(E, errno) << strerror(E); } while (0)






#define EXPECT_VOID_SYSCALL_FAIL_NOT(E, C) do { errno = 0; C; EXPECT_NE(E, errno) << #C << " failed with ECAPMODE"; } while (0)








#if defined(O_BENEATH)
#define EXPECT_OPENAT_FAIL_TRAVERSAL(fd, path, flags) do { SCOPED_TRACE(GTEST_STRINGIFY_(openat((fd), (path), (flags)))); const int result = openat((fd), (path), (flags)); if (((flags) & O_BENEATH) == O_BENEATH) { EXPECT_SYSCALL_FAIL(E_NO_TRAVERSE_O_BENEATH, result); } else { EXPECT_SYSCALL_FAIL(E_NO_TRAVERSE_CAPABILITY, result); } if (result >= 0) { close(result); } } while (0)










#else
#define EXPECT_OPENAT_FAIL_TRAVERSAL(fd, path, flags) do { SCOPED_TRACE(GTEST_STRINGIFY_(openat((fd), (path), (flags)))); const int result = openat((fd), (path), (flags)); EXPECT_SYSCALL_FAIL(E_NO_TRAVERSE_CAPABILITY, result); if (result >= 0) { close(result); } } while (0)






#endif


#define EXPECT_CAPMODE(C) EXPECT_SYSCALL_FAIL(ECAPMODE, C)


#define EXPECT_FAIL_NOT_CAPMODE(C) EXPECT_SYSCALL_FAIL_NOT(ECAPMODE, C)
#define EXPECT_FAIL_VOID_NOT_CAPMODE(C) EXPECT_VOID_SYSCALL_FAIL_NOT(ECAPMODE, C)


#define EXPECT_NOTCAPABLE(C) EXPECT_SYSCALL_FAIL(ENOTCAPABLE, C)


#define EXPECT_FAIL_NOT_NOTCAPABLE(C) EXPECT_SYSCALL_FAIL_NOT(ENOTCAPABLE, C)


#define EXPECT_CAPFAIL(C) do { int rc = C; EXPECT_GT(0, rc); EXPECT_TRUE(errno == ECAPMODE || errno == ENOTCAPABLE) << #C << " did not fail with ECAPMODE/ENOTCAPABLE but " << errno << "(" << strerror(errno) << ")"; } while (0)









#define EXPECT_RIGHTS_IN(rights, max) EXPECT_TRUE(cap_rights_contains((max), (rights))) << "rights " << std::hex << *(rights) << " not a subset of " << std::hex << *(max)





#define EXPECT_RIGHTS_EQ(a, b) do { EXPECT_RIGHTS_IN((a), (b)); EXPECT_RIGHTS_IN((b), (a)); } while (0)












char ProcessState(int pid);



#define EXPECT_PID_REACHES_STATES(pid, expected1, expected2) { int counter = 5; char state; do { state = ProcessState(pid); if (state == expected1 || state == expected2) break; usleep(100000); } while (--counter > 0); EXPECT_TRUE(state == expected1 || state == expected2) << " pid " << pid << " in state " << state; }











#define EXPECT_PID_ALIVE(pid) EXPECT_PID_REACHES_STATES(pid, 'R', 'S')
#define EXPECT_PID_DEAD(pid) EXPECT_PID_REACHES_STATES(pid, 'Z', '\0')
#define EXPECT_PID_ZOMBIE(pid) EXPECT_PID_REACHES_STATES(pid, 'Z', 'Z');
#define EXPECT_PID_GONE(pid) EXPECT_PID_REACHES_STATES(pid, '\0', '\0');

enum {

MSG_CHILD_STARTED = 1234,
MSG_CHILD_FD_RECEIVED = 4321,

MSG_PARENT_REQUEST_CHILD_EXIT = 9999,
MSG_PARENT_CLOSED_FD = 10000,
MSG_PARENT_CHILD_SHOULD_RUN = 10001,
};

#define SEND_INT_MESSAGE(fd, message) do { int _msg = message; EXPECT_EQ(sizeof(_msg), (size_t)write(fd, &_msg, sizeof(_msg))); } while (0)





#define AWAIT_INT_MESSAGE(fd, expected) do { int _msg = 0; EXPECT_EQ(sizeof(_msg), (size_t)read(fd, &_msg, sizeof(_msg))); EXPECT_EQ(expected, _msg); } while (0)







#define GTEST_SKIP_IF_NOT_ROOT() if (getuid() != 0) { GTEST_SKIP() << "requires root"; }


extern std::string capsicum_test_bindir;

#endif
