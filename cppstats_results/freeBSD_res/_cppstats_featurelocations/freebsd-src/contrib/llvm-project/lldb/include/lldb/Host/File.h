







#if !defined(LLDB_HOST_FILE_H)
#define LLDB_HOST_FILE_H

#include "lldb/Host/PosixApi.h"
#include "lldb/Utility/IOObject.h"
#include "lldb/Utility/Status.h"
#include "lldb/lldb-private.h"
#include "llvm/ADT/BitmaskEnum.h"

#include <cstdarg>
#include <cstdio>
#include <mutex>
#include <sys/types.h>

namespace lldb_private {

LLVM_ENABLE_BITMASK_ENUMS_IN_NAMESPACE();








class File : public IOObject {
public:
static int kInvalidDescriptor;
static FILE *kInvalidStream;








enum OpenOptions : uint32_t {
eOpenOptionRead = (1u << 0),
eOpenOptionWrite = (1u << 1),
eOpenOptionAppend =
(1u << 2),
eOpenOptionTruncate = (1u << 3),
eOpenOptionNonBlocking = (1u << 4),
eOpenOptionCanCreate = (1u << 5),
eOpenOptionCanCreateNewOnly =
(1u << 6),
eOpenOptionDontFollowSymlinks = (1u << 7),
eOpenOptionCloseOnExec =
(1u << 8),
LLVM_MARK_AS_BITMASK_ENUM( eOpenOptionCloseOnExec)
};

static mode_t ConvertOpenOptionsForPOSIXOpen(OpenOptions open_options);
static llvm::Expected<OpenOptions> GetOptionsFromMode(llvm::StringRef mode);
static bool DescriptorIsValid(int descriptor) { return descriptor >= 0; };
static llvm::Expected<const char *>
GetStreamOpenModeFromOptions(OpenOptions options);

File() : IOObject(eFDTypeFile){};














Status Read(void *buf, size_t &num_bytes) override;














Status Write(const void *buf, size_t &num_bytes) override;





bool IsValid() const override;






Status Close() override;










WaitableHandle GetWaitableHandle() override;







virtual Status GetFileSpec(FileSpec &file_spec) const;








virtual int GetDescriptor() const;









virtual FILE *GetStream();



















virtual off_t SeekFromStart(off_t offset, Status *error_ptr = nullptr);



















virtual off_t SeekFromCurrent(off_t offset, Status *error_ptr = nullptr);




















virtual off_t SeekFromEnd(off_t offset, Status *error_ptr = nullptr);






















virtual Status Read(void *dst, size_t &num_bytes, off_t &offset);
























virtual Status Write(const void *src, size_t &num_bytes, off_t &offset);






virtual Status Flush();






virtual Status Sync();














size_t Printf(const char *format, ...) __attribute__((format(printf, 2, 3)));











virtual size_t PrintfVarArg(const char *format, va_list args);











virtual llvm::Expected<OpenOptions> GetOptions() const;

llvm::Expected<const char *> GetOpenMode() const {
auto opts = GetOptions();
if (!opts)
return opts.takeError();
return GetStreamOpenModeFromOptions(opts.get());
}






uint32_t GetPermissions(Status &error) const;






bool GetIsInteractive();










bool GetIsRealTerminal();





bool GetIsTerminalWithColors();

operator bool() const { return IsValid(); };

bool operator!() const { return !IsValid(); };

static char ID;
virtual bool isA(const void *classID) const { return classID == &ID; }
static bool classof(const File *file) { return file->isA(&ID); }

protected:
LazyBool m_is_interactive = eLazyBoolCalculate;
LazyBool m_is_real_terminal = eLazyBoolCalculate;
LazyBool m_supports_colors = eLazyBoolCalculate;

void CalculateInteractiveAndTerminal();

private:
File(const File &) = delete;
const File &operator=(const File &) = delete;
};

class NativeFile : public File {
public:
NativeFile() : m_descriptor(kInvalidDescriptor), m_stream(kInvalidStream) {}

NativeFile(FILE *fh, bool transfer_ownership)
: m_descriptor(kInvalidDescriptor), m_own_descriptor(false), m_stream(fh),
m_options(), m_own_stream(transfer_ownership) {}

NativeFile(int fd, OpenOptions options, bool transfer_ownership)
: m_descriptor(fd), m_own_descriptor(transfer_ownership),
m_stream(kInvalidStream), m_options(options), m_own_stream(false) {}

~NativeFile() override { Close(); }

bool IsValid() const override {
return DescriptorIsValid() || StreamIsValid();
}

Status Read(void *buf, size_t &num_bytes) override;
Status Write(const void *buf, size_t &num_bytes) override;
Status Close() override;
WaitableHandle GetWaitableHandle() override;
Status GetFileSpec(FileSpec &file_spec) const override;
int GetDescriptor() const override;
FILE *GetStream() override;
off_t SeekFromStart(off_t offset, Status *error_ptr = nullptr) override;
off_t SeekFromCurrent(off_t offset, Status *error_ptr = nullptr) override;
off_t SeekFromEnd(off_t offset, Status *error_ptr = nullptr) override;
Status Read(void *dst, size_t &num_bytes, off_t &offset) override;
Status Write(const void *src, size_t &num_bytes, off_t &offset) override;
Status Flush() override;
Status Sync() override;
size_t PrintfVarArg(const char *format, va_list args) override;
llvm::Expected<OpenOptions> GetOptions() const override;

static char ID;
virtual bool isA(const void *classID) const override {
return classID == &ID || File::isA(classID);
}
static bool classof(const File *file) { return file->isA(&ID); }

protected:
bool DescriptorIsValid() const {
return File::DescriptorIsValid(m_descriptor);
}
bool StreamIsValid() const { return m_stream != kInvalidStream; }


int m_descriptor;
bool m_own_descriptor = false;
FILE *m_stream;
OpenOptions m_options{};
bool m_own_stream = false;
std::mutex offset_access_mutex;

private:
NativeFile(const NativeFile &) = delete;
const NativeFile &operator=(const NativeFile &) = delete;
};

}

#endif
