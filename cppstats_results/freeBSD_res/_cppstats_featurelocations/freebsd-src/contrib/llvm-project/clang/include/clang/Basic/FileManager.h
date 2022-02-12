












#if !defined(LLVM_CLANG_BASIC_FILEMANAGER_H)
#define LLVM_CLANG_BASIC_FILEMANAGER_H

#include "clang/Basic/DirectoryEntry.h"
#include "clang/Basic/FileEntry.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/VirtualFileSystem.h"
#include <ctime>
#include <map>
#include <memory>
#include <string>

namespace llvm {

class MemoryBuffer;

}

namespace clang {

class FileSystemStatCache;








class FileManager : public RefCountedBase<FileManager> {
IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS;
FileSystemOptions FileSystemOpts;


std::map<llvm::sys::fs::UniqueID, DirectoryEntry> UniqueRealDirs;


std::map<llvm::sys::fs::UniqueID, FileEntry> UniqueRealFiles;





SmallVector<std::unique_ptr<DirectoryEntry>, 4> VirtualDirectoryEntries;

SmallVector<std::unique_ptr<FileEntry>, 4> VirtualFileEntries;



SmallVector<std::unique_ptr<FileEntry>, 0> BypassFileEntries;










llvm::StringMap<llvm::ErrorOr<DirectoryEntry &>, llvm::BumpPtrAllocator>
SeenDirEntries;






llvm::StringMap<llvm::ErrorOr<FileEntryRef::MapValue>, llvm::BumpPtrAllocator>
SeenFileEntries;





std::unique_ptr<llvm::StringMap<llvm::ErrorOr<FileEntryRef::MapValue>>>
SeenBypassFileEntries;


Optional<FileEntryRef> STDIN;


llvm::DenseMap<const void *, llvm::StringRef> CanonicalNames;


llvm::BumpPtrAllocator CanonicalNameStorage;



unsigned NextFileUID;


std::unique_ptr<FileSystemStatCache> StatCache;

std::error_code getStatValue(StringRef Path, llvm::vfs::Status &Status,
bool isFile,
std::unique_ptr<llvm::vfs::File> *F);



void addAncestorsAsVirtualDirs(StringRef Path);


void fillRealPathName(FileEntry *UFE, llvm::StringRef FileName);

public:




FileManager(const FileSystemOptions &FileSystemOpts,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS = nullptr);
~FileManager();








void setStatCache(std::unique_ptr<FileSystemStatCache> statCache);


void clearStatCache();


size_t getNumUniqueRealFiles() const { return UniqueRealFiles.size(); }











llvm::Expected<DirectoryEntryRef> getDirectoryRef(StringRef DirName,
bool CacheFailure = true);


llvm::Optional<DirectoryEntryRef>
getOptionalDirectoryRef(StringRef DirName, bool CacheFailure = true) {
return llvm::expectedToOptional(getDirectoryRef(DirName, CacheFailure));
}














llvm::ErrorOr<const DirectoryEntry *>
getDirectory(StringRef DirName, bool CacheFailure = true);















llvm::ErrorOr<const FileEntry *>
getFile(StringRef Filename, bool OpenFile = false, bool CacheFailure = true);















llvm::Expected<FileEntryRef> getFileRef(StringRef Filename,
bool OpenFile = false,
bool CacheFailure = true);







llvm::Expected<FileEntryRef> getSTDIN();


llvm::Optional<FileEntryRef> getOptionalFileRef(StringRef Filename,
bool OpenFile = false,
bool CacheFailure = true) {
return llvm::expectedToOptional(
getFileRef(Filename, OpenFile, CacheFailure));
}


FileSystemOptions &getFileSystemOpts() { return FileSystemOpts; }
const FileSystemOptions &getFileSystemOpts() const { return FileSystemOpts; }

llvm::vfs::FileSystem &getVirtualFileSystem() const { return *FS; }

void setVirtualFileSystem(IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS) {
this->FS = std::move(FS);
}





FileEntryRef getVirtualFileRef(StringRef Filename, off_t Size,
time_t ModificationTime);

const FileEntry *getVirtualFile(StringRef Filename, off_t Size,
time_t ModificationTime);









llvm::Optional<FileEntryRef> getBypassFile(FileEntryRef VFE);



llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
getBufferForFile(const FileEntry *Entry, bool isVolatile = false,
bool RequiresNullTerminator = true);
llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
getBufferForFile(StringRef Filename, bool isVolatile = false,
bool RequiresNullTerminator = true) {
return getBufferForFileImpl(Filename, -1, isVolatile,
RequiresNullTerminator);
}

private:
llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
getBufferForFileImpl(StringRef Filename, int64_t FileSize, bool isVolatile,
bool RequiresNullTerminator);

public:






std::error_code getNoncachedStatValue(StringRef Path,
llvm::vfs::Status &Result);





bool FixupRelativePath(SmallVectorImpl<char> &path) const;




bool makeAbsolutePath(SmallVectorImpl<char> &Path) const;



void GetUniqueIDMapping(
SmallVectorImpl<const FileEntry *> &UIDToFiles) const;






StringRef getCanonicalName(const DirectoryEntry *Dir);






StringRef getCanonicalName(const FileEntry *File);

void PrintStats() const;
};

}

#endif
