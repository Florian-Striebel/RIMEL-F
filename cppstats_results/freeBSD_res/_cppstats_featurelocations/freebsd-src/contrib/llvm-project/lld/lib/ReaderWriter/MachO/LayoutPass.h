







#if !defined(LLD_READER_WRITER_MACHO_LAYOUT_PASS_H)
#define LLD_READER_WRITER_MACHO_LAYOUT_PASS_H

#include "lld/Core/File.h"
#include "lld/Core/Pass.h"
#include "lld/Core/Reader.h"
#include "lld/Core/Simple.h"
#include "llvm/ADT/DenseMap.h"
#include <map>
#include <string>
#include <vector>

namespace lld {
class DefinedAtom;
class SimpleFile;

namespace mach_o {






class LayoutPass : public Pass {
public:
struct SortKey {
SortKey(OwningAtomPtr<DefinedAtom> &&atom,
const DefinedAtom *root, uint64_t override)
: _atom(std::move(atom)), _root(root), _override(override) {}
OwningAtomPtr<DefinedAtom> _atom;
const DefinedAtom *_root;
uint64_t _override;



SortKey(SortKey &&key) : _atom(std::move(key._atom)), _root(key._root),
_override(key._override) {
key._root = nullptr;
}

SortKey &operator=(SortKey &&key) {
_atom = std::move(key._atom);
_root = key._root;
key._root = nullptr;
_override = key._override;
return *this;
}

private:
SortKey(const SortKey &) = delete;
void operator=(const SortKey&) = delete;
};

typedef std::function<bool (const DefinedAtom *left, const DefinedAtom *right,
bool &leftBeforeRight)> SortOverride;

LayoutPass(const Registry &registry, SortOverride sorter);


llvm::Error perform(SimpleFile &mergedFile) override;

~LayoutPass() override = default;

private:


void buildFollowOnTable(const File::AtomRange<DefinedAtom> &range);


void buildOrdinalOverrideMap(const File::AtomRange<DefinedAtom> &range);

const Registry &_registry;
SortOverride _customSorter;

typedef llvm::DenseMap<const DefinedAtom *, const DefinedAtom *> AtomToAtomT;
typedef llvm::DenseMap<const DefinedAtom *, uint64_t> AtomToOrdinalT;






AtomToAtomT _followOnNexts;




AtomToAtomT _followOnRoots;

AtomToOrdinalT _ordinalOverrideMap;


const DefinedAtom *findAtomFollowedBy(const DefinedAtom *targetAtom);
bool checkAllPrevAtomsZeroSize(const DefinedAtom *targetAtom);

void setChainRoot(const DefinedAtom *targetAtom, const DefinedAtom *root);

std::vector<SortKey> decorate(File::AtomRange<DefinedAtom> &atomRange) const;

void undecorate(File::AtomRange<DefinedAtom> &atomRange,
std::vector<SortKey> &keys) const;


void checkFollowonChain(const File::AtomRange<DefinedAtom> &range);
};

}
}

#endif
