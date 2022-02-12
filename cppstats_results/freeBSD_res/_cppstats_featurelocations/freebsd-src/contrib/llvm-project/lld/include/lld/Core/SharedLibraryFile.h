







#if !defined(LLD_CORE_SHARED_LIBRARY_FILE_H)
#define LLD_CORE_SHARED_LIBRARY_FILE_H

#include "lld/Core/File.h"

namespace lld {





class SharedLibraryFile : public File {
public:
static bool classof(const File *f) {
return f->kind() == kindSharedLibrary;
}




virtual OwningAtomPtr<SharedLibraryAtom> exports(StringRef name) const = 0;


virtual StringRef getDSOName() const = 0;

const AtomRange<DefinedAtom> defined() const override {
return _definedAtoms;
}

const AtomRange<UndefinedAtom> undefined() const override {
return _undefinedAtoms;
}

const AtomRange<SharedLibraryAtom> sharedLibrary() const override {
return _sharedLibraryAtoms;
}

const AtomRange<AbsoluteAtom> absolute() const override {
return _absoluteAtoms;
}

void clearAtoms() override {
_definedAtoms.clear();
_undefinedAtoms.clear();
_sharedLibraryAtoms.clear();
_absoluteAtoms.clear();
}

protected:

explicit SharedLibraryFile(StringRef path) : File(path, kindSharedLibrary) {}

AtomVector<DefinedAtom> _definedAtoms;
AtomVector<UndefinedAtom> _undefinedAtoms;
AtomVector<SharedLibraryAtom> _sharedLibraryAtoms;
AtomVector<AbsoluteAtom> _absoluteAtoms;
};

}

#endif
