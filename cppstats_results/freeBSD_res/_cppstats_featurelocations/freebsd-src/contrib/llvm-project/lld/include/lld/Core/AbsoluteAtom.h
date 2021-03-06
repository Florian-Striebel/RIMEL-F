







#if !defined(LLD_CORE_ABSOLUTE_ATOM_H)
#define LLD_CORE_ABSOLUTE_ATOM_H

#include "lld/Core/Atom.h"

namespace lld {



class AbsoluteAtom : public Atom {
public:

virtual uint64_t value() const = 0;






virtual Scope scope() const = 0;

static bool classof(const Atom *a) {
return a->definition() == definitionAbsolute;
}

static bool classof(const AbsoluteAtom *) { return true; }

protected:
AbsoluteAtom() : Atom(definitionAbsolute) {}
};

}

#endif
