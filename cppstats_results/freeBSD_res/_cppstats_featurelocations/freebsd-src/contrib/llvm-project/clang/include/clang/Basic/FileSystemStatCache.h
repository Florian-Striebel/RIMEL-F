












#if !defined(LLVM_CLANG_BASIC_FILESYSTEMSTATCACHE_H)
#define LLVM_CLANG_BASIC_FILESYSTEMSTATCACHE_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/VirtualFileSystem.h"
#include <cstdint>
#include <ctime>
#include <memory>
#include <string>
#include <utility>

namespace clang {




class FileSystemStatCache {
virtual void anchor();

public:
virtual ~FileSystemStatCache() = default;











static std::error_code
get(StringRef Path, llvm::vfs::Status &Status, bool isFile,
std::unique_ptr<llvm::vfs::File> *F,
FileSystemStatCache *Cache, llvm::vfs::FileSystem &FS);

protected:



virtual std::error_code getStat(StringRef Path, llvm::vfs::Status &Status,
bool isFile,
std::unique_ptr<llvm::vfs::File> *F,
llvm::vfs::FileSystem &FS) = 0;
};




class MemorizeStatCalls : public FileSystemStatCache {
public:

llvm::StringMap<llvm::vfs::Status, llvm::BumpPtrAllocator> StatCalls;

using iterator =
llvm::StringMap<llvm::vfs::Status,
llvm::BumpPtrAllocator>::const_iterator;

iterator begin() const { return StatCalls.begin(); }
iterator end() const { return StatCalls.end(); }

std::error_code getStat(StringRef Path, llvm::vfs::Status &Status,
bool isFile,
std::unique_ptr<llvm::vfs::File> *F,
llvm::vfs::FileSystem &FS) override;
};

}

#endif
