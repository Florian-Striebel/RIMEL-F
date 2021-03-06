







#if !defined(LLD_CORE_ATOM_H)
#define LLD_CORE_ATOM_H

#include "lld/Common/LLVM.h"
#include "llvm/ADT/StringRef.h"

namespace lld {

class File;

template<typename T>
class OwningAtomPtr;








class Atom {
template<typename T> friend class OwningAtomPtr;

public:

enum Definition {
definitionRegular,
definitionAbsolute,
definitionUndefined,
definitionSharedLibrary
};


enum Scope {
scopeTranslationUnit,

scopeLinkageUnit,

scopeGlobal

};


virtual const File& file() const = 0;



virtual StringRef name() const = 0;



Definition definition() const { return _definition; }

static bool classof(const Atom *a) { return true; }

protected:

explicit Atom(Definition def) : _definition(def) {}





virtual ~Atom() = default;

private:
Definition _definition;
};



template<typename T>
class OwningAtomPtr {
private:
OwningAtomPtr(const OwningAtomPtr &) = delete;
void operator=(const OwningAtomPtr &) = delete;

public:
OwningAtomPtr() = default;
OwningAtomPtr(T *atom) : atom(atom) { }

~OwningAtomPtr() {
if (atom)
runDestructor(atom);
}

void runDestructor(Atom *atom) {
atom->~Atom();
}

OwningAtomPtr(OwningAtomPtr &&ptr) : atom(ptr.atom) {
ptr.atom = nullptr;
}

void operator=(OwningAtomPtr&& ptr) {
if (atom)
runDestructor(atom);
atom = ptr.atom;
ptr.atom = nullptr;
}

T *const &get() const {
return atom;
}

T *&get() {
return atom;
}

T *release() {
auto *v = atom;
atom = nullptr;
return v;
}

private:
T *atom = nullptr;
};

}

#endif
