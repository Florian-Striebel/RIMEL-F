












#if !defined(LLVM_CLANG_TOOLING_JSONCOMPILATIONDATABASE_H)
#define LLVM_CLANG_TOOLING_JSONCOMPILATIONDATABASE_H

#include "clang/Basic/LLVM.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/FileMatchTrie.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/YAMLParser.h"
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace clang {
namespace tooling {


























enum class JSONCommandLineSyntax { Windows, Gnu, AutoDetect };
class JSONCompilationDatabase : public CompilationDatabase {
public:




static std::unique_ptr<JSONCompilationDatabase>
loadFromFile(StringRef FilePath, std::string &ErrorMessage,
JSONCommandLineSyntax Syntax);




static std::unique_ptr<JSONCompilationDatabase>
loadFromBuffer(StringRef DatabaseString, std::string &ErrorMessage,
JSONCommandLineSyntax Syntax);






std::vector<CompileCommand>
getCompileCommands(StringRef FilePath) const override;




std::vector<std::string> getAllFiles() const override;



std::vector<CompileCommand> getAllCompileCommands() const override;

private:

JSONCompilationDatabase(std::unique_ptr<llvm::MemoryBuffer> Database,
JSONCommandLineSyntax Syntax)
: Database(std::move(Database)), Syntax(Syntax),
YAMLStream(this->Database->getBuffer(), SM) {}





bool parse(std::string &ErrorMessage);








using CompileCommandRef =
std::tuple<llvm::yaml::ScalarNode *, llvm::yaml::ScalarNode *,
std::vector<llvm::yaml::ScalarNode *>,
llvm::yaml::ScalarNode *>;


void getCommands(ArrayRef<CompileCommandRef> CommandsRef,
std::vector<CompileCommand> &Commands) const;


llvm::StringMap<std::vector<CompileCommandRef>> IndexByFile;



std::vector<CompileCommandRef> AllCommands;

FileMatchTrie MatchTrie;

std::unique_ptr<llvm::MemoryBuffer> Database;
JSONCommandLineSyntax Syntax;
llvm::SourceMgr SM;
llvm::yaml::Stream YAMLStream;
};

}
}

#endif
