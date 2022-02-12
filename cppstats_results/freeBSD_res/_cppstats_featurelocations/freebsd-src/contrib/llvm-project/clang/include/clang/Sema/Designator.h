












#if !defined(LLVM_CLANG_SEMA_DESIGNATOR_H)
#define LLVM_CLANG_SEMA_DESIGNATOR_H

#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/SmallVector.h"

namespace clang {

class Expr;
class IdentifierInfo;
class Sema;











class Designator {
public:
enum DesignatorKind {
FieldDesignator, ArrayDesignator, ArrayRangeDesignator
};
private:
Designator() {};

DesignatorKind Kind;

struct FieldDesignatorInfo {
const IdentifierInfo *II;
SourceLocation DotLoc;
SourceLocation NameLoc;
};
struct ArrayDesignatorInfo {
Expr *Index;
SourceLocation LBracketLoc;
mutable SourceLocation RBracketLoc;
};
struct ArrayRangeDesignatorInfo {
Expr *Start, *End;
SourceLocation LBracketLoc, EllipsisLoc;
mutable SourceLocation RBracketLoc;
};

union {
FieldDesignatorInfo FieldInfo;
ArrayDesignatorInfo ArrayInfo;
ArrayRangeDesignatorInfo ArrayRangeInfo;
};

public:

DesignatorKind getKind() const { return Kind; }
bool isFieldDesignator() const { return Kind == FieldDesignator; }
bool isArrayDesignator() const { return Kind == ArrayDesignator; }
bool isArrayRangeDesignator() const { return Kind == ArrayRangeDesignator; }

const IdentifierInfo *getField() const {
assert(isFieldDesignator() && "Invalid accessor");
return FieldInfo.II;
}

SourceLocation getDotLoc() const {
assert(isFieldDesignator() && "Invalid accessor");
return FieldInfo.DotLoc;
}

SourceLocation getFieldLoc() const {
assert(isFieldDesignator() && "Invalid accessor");
return FieldInfo.NameLoc;
}

Expr *getArrayIndex() const {
assert(isArrayDesignator() && "Invalid accessor");
return ArrayInfo.Index;
}

Expr *getArrayRangeStart() const {
assert(isArrayRangeDesignator() && "Invalid accessor");
return ArrayRangeInfo.Start;
}
Expr *getArrayRangeEnd() const {
assert(isArrayRangeDesignator() && "Invalid accessor");
return ArrayRangeInfo.End;
}

SourceLocation getLBracketLoc() const {
assert((isArrayDesignator() || isArrayRangeDesignator()) &&
"Invalid accessor");
if (isArrayDesignator())
return ArrayInfo.LBracketLoc;
else
return ArrayRangeInfo.LBracketLoc;
}

SourceLocation getRBracketLoc() const {
assert((isArrayDesignator() || isArrayRangeDesignator()) &&
"Invalid accessor");
if (isArrayDesignator())
return ArrayInfo.RBracketLoc;
else
return ArrayRangeInfo.RBracketLoc;
}

SourceLocation getEllipsisLoc() const {
assert(isArrayRangeDesignator() && "Invalid accessor");
return ArrayRangeInfo.EllipsisLoc;
}

static Designator getField(const IdentifierInfo *II, SourceLocation DotLoc,
SourceLocation NameLoc) {
Designator D;
D.Kind = FieldDesignator;
new (&D.FieldInfo) FieldDesignatorInfo;
D.FieldInfo.II = II;
D.FieldInfo.DotLoc = DotLoc;
D.FieldInfo.NameLoc = NameLoc;
return D;
}

static Designator getArray(Expr *Index,
SourceLocation LBracketLoc) {
Designator D;
D.Kind = ArrayDesignator;
new (&D.ArrayInfo) ArrayDesignatorInfo;
D.ArrayInfo.Index = Index;
D.ArrayInfo.LBracketLoc = LBracketLoc;
D.ArrayInfo.RBracketLoc = SourceLocation();
return D;
}

static Designator getArrayRange(Expr *Start,
Expr *End,
SourceLocation LBracketLoc,
SourceLocation EllipsisLoc) {
Designator D;
D.Kind = ArrayRangeDesignator;
new (&D.ArrayRangeInfo) ArrayRangeDesignatorInfo;
D.ArrayRangeInfo.Start = Start;
D.ArrayRangeInfo.End = End;
D.ArrayRangeInfo.LBracketLoc = LBracketLoc;
D.ArrayRangeInfo.EllipsisLoc = EllipsisLoc;
D.ArrayRangeInfo.RBracketLoc = SourceLocation();
return D;
}

void setRBracketLoc(SourceLocation RBracketLoc) const {
assert((isArrayDesignator() || isArrayRangeDesignator()) &&
"Invalid accessor");
if (isArrayDesignator())
ArrayInfo.RBracketLoc = RBracketLoc;
else
ArrayRangeInfo.RBracketLoc = RBracketLoc;
}



void ClearExprs(Sema &Actions) {}



void FreeExprs(Sema &Actions) {}
};




class Designation {

SmallVector<Designator, 2> Designators;

public:

void AddDesignator(Designator D) {
Designators.push_back(D);
}

bool empty() const { return Designators.empty(); }

unsigned getNumDesignators() const { return Designators.size(); }
const Designator &getDesignator(unsigned Idx) const {
assert(Idx < Designators.size());
return Designators[Idx];
}



void ClearExprs(Sema &Actions) {}



void FreeExprs(Sema &Actions) {}
};

}

#endif
