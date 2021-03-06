














#if !defined(LLVM_CLANG_ANALYSIS_CONSTRUCTIONCONTEXT_H)
#define LLVM_CLANG_ANALYSIS_CONSTRUCTIONCONTEXT_H

#include "clang/Analysis/Support/BumpVector.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"

namespace clang {




class ConstructionContextItem {
public:
enum ItemKind {
VariableKind,
NewAllocatorKind,
ReturnKind,
MaterializationKind,
TemporaryDestructorKind,
ElidedDestructorKind,
ElidableConstructorKind,
ArgumentKind,
STATEMENT_WITH_INDEX_KIND_BEGIN=ArgumentKind,
STATEMENT_WITH_INDEX_KIND_END=ArgumentKind,
STATEMENT_KIND_BEGIN = VariableKind,
STATEMENT_KIND_END = ArgumentKind,
InitializerKind,
INITIALIZER_KIND_BEGIN=InitializerKind,
INITIALIZER_KIND_END=InitializerKind
};

LLVM_DUMP_METHOD static StringRef getKindAsString(ItemKind K) {
switch (K) {
case VariableKind: return "construct into local variable";
case NewAllocatorKind: return "construct into new-allocator";
case ReturnKind: return "construct into return address";
case MaterializationKind: return "materialize temporary";
case TemporaryDestructorKind: return "destroy temporary";
case ElidedDestructorKind: return "elide destructor";
case ElidableConstructorKind: return "elide constructor";
case ArgumentKind: return "construct into argument";
case InitializerKind: return "construct into member variable";
};
llvm_unreachable("Unknown ItemKind");
}

private:
const void *const Data;
const ItemKind Kind;
const unsigned Index = 0;

bool hasStatement() const {
return Kind >= STATEMENT_KIND_BEGIN &&
Kind <= STATEMENT_KIND_END;
}

bool hasIndex() const {
return Kind >= STATEMENT_WITH_INDEX_KIND_BEGIN &&
Kind >= STATEMENT_WITH_INDEX_KIND_END;
}

bool hasInitializer() const {
return Kind >= INITIALIZER_KIND_BEGIN &&
Kind <= INITIALIZER_KIND_END;
}

public:



ConstructionContextItem(const DeclStmt *DS)
: Data(DS), Kind(VariableKind) {}

ConstructionContextItem(const CXXNewExpr *NE)
: Data(NE), Kind(NewAllocatorKind) {}

ConstructionContextItem(const ReturnStmt *RS)
: Data(RS), Kind(ReturnKind) {}

ConstructionContextItem(const MaterializeTemporaryExpr *MTE)
: Data(MTE), Kind(MaterializationKind) {}

ConstructionContextItem(const CXXBindTemporaryExpr *BTE,
bool IsElided = false)
: Data(BTE),
Kind(IsElided ? ElidedDestructorKind : TemporaryDestructorKind) {}

ConstructionContextItem(const CXXConstructExpr *CE)
: Data(CE), Kind(ElidableConstructorKind) {}

ConstructionContextItem(const CallExpr *CE, unsigned Index)
: Data(CE), Kind(ArgumentKind), Index(Index) {}

ConstructionContextItem(const CXXConstructExpr *CE, unsigned Index)
: Data(CE), Kind(ArgumentKind), Index(Index) {}

ConstructionContextItem(const CXXInheritedCtorInitExpr *CE, unsigned Index)
: Data(CE), Kind(ArgumentKind), Index(Index) {}

ConstructionContextItem(const ObjCMessageExpr *ME, unsigned Index)
: Data(ME), Kind(ArgumentKind), Index(Index) {}


ConstructionContextItem(const Expr *E, unsigned Index)
: Data(E), Kind(ArgumentKind), Index(Index) {
assert(isa<CallExpr>(E) || isa<CXXConstructExpr>(E) ||
isa<CXXInheritedCtorInitExpr>(E) || isa<ObjCMessageExpr>(E));
}

ConstructionContextItem(const CXXCtorInitializer *Init)
: Data(Init), Kind(InitializerKind), Index(0) {}

ItemKind getKind() const { return Kind; }

LLVM_DUMP_METHOD StringRef getKindAsString() const {
return getKindAsString(getKind());
}





const Stmt *getStmt() const {
assert(hasStatement());
return static_cast<const Stmt *>(Data);
}

const Stmt *getStmtOrNull() const {
return hasStatement() ? getStmt() : nullptr;
}




const CXXCtorInitializer *getCXXCtorInitializer() const {
assert(hasInitializer());
return static_cast<const CXXCtorInitializer *>(Data);
}





unsigned getIndex() const {


assert(hasIndex());
return Index;
}

void Profile(llvm::FoldingSetNodeID &ID) const {
ID.AddPointer(Data);
ID.AddInteger(Kind);
ID.AddInteger(Index);
}

bool operator==(const ConstructionContextItem &Other) const {



return std::make_tuple(Data, Kind, Index) ==
std::make_tuple(Other.Data, Other.Kind, Other.Index);
}

bool operator<(const ConstructionContextItem &Other) const {
return std::make_tuple(Data, Kind, Index) <
std::make_tuple(Other.Data, Other.Kind, Other.Index);
}
};




















class ConstructionContextLayer {
const ConstructionContextLayer *Parent = nullptr;
ConstructionContextItem Item;

ConstructionContextLayer(ConstructionContextItem Item,
const ConstructionContextLayer *Parent)
: Parent(Parent), Item(Item) {}

public:
static const ConstructionContextLayer *
create(BumpVectorContext &C, const ConstructionContextItem &Item,
const ConstructionContextLayer *Parent = nullptr);

const ConstructionContextItem &getItem() const { return Item; }
const ConstructionContextLayer *getParent() const { return Parent; }
bool isLast() const { return !Parent; }






bool isStrictlyMoreSpecificThan(const ConstructionContextLayer *Other) const;
};






class ConstructionContext {
public:
enum Kind {
SimpleVariableKind,
CXX17ElidedCopyVariableKind,
VARIABLE_BEGIN = SimpleVariableKind,
VARIABLE_END = CXX17ElidedCopyVariableKind,
SimpleConstructorInitializerKind,
CXX17ElidedCopyConstructorInitializerKind,
INITIALIZER_BEGIN = SimpleConstructorInitializerKind,
INITIALIZER_END = CXX17ElidedCopyConstructorInitializerKind,
NewAllocatedObjectKind,
SimpleTemporaryObjectKind,
ElidedTemporaryObjectKind,
TEMPORARY_BEGIN = SimpleTemporaryObjectKind,
TEMPORARY_END = ElidedTemporaryObjectKind,
SimpleReturnedValueKind,
CXX17ElidedCopyReturnedValueKind,
RETURNED_VALUE_BEGIN = SimpleReturnedValueKind,
RETURNED_VALUE_END = CXX17ElidedCopyReturnedValueKind,
ArgumentKind
};

protected:
Kind K;



explicit ConstructionContext(Kind K) : K(K) {}

private:

template <typename T, typename... ArgTypes>
static T *create(BumpVectorContext &C, ArgTypes... Args) {
auto *CC = C.getAllocator().Allocate<T>();
return new (CC) T(Args...);
}




static const ConstructionContext *createMaterializedTemporaryFromLayers(
BumpVectorContext &C, const MaterializeTemporaryExpr *MTE,
const CXXBindTemporaryExpr *BTE,
const ConstructionContextLayer *ParentLayer);





static const ConstructionContext *
createBoundTemporaryFromLayers(
BumpVectorContext &C, const CXXBindTemporaryExpr *BTE,
const ConstructionContextLayer *ParentLayer);

public:



static const ConstructionContext *
createFromLayers(BumpVectorContext &C,
const ConstructionContextLayer *TopLayer);

Kind getKind() const { return K; }
};


class VariableConstructionContext : public ConstructionContext {
const DeclStmt *DS;

protected:
VariableConstructionContext(ConstructionContext::Kind K, const DeclStmt *DS)
: ConstructionContext(K), DS(DS) {
assert(classof(this));
assert(DS);
}

public:
const DeclStmt *getDeclStmt() const { return DS; }

static bool classof(const ConstructionContext *CC) {
return CC->getKind() >= VARIABLE_BEGIN &&
CC->getKind() <= VARIABLE_END;
}
};





class SimpleVariableConstructionContext : public VariableConstructionContext {
friend class ConstructionContext;

explicit SimpleVariableConstructionContext(const DeclStmt *DS)
: VariableConstructionContext(ConstructionContext::SimpleVariableKind,
DS) {}

public:
static bool classof(const ConstructionContext *CC) {
return CC->getKind() == SimpleVariableKind;
}
};










class CXX17ElidedCopyVariableConstructionContext
: public VariableConstructionContext {
const CXXBindTemporaryExpr *BTE;

friend class ConstructionContext;

explicit CXX17ElidedCopyVariableConstructionContext(
const DeclStmt *DS, const CXXBindTemporaryExpr *BTE)
: VariableConstructionContext(CXX17ElidedCopyVariableKind, DS), BTE(BTE) {
assert(BTE);
}

public:
const CXXBindTemporaryExpr *getCXXBindTemporaryExpr() const { return BTE; }

static bool classof(const ConstructionContext *CC) {
return CC->getKind() == CXX17ElidedCopyVariableKind;
}
};


class ConstructorInitializerConstructionContext : public ConstructionContext {
const CXXCtorInitializer *I;

protected:
explicit ConstructorInitializerConstructionContext(
ConstructionContext::Kind K, const CXXCtorInitializer *I)
: ConstructionContext(K), I(I) {
assert(classof(this));
assert(I);
}

public:
const CXXCtorInitializer *getCXXCtorInitializer() const { return I; }

static bool classof(const ConstructionContext *CC) {
return CC->getKind() >= INITIALIZER_BEGIN &&
CC->getKind() <= INITIALIZER_END;
}
};



class SimpleConstructorInitializerConstructionContext
: public ConstructorInitializerConstructionContext {
friend class ConstructionContext;

explicit SimpleConstructorInitializerConstructionContext(
const CXXCtorInitializer *I)
: ConstructorInitializerConstructionContext(
ConstructionContext::SimpleConstructorInitializerKind, I) {}

public:
static bool classof(const ConstructionContext *CC) {
return CC->getKind() == SimpleConstructorInitializerKind;
}
};











class CXX17ElidedCopyConstructorInitializerConstructionContext
: public ConstructorInitializerConstructionContext {
const CXXBindTemporaryExpr *BTE;

friend class ConstructionContext;

explicit CXX17ElidedCopyConstructorInitializerConstructionContext(
const CXXCtorInitializer *I, const CXXBindTemporaryExpr *BTE)
: ConstructorInitializerConstructionContext(
CXX17ElidedCopyConstructorInitializerKind, I),
BTE(BTE) {
assert(BTE);
}

public:
const CXXBindTemporaryExpr *getCXXBindTemporaryExpr() const { return BTE; }

static bool classof(const ConstructionContext *CC) {
return CC->getKind() == CXX17ElidedCopyConstructorInitializerKind;
}
};



class NewAllocatedObjectConstructionContext : public ConstructionContext {
const CXXNewExpr *NE;

friend class ConstructionContext;

explicit NewAllocatedObjectConstructionContext(const CXXNewExpr *NE)
: ConstructionContext(ConstructionContext::NewAllocatedObjectKind),
NE(NE) {
assert(NE);
}

public:
const CXXNewExpr *getCXXNewExpr() const { return NE; }

static bool classof(const ConstructionContext *CC) {
return CC->getKind() == NewAllocatedObjectKind;
}
};





class TemporaryObjectConstructionContext : public ConstructionContext {
const CXXBindTemporaryExpr *BTE;
const MaterializeTemporaryExpr *MTE;

protected:
explicit TemporaryObjectConstructionContext(
ConstructionContext::Kind K, const CXXBindTemporaryExpr *BTE,
const MaterializeTemporaryExpr *MTE)
: ConstructionContext(K), BTE(BTE), MTE(MTE) {




}

public:


const CXXBindTemporaryExpr *getCXXBindTemporaryExpr() const {
return BTE;
}






const MaterializeTemporaryExpr *getMaterializedTemporaryExpr() const {
return MTE;
}

static bool classof(const ConstructionContext *CC) {
return CC->getKind() >= TEMPORARY_BEGIN && CC->getKind() <= TEMPORARY_END;
}
};





class SimpleTemporaryObjectConstructionContext
: public TemporaryObjectConstructionContext {
friend class ConstructionContext;

explicit SimpleTemporaryObjectConstructionContext(
const CXXBindTemporaryExpr *BTE, const MaterializeTemporaryExpr *MTE)
: TemporaryObjectConstructionContext(
ConstructionContext::SimpleTemporaryObjectKind, BTE, MTE) {}

public:
static bool classof(const ConstructionContext *CC) {
return CC->getKind() == SimpleTemporaryObjectKind;
}
};









class ElidedTemporaryObjectConstructionContext
: public TemporaryObjectConstructionContext {
const CXXConstructExpr *ElidedCE;
const ConstructionContext *ElidedCC;

friend class ConstructionContext;

explicit ElidedTemporaryObjectConstructionContext(
const CXXBindTemporaryExpr *BTE, const MaterializeTemporaryExpr *MTE,
const CXXConstructExpr *ElidedCE, const ConstructionContext *ElidedCC)
: TemporaryObjectConstructionContext(
ConstructionContext::ElidedTemporaryObjectKind, BTE, MTE),
ElidedCE(ElidedCE), ElidedCC(ElidedCC) {



assert(ElidedCE && ElidedCE->isElidable() && ElidedCC);
}

public:
const CXXConstructExpr *getConstructorAfterElision() const {
return ElidedCE;
}

const ConstructionContext *getConstructionContextAfterElision() const {
return ElidedCC;
}

static bool classof(const ConstructionContext *CC) {
return CC->getKind() == ElidedTemporaryObjectKind;
}
};

class ReturnedValueConstructionContext : public ConstructionContext {
const ReturnStmt *RS;

protected:
explicit ReturnedValueConstructionContext(ConstructionContext::Kind K,
const ReturnStmt *RS)
: ConstructionContext(K), RS(RS) {
assert(classof(this));
assert(RS);
}

public:
const ReturnStmt *getReturnStmt() const { return RS; }

static bool classof(const ConstructionContext *CC) {
return CC->getKind() >= RETURNED_VALUE_BEGIN &&
CC->getKind() <= RETURNED_VALUE_END;
}
};






class SimpleReturnedValueConstructionContext
: public ReturnedValueConstructionContext {
friend class ConstructionContext;

explicit SimpleReturnedValueConstructionContext(const ReturnStmt *RS)
: ReturnedValueConstructionContext(
ConstructionContext::SimpleReturnedValueKind, RS) {}

public:
static bool classof(const ConstructionContext *CC) {
return CC->getKind() == SimpleReturnedValueKind;
}
};









class CXX17ElidedCopyReturnedValueConstructionContext
: public ReturnedValueConstructionContext {
const CXXBindTemporaryExpr *BTE;

friend class ConstructionContext;

explicit CXX17ElidedCopyReturnedValueConstructionContext(
const ReturnStmt *RS, const CXXBindTemporaryExpr *BTE)
: ReturnedValueConstructionContext(
ConstructionContext::CXX17ElidedCopyReturnedValueKind, RS),
BTE(BTE) {
assert(BTE);
}

public:
const CXXBindTemporaryExpr *getCXXBindTemporaryExpr() const { return BTE; }

static bool classof(const ConstructionContext *CC) {
return CC->getKind() == CXX17ElidedCopyReturnedValueKind;
}
};

class ArgumentConstructionContext : public ConstructionContext {

const Expr *CE;




unsigned Index;


const CXXBindTemporaryExpr *BTE;

friend class ConstructionContext;

explicit ArgumentConstructionContext(const Expr *CE, unsigned Index,
const CXXBindTemporaryExpr *BTE)
: ConstructionContext(ArgumentKind), CE(CE),
Index(Index), BTE(BTE) {
assert(isa<CallExpr>(CE) || isa<CXXConstructExpr>(CE) ||
isa<ObjCMessageExpr>(CE));

}

public:
const Expr *getCallLikeExpr() const { return CE; }
unsigned getIndex() const { return Index; }
const CXXBindTemporaryExpr *getCXXBindTemporaryExpr() const { return BTE; }

static bool classof(const ConstructionContext *CC) {
return CC->getKind() == ArgumentKind;
}
};

}

#endif
