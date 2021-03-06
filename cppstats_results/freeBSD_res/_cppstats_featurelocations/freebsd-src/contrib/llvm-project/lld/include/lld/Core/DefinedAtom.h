







#if !defined(LLD_CORE_DEFINED_ATOM_H)
#define LLD_CORE_DEFINED_ATOM_H

#include "lld/Common/LLVM.h"
#include "lld/Core/Atom.h"
#include "lld/Core/Reference.h"
#include "llvm/Support/ErrorHandling.h"

namespace lld {
class File;

































































class DefinedAtom : public Atom {
public:
enum Interposable {
interposeNo,
interposeYes,
interposeYesAndRuntimeWeak

};

enum Merge {
mergeNo,
mergeAsTentative,
mergeAsWeak,


mergeAsWeakAndAddressUsed,

mergeSameNameAndSize,
mergeByLargestSection,
mergeByContent,
};

enum ContentType {
typeUnknown,
typeMachHeader,
typeCode,
typeResolver,
typeBranchIsland,
typeBranchShim,
typeStub,
typeStubHelper,
typeConstant,
typeCString,
typeUTF16String,
typeCFI,
typeLSDA,
typeLiteral4,
typeLiteral8,
typeLiteral16,
typeData,
typeDataFast,
typeZeroFill,
typeZeroFillFast,
typeConstData,
typeObjC1Class,
typeLazyPointer,
typeLazyDylibPointer,
typeNonLazyPointer,
typeCFString,
typeGOT,
typeInitializerPtr,
typeTerminatorPtr,
typeCStringPtr,
typeObjCClassPtr,
typeObjC2CategoryList,
typeObjCImageInfo,
typeObjCMethodList,
typeDTraceDOF,
typeInterposingTuples,
typeTempLTO,
typeCompactUnwindInfo,
typeProcessedUnwindInfo,
typeThunkTLV,
typeTLVInitialData,
typeTLVInitialZeroFill,
typeTLVInitializerPtr,
typeDSOHandle,
typeSectCreate,
};




enum ContentPermissions {
perm___ = 0,
permR__ = 8,
permRW_ = 8 + 2,
permRW_L = 8 + 2 + 1,

permR_X = 8 + 4,
permRWX = 8 + 2 + 4,
permUnknown = 16
};

enum SectionChoice {
sectionBasedOnContent,
sectionCustomPreferred,
sectionCustomRequired
};

enum DeadStripKind {
deadStripNormal,
deadStripNever,
deadStripAlways
};

enum DynamicExport {


dynamicExportNormal,

dynamicExportAlways,
};


enum CodeModel {
codeNA,

codeMipsPIC,
codeMipsMicro,
codeMipsMicroPIC,
codeMips16,

codeARMThumb,
codeARM_a,
codeARM_d,
codeARM_t,
};

struct Alignment {
Alignment(int v, int m = 0) : value(v), modulus(m) {}

uint16_t value;
uint16_t modulus;

bool operator==(const Alignment &rhs) const {
return (value == rhs.value) && (modulus == rhs.modulus);
}
};





virtual uint64_t ordinal() const = 0;





virtual uint64_t size() const = 0;






virtual uint64_t sectionSize() const { return 0; }







virtual Scope scope() const = 0;



virtual Interposable interposable() const = 0;


virtual Merge merge() const = 0;


virtual ContentType contentType() const = 0;



virtual Alignment alignment() const = 0;




virtual SectionChoice sectionChoice() const = 0;



virtual StringRef customSectionName() const = 0;


virtual DeadStripKind deadStrip() const = 0;


virtual DynamicExport dynamicExport() const {
return dynamicExportNormal;
}


virtual CodeModel codeModel() const { return codeNA; }






virtual ContentPermissions permissions() const;



virtual ArrayRef<uint8_t> rawContent() const = 0;




class reference_iterator {
public:
reference_iterator(const DefinedAtom &a, const void *it)
: _atom(a), _it(it) { }

const Reference *operator*() const {
return _atom.derefIterator(_it);
}

const Reference *operator->() const {
return _atom.derefIterator(_it);
}

bool operator==(const reference_iterator &other) const {
return _it == other._it;
}

bool operator!=(const reference_iterator &other) const {
return !(*this == other);
}

reference_iterator &operator++() {
_atom.incrementIterator(_it);
return *this;
}
private:
const DefinedAtom &_atom;
const void *_it;
};


virtual reference_iterator begin() const = 0;


virtual reference_iterator end() const = 0;


virtual void addReference(Reference::KindNamespace ns,
Reference::KindArch arch,
Reference::KindValue kindValue, uint64_t off,
const Atom *target, Reference::Addend a) {
llvm_unreachable("Subclass does not permit adding references");
}

static bool classof(const Atom *a) {
return a->definition() == definitionRegular;
}


static ContentPermissions permissions(ContentType type);


bool occupiesDiskSpace() const {
ContentType atomContentType = contentType();
return !(atomContentType == DefinedAtom::typeZeroFill ||
atomContentType == DefinedAtom::typeZeroFillFast ||
atomContentType == DefinedAtom::typeTLVInitialZeroFill);
}




bool relocsToDefinedCanBeImplicit() const {
ContentType atomContentType = contentType();
return atomContentType == typeCFI;
}

protected:


DefinedAtom() : Atom(definitionRegular) { }

~DefinedAtom() override = default;



virtual const Reference *derefIterator(const void *iter) const = 0;



virtual void incrementIterator(const void *&iter) const = 0;
};
}

#endif
