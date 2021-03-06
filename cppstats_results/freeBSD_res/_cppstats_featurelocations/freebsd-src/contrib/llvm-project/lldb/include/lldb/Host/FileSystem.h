







#if !defined(LLDB_HOST_FILESYSTEM_H)
#define LLDB_HOST_FILESYSTEM_H

#include "lldb/Host/File.h"
#include "lldb/Utility/DataBufferLLVM.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/Status.h"

#include "llvm/ADT/Optional.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/FileCollector.h"
#include "llvm/Support/VirtualFileSystem.h"

#include "lldb/lldb-types.h"

#include <cstdint>
#include <cstdio>
#include <sys/stat.h>

namespace lldb_private {
class FileSystem {
public:
static const char *DEV_NULL;
static const char *PATH_CONVERSION_ERROR;

FileSystem()
: m_fs(llvm::vfs::getRealFileSystem()), m_collector(nullptr),
m_home_directory() {}
FileSystem(std::shared_ptr<llvm::FileCollectorBase> collector)
: m_fs(llvm::vfs::getRealFileSystem()), m_collector(std::move(collector)),
m_home_directory(), m_mapped(false) {}
FileSystem(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> fs,
bool mapped = false)
: m_fs(std::move(fs)), m_collector(nullptr), m_home_directory(),
m_mapped(mapped) {}

FileSystem(const FileSystem &fs) = delete;
FileSystem &operator=(const FileSystem &fs) = delete;

static FileSystem &Instance();

static void Initialize();
static void Initialize(std::shared_ptr<llvm::FileCollectorBase> collector);
static llvm::Error Initialize(const FileSpec &mapping);
static void Initialize(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> fs);
static void Terminate();

Status Symlink(const FileSpec &src, const FileSpec &dst);
Status Readlink(const FileSpec &src, FileSpec &dst);

Status ResolveSymbolicLink(const FileSpec &src, FileSpec &dst);


FILE *Fopen(const char *path, const char *mode);


int Open(const char *path, int flags, int mode);

llvm::Expected<std::unique_ptr<File>>
Open(const FileSpec &file_spec, File::OpenOptions options,
uint32_t permissions = lldb::eFilePermissionsFileDefault,
bool should_close_fd = true);



llvm::vfs::directory_iterator DirBegin(const FileSpec &file_spec,
std::error_code &ec);
llvm::vfs::directory_iterator DirBegin(const llvm::Twine &dir,
std::error_code &ec);




llvm::ErrorOr<llvm::vfs::Status> GetStatus(const FileSpec &file_spec) const;
llvm::ErrorOr<llvm::vfs::Status> GetStatus(const llvm::Twine &path) const;




llvm::sys::TimePoint<> GetModificationTime(const FileSpec &file_spec) const;
llvm::sys::TimePoint<> GetModificationTime(const llvm::Twine &path) const;




uint64_t GetByteSize(const FileSpec &file_spec) const;
uint64_t GetByteSize(const llvm::Twine &path) const;







uint32_t GetPermissions(const FileSpec &file_spec) const;
uint32_t GetPermissions(const llvm::Twine &path) const;
uint32_t GetPermissions(const FileSpec &file_spec, std::error_code &ec) const;
uint32_t GetPermissions(const llvm::Twine &path, std::error_code &ec) const;




bool Exists(const FileSpec &file_spec) const;
bool Exists(const llvm::Twine &path) const;




bool Readable(const FileSpec &file_spec) const;
bool Readable(const llvm::Twine &path) const;




bool IsDirectory(const FileSpec &file_spec) const;
bool IsDirectory(const llvm::Twine &path) const;




bool IsLocal(const FileSpec &file_spec) const;
bool IsLocal(const llvm::Twine &path) const;




std::error_code MakeAbsolute(llvm::SmallVectorImpl<char> &path) const;
std::error_code MakeAbsolute(FileSpec &file_spec) const;




void Resolve(llvm::SmallVectorImpl<char> &path);
void Resolve(FileSpec &file_spec);




std::shared_ptr<DataBufferLLVM> CreateDataBuffer(const llvm::Twine &path,
uint64_t size = 0,
uint64_t offset = 0);
std::shared_ptr<DataBufferLLVM> CreateDataBuffer(const FileSpec &file_spec,
uint64_t size = 0,
uint64_t offset = 0);



bool ResolveExecutableLocation(FileSpec &file_spec);


bool GetHomeDirectory(llvm::SmallVectorImpl<char> &path) const;
bool GetHomeDirectory(FileSpec &file_spec) const;

enum EnumerateDirectoryResult {

eEnumerateDirectoryResultNext,


eEnumerateDirectoryResultEnter,

eEnumerateDirectoryResultQuit
};

typedef EnumerateDirectoryResult (*EnumerateDirectoryCallbackType)(
void *baton, llvm::sys::fs::file_type file_type, llvm::StringRef);

typedef std::function<EnumerateDirectoryResult(
llvm::sys::fs::file_type file_type, llvm::StringRef)>
DirectoryCallback;

void EnumerateDirectory(llvm::Twine path, bool find_directories,
bool find_files, bool find_other,
EnumerateDirectoryCallbackType callback,
void *callback_baton);

std::error_code GetRealPath(const llvm::Twine &path,
llvm::SmallVectorImpl<char> &output) const;

llvm::ErrorOr<std::string> GetExternalPath(const llvm::Twine &path);
llvm::ErrorOr<std::string> GetExternalPath(const FileSpec &file_spec);

llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> GetVirtualFileSystem() {
return m_fs;
}

void Collect(const FileSpec &file_spec);
void Collect(const llvm::Twine &file);

void SetHomeDirectory(std::string home_directory);

private:
static llvm::Optional<FileSystem> &InstanceImpl();
llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> m_fs;
std::shared_ptr<llvm::FileCollectorBase> m_collector;
std::string m_home_directory;
bool m_mapped = false;
};
}

#endif
