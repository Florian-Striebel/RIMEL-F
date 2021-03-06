







#if !defined(LLDB_HOST_POSIX_PIPEPOSIX_H)
#define LLDB_HOST_POSIX_PIPEPOSIX_H
#if defined(__cplusplus)

#include "lldb/Host/PipeBase.h"

namespace lldb_private {






class PipePosix : public PipeBase {
public:
static int kInvalidDescriptor;

PipePosix();
PipePosix(lldb::pipe_t read, lldb::pipe_t write);
PipePosix(const PipePosix &) = delete;
PipePosix(PipePosix &&pipe_posix);
PipePosix &operator=(const PipePosix &) = delete;
PipePosix &operator=(PipePosix &&pipe_posix);

~PipePosix() override;

Status CreateNew(bool child_process_inherit) override;
Status CreateNew(llvm::StringRef name, bool child_process_inherit) override;
Status CreateWithUniqueName(llvm::StringRef prefix,
bool child_process_inherit,
llvm::SmallVectorImpl<char> &name) override;
Status OpenAsReader(llvm::StringRef name,
bool child_process_inherit) override;
Status
OpenAsWriterWithTimeout(llvm::StringRef name, bool child_process_inherit,
const std::chrono::microseconds &timeout) override;

bool CanRead() const override;
bool CanWrite() const override;

lldb::pipe_t GetReadPipe() const override {
return lldb::pipe_t(GetReadFileDescriptor());
}
lldb::pipe_t GetWritePipe() const override {
return lldb::pipe_t(GetWriteFileDescriptor());
}

int GetReadFileDescriptor() const override;
int GetWriteFileDescriptor() const override;
int ReleaseReadFileDescriptor() override;
int ReleaseWriteFileDescriptor() override;
void CloseReadFileDescriptor() override;
void CloseWriteFileDescriptor() override;


void Close() override;

Status Delete(llvm::StringRef name) override;

Status Write(const void *buf, size_t size, size_t &bytes_written) override;
Status ReadWithTimeout(void *buf, size_t size,
const std::chrono::microseconds &timeout,
size_t &bytes_read) override;

private:
int m_fds[2];
};

}

#endif
#endif
