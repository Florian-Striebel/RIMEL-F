







#if !defined(LLVM_CLANG_FRONTEND_LAYOUTOVERRIDESOURCE_H)
#define LLVM_CLANG_FRONTEND_LAYOUTOVERRIDESOURCE_H

#include "clang/AST/ExternalASTSource.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

namespace clang {






class LayoutOverrideSource : public ExternalASTSource {

struct Layout {

uint64_t Size;


uint64_t Align;


SmallVector<uint64_t, 8> FieldOffsets;
};


llvm::StringMap<Layout> Layouts;

public:




explicit LayoutOverrideSource(StringRef Filename);



bool
layoutRecordType(const RecordDecl *Record,
uint64_t &Size, uint64_t &Alignment,
llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets,
llvm::DenseMap<const CXXRecordDecl *, CharUnits> &BaseOffsets,
llvm::DenseMap<const CXXRecordDecl *,
CharUnits> &VirtualBaseOffsets) override;


void dump();
};
}

#endif
