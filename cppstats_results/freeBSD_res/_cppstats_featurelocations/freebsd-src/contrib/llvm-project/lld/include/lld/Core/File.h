







#if !defined(LLD_CORE_FILE_H)
#define LLD_CORE_FILE_H

#include "lld/Core/AbsoluteAtom.h"
#include "lld/Core/DefinedAtom.h"
#include "lld/Core/SharedLibraryAtom.h"
#include "lld/Core/UndefinedAtom.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/ErrorHandling.h"
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace lld {

class LinkingContext;













class File {
public:
virtual ~File();


enum Kind {
kindErrorObject,
kindNormalizedObject,
kindMachObject,
kindCEntryObject,
kindHeaderObject,
kindEntryObject,
kindUndefinedSymsObject,
kindStubHelperObject,
kindResolverMergedObject,
kindSectCreateObject,
kindSharedLibrary,
kindArchiveLibrary
};


Kind kind() const {
return _kind;
}




StringRef path() const {
if (_archivePath.empty())
return _path;
if (_archiveMemberPath.empty())
_archiveMemberPath = (_archivePath + "(" + _path + ")").str();
return _archiveMemberPath;
}



StringRef archivePath() const { return _archivePath; }
void setArchivePath(StringRef path) { _archivePath = std::string(path); }


StringRef memberPath() const { return _path; }


uint64_t ordinal() const {
assert(_ordinal != UINT64_MAX);
return _ordinal;
}


bool hasOrdinal() const { return (_ordinal != UINT64_MAX); }


void setOrdinal(uint64_t ordinal) const { _ordinal = ordinal; }


uint64_t getNextAtomOrdinalAndIncrement() const {
return _nextAtomOrdinal++;
}


llvm::BumpPtrAllocator &allocator() const {
return _allocator;
}


template <typename T> using AtomVector = std::vector<OwningAtomPtr<T>>;


template <typename T> class AtomRange {
public:
AtomRange(AtomVector<T> &v) : _v(v) {}
AtomRange(const AtomVector<T> &v) : _v(const_cast<AtomVector<T> &>(v)) {}

using ConstDerefFn = const T* (*)(const OwningAtomPtr<T>&);
using DerefFn = T* (*)(OwningAtomPtr<T>&);

typedef llvm::mapped_iterator<typename AtomVector<T>::const_iterator,
ConstDerefFn> ConstItTy;
typedef llvm::mapped_iterator<typename AtomVector<T>::iterator,
DerefFn> ItTy;

static const T* DerefConst(const OwningAtomPtr<T> &p) {
return p.get();
}

static T* Deref(OwningAtomPtr<T> &p) {
return p.get();
}

ConstItTy begin() const {
return ConstItTy(_v.begin(), ConstDerefFn(DerefConst));
}
ConstItTy end() const {
return ConstItTy(_v.end(), ConstDerefFn(DerefConst));
}

ItTy begin() {
return ItTy(_v.begin(), DerefFn(Deref));
}
ItTy end() {
return ItTy(_v.end(), DerefFn(Deref));
}

llvm::iterator_range<typename AtomVector<T>::iterator> owning_ptrs() {
return llvm::make_range(_v.begin(), _v.end());
}

llvm::iterator_range<typename AtomVector<T>::iterator> owning_ptrs() const {
return llvm::make_range(_v.begin(), _v.end());
}

bool empty() const {
return _v.empty();
}

size_t size() const {
return _v.size();
}

const OwningAtomPtr<T> &operator[](size_t idx) const {
return _v[idx];
}

OwningAtomPtr<T> &operator[](size_t idx) {
return _v[idx];
}

private:
AtomVector<T> &_v;
};



virtual const AtomRange<DefinedAtom> defined() const = 0;



virtual const AtomRange<UndefinedAtom> undefined() const = 0;



virtual const AtomRange<SharedLibraryAtom> sharedLibrary() const = 0;



virtual const AtomRange<AbsoluteAtom> absolute() const = 0;





virtual void clearAtoms() = 0;





void setLastError(std::error_code err) { _lastError = err; }

std::error_code parse();






void setSharedMemoryBuffer(std::shared_ptr<MemoryBuffer> mb) {
_sharedMemoryBuffer = mb;
}

protected:

File(StringRef p, Kind kind)
: _path(p), _kind(kind), _ordinal(UINT64_MAX),
_nextAtomOrdinal(0) {}



virtual std::error_code doParse() { return std::error_code(); }

static AtomVector<DefinedAtom> _noDefinedAtoms;
static AtomVector<UndefinedAtom> _noUndefinedAtoms;
static AtomVector<SharedLibraryAtom> _noSharedLibraryAtoms;
static AtomVector<AbsoluteAtom> _noAbsoluteAtoms;
mutable llvm::BumpPtrAllocator _allocator;

private:
StringRef _path;
std::string _archivePath;
mutable std::string _archiveMemberPath;
Kind _kind;
mutable uint64_t _ordinal;
mutable uint64_t _nextAtomOrdinal;
std::shared_ptr<MemoryBuffer> _sharedMemoryBuffer;
llvm::Optional<std::error_code> _lastError;
std::mutex _parseMutex;
};






class ErrorFile : public File {
public:
ErrorFile(StringRef path, std::error_code ec)
: File(path, kindErrorObject), _ec(ec) {}

std::error_code doParse() override { return _ec; }

const AtomRange<DefinedAtom> defined() const override {
llvm_unreachable("internal error");
}
const AtomRange<UndefinedAtom> undefined() const override {
llvm_unreachable("internal error");
}
const AtomRange<SharedLibraryAtom> sharedLibrary() const override {
llvm_unreachable("internal error");
}
const AtomRange<AbsoluteAtom> absolute() const override {
llvm_unreachable("internal error");
}

void clearAtoms() override {
}

private:
std::error_code _ec;
};

}

#endif
