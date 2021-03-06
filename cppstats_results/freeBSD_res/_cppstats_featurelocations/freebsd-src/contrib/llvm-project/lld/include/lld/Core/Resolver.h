







#if !defined(LLD_CORE_RESOLVER_H)
#define LLD_CORE_RESOLVER_H

#include "lld/Core/ArchiveLibraryFile.h"
#include "lld/Core/File.h"
#include "lld/Core/SharedLibraryFile.h"
#include "lld/Core/Simple.h"
#include "lld/Core/SymbolTable.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/Support/ErrorOr.h"
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lld {

class Atom;
class LinkingContext;



class Resolver {
public:
Resolver(LinkingContext &ctx) : _ctx(ctx), _result(new MergedFile()) {}


void doDefinedAtom(OwningAtomPtr<DefinedAtom> atom);
bool doUndefinedAtom(OwningAtomPtr<UndefinedAtom> atom);
void doSharedLibraryAtom(OwningAtomPtr<SharedLibraryAtom> atom);
void doAbsoluteAtom(OwningAtomPtr<AbsoluteAtom> atom);



llvm::Expected<bool> handleFile(File &);


llvm::Expected<bool> handleArchiveFile(File &);


llvm::Error handleSharedLibrary(File &);


bool resolve();

std::unique_ptr<SimpleFile> resultFile() { return std::move(_result); }

private:
typedef std::function<llvm::Expected<bool>(StringRef)> UndefCallback;

bool undefinesAdded(int begin, int end);
File *getFile(int &index);


bool resolveUndefines();
void updateReferences();
void deadStripOptimize();
bool checkUndefines();
void removeCoalescedAwayAtoms();
llvm::Expected<bool> forEachUndefines(File &file, UndefCallback callback);

void markLive(const Atom *atom);

class MergedFile : public SimpleFile {
public:
MergedFile() : SimpleFile("<linker-internal>", kindResolverMergedObject) {}
void addAtoms(llvm::MutableArrayRef<OwningAtomPtr<Atom>> atoms);
};

LinkingContext &_ctx;
SymbolTable _symbolTable;
std::vector<OwningAtomPtr<Atom>> _atoms;
std::set<const Atom *> _deadStripRoots;
llvm::DenseSet<const Atom *> _liveAtoms;
llvm::DenseSet<const Atom *> _deadAtoms;
std::unique_ptr<MergedFile> _result;
std::unordered_multimap<const Atom *, const Atom *> _reverseRef;


std::vector<File *> _files;
std::map<File *, bool> _newUndefinesAdded;


std::vector<StringRef> _undefines;






std::map<File *, size_t> _undefineIndex;
};

}

#endif
