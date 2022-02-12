































#if !defined(TSAN_FD_H)
#define TSAN_FD_H

#include "tsan_rtl.h"

namespace __tsan {

void FdInit();
void FdAcquire(ThreadState *thr, uptr pc, int fd);
void FdRelease(ThreadState *thr, uptr pc, int fd);
void FdAccess(ThreadState *thr, uptr pc, int fd);
void FdClose(ThreadState *thr, uptr pc, int fd, bool write = true);
void FdFileCreate(ThreadState *thr, uptr pc, int fd);
void FdDup(ThreadState *thr, uptr pc, int oldfd, int newfd, bool write);
void FdPipeCreate(ThreadState *thr, uptr pc, int rfd, int wfd);
void FdEventCreate(ThreadState *thr, uptr pc, int fd);
void FdSignalCreate(ThreadState *thr, uptr pc, int fd);
void FdInotifyCreate(ThreadState *thr, uptr pc, int fd);
void FdPollCreate(ThreadState *thr, uptr pc, int fd);
void FdSocketCreate(ThreadState *thr, uptr pc, int fd);
void FdSocketAccept(ThreadState *thr, uptr pc, int fd, int newfd);
void FdSocketConnecting(ThreadState *thr, uptr pc, int fd);
void FdSocketConnect(ThreadState *thr, uptr pc, int fd);
bool FdLocation(uptr addr, int *fd, int *tid, u32 *stack);
void FdOnFork(ThreadState *thr, uptr pc);

uptr File2addr(const char *path);
uptr Dir2addr(const char *path);

}

#endif
