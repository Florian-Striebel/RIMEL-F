











#if !defined(LLVM_CLANG_LEX_HEADERMAP_H)
#define LLVM_CLANG_LEX_HEADERMAP_H

#include "clang/Basic/FileManager.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/MemoryBuffer.h"
#include <memory>

namespace clang {

struct HMapBucket;
struct HMapHeader;


class HeaderMapImpl {
std::unique_ptr<const llvm::MemoryBuffer> FileBuffer;
bool NeedsBSwap;
mutable llvm::StringMap<StringRef> ReverseMap;

public:
HeaderMapImpl(std::unique_ptr<const llvm::MemoryBuffer> File, bool NeedsBSwap)
: FileBuffer(std::move(File)), NeedsBSwap(NeedsBSwap) {}


static bool checkHeader(const llvm::MemoryBuffer &File, bool &NeedsByteSwap);



StringRef lookupFilename(StringRef Filename,
SmallVectorImpl<char> &DestPath) const;


StringRef getFileName() const;


void dump() const;


StringRef reverseLookupFilename(StringRef DestPath) const;

private:
unsigned getEndianAdjustedWord(unsigned X) const;
const HMapHeader &getHeader() const;
HMapBucket getBucket(unsigned BucketNo) const;



Optional<StringRef> getString(unsigned StrTabIdx) const;
};





class HeaderMap : private HeaderMapImpl {
HeaderMap(std::unique_ptr<const llvm::MemoryBuffer> File, bool BSwap)
: HeaderMapImpl(std::move(File), BSwap) {}

public:


static std::unique_ptr<HeaderMap> Create(const FileEntry *FE,
FileManager &FM);






Optional<FileEntryRef> LookupFile(StringRef Filename, FileManager &FM) const;

using HeaderMapImpl::dump;
using HeaderMapImpl::getFileName;
using HeaderMapImpl::lookupFilename;
using HeaderMapImpl::reverseLookupFilename;
};

}

#endif
