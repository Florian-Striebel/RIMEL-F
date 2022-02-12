












#if !defined(LLVM_CLANG_CODEGEN_SWIFTCALLINGCONV_H)
#define LLVM_CLANG_CODEGEN_SWIFTCALLINGCONV_H

#include "clang/AST/CanonicalType.h"
#include "clang/AST/CharUnits.h"
#include "clang/AST/Type.h"
#include "llvm/Support/TrailingObjects.h"
#include <cassert>

namespace llvm {
class IntegerType;
class Type;
class StructType;
class VectorType;
}

namespace clang {
class Decl;
class FieldDecl;
class ASTRecordLayout;

namespace CodeGen {
class ABIArgInfo;
class CodeGenModule;
class CGFunctionInfo;

namespace swiftcall {

class SwiftAggLowering {
CodeGenModule &CGM;

struct StorageEntry {
CharUnits Begin;
CharUnits End;
llvm::Type *Type;

CharUnits getWidth() const {
return End - Begin;
}
};
SmallVector<StorageEntry, 4> Entries;
bool Finished = false;

public:
SwiftAggLowering(CodeGenModule &CGM) : CGM(CGM) {}

void addOpaqueData(CharUnits begin, CharUnits end) {
addEntry(nullptr, begin, end);
}

void addTypedData(QualType type, CharUnits begin);
void addTypedData(const RecordDecl *record, CharUnits begin);
void addTypedData(const RecordDecl *record, CharUnits begin,
const ASTRecordLayout &layout);
void addTypedData(llvm::Type *type, CharUnits begin);
void addTypedData(llvm::Type *type, CharUnits begin, CharUnits end);

void finish();


bool empty() const {
assert(Finished && "didn't finish lowering before calling empty()");
return Entries.empty();
}












bool shouldPassIndirectly(bool asReturnValue) const;

using EnumerationCallback =
llvm::function_ref<void(CharUnits offset, CharUnits end, llvm::Type *type)>;





void enumerateComponents(EnumerationCallback callback) const;









std::pair<llvm::StructType*, llvm::Type*> getCoerceAndExpandTypes() const;

private:
void addBitFieldData(const FieldDecl *field, CharUnits begin,
uint64_t bitOffset);
void addLegalTypedData(llvm::Type *type, CharUnits begin, CharUnits end);
void addEntry(llvm::Type *type, CharUnits begin, CharUnits end);
void splitVectorEntry(unsigned index);
static bool shouldMergeEntries(const StorageEntry &first,
const StorageEntry &second,
CharUnits chunkSize);
};



bool shouldPassIndirectly(CodeGenModule &CGM,
ArrayRef<llvm::Type*> types,
bool asReturnValue);


CharUnits getMaximumVoluntaryIntegerSize(CodeGenModule &CGM);


CharUnits getNaturalAlignment(CodeGenModule &CGM, llvm::Type *type);



bool isLegalIntegerType(CodeGenModule &CGM, llvm::IntegerType *type);



bool isLegalVectorType(CodeGenModule &CGM, CharUnits vectorSize,
llvm::VectorType *vectorTy);
bool isLegalVectorType(CodeGenModule &CGM, CharUnits vectorSize,
llvm::Type *eltTy, unsigned numElts);


std::pair<llvm::Type*, unsigned>
splitLegalVectorType(CodeGenModule &CGM, CharUnits vectorSize,
llvm::VectorType *vectorTy);





void legalizeVectorType(CodeGenModule &CGM, CharUnits vectorSize,
llvm::VectorType *vectorTy,
llvm::SmallVectorImpl<llvm::Type*> &types);









bool mustPassRecordIndirectly(CodeGenModule &CGM, const RecordDecl *record);


ABIArgInfo classifyReturnType(CodeGenModule &CGM, CanQualType type);


ABIArgInfo classifyArgumentType(CodeGenModule &CGM, CanQualType type);



void computeABIInfo(CodeGenModule &CGM, CGFunctionInfo &FI);


bool isSwiftErrorLoweredInRegister(CodeGenModule &CGM);

}
}
}

#endif
