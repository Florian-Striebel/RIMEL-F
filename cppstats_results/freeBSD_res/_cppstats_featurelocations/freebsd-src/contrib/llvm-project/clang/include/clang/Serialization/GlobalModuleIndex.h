













#if !defined(LLVM_CLANG_SERIALIZATION_GLOBALMODULEINDEX_H)
#define LLVM_CLANG_SERIALIZATION_GLOBALMODULEINDEX_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include <memory>
#include <utility>

namespace llvm {
class BitstreamCursor;
class MemoryBuffer;
}

namespace clang {

class DirectoryEntry;
class FileEntry;
class FileManager;
class IdentifierIterator;
class PCHContainerOperations;
class PCHContainerReader;

namespace serialization {
class ModuleFile;
}












class GlobalModuleIndex {
using ModuleFile = serialization::ModuleFile;



std::unique_ptr<llvm::MemoryBuffer> Buffer;






void *IdentifierIndex;


struct ModuleInfo {
ModuleInfo() : File(), Size(), ModTime() { }


ModuleFile *File;


std::string FileName;


off_t Size;



time_t ModTime;



llvm::SmallVector<unsigned, 4> Dependencies;
};






llvm::SmallVector<ModuleInfo, 16> Modules;



llvm::DenseMap<ModuleFile *, unsigned> ModulesByFile;





llvm::StringMap<unsigned> UnresolvedModules;


unsigned NumIdentifierLookups;



unsigned NumIdentifierLookupHits;


explicit GlobalModuleIndex(std::unique_ptr<llvm::MemoryBuffer> Buffer,
llvm::BitstreamCursor Cursor);

GlobalModuleIndex(const GlobalModuleIndex &) = delete;
GlobalModuleIndex &operator=(const GlobalModuleIndex &) = delete;

public:
~GlobalModuleIndex();








static std::pair<GlobalModuleIndex *, llvm::Error>
readIndex(llvm::StringRef Path);




IdentifierIterator *createIdentifierIterator() const;





void getKnownModules(llvm::SmallVectorImpl<ModuleFile *> &ModuleFiles);



void getModuleDependencies(ModuleFile *File,
llvm::SmallVectorImpl<ModuleFile *> &Dependencies);


typedef llvm::SmallPtrSet<ModuleFile *, 4> HitSet;










bool lookupIdentifier(llvm::StringRef Name, HitSet &Hits);





bool loadedModuleFile(ModuleFile *File);


void printStats();


void dump();








static llvm::Error writeIndex(FileManager &FileMgr,
const PCHContainerReader &PCHContainerRdr,
llvm::StringRef Path);
};
}

#endif
