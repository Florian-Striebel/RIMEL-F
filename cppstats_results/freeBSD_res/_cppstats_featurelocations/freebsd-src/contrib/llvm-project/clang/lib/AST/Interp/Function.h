













#if !defined(LLVM_CLANG_AST_INTERP_FUNCTION_H)
#define LLVM_CLANG_AST_INTERP_FUNCTION_H

#include "Pointer.h"
#include "Source.h"
#include "clang/AST/Decl.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {
namespace interp {
class Program;
class ByteCodeEmitter;
enum PrimType : uint32_t;




class Scope {
public:

struct Local {

unsigned Offset;

Descriptor *Desc;
};

using LocalVectorTy = llvm::SmallVector<Local, 8>;

Scope(LocalVectorTy &&Descriptors) : Descriptors(std::move(Descriptors)) {}

llvm::iterator_range<LocalVectorTy::iterator> locals() {
return llvm::make_range(Descriptors.begin(), Descriptors.end());
}

private:

LocalVectorTy Descriptors;
};





class Function {
public:
using ParamDescriptor = std::pair<PrimType, Descriptor *>;


unsigned getFrameSize() const { return FrameSize; }

unsigned getArgSize() const { return ArgSize; }


CodePtr getCodeBegin() const;

CodePtr getCodeEnd() const;


const FunctionDecl *getDecl() const { return F; }


SourceLocation getLoc() const { return Loc; }


ParamDescriptor getParamDescriptor(unsigned Offset) const;


bool hasRVO() const { return ParamTypes.size() != Params.size(); }


llvm::iterator_range<llvm::SmallVector<Scope, 2>::iterator> scopes() {
return llvm::make_range(Scopes.begin(), Scopes.end());
}


using arg_reverse_iterator = SmallVectorImpl<PrimType>::reverse_iterator;
llvm::iterator_range<arg_reverse_iterator> args_reverse() {
return llvm::make_range(ParamTypes.rbegin(), ParamTypes.rend());
}


Scope &getScope(unsigned Idx) { return Scopes[Idx]; }


SourceInfo getSource(CodePtr PC) const;


bool isConstexpr() const { return IsValid; }


bool isVirtual() const;


bool isConstructor() const { return isa<CXXConstructorDecl>(F); }

private:

Function(Program &P, const FunctionDecl *F, unsigned ArgSize,
llvm::SmallVector<PrimType, 8> &&ParamTypes,
llvm::DenseMap<unsigned, ParamDescriptor> &&Params);


void setCode(unsigned NewFrameSize, std::vector<char> &&NewCode, SourceMap &&NewSrcMap,
llvm::SmallVector<Scope, 2> &&NewScopes) {
FrameSize = NewFrameSize;
Code = std::move(NewCode);
SrcMap = std::move(NewSrcMap);
Scopes = std::move(NewScopes);
IsValid = true;
}

private:
friend class Program;
friend class ByteCodeEmitter;


Program &P;

SourceLocation Loc;

const FunctionDecl *F;

unsigned FrameSize;

unsigned ArgSize;

std::vector<char> Code;

SourceMap SrcMap;

llvm::SmallVector<Scope, 2> Scopes;

llvm::SmallVector<PrimType, 8> ParamTypes;

llvm::DenseMap<unsigned, ParamDescriptor> Params;

bool IsValid = false;

public:

void dump() const;
void dump(llvm::raw_ostream &OS) const;
};

}
}

#endif
