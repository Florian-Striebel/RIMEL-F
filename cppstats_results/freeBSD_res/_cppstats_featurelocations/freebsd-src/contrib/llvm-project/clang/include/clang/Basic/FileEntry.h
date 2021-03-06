












#if !defined(LLVM_CLANG_BASIC_FILEENTRY_H)
#define LLVM_CLANG_BASIC_FILEENTRY_H

#include "clang/Basic/DirectoryEntry.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/FileSystem/UniqueID.h"

namespace llvm {

class MemoryBuffer;

namespace vfs {

class File;

}
}

namespace clang {

class FileEntryRef;

}

namespace llvm {
namespace optional_detail {


template <>
class OptionalStorage<clang::FileEntryRef, true>;

}
}

namespace clang {

class FileEntry;



class FileEntryRef {
public:
StringRef getName() const { return ME->first(); }
const FileEntry &getFileEntry() const {
return *ME->second->V.get<FileEntry *>();
}
DirectoryEntryRef getDir() const { return *ME->second->Dir; }

inline bool isValid() const;
inline off_t getSize() const;
inline unsigned getUID() const;
inline const llvm::sys::fs::UniqueID &getUniqueID() const;
inline time_t getModificationTime() const;
inline bool isNamedPipe() const;
inline void closeFile() const;



friend bool operator==(const FileEntryRef &LHS, const FileEntryRef &RHS) {
return &LHS.getFileEntry() == &RHS.getFileEntry();
}
friend bool operator==(const FileEntry *LHS, const FileEntryRef &RHS) {
return LHS == &RHS.getFileEntry();
}
friend bool operator==(const FileEntryRef &LHS, const FileEntry *RHS) {
return &LHS.getFileEntry() == RHS;
}
friend bool operator!=(const FileEntryRef &LHS, const FileEntryRef &RHS) {
return !(LHS == RHS);
}
friend bool operator!=(const FileEntry *LHS, const FileEntryRef &RHS) {
return !(LHS == RHS);
}
friend bool operator!=(const FileEntryRef &LHS, const FileEntry *RHS) {
return !(LHS == RHS);
}



friend llvm::hash_code hash_value(FileEntryRef Ref) {
return llvm::hash_value(&Ref.getFileEntry());
}

struct MapValue;


using MapEntry = llvm::StringMapEntry<llvm::ErrorOr<MapValue>>;


struct MapValue {








llvm::PointerUnion<FileEntry *, const void *> V;


Optional<DirectoryEntryRef> Dir;

MapValue() = delete;
MapValue(FileEntry &FE, DirectoryEntryRef Dir) : V(&FE), Dir(Dir) {}
MapValue(MapEntry &ME) : V(&ME) {}
};


bool isSameRef(const FileEntryRef &RHS) const { return ME == RHS.ME; }


















operator const FileEntry *() const { return &getFileEntry(); }

FileEntryRef() = delete;
explicit FileEntryRef(const MapEntry &ME) : ME(&ME) {
assert(ME.second && "Expected payload");
assert(ME.second->V && "Expected non-null");
assert(ME.second->V.is<FileEntry *>() && "Expected FileEntry");
}



const clang::FileEntryRef::MapEntry &getMapEntry() const { return *ME; }

private:
friend class FileMgr::MapEntryOptionalStorage<FileEntryRef>;
struct optional_none_tag {};


FileEntryRef(optional_none_tag) : ME(nullptr) {}
bool hasOptionalValue() const { return ME; }

friend struct llvm::DenseMapInfo<FileEntryRef>;
struct dense_map_empty_tag {};
struct dense_map_tombstone_tag {};


FileEntryRef(dense_map_empty_tag)
: ME(llvm::DenseMapInfo<const MapEntry *>::getEmptyKey()) {}
FileEntryRef(dense_map_tombstone_tag)
: ME(llvm::DenseMapInfo<const MapEntry *>::getTombstoneKey()) {}
bool isSpecialDenseMapKey() const {
return isSameRef(FileEntryRef(dense_map_empty_tag())) ||
isSameRef(FileEntryRef(dense_map_tombstone_tag()));
}

const MapEntry *ME;
};

static_assert(sizeof(FileEntryRef) == sizeof(const FileEntry *),
"FileEntryRef must avoid size overhead");

static_assert(std::is_trivially_copyable<FileEntryRef>::value,
"FileEntryRef must be trivially copyable");

}

