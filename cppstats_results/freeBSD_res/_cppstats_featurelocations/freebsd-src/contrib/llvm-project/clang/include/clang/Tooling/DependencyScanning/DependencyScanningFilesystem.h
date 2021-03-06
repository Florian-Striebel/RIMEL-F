







#if !defined(LLVM_CLANG_TOOLING_DEPENDENCY_SCANNING_FILESYSTEM_H)
#define LLVM_CLANG_TOOLING_DEPENDENCY_SCANNING_FILESYSTEM_H

#include "clang/Basic/LLVM.h"
#include "clang/Lex/PreprocessorExcludedConditionalDirectiveSkipMapping.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/VirtualFileSystem.h"
#include <mutex>

namespace clang {
namespace tooling {
namespace dependencies {










class CachedFileSystemEntry {
public:

CachedFileSystemEntry() : MaybeStat(llvm::vfs::Status()) {}

CachedFileSystemEntry(std::error_code Error) : MaybeStat(std::move(Error)) {}









static CachedFileSystemEntry createFileEntry(StringRef Filename,
llvm::vfs::FileSystem &FS,
bool Minimize = true);


static CachedFileSystemEntry createDirectoryEntry(llvm::vfs::Status &&Stat);


bool isValid() const { return !MaybeStat || MaybeStat->isStatusKnown(); }


bool isDirectory() const { return MaybeStat && MaybeStat->isDirectory(); }


llvm::ErrorOr<StringRef> getContents() const {
if (!MaybeStat)
return MaybeStat.getError();
assert(!MaybeStat->isDirectory() && "not a file");
assert(isValid() && "not initialized");
return Contents.str();
}


llvm::ErrorOr<llvm::vfs::Status> getStatus() const {
assert(isValid() && "not initialized");
return MaybeStat;
}


StringRef getName() const {
assert(isValid() && "not initialized");
return MaybeStat->getName();
}



const PreprocessorSkippedRangeMapping &getPPSkippedRangeMapping() const {
return PPSkippedRangeMapping;
}

CachedFileSystemEntry(CachedFileSystemEntry &&) = default;
CachedFileSystemEntry &operator=(CachedFileSystemEntry &&) = default;

CachedFileSystemEntry(const CachedFileSystemEntry &) = delete;
CachedFileSystemEntry &operator=(const CachedFileSystemEntry &) = delete;

private:
llvm::ErrorOr<llvm::vfs::Status> MaybeStat;




llvm::SmallString<1> Contents;
PreprocessorSkippedRangeMapping PPSkippedRangeMapping;
};







class DependencyScanningFilesystemSharedCache {
public:
struct SharedFileSystemEntry {
std::mutex ValueLock;
CachedFileSystemEntry Value;
};





SharedFileSystemEntry &get(StringRef Key, bool Minimized);

private:
class SingleCache {
public:
SingleCache();

SharedFileSystemEntry &get(StringRef Key);

private:
struct CacheShard {
std::mutex CacheLock;
llvm::StringMap<SharedFileSystemEntry, llvm::BumpPtrAllocator> Cache;
};
std::unique_ptr<CacheShard[]> CacheShards;
unsigned NumShards;
};

SingleCache CacheMinimized;
SingleCache CacheOriginal;
};




class DependencyScanningFilesystemLocalCache {
private:
using SingleCache =
llvm::StringMap<const CachedFileSystemEntry *, llvm::BumpPtrAllocator>;

SingleCache CacheMinimized;
SingleCache CacheOriginal;

SingleCache &selectCache(bool Minimized) {
return Minimized ? CacheMinimized : CacheOriginal;
}

public:
void setCachedEntry(StringRef Filename, bool Minimized,
const CachedFileSystemEntry *Entry) {
SingleCache &Cache = selectCache(Minimized);
bool IsInserted = Cache.try_emplace(Filename, Entry).second;
(void)IsInserted;
assert(IsInserted && "local cache is updated more than once");
}

const CachedFileSystemEntry *getCachedEntry(StringRef Filename,
bool Minimized) {
SingleCache &Cache = selectCache(Minimized);
auto It = Cache.find(Filename);
return It == Cache.end() ? nullptr : It->getValue();
}
};










class DependencyScanningWorkerFilesystem : public llvm::vfs::ProxyFileSystem {
public:
DependencyScanningWorkerFilesystem(
DependencyScanningFilesystemSharedCache &SharedCache,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS,
ExcludedPreprocessorDirectiveSkipMapping *PPSkipMappings)
: ProxyFileSystem(std::move(FS)), SharedCache(SharedCache),
PPSkipMappings(PPSkipMappings) {}

llvm::ErrorOr<llvm::vfs::Status> status(const Twine &Path) override;
llvm::ErrorOr<std::unique_ptr<llvm::vfs::File>>
openFileForRead(const Twine &Path) override;

void clearIgnoredFiles() { IgnoredFiles.clear(); }
void ignoreFile(StringRef Filename);

private:
bool shouldIgnoreFile(StringRef Filename);

llvm::ErrorOr<const CachedFileSystemEntry *>
getOrCreateFileSystemEntry(const StringRef Filename);


DependencyScanningFilesystemSharedCache &SharedCache;


DependencyScanningFilesystemLocalCache Cache;



ExcludedPreprocessorDirectiveSkipMapping *PPSkipMappings;

llvm::StringSet<> IgnoredFiles;
};

}
}
}

#endif
