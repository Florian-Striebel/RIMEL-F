







#if !defined(LLD_CORE_LINKING_CONTEXT_H)
#define LLD_CORE_LINKING_CONTEXT_H

#include "lld/Core/Node.h"
#include "lld/Core/Reader.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Error.h"
#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace lld {

class PassManager;
class File;
class Writer;
class Node;
class SharedLibraryFile;






class LinkingContext {
public:
virtual ~LinkingContext();






virtual StringRef entrySymbolName() const { return _entrySymbolName; }




bool deadStrip() const { return _deadStrip; }





bool globalsAreDeadStripRoots() const { return _globalsAreDeadStripRoots; }





ArrayRef<StringRef> deadStripRoots() const {
return _deadStripRoots;
}



void addDeadStripRoot(StringRef symbolName) {
assert(!symbolName.empty() && "Empty symbol cannot be a dead strip root");
_deadStripRoots.push_back(symbolName);
}








bool printRemainingUndefines() const { return _printRemainingUndefines; }




bool allowRemainingUndefines() const { return _allowRemainingUndefines; }





bool allowShlibUndefines() const { return _allowShlibUndefines; }







bool logInputFiles() const { return _logInputFiles; }






ArrayRef<const char *> llvmOptions() const { return _llvmOptions; }



void setOutputPath(StringRef str) { _outputPath = str; }




void setEntrySymbolName(StringRef name) {
_entrySymbolName = name;
}

void setDeadStripping(bool enable) { _deadStrip = enable; }
void setGlobalsAreDeadStripRoots(bool v) { _globalsAreDeadStripRoots = v; }

void setPrintRemainingUndefines(bool print) {
_printRemainingUndefines = print;
}

void setAllowRemainingUndefines(bool allow) {
_allowRemainingUndefines = allow;
}

void setAllowShlibUndefines(bool allow) { _allowShlibUndefines = allow; }
void setLogInputFiles(bool log) { _logInputFiles = log; }

void appendLLVMOption(const char *opt) { _llvmOptions.push_back(opt); }

std::vector<std::unique_ptr<Node>> &getNodes() { return _nodes; }
const std::vector<std::unique_ptr<Node>> &getNodes() const { return _nodes; }






void addInitialUndefinedSymbol(StringRef symbolName) {
_initialUndefinedSymbols.push_back(symbolName);
}


typedef std::vector<StringRef> StringRefVector;
typedef StringRefVector::iterator StringRefVectorIter;
typedef StringRefVector::const_iterator StringRefVectorConstIter;





virtual void createInternalFiles(std::vector<std::unique_ptr<File>> &) const;



ArrayRef<StringRef> initialUndefinedSymbols() const {
return _initialUndefinedSymbols;
}







bool validate();


virtual std::string demangle(StringRef symbolName) const = 0;









StringRef outputPath() const { return _outputPath; }


const Registry &registry() const { return _registry; }
Registry &registry() { return _registry; }




virtual void createImplicitFiles(std::vector<std::unique_ptr<File>> &) = 0;



virtual void addPasses(PassManager &pm) = 0;




virtual llvm::Error writeFile(const File &linkedFile) const;


virtual uint64_t getNextOrdinalAndIncrement() const { return _nextOrdinal++; }



virtual void finalizeInputFiles() = 0;






virtual llvm::Error handleLoadedFile(File &file) = 0;


protected:
LinkingContext();


virtual Writer &writer() const = 0;


virtual std::unique_ptr<File> createEntrySymbolFile() const;
std::unique_ptr<File> createEntrySymbolFile(StringRef filename) const;


virtual std::unique_ptr<File> createUndefinedSymbolFile() const;
std::unique_ptr<File> createUndefinedSymbolFile(StringRef filename) const;

StringRef _outputPath;
StringRef _entrySymbolName;
bool _deadStrip = false;
bool _globalsAreDeadStripRoots = false;
bool _printRemainingUndefines = true;
bool _allowRemainingUndefines = false;
bool _logInputFiles = false;
bool _allowShlibUndefines = false;
std::vector<StringRef> _deadStripRoots;
std::vector<const char *> _llvmOptions;
StringRefVector _initialUndefinedSymbols;
std::vector<std::unique_ptr<Node>> _nodes;
mutable llvm::BumpPtrAllocator _allocator;
mutable uint64_t _nextOrdinal = 0;
Registry _registry;

private:

virtual bool validateImpl() = 0;
};

}

#endif
