












#if !defined(LLVM_CLANG_SERIALIZATION_MODULEMANAGER_H)
#define LLVM_CLANG_SERIALIZATION_MODULEMANAGER_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/Module.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Serialization/ModuleFile.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include <cstdint>
#include <ctime>
#include <memory>
#include <string>
#include <utility>

namespace clang {

class FileEntry;
class FileManager;
class GlobalModuleIndex;
class HeaderSearch;
class InMemoryModuleCache;
class ModuleMap;
class PCHContainerReader;

namespace serialization {


class ModuleManager {


SmallVector<std::unique_ptr<ModuleFile>, 2> Chain;




SmallVector<ModuleFile *, 2> PCHChain;



SmallVector<ModuleFile *, 2> Roots;


llvm::DenseMap<const FileEntry *, ModuleFile *> Modules;



FileManager &FileMgr;


IntrusiveRefCntPtr<InMemoryModuleCache> ModuleCache;


const PCHContainerReader &PCHContainerRdr;


const HeaderSearch &HeaderSearchInfo;


llvm::DenseMap<const FileEntry *, std::unique_ptr<llvm::MemoryBuffer>>
InMemoryBuffers;


SmallVector<ModuleFile *, 4> VisitOrder;









SmallVector<ModuleFile *, 4> ModulesInCommonWithGlobalIndex;





GlobalModuleIndex *GlobalIndex = nullptr;



struct VisitState {
explicit VisitState(unsigned N) : VisitNumber(N, 0) {
Stack.reserve(N);
}

~VisitState() {
delete NextState;
}



SmallVector<ModuleFile *, 4> Stack;



SmallVector<unsigned, 4> VisitNumber;


unsigned NextVisitNumber = 1;


VisitState *NextState = nullptr;
};


VisitState *FirstVisitState = nullptr;

VisitState *allocateVisitState();
void returnVisitState(VisitState *State);

public:
using ModuleIterator = llvm::pointee_iterator<
SmallVectorImpl<std::unique_ptr<ModuleFile>>::iterator>;
using ModuleConstIterator = llvm::pointee_iterator<
SmallVectorImpl<std::unique_ptr<ModuleFile>>::const_iterator>;
using ModuleReverseIterator = llvm::pointee_iterator<
SmallVectorImpl<std::unique_ptr<ModuleFile>>::reverse_iterator>;
using ModuleOffset = std::pair<uint32_t, StringRef>;

explicit ModuleManager(FileManager &FileMgr, InMemoryModuleCache &ModuleCache,
const PCHContainerReader &PCHContainerRdr,
const HeaderSearch &HeaderSearchInfo);
~ModuleManager();


ModuleIterator begin() { return Chain.begin(); }


ModuleIterator end() { return Chain.end(); }


ModuleConstIterator begin() const { return Chain.begin(); }


ModuleConstIterator end() const { return Chain.end(); }


ModuleReverseIterator rbegin() { return Chain.rbegin(); }


ModuleReverseIterator rend() { return Chain.rend(); }


llvm::iterator_range<SmallVectorImpl<ModuleFile *>::const_iterator>
pch_modules() const {
return llvm::make_range(PCHChain.begin(), PCHChain.end());
}



ModuleFile &getPrimaryModule() { return *Chain[0]; }



ModuleFile &getPrimaryModule() const { return *Chain[0]; }


ModuleFile &operator[](unsigned Index) const { return *Chain[Index]; }


ModuleFile *lookupByFileName(StringRef FileName) const;


ModuleFile *lookupByModuleName(StringRef ModName) const;


ModuleFile *lookup(const FileEntry *File) const;


std::unique_ptr<llvm::MemoryBuffer> lookupBuffer(StringRef Name);


unsigned size() const { return Chain.size(); }


enum AddModuleResult {

AlreadyLoaded,


NewlyLoaded,


Missing,


OutOfDate
};

using ASTFileSignatureReader = ASTFileSignature (*)(StringRef);



































AddModuleResult addModule(StringRef FileName, ModuleKind Type,
SourceLocation ImportLoc,
ModuleFile *ImportedBy, unsigned Generation,
off_t ExpectedSize, time_t ExpectedModTime,
ASTFileSignature ExpectedSignature,
ASTFileSignatureReader ReadSignature,
ModuleFile *&Module,
std::string &ErrorStr);


void removeModules(ModuleIterator First, ModuleMap *modMap);


void addInMemoryBuffer(StringRef FileName,
std::unique_ptr<llvm::MemoryBuffer> Buffer);


void setGlobalIndex(GlobalModuleIndex *Index);



void moduleFileAccepted(ModuleFile *MF);




















void visit(llvm::function_ref<bool(ModuleFile &M)> Visitor,
llvm::SmallPtrSetImpl<ModuleFile *> *ModuleFilesHit = nullptr);


















bool lookupModuleFile(StringRef FileName, off_t ExpectedSize,
time_t ExpectedModTime, Optional<FileEntryRef> &File);


void viewGraph();

InMemoryModuleCache &getModuleCache() const { return *ModuleCache; }
};

}

}

#endif
