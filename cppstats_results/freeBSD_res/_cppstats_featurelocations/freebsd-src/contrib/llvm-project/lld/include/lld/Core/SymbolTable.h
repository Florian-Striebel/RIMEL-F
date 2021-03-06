







#if !defined(LLD_CORE_SYMBOL_TABLE_H)
#define LLD_CORE_SYMBOL_TABLE_H

#include "lld/Common/LLVM.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/Support/DJB.h"
#include <cstring>
#include <map>
#include <vector>

namespace lld {

class AbsoluteAtom;
class Atom;
class DefinedAtom;
class LinkingContext;
class ResolverOptions;
class SharedLibraryAtom;
class UndefinedAtom;






class SymbolTable {
public:

bool add(const DefinedAtom &);


bool add(const UndefinedAtom &);


bool add(const SharedLibraryAtom &);


bool add(const AbsoluteAtom &);


const Atom *findByName(StringRef sym);


std::vector<const UndefinedAtom *> undefines();


const Atom *replacement(const Atom *);


bool isCoalescedAway(const Atom *);

private:
typedef llvm::DenseMap<const Atom *, const Atom *> AtomToAtom;

struct StringRefMappingInfo {
static StringRef getEmptyKey() { return StringRef(); }
static StringRef getTombstoneKey() { return StringRef(" ", 1); }
static unsigned getHashValue(StringRef const val) {
return llvm::djbHash(val, 0);
}
static bool isEqual(StringRef const lhs, StringRef const rhs) {
return lhs.equals(rhs);
}
};
typedef llvm::DenseMap<StringRef, const Atom *,
StringRefMappingInfo> NameToAtom;

struct AtomMappingInfo {
static const DefinedAtom * getEmptyKey() { return nullptr; }
static const DefinedAtom * getTombstoneKey() { return (DefinedAtom*)(-1); }
static unsigned getHashValue(const DefinedAtom * const Val);
static bool isEqual(const DefinedAtom * const LHS,
const DefinedAtom * const RHS);
};
typedef llvm::DenseSet<const DefinedAtom*, AtomMappingInfo> AtomContentSet;

bool addByName(const Atom &);
bool addByContent(const DefinedAtom &);

AtomToAtom _replacedAtoms;
NameToAtom _nameTable;
AtomContentSet _contentTable;
};

}

#endif
