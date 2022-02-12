







#if !defined(LLD_CORE_SHARED_LIBRARY_ATOM_H)
#define LLD_CORE_SHARED_LIBRARY_ATOM_H

#include "lld/Core/Atom.h"

namespace lld {



class SharedLibraryAtom : public Atom {
public:
enum class Type : uint32_t {
Unknown,
Code,
Data,
};



virtual StringRef loadName() const = 0;



virtual bool canBeNullAtRuntime() const = 0;

virtual Type type() const = 0;

virtual uint64_t size() const = 0;

static bool classof(const Atom *a) {
return a->definition() == definitionSharedLibrary;
}

static inline bool classof(const SharedLibraryAtom *) { return true; }

protected:
SharedLibraryAtom() : Atom(definitionSharedLibrary) {}

~SharedLibraryAtom() override = default;
};

}

#endif
