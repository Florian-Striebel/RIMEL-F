












#if !defined(LLVM_CLANG_LIB_CODEGEN_CODEGENTBAA_H)
#define LLVM_CLANG_LIB_CODEGEN_CODEGENTBAA_H

#include "clang/AST/Type.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/Metadata.h"

namespace clang {
class ASTContext;
class CodeGenOptions;
class LangOptions;
class MangleContext;
class QualType;
class Type;

namespace CodeGen {
class CGRecordLayout;


enum class TBAAAccessKind : unsigned {
Ordinary,
MayAlias,
Incomplete,
};


struct TBAAAccessInfo {
TBAAAccessInfo(TBAAAccessKind Kind, llvm::MDNode *BaseType,
llvm::MDNode *AccessType, uint64_t Offset, uint64_t Size)
: Kind(Kind), BaseType(BaseType), AccessType(AccessType),
Offset(Offset), Size(Size)
{}

TBAAAccessInfo(llvm::MDNode *BaseType, llvm::MDNode *AccessType,
uint64_t Offset, uint64_t Size)
: TBAAAccessInfo(TBAAAccessKind::Ordinary, BaseType, AccessType,
Offset, Size)
{}

explicit TBAAAccessInfo(llvm::MDNode *AccessType, uint64_t Size)
: TBAAAccessInfo( nullptr, AccessType, 0, Size)
{}

TBAAAccessInfo()
: TBAAAccessInfo( nullptr, 0)
{}

static TBAAAccessInfo getMayAliasInfo() {
return TBAAAccessInfo(TBAAAccessKind::MayAlias,
nullptr, nullptr,
0, 0);
}

bool isMayAlias() const { return Kind == TBAAAccessKind::MayAlias; }

static TBAAAccessInfo getIncompleteInfo() {
return TBAAAccessInfo(TBAAAccessKind::Incomplete,
nullptr, nullptr,
0, 0);
}

bool isIncomplete() const { return Kind == TBAAAccessKind::Incomplete; }

bool operator==(const TBAAAccessInfo &Other) const {
return Kind == Other.Kind &&
BaseType == Other.BaseType &&
AccessType == Other.AccessType &&
Offset == Other.Offset &&
Size == Other.Size;
}

bool operator!=(const TBAAAccessInfo &Other) const {
return !(*this == Other);
}

explicit operator bool() const {
return *this != TBAAAccessInfo();
}


TBAAAccessKind Kind;




llvm::MDNode *BaseType;



llvm::MDNode *AccessType;



uint64_t Offset;


uint64_t Size;
};



class CodeGenTBAA {
ASTContext &Context;
llvm::Module &Module;
const CodeGenOptions &CodeGenOpts;
const LangOptions &Features;
MangleContext &MContext;


llvm::MDBuilder MDHelper;



llvm::DenseMap<const Type *, llvm::MDNode *> MetadataCache;

llvm::DenseMap<const Type *, llvm::MDNode *> BaseTypeMetadataCache;

llvm::DenseMap<TBAAAccessInfo, llvm::MDNode *> AccessTagMetadataCache;



llvm::DenseMap<const Type *, llvm::MDNode *> StructMetadataCache;

llvm::MDNode *Root;
llvm::MDNode *Char;



llvm::MDNode *getRoot();



llvm::MDNode *getChar();



bool CollectFields(uint64_t BaseOffset,
QualType Ty,
SmallVectorImpl<llvm::MDBuilder::TBAAStructField> &Fields,
bool MayAlias);



llvm::MDNode *createScalarTypeNode(StringRef Name, llvm::MDNode *Parent,
uint64_t Size);



llvm::MDNode *getTypeInfoHelper(const Type *Ty);



llvm::MDNode *getBaseTypeInfoHelper(const Type *Ty);

public:
CodeGenTBAA(ASTContext &Ctx, llvm::Module &M, const CodeGenOptions &CGO,
const LangOptions &Features, MangleContext &MContext);
~CodeGenTBAA();



llvm::MDNode *getTypeInfo(QualType QTy);



TBAAAccessInfo getAccessInfo(QualType AccessType);



TBAAAccessInfo getVTablePtrAccessInfo(llvm::Type *VTablePtrType);



llvm::MDNode *getTBAAStructInfo(QualType QTy);



llvm::MDNode *getBaseTypeInfo(QualType QTy);


llvm::MDNode *getAccessTagInfo(TBAAAccessInfo Info);



TBAAAccessInfo mergeTBAAInfoForCast(TBAAAccessInfo SourceInfo,
TBAAAccessInfo TargetInfo);



TBAAAccessInfo mergeTBAAInfoForConditionalOperator(TBAAAccessInfo InfoA,
TBAAAccessInfo InfoB);



TBAAAccessInfo mergeTBAAInfoForMemoryTransfer(TBAAAccessInfo DestInfo,
TBAAAccessInfo SrcInfo);
};

}
}

namespace llvm {

template<> struct DenseMapInfo<clang::CodeGen::TBAAAccessInfo> {
static clang::CodeGen::TBAAAccessInfo getEmptyKey() {
unsigned UnsignedKey = DenseMapInfo<unsigned>::getEmptyKey();
return clang::CodeGen::TBAAAccessInfo(
static_cast<clang::CodeGen::TBAAAccessKind>(UnsignedKey),
DenseMapInfo<MDNode *>::getEmptyKey(),
DenseMapInfo<MDNode *>::getEmptyKey(),
DenseMapInfo<uint64_t>::getEmptyKey(),
DenseMapInfo<uint64_t>::getEmptyKey());
}

static clang::CodeGen::TBAAAccessInfo getTombstoneKey() {
unsigned UnsignedKey = DenseMapInfo<unsigned>::getTombstoneKey();
return clang::CodeGen::TBAAAccessInfo(
static_cast<clang::CodeGen::TBAAAccessKind>(UnsignedKey),
DenseMapInfo<MDNode *>::getTombstoneKey(),
DenseMapInfo<MDNode *>::getTombstoneKey(),
DenseMapInfo<uint64_t>::getTombstoneKey(),
DenseMapInfo<uint64_t>::getTombstoneKey());
}

static unsigned getHashValue(const clang::CodeGen::TBAAAccessInfo &Val) {
auto KindValue = static_cast<unsigned>(Val.Kind);
return DenseMapInfo<unsigned>::getHashValue(KindValue) ^
DenseMapInfo<MDNode *>::getHashValue(Val.BaseType) ^
DenseMapInfo<MDNode *>::getHashValue(Val.AccessType) ^
DenseMapInfo<uint64_t>::getHashValue(Val.Offset) ^
DenseMapInfo<uint64_t>::getHashValue(Val.Size);
}

static bool isEqual(const clang::CodeGen::TBAAAccessInfo &LHS,
const clang::CodeGen::TBAAAccessInfo &RHS) {
return LHS == RHS;
}
};

}

#endif
