

























#if !defined(LLVM_CLANG_TOOLING_COMPILATIONDATABASE_H)
#define LLVM_CLANG_TOOLING_COMPILATIONDATABASE_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/VirtualFileSystem.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace clang {
namespace tooling {


struct CompileCommand {
CompileCommand() = default;
CompileCommand(const Twine &Directory, const Twine &Filename,
std::vector<std::string> CommandLine, const Twine &Output)
: Directory(Directory.str()), Filename(Filename.str()),
CommandLine(std::move(CommandLine)), Output(Output.str()) {}


std::string Directory;


std::string Filename;


std::vector<std::string> CommandLine;


std::string Output;




std::string Heuristic;

friend bool operator==(const CompileCommand &LHS, const CompileCommand &RHS) {
return LHS.Directory == RHS.Directory && LHS.Filename == RHS.Filename &&
LHS.CommandLine == RHS.CommandLine && LHS.Output == RHS.Output &&
LHS.Heuristic == RHS.Heuristic;
}

friend bool operator!=(const CompileCommand &LHS, const CompileCommand &RHS) {
return !(LHS == RHS);
}
};









class CompilationDatabase {
public:
virtual ~CompilationDatabase();













static std::unique_ptr<CompilationDatabase>
loadFromDirectory(StringRef BuildDirectory, std::string &ErrorMessage);





static std::unique_ptr<CompilationDatabase>
autoDetectFromSource(StringRef SourceFile, std::string &ErrorMessage);





static std::unique_ptr<CompilationDatabase>
autoDetectFromDirectory(StringRef SourceDir, std::string &ErrorMessage);










virtual std::vector<CompileCommand> getCompileCommands(
StringRef FilePath) const = 0;





virtual std::vector<std::string> getAllFiles() const { return {}; }










virtual std::vector<CompileCommand> getAllCompileCommands() const;
};





class FixedCompilationDatabase : public CompilationDatabase {
public:



























static std::unique_ptr<FixedCompilationDatabase>
loadFromCommandLine(int &Argc, const char *const *Argv, std::string &ErrorMsg,
const Twine &Directory = ".");



static std::unique_ptr<FixedCompilationDatabase>
loadFromFile(StringRef Path, std::string &ErrorMsg);



static std::unique_ptr<FixedCompilationDatabase>
loadFromBuffer(StringRef Directory, StringRef Data, std::string &ErrorMsg);



FixedCompilationDatabase(const Twine &Directory,
ArrayRef<std::string> CommandLine);






std::vector<CompileCommand>
getCompileCommands(StringRef FilePath) const override;

private:


std::vector<CompileCommand> CompileCommands;
};




tooling::CompileCommand transferCompileCommand(tooling::CompileCommand,
StringRef Filename);





std::unique_ptr<CompilationDatabase>
inferMissingCompileCommands(std::unique_ptr<CompilationDatabase>);




std::unique_ptr<CompilationDatabase>
inferTargetAndDriverMode(std::unique_ptr<CompilationDatabase> Base);



std::unique_ptr<CompilationDatabase>
expandResponseFiles(std::unique_ptr<CompilationDatabase> Base,
llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS);

}
}

#endif
