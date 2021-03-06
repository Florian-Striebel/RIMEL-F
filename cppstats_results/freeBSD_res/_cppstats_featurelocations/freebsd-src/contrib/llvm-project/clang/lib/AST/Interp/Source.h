











#if !defined(LLVM_CLANG_AST_INTERP_SOURCE_H)
#define LLVM_CLANG_AST_INTERP_SOURCE_H

#include "clang/AST/Decl.h"
#include "clang/AST/Stmt.h"
#include "llvm/Support/Endian.h"

namespace clang {
namespace interp {
class Function;


class CodePtr {
public:
CodePtr() : Ptr(nullptr) {}

CodePtr &operator+=(int32_t Offset) {
Ptr += Offset;
return *this;
}

int32_t operator-(const CodePtr &RHS) const {
assert(Ptr != nullptr && RHS.Ptr != nullptr && "Invalid code pointer");
return Ptr - RHS.Ptr;
}

CodePtr operator-(size_t RHS) const {
assert(Ptr != nullptr && "Invalid code pointer");
return CodePtr(Ptr - RHS);
}

bool operator!=(const CodePtr &RHS) const { return Ptr != RHS.Ptr; }


template <typename T> T read() {
T Value = ReadHelper<T>(Ptr);
Ptr += sizeof(T);
return Value;
}

private:

CodePtr(const char *Ptr) : Ptr(Ptr) {}


template <typename T>
static std::enable_if_t<!std::is_pointer<T>::value, T>
ReadHelper(const char *Ptr) {
using namespace llvm::support;
return endian::read<T, endianness::native, 1>(Ptr);
}

template <typename T>
static std::enable_if_t<std::is_pointer<T>::value, T>
ReadHelper(const char *Ptr) {
using namespace llvm::support;
auto Punned = endian::read<uintptr_t, endianness::native, 1>(Ptr);
return reinterpret_cast<T>(Punned);
}

private:
friend class Function;


const char *Ptr;
};


class SourceInfo {
public:
SourceInfo() {}
SourceInfo(const Stmt *E) : Source(E) {}
SourceInfo(const Decl *D) : Source(D) {}

SourceLocation getLoc() const;

const Stmt *asStmt() const { return Source.dyn_cast<const Stmt *>(); }
const Decl *asDecl() const { return Source.dyn_cast<const Decl *>(); }
const Expr *asExpr() const;

operator bool() const { return !Source.isNull(); }

private:
llvm::PointerUnion<const Decl *, const Stmt *> Source;
};

using SourceMap = std::vector<std::pair<unsigned, SourceInfo>>;


class SourceMapper {
public:
virtual ~SourceMapper() {}


virtual SourceInfo getSource(Function *F, CodePtr PC) const = 0;


const Expr *getExpr(Function *F, CodePtr PC) const;

SourceLocation getLocation(Function *F, CodePtr PC) const;
};

}
}

#endif
