











#if !defined(LLVM_CLANG_AST_CXXINHERITANCE_H)
#define LLVM_CLANG_AST_CXXINHERITANCE_H

#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/Type.h"
#include "clang/AST/TypeOrdering.h"
#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/iterator_range.h"
#include <list>
#include <memory>
#include <utility>

namespace clang {

class ASTContext;
class NamedDecl;








struct CXXBasePathElement {



const CXXBaseSpecifier *Base;


const CXXRecordDecl *Class;







int SubobjectNumber;
};









class CXXBasePath : public SmallVector<CXXBasePathElement, 4> {
public:



AccessSpecifier Access = AS_public;

CXXBasePath() = default;


DeclContext::lookup_iterator Decls;

void clear() {
SmallVectorImpl<CXXBasePathElement>::clear();
Access = AS_public;
}
};






























class CXXBasePaths {
friend class CXXRecordDecl;


const CXXRecordDecl *Origin = nullptr;



std::list<CXXBasePath> Paths;







struct IsVirtBaseAndNumberNonVirtBases {
unsigned IsVirtBase : 1;
unsigned NumberOfNonVirtBases : 31;
};
llvm::SmallDenseMap<QualType, IsVirtBaseAndNumberNonVirtBases, 8>
ClassSubobjects;



llvm::SmallPtrSet<const CXXRecordDecl *, 4> VisitedDependentRecords;


const RecordType *DetectedVirtual = nullptr;



CXXBasePath ScratchPath;




bool FindAmbiguities;




bool RecordPaths;




bool DetectVirtual;

bool lookupInBases(ASTContext &Context, const CXXRecordDecl *Record,
CXXRecordDecl::BaseMatchesCallback BaseMatches,
bool LookupInDependent = false);

public:
using paths_iterator = std::list<CXXBasePath>::iterator;
using const_paths_iterator = std::list<CXXBasePath>::const_iterator;
using decl_iterator = NamedDecl **;



explicit CXXBasePaths(bool FindAmbiguities = true, bool RecordPaths = true,
bool DetectVirtual = true)
: FindAmbiguities(FindAmbiguities), RecordPaths(RecordPaths),
DetectVirtual(DetectVirtual) {}

paths_iterator begin() { return Paths.begin(); }
paths_iterator end() { return Paths.end(); }
const_paths_iterator begin() const { return Paths.begin(); }
const_paths_iterator end() const { return Paths.end(); }

CXXBasePath& front() { return Paths.front(); }
const CXXBasePath& front() const { return Paths.front(); }

using decl_range = llvm::iterator_range<decl_iterator>;




bool isAmbiguous(CanQualType BaseType);


bool isFindingAmbiguities() const { return FindAmbiguities; }


bool isRecordingPaths() const { return RecordPaths; }


void setRecordingPaths(bool RP) { RecordPaths = RP; }


bool isDetectingVirtual() const { return DetectVirtual; }



const RecordType* getDetectedVirtual() const {
return DetectedVirtual;
}



const CXXRecordDecl *getOrigin() const { return Origin; }
void setOrigin(const CXXRecordDecl *Rec) { Origin = Rec; }


void clear();



void swap(CXXBasePaths &Other);
};



struct UniqueVirtualMethod {

CXXMethodDecl *Method = nullptr;



unsigned Subobject = 0;




const CXXRecordDecl *InVirtualSubobject = nullptr;

UniqueVirtualMethod() = default;

UniqueVirtualMethod(CXXMethodDecl *Method, unsigned Subobject,
const CXXRecordDecl *InVirtualSubobject)
: Method(Method), Subobject(Subobject),
InVirtualSubobject(InVirtualSubobject) {}

friend bool operator==(const UniqueVirtualMethod &X,
const UniqueVirtualMethod &Y) {
return X.Method == Y.Method && X.Subobject == Y.Subobject &&
X.InVirtualSubobject == Y.InVirtualSubobject;
}

friend bool operator!=(const UniqueVirtualMethod &X,
const UniqueVirtualMethod &Y) {
return !(X == Y);
}
};








class OverridingMethods {
using ValuesT = SmallVector<UniqueVirtualMethod, 4>;
using MapType = llvm::MapVector<unsigned, ValuesT>;

MapType Overrides;

public:

using iterator = MapType::iterator;
using const_iterator = MapType::const_iterator;

iterator begin() { return Overrides.begin(); }
const_iterator begin() const { return Overrides.begin(); }
iterator end() { return Overrides.end(); }
const_iterator end() const { return Overrides.end(); }
unsigned size() const { return Overrides.size(); }



using overriding_iterator =
SmallVectorImpl<UniqueVirtualMethod>::iterator;
using overriding_const_iterator =
SmallVectorImpl<UniqueVirtualMethod>::const_iterator;


void add(unsigned OverriddenSubobject, UniqueVirtualMethod Overriding);




void add(const OverridingMethods &Other);



void replaceAll(UniqueVirtualMethod Overriding);
};




















































class CXXFinalOverriderMap
: public llvm::MapVector<const CXXMethodDecl *, OverridingMethods> {};


class CXXIndirectPrimaryBaseSet
: public llvm::SmallSet<const CXXRecordDecl*, 32> {};

inline bool
inheritanceModelHasVBPtrOffsetField(MSInheritanceModel Inheritance) {
return Inheritance == MSInheritanceModel::Unspecified;
}



inline bool inheritanceModelHasNVOffsetField(bool IsMemberFunction,
MSInheritanceModel Inheritance) {
return IsMemberFunction && Inheritance >= MSInheritanceModel::Multiple;
}

inline bool
inheritanceModelHasVBTableOffsetField(MSInheritanceModel Inheritance) {
return Inheritance >= MSInheritanceModel::Virtual;
}

inline bool inheritanceModelHasOnlyOneField(bool IsMemberFunction,
MSInheritanceModel Inheritance) {
if (IsMemberFunction)
return Inheritance <= MSInheritanceModel::Single;
return Inheritance <= MSInheritanceModel::Multiple;
}

}

#endif
