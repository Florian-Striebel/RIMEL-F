











#if !defined(LLVM_CLANG_LIB_SERIALIZATION_ASTREADERINTERNALS_H)
#define LLVM_CLANG_LIB_SERIALIZATION_ASTREADERINTERNALS_H

#include "MultiOnDiskHashTable.h"
#include "clang/AST/DeclarationName.h"
#include "clang/Basic/LLVM.h"
#include "clang/Serialization/ASTBitCodes.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/OnDiskHashTable.h"
#include <ctime>
#include <utility>

namespace clang {

class ASTReader;
class FileEntry;
struct HeaderFileInfo;
class HeaderSearch;
class IdentifierTable;
class ObjCMethodDecl;

namespace serialization {

class ModuleFile;

namespace reader {



class ASTDeclContextNameLookupTrait {
ASTReader &Reader;
ModuleFile &F;

public:

static const int MaxTables = 4;


using data_type = SmallVector<DeclID, 4>;

struct data_type_builder {
data_type &Data;
llvm::DenseSet<DeclID> Found;

data_type_builder(data_type &D) : Data(D) {}

void insert(DeclID ID) {

if (Found.empty() && !Data.empty()) {
if (Data.size() <= 4) {
for (auto I : Found)
if (I == ID)
return;
Data.push_back(ID);
return;
}


Found.insert(Data.begin(), Data.end());
}

if (Found.insert(ID).second)
Data.push_back(ID);
}
};
using hash_value_type = unsigned;
using offset_type = unsigned;
using file_type = ModuleFile *;

using external_key_type = DeclarationName;
using internal_key_type = DeclarationNameKey;

explicit ASTDeclContextNameLookupTrait(ASTReader &Reader, ModuleFile &F)
: Reader(Reader), F(F) {}

static bool EqualKey(const internal_key_type &a, const internal_key_type &b) {
return a == b;
}

static hash_value_type ComputeHash(const internal_key_type &Key) {
return Key.getHash();
}

static internal_key_type GetInternalKey(const external_key_type &Name) {
return Name;
}

static std::pair<unsigned, unsigned>
ReadKeyDataLength(const unsigned char *&d);

internal_key_type ReadKey(const unsigned char *d, unsigned);

void ReadDataInto(internal_key_type, const unsigned char *d,
unsigned DataLen, data_type_builder &Val);

static void MergeDataInto(const data_type &From, data_type_builder &To) {
To.Data.reserve(To.Data.size() + From.size());
for (DeclID ID : From)
To.insert(ID);
}

file_type ReadFileRef(const unsigned char *&d);
};

struct DeclContextLookupTable {
MultiOnDiskHashTable<ASTDeclContextNameLookupTrait> Table;
};









class ASTIdentifierLookupTraitBase {
public:
using external_key_type = StringRef;
using internal_key_type = StringRef;
using hash_value_type = unsigned;
using offset_type = unsigned;

static bool EqualKey(const internal_key_type& a, const internal_key_type& b) {
return a == b;
}

static hash_value_type ComputeHash(const internal_key_type& a);

static std::pair<unsigned, unsigned>
ReadKeyDataLength(const unsigned char*& d);


static const internal_key_type&
GetInternalKey(const external_key_type& x) { return x; }


static const external_key_type&
GetExternalKey(const internal_key_type& x) { return x; }

static internal_key_type ReadKey(const unsigned char* d, unsigned n);
};


class ASTIdentifierLookupTrait : public ASTIdentifierLookupTraitBase {
ASTReader &Reader;
ModuleFile &F;




IdentifierInfo *KnownII;

public:
using data_type = IdentifierInfo *;

ASTIdentifierLookupTrait(ASTReader &Reader, ModuleFile &F,
IdentifierInfo *II = nullptr)
: Reader(Reader), F(F), KnownII(II) {}

data_type ReadData(const internal_key_type& k,
const unsigned char* d,
unsigned DataLen);

IdentID ReadIdentifierID(const unsigned char *d);

ASTReader &getReader() const { return Reader; }
};



using ASTIdentifierLookupTable =
llvm::OnDiskIterableChainedHashTable<ASTIdentifierLookupTrait>;



class ASTSelectorLookupTrait {
ASTReader &Reader;
ModuleFile &F;

public:
struct data_type {
SelectorID ID;
unsigned InstanceBits;
unsigned FactoryBits;
bool InstanceHasMoreThanOneDecl;
bool FactoryHasMoreThanOneDecl;
SmallVector<ObjCMethodDecl *, 2> Instance;
SmallVector<ObjCMethodDecl *, 2> Factory;
};

using external_key_type = Selector;
using internal_key_type = external_key_type;
using hash_value_type = unsigned;
using offset_type = unsigned;

ASTSelectorLookupTrait(ASTReader &Reader, ModuleFile &F)
: Reader(Reader), F(F) {}

static bool EqualKey(const internal_key_type& a,
const internal_key_type& b) {
return a == b;
}

static hash_value_type ComputeHash(Selector Sel);

static const internal_key_type&
GetInternalKey(const external_key_type& x) { return x; }

static std::pair<unsigned, unsigned>
ReadKeyDataLength(const unsigned char*& d);

internal_key_type ReadKey(const unsigned char* d, unsigned);
data_type ReadData(Selector, const unsigned char* d, unsigned DataLen);
};


using ASTSelectorLookupTable =
llvm::OnDiskChainedHashTable<ASTSelectorLookupTrait>;










class HeaderFileInfoTrait {
ASTReader &Reader;
ModuleFile &M;
HeaderSearch *HS;
const char *FrameworkStrings;

public:
using external_key_type = const FileEntry *;

struct internal_key_type {
off_t Size;
time_t ModTime;
StringRef Filename;
bool Imported;
};

using internal_key_ref = const internal_key_type &;

using data_type = HeaderFileInfo;
using hash_value_type = unsigned;
using offset_type = unsigned;

HeaderFileInfoTrait(ASTReader &Reader, ModuleFile &M, HeaderSearch *HS,
const char *FrameworkStrings)
: Reader(Reader), M(M), HS(HS), FrameworkStrings(FrameworkStrings) {}

static hash_value_type ComputeHash(internal_key_ref ikey);
internal_key_type GetInternalKey(const FileEntry *FE);
bool EqualKey(internal_key_ref a, internal_key_ref b);

static std::pair<unsigned, unsigned>
ReadKeyDataLength(const unsigned char*& d);

static internal_key_type ReadKey(const unsigned char *d, unsigned);

data_type ReadData(internal_key_ref,const unsigned char *d, unsigned DataLen);
};


using HeaderFileInfoLookupTable =
llvm::OnDiskChainedHashTable<HeaderFileInfoTrait>;

}

}

}

#endif
