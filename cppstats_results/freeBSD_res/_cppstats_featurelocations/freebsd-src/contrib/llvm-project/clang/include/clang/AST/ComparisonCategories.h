












#if !defined(LLVM_CLANG_AST_COMPARISONCATEGORIES_H)
#define LLVM_CLANG_AST_COMPARISONCATEGORIES_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/DenseMap.h"
#include <array>
#include <cassert>
#include <vector>

namespace llvm {
class StringRef;
class APSInt;
}

namespace clang {

class ASTContext;
class VarDecl;
class CXXRecordDecl;
class Sema;
class QualType;
class NamespaceDecl;







enum class ComparisonCategoryType : unsigned char {
PartialOrdering,
WeakOrdering,
StrongOrdering,
First = PartialOrdering,
Last = StrongOrdering
};



inline ComparisonCategoryType commonComparisonType(ComparisonCategoryType A,
ComparisonCategoryType B) {
return A < B ? A : B;
}



Optional<ComparisonCategoryType> getComparisonCategoryForBuiltinCmp(QualType T);




enum class ComparisonCategoryResult : unsigned char {
Equal,
Equivalent,
Less,
Greater,
Unordered,
Last = Unordered
};

class ComparisonCategoryInfo {
friend class ComparisonCategories;
friend class Sema;

public:
ComparisonCategoryInfo(const ASTContext &Ctx, CXXRecordDecl *RD,
ComparisonCategoryType Kind)
: Ctx(Ctx), Record(RD), Kind(Kind) {}

struct ValueInfo {
ComparisonCategoryResult Kind;
VarDecl *VD;

ValueInfo(ComparisonCategoryResult Kind, VarDecl *VD)
: Kind(Kind), VD(VD) {}



bool hasValidIntValue() const;



llvm::APSInt getIntValue() const;
};
private:
const ASTContext &Ctx;



mutable llvm::SmallVector<
ValueInfo, static_cast<unsigned>(ComparisonCategoryResult::Last) + 1>
Objects;






ValueInfo *lookupValueInfo(ComparisonCategoryResult ValueKind) const;

public:



CXXRecordDecl *Record = nullptr;


ComparisonCategoryType Kind;

public:
QualType getType() const;

const ValueInfo *getValueInfo(ComparisonCategoryResult ValueKind) const {
ValueInfo *Info = lookupValueInfo(ValueKind);
assert(Info &&
"comparison category does not contain the specified result kind");
assert(Info->hasValidIntValue() &&
"couldn't determine the integer constant for this value");
return Info;
}



bool isStrong() const {
using CCK = ComparisonCategoryType;
return Kind == CCK::StrongOrdering;
}


bool isPartial() const {
using CCK = ComparisonCategoryType;
return Kind == CCK::PartialOrdering;
}




ComparisonCategoryResult makeWeakResult(ComparisonCategoryResult Res) const {
using CCR = ComparisonCategoryResult;
if (!isStrong() && Res == CCR::Equal)
return CCR::Equivalent;
return Res;
}

const ValueInfo *getEqualOrEquiv() const {
return getValueInfo(makeWeakResult(ComparisonCategoryResult::Equal));
}
const ValueInfo *getLess() const {
return getValueInfo(ComparisonCategoryResult::Less);
}
const ValueInfo *getGreater() const {
return getValueInfo(ComparisonCategoryResult::Greater);
}
const ValueInfo *getUnordered() const {
assert(isPartial());
return getValueInfo(ComparisonCategoryResult::Unordered);
}
};

class ComparisonCategories {
public:
static StringRef getCategoryString(ComparisonCategoryType Kind);
static StringRef getResultString(ComparisonCategoryResult Kind);



static std::vector<ComparisonCategoryResult>
getPossibleResultsForType(ComparisonCategoryType Type);



const ComparisonCategoryInfo &getInfo(ComparisonCategoryType Kind) const {
const ComparisonCategoryInfo *Result = lookupInfo(Kind);
assert(Result != nullptr &&
"information for specified comparison category has not been built");
return *Result;
}






const ComparisonCategoryInfo &getInfoForType(QualType Ty) const;

public:




const ComparisonCategoryInfo *lookupInfo(ComparisonCategoryType Kind) const;

ComparisonCategoryInfo *lookupInfo(ComparisonCategoryType Kind) {
const auto &This = *this;
return const_cast<ComparisonCategoryInfo *>(This.lookupInfo(Kind));
}

const ComparisonCategoryInfo *lookupInfoForType(QualType Ty) const;

private:
friend class ASTContext;

explicit ComparisonCategories(const ASTContext &Ctx) : Ctx(Ctx) {}

const ASTContext &Ctx;



mutable llvm::DenseMap<char, ComparisonCategoryInfo> Data;
mutable NamespaceDecl *StdNS = nullptr;
};

}

#endif
