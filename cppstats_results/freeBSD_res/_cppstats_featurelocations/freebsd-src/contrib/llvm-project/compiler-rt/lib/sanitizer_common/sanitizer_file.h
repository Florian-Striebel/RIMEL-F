













#if !defined(SANITIZER_FILE_H)
#define SANITIZER_FILE_H

#include "sanitizer_interface_internal.h"
#include "sanitizer_internal_defs.h"
#include "sanitizer_libc.h"
#include "sanitizer_mutex.h"

namespace __sanitizer {

struct ReportFile {
void Write(const char *buffer, uptr length);
bool SupportsColors();
void SetReportPath(const char *path);
const char *GetReportPath();





StaticSpinMutex *mu;


fd_t fd;

char path_prefix[kMaxPathLength];

char full_path[kMaxPathLength];


uptr fd_pid;

private:
void ReopenIfNecessary();
};
extern ReportFile report_file;

enum FileAccessMode {
RdOnly,
WrOnly,
RdWr
};


fd_t OpenFile(const char *filename, FileAccessMode mode,
error_t *errno_p = nullptr);
void CloseFile(fd_t);


bool ReadFromFile(fd_t fd, void *buff, uptr buff_size,
uptr *bytes_read = nullptr, error_t *error_p = nullptr);
bool WriteToFile(fd_t fd, const void *buff, uptr buff_size,
uptr *bytes_written = nullptr, error_t *error_p = nullptr);


struct FileCloser {
explicit FileCloser(fd_t fd) : fd(fd) {}
~FileCloser() { CloseFile(fd); }
fd_t fd;
};

bool SupportsColoredOutput(fd_t fd);


const char *GetPwd();
bool FileExists(const char *filename);
char *FindPathToBinary(const char *name);
bool IsPathSeparator(const char c);
bool IsAbsolutePath(const char *path);






pid_t StartSubprocess(const char *filename, const char *const argv[],
const char *const envp[], fd_t stdin_fd = kInvalidFd,
fd_t stdout_fd = kInvalidFd, fd_t stderr_fd = kInvalidFd);

bool IsProcessRunning(pid_t pid);


int WaitForProcess(pid_t pid);




void *MapFileToMemory(const char *file_name, uptr *buff_size);
void *MapWritableFileToMemory(void *addr, uptr size, fd_t fd, OFF_T offset);

}

#endif
