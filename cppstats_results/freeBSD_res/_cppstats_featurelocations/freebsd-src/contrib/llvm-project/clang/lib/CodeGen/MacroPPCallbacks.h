











#if !defined(LLVM_CLANG_LIB_CODEGEN_MACROPPCALLBACKS_H)
#define LLVM_CLANG_LIB_CODEGEN_MACROPPCALLBACKS_H

#include "clang/Lex/PPCallbacks.h"

namespace llvm {
class DIMacroFile;
class DIMacroNode;
}
namespace clang {
class Preprocessor;
class MacroInfo;
class CodeGenerator;

class MacroPPCallbacks : public PPCallbacks {

CodeGenerator *Gen;


Preprocessor &PP;


SourceLocation LastHashLoc;



int EnteredCommandLineIncludeFiles = 0;

enum FileScopeStatus {
NoScope = 0,
InitializedScope,
BuiltinScope,
CommandLineIncludeScope,
MainFileScope
};
FileScopeStatus Status;



llvm::SmallVector<llvm::DIMacroFile *, 4> Scopes;



llvm::DIMacroFile *getCurrentScope();





SourceLocation getCorrectLocation(SourceLocation Loc);










static void writeMacroDefinition(const IdentifierInfo &II,
const MacroInfo &MI, Preprocessor &PP,
raw_ostream &Name, raw_ostream &Value);


void updateStatusToNextScope();




void FileEntered(SourceLocation Loc);




void FileExited(SourceLocation Loc);

public:
MacroPPCallbacks(CodeGenerator *Gen, Preprocessor &PP);





void FileChanged(SourceLocation Loc, FileChangeReason Reason,
SrcMgr::CharacteristicKind FileType,
FileID PrevFID = FileID()) override;


void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok,
StringRef FileName, bool IsAngled,
CharSourceRange FilenameRange, const FileEntry *File,
StringRef SearchPath, StringRef RelativePath,
const Module *Imported,
SrcMgr::CharacteristicKind FileType) override;


void MacroDefined(const Token &MacroNameTok,
const MacroDirective *MD) override;




void MacroUndefined(const Token &MacroNameTok, const MacroDefinition &MD,
const MacroDirective *Undef) override;
};

}

#endif
