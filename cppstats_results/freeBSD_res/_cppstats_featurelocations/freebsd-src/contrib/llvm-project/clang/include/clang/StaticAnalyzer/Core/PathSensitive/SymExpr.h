











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_SYMEXPR_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_SYMEXPR_H

#include "clang/AST/Type.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/SmallVector.h"
#include <cassert>

namespace clang {
namespace ento {

class MemRegion;



class SymExpr : public llvm::FoldingSetNode {
virtual void anchor();

public:
enum Kind {
#define SYMBOL(Id, Parent) Id##Kind,
#define SYMBOL_RANGE(Id, First, Last) BEGIN_##Id = First, END_##Id = Last,
#include "clang/StaticAnalyzer/Core/PathSensitive/Symbols.def"
};

private:
Kind K;

protected:
SymExpr(Kind k) : K(k) {}

static bool isValidTypeForSymbol(QualType T) {


return !T.isNull() && !T->isVoidType();
}

mutable unsigned Complexity = 0;

public:
virtual ~SymExpr() = default;

Kind getKind() const { return K; }

virtual void dump() const;

virtual void dumpToStream(raw_ostream &os) const {}

virtual QualType getType() const = 0;
virtual void Profile(llvm::FoldingSetNodeID &profile) = 0;






class symbol_iterator {
SmallVector<const SymExpr *, 5> itr;

void expand();

public:
symbol_iterator() = default;
symbol_iterator(const SymExpr *SE);

symbol_iterator &operator++();
const SymExpr *operator*();

bool operator==(const symbol_iterator &X) const;
bool operator!=(const symbol_iterator &X) const;
};

symbol_iterator symbol_begin() const { return symbol_iterator(this); }
static symbol_iterator symbol_end() { return symbol_iterator(); }

virtual unsigned computeComplexity() const = 0;











virtual const MemRegion *getOriginRegion() const { return nullptr; }
};

inline raw_ostream &operator<<(raw_ostream &os,
const clang::ento::SymExpr *SE) {
SE->dumpToStream(os);
return os;
}

using SymbolRef = const SymExpr *;
using SymbolRefSmallVectorTy = SmallVector<SymbolRef, 2>;
using SymbolID = unsigned;



class SymbolData : public SymExpr {
const SymbolID Sym;

void anchor() override;

protected:
SymbolData(Kind k, SymbolID sym) : SymExpr(k), Sym(sym) {
assert(classof(this));
}

public:
~SymbolData() override = default;


virtual StringRef getKindStr() const = 0;

SymbolID getSymbolID() const { return Sym; }

unsigned computeComplexity() const override {
return 1;
};


static inline bool classof(const SymExpr *SE) {
Kind k = SE->getKind();
return k >= BEGIN_SYMBOLS && k <= END_SYMBOLS;
}
};

}
}

#endif
