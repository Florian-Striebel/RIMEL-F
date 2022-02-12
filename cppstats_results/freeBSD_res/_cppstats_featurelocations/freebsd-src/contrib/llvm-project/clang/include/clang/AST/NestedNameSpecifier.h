












#if !defined(LLVM_CLANG_AST_NESTEDNAMESPECIFIER_H)
#define LLVM_CLANG_AST_NESTEDNAMESPECIFIER_H

#include "clang/AST/DependenceFlags.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/Support/Compiler.h"
#include <cstdint>
#include <cstdlib>
#include <utility>

namespace clang {

class ASTContext;
class CXXRecordDecl;
class IdentifierInfo;
class LangOptions;
class NamespaceAliasDecl;
class NamespaceDecl;
struct PrintingPolicy;
class Type;
class TypeLoc;











class NestedNameSpecifier : public llvm::FoldingSetNode {

enum StoredSpecifierKind {
StoredIdentifier = 0,
StoredDecl = 1,
StoredTypeSpec = 2,
StoredTypeSpecWithTemplate = 3
};







llvm::PointerIntPair<NestedNameSpecifier *, 2, StoredSpecifierKind> Prefix;








void* Specifier = nullptr;

public:


enum SpecifierKind {

Identifier,


Namespace,


NamespaceAlias,


TypeSpec,



TypeSpecWithTemplate,


Global,



Super
};

private:

NestedNameSpecifier() : Prefix(nullptr, StoredIdentifier) {}



NestedNameSpecifier(const NestedNameSpecifier &Other) = default;



static NestedNameSpecifier *FindOrInsert(const ASTContext &Context,
const NestedNameSpecifier &Mockup);

public:
NestedNameSpecifier &operator=(const NestedNameSpecifier &) = delete;






static NestedNameSpecifier *Create(const ASTContext &Context,
NestedNameSpecifier *Prefix,
IdentifierInfo *II);


static NestedNameSpecifier *Create(const ASTContext &Context,
NestedNameSpecifier *Prefix,
const NamespaceDecl *NS);


static NestedNameSpecifier *Create(const ASTContext &Context,
NestedNameSpecifier *Prefix,
NamespaceAliasDecl *Alias);


static NestedNameSpecifier *Create(const ASTContext &Context,
NestedNameSpecifier *Prefix,
bool Template, const Type *T);







static NestedNameSpecifier *Create(const ASTContext &Context,
IdentifierInfo *II);



static NestedNameSpecifier *GlobalSpecifier(const ASTContext &Context);



static NestedNameSpecifier *SuperSpecifier(const ASTContext &Context,
CXXRecordDecl *RD);








NestedNameSpecifier *getPrefix() const { return Prefix.getPointer(); }


SpecifierKind getKind() const;



IdentifierInfo *getAsIdentifier() const {
if (Prefix.getInt() == StoredIdentifier)
return (IdentifierInfo *)Specifier;

return nullptr;
}



NamespaceDecl *getAsNamespace() const;



NamespaceAliasDecl *getAsNamespaceAlias() const;



CXXRecordDecl *getAsRecordDecl() const;


const Type *getAsType() const {
if (Prefix.getInt() == StoredTypeSpec ||
Prefix.getInt() == StoredTypeSpecWithTemplate)
return (const Type *)Specifier;

return nullptr;
}

NestedNameSpecifierDependence getDependence() const;



bool isDependent() const;



bool isInstantiationDependent() const;



bool containsUnexpandedParameterPack() const;


bool containsErrors() const;





void print(raw_ostream &OS, const PrintingPolicy &Policy,
bool ResolveTemplateArguments = false) const;

void Profile(llvm::FoldingSetNodeID &ID) const {
ID.AddPointer(Prefix.getOpaqueValue());
ID.AddPointer(Specifier);
}



void dump(const LangOptions &LO) const;
void dump() const;
void dump(llvm::raw_ostream &OS) const;
void dump(llvm::raw_ostream &OS, const LangOptions &LO) const;
};



class NestedNameSpecifierLoc {
NestedNameSpecifier *Qualifier = nullptr;
void *Data = nullptr;



static unsigned getLocalDataLength(NestedNameSpecifier *Qualifier);



static unsigned getDataLength(NestedNameSpecifier *Qualifier);

public:

NestedNameSpecifierLoc() = default;



NestedNameSpecifierLoc(NestedNameSpecifier *Qualifier, void *Data)
: Qualifier(Qualifier), Data(Data) {}



explicit operator bool() const { return Qualifier; }



bool hasQualifier() const { return Qualifier; }



NestedNameSpecifier *getNestedNameSpecifier() const {
return Qualifier;
}


void *getOpaqueData() const { return Data; }







SourceRange getSourceRange() const LLVM_READONLY;







SourceRange getLocalSourceRange() const;



SourceLocation getBeginLoc() const {
return getSourceRange().getBegin();
}



SourceLocation getEndLoc() const {
return getSourceRange().getEnd();
}



SourceLocation getLocalBeginLoc() const {
return getLocalSourceRange().getBegin();
}



SourceLocation getLocalEndLoc() const {
return getLocalSourceRange().getEnd();
}







NestedNameSpecifierLoc getPrefix() const {
if (!Qualifier)
return *this;

return NestedNameSpecifierLoc(Qualifier->getPrefix(), Data);
}



TypeLoc getTypeLoc() const;



unsigned getDataLength() const { return getDataLength(Qualifier); }

friend bool operator==(NestedNameSpecifierLoc X,
NestedNameSpecifierLoc Y) {
return X.Qualifier == Y.Qualifier && X.Data == Y.Data;
}

friend bool operator!=(NestedNameSpecifierLoc X,
NestedNameSpecifierLoc Y) {
return !(X == Y);
}
};




class NestedNameSpecifierLocBuilder {


NestedNameSpecifier *Representation = nullptr;







char *Buffer = nullptr;



unsigned BufferSize = 0;



unsigned BufferCapacity = 0;

public:
NestedNameSpecifierLocBuilder() = default;
NestedNameSpecifierLocBuilder(const NestedNameSpecifierLocBuilder &Other);

NestedNameSpecifierLocBuilder &
operator=(const NestedNameSpecifierLocBuilder &Other);

~NestedNameSpecifierLocBuilder() {
if (BufferCapacity)
free(Buffer);
}


NestedNameSpecifier *getRepresentation() const { return Representation; }












void Extend(ASTContext &Context, SourceLocation TemplateKWLoc, TypeLoc TL,
SourceLocation ColonColonLoc);












void Extend(ASTContext &Context, IdentifierInfo *Identifier,
SourceLocation IdentifierLoc, SourceLocation ColonColonLoc);












void Extend(ASTContext &Context, NamespaceDecl *Namespace,
SourceLocation NamespaceLoc, SourceLocation ColonColonLoc);













void Extend(ASTContext &Context, NamespaceAliasDecl *Alias,
SourceLocation AliasLoc, SourceLocation ColonColonLoc);



void MakeGlobal(ASTContext &Context, SourceLocation ColonColonLoc);














void MakeSuper(ASTContext &Context, CXXRecordDecl *RD,
SourceLocation SuperLoc, SourceLocation ColonColonLoc);







void MakeTrivial(ASTContext &Context, NestedNameSpecifier *Qualifier,
SourceRange R);



void Adopt(NestedNameSpecifierLoc Other);


SourceRange getSourceRange() const LLVM_READONLY {
return NestedNameSpecifierLoc(Representation, Buffer).getSourceRange();
}






NestedNameSpecifierLoc getWithLocInContext(ASTContext &Context) const;






NestedNameSpecifierLoc getTemporary() const {
return NestedNameSpecifierLoc(Representation, Buffer);
}



void Clear() {
Representation = nullptr;
BufferSize = 0;
}






std::pair<char *, unsigned> getBuffer() const {
return std::make_pair(Buffer, BufferSize);
}
};



inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &DB,
NestedNameSpecifier *NNS) {
DB.AddTaggedVal(reinterpret_cast<intptr_t>(NNS),
DiagnosticsEngine::ak_nestednamespec);
return DB;
}

}

namespace llvm {

template <> struct DenseMapInfo<clang::NestedNameSpecifierLoc> {
using FirstInfo = DenseMapInfo<clang::NestedNameSpecifier *>;
using SecondInfo = DenseMapInfo<void *>;

static clang::NestedNameSpecifierLoc getEmptyKey() {
return clang::NestedNameSpecifierLoc(FirstInfo::getEmptyKey(),
SecondInfo::getEmptyKey());
}

static clang::NestedNameSpecifierLoc getTombstoneKey() {
return clang::NestedNameSpecifierLoc(FirstInfo::getTombstoneKey(),
SecondInfo::getTombstoneKey());
}

static unsigned getHashValue(const clang::NestedNameSpecifierLoc &PairVal) {
return hash_combine(
FirstInfo::getHashValue(PairVal.getNestedNameSpecifier()),
SecondInfo::getHashValue(PairVal.getOpaqueData()));
}

static bool isEqual(const clang::NestedNameSpecifierLoc &LHS,
const clang::NestedNameSpecifierLoc &RHS) {
return LHS == RHS;
}
};
}

#endif
