







#if !defined(LLVM_CLANG_INDEX_SerializablePathCollection_H)
#define LLVM_CLANG_INDEX_SerializablePathCollection_H

#include "clang/Basic/FileManager.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator.h"

#include <string>
#include <vector>

namespace clang {
namespace index {


class StringPool {
llvm::SmallString<512> Buffer;

public:
struct StringOffsetSize {
std::size_t Offset;
std::size_t Size;

StringOffsetSize(size_t Offset, size_t Size) : Offset(Offset), Size(Size) {}
};

StringOffsetSize add(StringRef Str);
StringRef getBuffer() const { return Buffer; }
};


class PathPool {
public:

enum class RootDirKind {
Regular = 0,
CurrentWorkDir = 1,
SysRoot = 2,
};

struct DirPath {
RootDirKind Root;
StringPool::StringOffsetSize Path;

DirPath(RootDirKind Root, const StringPool::StringOffsetSize &Path)
: Root(Root), Path(Path) {}
};

struct FilePath {
DirPath Dir;
StringPool::StringOffsetSize Filename;

FilePath(const DirPath &Dir, const StringPool::StringOffsetSize &Filename)
: Dir(Dir), Filename(Filename) {}
};


size_t addFilePath(RootDirKind Root, const StringPool::StringOffsetSize &Dir,
StringRef Filename);


StringPool::StringOffsetSize addDirPath(StringRef Dir);

llvm::ArrayRef<FilePath> getFilePaths() const;

StringRef getPaths() const;

private:
StringPool Paths;
std::vector<FilePath> FilePaths;
};


class SerializablePathCollection {
std::string WorkDir;
std::string SysRoot;

PathPool Paths;
llvm::DenseMap<const clang::FileEntry *, std::size_t> UniqueFiles;
llvm::StringMap<PathPool::DirPath, llvm::BumpPtrAllocator> UniqueDirs;

public:
const StringPool::StringOffsetSize WorkDirPath;
const StringPool::StringOffsetSize SysRootPath;
const StringPool::StringOffsetSize OutputFilePath;

SerializablePathCollection(llvm::StringRef CurrentWorkDir,
llvm::StringRef SysRoot,
llvm::StringRef OutputFile);


llvm::StringRef getPathsBuffer() const { return Paths.getPaths(); }



ArrayRef<PathPool::FilePath> getFilePaths() const {
return Paths.getFilePaths();
}



size_t tryStoreFilePath(const clang::FileEntry &FE);

private:



StringPool::StringOffsetSize storePath(llvm::StringRef Path);


PathPool::DirPath tryStoreDirPath(llvm::StringRef dirStr);
};

}
}

#endif