namespace llvm {
namespace optional_detail {



template <>
class OptionalStorage<clang::FileEntryRef>
: public clang::FileMgr::MapEntryOptionalStorage<clang::FileEntryRef> {
using StorageImpl =
clang::FileMgr::MapEntryOptionalStorage<clang::FileEntryRef>;

public:
OptionalStorage() = default;

template <class... ArgTypes>
explicit OptionalStorage(in_place_t, ArgTypes &&...Args)
: StorageImpl(in_place_t{}, std::forward<ArgTypes>(Args)...) {}

OptionalStorage &operator=(clang::FileEntryRef Ref) {
StorageImpl::operator=(Ref);
return *this;
}
};

static_assert(sizeof(Optional<clang::FileEntryRef>) ==
sizeof(clang::FileEntryRef),
"Optional<FileEntryRef> must avoid size overhead");

static_assert(std::is_trivially_copyable<Optional<clang::FileEntryRef>>::value,
"Optional<FileEntryRef> should be trivially copyable");

}


template <> struct DenseMapInfo<clang::FileEntryRef> {
static inline clang::FileEntryRef getEmptyKey() {
return clang::FileEntryRef(clang::FileEntryRef::dense_map_empty_tag());
}

static inline clang::FileEntryRef getTombstoneKey() {
return clang::FileEntryRef(clang::FileEntryRef::dense_map_tombstone_tag());
}

static unsigned getHashValue(clang::FileEntryRef Val) {
return hash_value(Val);
}

static bool isEqual(clang::FileEntryRef LHS, clang::FileEntryRef RHS) {

if (LHS.isSameRef(RHS))
return true;


if (LHS.isSpecialDenseMapKey() || RHS.isSpecialDenseMapKey())
return false;


return LHS == RHS;
}
};

}

namespace clang {






















class OptionalFileEntryRefDegradesToFileEntryPtr
: public Optional<FileEntryRef> {
public:
OptionalFileEntryRefDegradesToFileEntryPtr() = default;
OptionalFileEntryRefDegradesToFileEntryPtr(
OptionalFileEntryRefDegradesToFileEntryPtr &&) = default;
OptionalFileEntryRefDegradesToFileEntryPtr(
const OptionalFileEntryRefDegradesToFileEntryPtr &) = default;
OptionalFileEntryRefDegradesToFileEntryPtr &
operator=(OptionalFileEntryRefDegradesToFileEntryPtr &&) = default;
OptionalFileEntryRefDegradesToFileEntryPtr &
operator=(const OptionalFileEntryRefDegradesToFileEntryPtr &) = default;

OptionalFileEntryRefDegradesToFileEntryPtr(llvm::NoneType) {}
OptionalFileEntryRefDegradesToFileEntryPtr(FileEntryRef Ref)
: Optional<FileEntryRef>(Ref) {}
OptionalFileEntryRefDegradesToFileEntryPtr(Optional<FileEntryRef> MaybeRef)
: Optional<FileEntryRef>(MaybeRef) {}

OptionalFileEntryRefDegradesToFileEntryPtr &operator=(llvm::NoneType) {
Optional<FileEntryRef>::operator=(None);
return *this;
}
OptionalFileEntryRefDegradesToFileEntryPtr &operator=(FileEntryRef Ref) {
Optional<FileEntryRef>::operator=(Ref);
return *this;
}
OptionalFileEntryRefDegradesToFileEntryPtr &
operator=(Optional<FileEntryRef> MaybeRef) {
Optional<FileEntryRef>::operator=(MaybeRef);
return *this;
}




operator const FileEntry *() const {
return hasValue() ? &getValue().getFileEntry() : nullptr;
}
};

static_assert(
std::is_trivially_copyable<
OptionalFileEntryRefDegradesToFileEntryPtr>::value,
"OptionalFileEntryRefDegradesToFileEntryPtr should be trivially copyable");






class FileEntry {
friend class FileManager;

std::string RealPathName;
off_t Size = 0;
time_t ModTime = 0;
const DirectoryEntry *Dir = nullptr;
llvm::sys::fs::UniqueID UniqueID;
unsigned UID = 0;
bool IsNamedPipe = false;
bool IsValid = false;


mutable std::unique_ptr<llvm::vfs::File> File;


std::unique_ptr<llvm::MemoryBuffer> Content;







Optional<FileEntryRef> LastRef;

public:
FileEntry();
~FileEntry();

FileEntry(const FileEntry &) = delete;
FileEntry &operator=(const FileEntry &) = delete;

StringRef getName() const { return LastRef->getName(); }
FileEntryRef getLastRef() const { return *LastRef; }

StringRef tryGetRealPathName() const { return RealPathName; }
bool isValid() const { return IsValid; }
off_t getSize() const { return Size; }
unsigned getUID() const { return UID; }
const llvm::sys::fs::UniqueID &getUniqueID() const { return UniqueID; }
time_t getModificationTime() const { return ModTime; }


const DirectoryEntry *getDir() const { return Dir; }

bool operator<(const FileEntry &RHS) const { return UniqueID < RHS.UniqueID; }



bool isNamedPipe() const { return IsNamedPipe; }

void closeFile() const;
};

bool FileEntryRef::isValid() const { return getFileEntry().isValid(); }

off_t FileEntryRef::getSize() const { return getFileEntry().getSize(); }

unsigned FileEntryRef::getUID() const { return getFileEntry().getUID(); }

const llvm::sys::fs::UniqueID &FileEntryRef::getUniqueID() const {
return getFileEntry().getUniqueID();
}

time_t FileEntryRef::getModificationTime() const {
return getFileEntry().getModificationTime();
}

bool FileEntryRef::isNamedPipe() const { return getFileEntry().isNamedPipe(); }

void FileEntryRef::closeFile() const { getFileEntry().closeFile(); }

}

#endif
