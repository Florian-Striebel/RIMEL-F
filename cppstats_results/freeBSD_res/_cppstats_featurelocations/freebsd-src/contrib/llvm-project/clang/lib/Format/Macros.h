



































#if !defined(CLANG_LIB_FORMAT_MACROS_H)
#define CLANG_LIB_FORMAT_MACROS_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Encoding.h"
#include "FormatToken.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

namespace llvm {
class MemoryBuffer;
}

namespace clang {
class IdentifierTable;
class SourceManager;

namespace format {
struct FormatStyle;





























class MacroExpander {
public:
using ArgsList = llvm::ArrayRef<llvm::SmallVector<FormatToken *, 8>>;














MacroExpander(const std::vector<std::string> &Macros,
clang::SourceManager &SourceMgr, const FormatStyle &Style,
llvm::SpecificBumpPtrAllocator<FormatToken> &Allocator,
IdentifierTable &IdentTable);
~MacroExpander();


bool defined(llvm::StringRef Name) const;



bool objectLike(llvm::StringRef Name) const;



llvm::SmallVector<FormatToken *, 8> expand(FormatToken *ID,
ArgsList Args) const;

private:
struct Definition;
class DefinitionParser;

void parseDefinition(const std::string &Macro);

clang::SourceManager &SourceMgr;
const FormatStyle &Style;
llvm::SpecificBumpPtrAllocator<FormatToken> &Allocator;
IdentifierTable &IdentTable;
std::vector<std::unique_ptr<llvm::MemoryBuffer>> Buffers;
llvm::StringMap<Definition> Definitions;
};

}
}

#endif
