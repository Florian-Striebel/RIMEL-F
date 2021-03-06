







#if !defined(LLD_CORE_REFERENCES_H)
#define LLD_CORE_REFERENCES_H

#include <cstdint>

namespace lld {

class Atom;
























class Reference {
public:

enum class KindNamespace {
all = 0,
testing = 1,
mach_o = 2,
};

KindNamespace kindNamespace() const { return (KindNamespace)_kindNamespace; }
void setKindNamespace(KindNamespace ns) { _kindNamespace = (uint8_t)ns; }


enum class KindArch { all, AArch64, ARM, x86, x86_64};

KindArch kindArch() const { return (KindArch)_kindArch; }
void setKindArch(KindArch a) { _kindArch = (uint8_t)a; }

typedef uint16_t KindValue;

KindValue kindValue() const { return _kindValue; }



void setKindValue(KindValue value) {
_kindValue = value;
}


enum {


kindLayoutAfter = 1,
kindAssociate,
};


typedef int64_t Addend;



virtual uint64_t offsetInAtom() const = 0;


virtual const Atom *target() const = 0;



virtual void setTarget(const Atom *) = 0;


virtual Addend addend() const = 0;


virtual void setAddend(Addend) = 0;


virtual uint32_t tag() const { return 0; }

protected:

Reference(KindNamespace ns, KindArch a, KindValue value)
: _kindValue(value), _kindNamespace((uint8_t)ns), _kindArch((uint8_t)a) {}





virtual ~Reference() = default;

KindValue _kindValue;
uint8_t _kindNamespace;
uint8_t _kindArch;
};

}

#endif
