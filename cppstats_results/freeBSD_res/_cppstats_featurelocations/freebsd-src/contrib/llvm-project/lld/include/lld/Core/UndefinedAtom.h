







#if !defined(LLD_CORE_UNDEFINED_ATOM_H)
#define LLD_CORE_UNDEFINED_ATOM_H

#include "lld/Core/Atom.h"

namespace lld {



class UndefinedAtom : public Atom {
public:




enum CanBeNull {

canBeNullNever,











canBeNullAtRuntime,









canBeNullAtBuildtime
};

virtual CanBeNull canBeNull() const = 0;

static bool classof(const Atom *a) {
return a->definition() == definitionUndefined;
}

static bool classof(const UndefinedAtom *) { return true; }

protected:
UndefinedAtom() : Atom(definitionUndefined) {}

~UndefinedAtom() override = default;
};

}

#endif
