







#if !defined(PROFILE_INSTRPROFILINGUTIL_H)
#define PROFILE_INSTRPROFILINGUTIL_H

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>


void __llvm_profile_recursive_mkdir(char *Pathname);


void __llvm_profile_set_dir_mode(unsigned Mode);


unsigned __llvm_profile_get_dir_mode(void);

int lprofLockFd(int fd);
int lprofUnlockFd(int fd);
int lprofLockFileHandle(FILE *F);
int lprofUnlockFileHandle(FILE *F);




FILE *lprofOpenFileEx(const char *Filename);

#if __ORBIS__
#include <sys/types.h>
static inline char *getenv(const char *name) { return NULL; }
static inline int setenv(const char *name, const char *value, int overwrite)
{ return 0; }
static pid_t fork() { return -1; }
#endif






const char *lprofGetPathPrefix(int *PrefixStrip, size_t *PrefixLen);





void lprofApplyPathPrefix(char *Dest, const char *PathStr, const char *Prefix,
size_t PrefixLen, int PrefixStrip);



const char *lprofFindFirstDirSeparator(const char *Path);


const char *lprofFindLastDirSeparator(const char *Path);

int lprofGetHostName(char *Name, int Len);

unsigned lprofBoolCmpXchg(void **Ptr, void *OldV, void *NewV);
void *lprofPtrFetchAdd(void **Mem, long ByteIncr);




int lprofSuspendSigKill();


void lprofRestoreSigKill();

static inline size_t lprofRoundUpTo(size_t x, size_t boundary) {
return (x + boundary - 1) & ~(boundary - 1);
}

static inline size_t lprofRoundDownTo(size_t x, size_t boundary) {
return x & ~(boundary - 1);
}

int lprofReleaseMemoryPagesToOS(uintptr_t Begin, uintptr_t End);

#endif
