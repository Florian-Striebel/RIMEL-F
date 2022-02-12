












#if !defined(LLVM_CLANG_BASIC_DIRECTORYENTRY_H)
#define LLVM_CLANG_BASIC_DIRECTORYENTRY_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ErrorOr.h"

namespace clang {
namespace FileMgr {

template <class RefTy> class MapEntryOptionalStorage;

}



class DirectoryEntry {
friend class FileManager;


StringRef Name;

public:
StringRef getName() const { return Name; }
};



class DirectoryEntryRef {
public:
const DirectoryEntry &getDirEntry() const { return *ME->getValue(); }

StringRef getName() const { return ME->getKey(); }



friend llvm::hash_code hash_value(DirectoryEntryRef Ref) {
return llvm::hash_value(&Ref.getDirEntry());
}

using MapEntry = llvm::StringMapEntry<llvm::ErrorOr<DirectoryEntry &>>;

const MapEntry &getMapEntry() const { return *ME; }


bool isSameRef(DirectoryEntryRef RHS) const { return ME == RHS.ME; }

DirectoryEntryRef() = delete;
DirectoryEntryRef(const MapEntry &ME) : ME(&ME) {}


















operator const DirectoryEntry *() const { return &getDirEntry(); }

private:
friend class FileMgr::MapEntryOptionalStorage<DirectoryEntryRef>;
struct optional_none_tag {};


DirectoryEntryRef(optional_none_tag) : ME(nullptr) {}
bool hasOptionalValue() const { return ME; }

friend struct llvm::DenseMapInfo<DirectoryEntryRef>;
struct dense_map_empty_tag {};
struct dense_map_tombstone_tag {};


DirectoryEntryRef(dense_map_empty_tag)
: ME(llvm::DenseMapInfo<const MapEntry *>::getEmptyKey()) {}
DirectoryEntryRef(dense_map_tombstone_tag)
: ME(llvm::DenseMapInfo<const MapEntry *>::getTombstoneKey()) {}
bool isSpecialDenseMapKey() const {
return isSameRef(DirectoryEntryRef(dense_map_empty_tag())) ||
isSameRef(DirectoryEntryRef(dense_map_tombstone_tag()));
}

const MapEntry *ME;
};

namespace FileMgr {



template <class RefTy> class MapEntryOptionalStorage {
using optional_none_tag = typename RefTy::optional_none_tag;
RefTy MaybeRef;

public:
MapEntryOptionalStorage() : MaybeRef(optional_none_tag()) {}

template <class... ArgTypes>
explicit MapEntryOptionalStorage(llvm::in_place_t, ArgTypes &&...Args)
: MaybeRef(std::forward<ArgTypes>(Args)...) {}

void reset() { MaybeRef = optional_none_tag(); }

bool hasValue() const { return MaybeRef.hasOptionalValue(); }

RefTy &getValue() LLVM_LVALUE_FUNCTION {
assert(hasValue());
return MaybeRef;
}
RefTy const &getValue() const LLVM_LVALUE_FUNCTION {
assert(hasValue());
return MaybeRef;
}
#if LLVM_HAS_RVALUE_REFERENCE_THIS
RefTy &&getValue() && {
assert(hasValue());
return std::move(MaybeRef);
}
#endif

template <class... Args> void emplace(Args &&...args) {
MaybeRef = RefTy(std::forward<Args>(args)...);
}

MapEntryOptionalStorage &operator=(RefTy Ref) {
MaybeRef = Ref;
return *this;
}
};

}
}

namespace llvm {
namespace optional_detail {



template <>
class OptionalStorage<clang::DirectoryEntryRef>
: public clang::FileMgr::MapEntryOptionalStorage<clang::DirectoryEntryRef> {
using StorageImpl =
clang::FileMgr::MapEntryOptionalStorage<clang::DirectoryEntryRef>;

public:
OptionalStorage() = default;

template <class... ArgTypes>
explicit OptionalStorage(in_place_t, ArgTypes &&...Args)
: StorageImpl(in_place_t{}, std::forward<ArgTypes>(Args)...) {}

OptionalStorage &operator=(clang::DirectoryEntryRef Ref) {
StorageImpl::operator=(Ref);
return *this;
}
};

static_assert(sizeof(Optional<clang::DirectoryEntryRef>) ==
sizeof(clang::DirectoryEntryRef),
"Optional<DirectoryEntryRef> must avoid size overhead");

static_assert(
std::is_trivially_copyable<Optional<clang::DirectoryEntryRef>>::value,
"Optional<DirectoryEntryRef> should be trivially copyable");

}


template <> struct DenseMapInfo<clang::DirectoryEntryRef> {
static inline clang::DirectoryEntryRef getEmptyKey() {
return clang::DirectoryEntryRef(
clang::DirectoryEntryRef::dense_map_empty_tag());
}

static inline clang::DirectoryEntryRef getTombstoneKey() {
return clang::DirectoryEntryRef(
clang::DirectoryEntryRef::dense_map_tombstone_tag());
}

static unsigned getHashValue(clang::DirectoryEntryRef Val) {
return hash_value(Val);
}

static bool isEqual(clang::DirectoryEntryRef LHS,
clang::DirectoryEntryRef RHS) {

if (LHS.isSameRef(RHS))
return true;


if (LHS.isSpecialDenseMapKey() || RHS.isSpecialDenseMapKey())
return false;


return LHS == RHS;
}
};

}

namespace clang {























class OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr
: public Optional<DirectoryEntryRef> {
public:
OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr() = default;
OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr(
OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr &&) = default;
OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr(
const OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr &) = default;
OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr &
operator=(OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr &&) = default;
OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr &
operator=(const OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr &) = default;

OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr(llvm::NoneType) {}
OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr(DirectoryEntryRef Ref)
: Optional<DirectoryEntryRef>(Ref) {}
OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr(Optional<DirectoryEntryRef> MaybeRef)
: Optional<DirectoryEntryRef>(MaybeRef) {}

OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr &operator=(llvm::NoneType) {
Optional<DirectoryEntryRef>::operator=(None);
return *this;
}
OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr &operator=(DirectoryEntryRef Ref) {
Optional<DirectoryEntryRef>::operator=(Ref);
return *this;
}
OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr &
operator=(Optional<DirectoryEntryRef> MaybeRef) {
Optional<DirectoryEntryRef>::operator=(MaybeRef);
return *this;
}




operator const DirectoryEntry *() const {
return hasValue() ? &getValue().getDirEntry() : nullptr;
}
};

static_assert(std::is_trivially_copyable<
OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr>::value,
"OptionalDirectoryEntryRefDegradesToDirectoryEntryPtr should be "
"trivially copyable");

}

#endif
