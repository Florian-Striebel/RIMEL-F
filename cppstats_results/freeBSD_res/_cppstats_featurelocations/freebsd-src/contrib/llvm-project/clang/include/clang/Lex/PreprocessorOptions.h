







#if !defined(LLVM_CLANG_LEX_PREPROCESSOROPTIONS_H_)
#define LLVM_CLANG_LEX_PREPROCESSOROPTIONS_H_

#include "clang/Basic/BitmaskEnum.h"
#include "clang/Basic/LLVM.h"
#include "clang/Lex/PreprocessorExcludedConditionalDirectiveSkipMapping.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace llvm {

class MemoryBuffer;

}

namespace clang {


enum ObjCXXARCStandardLibraryKind {
ARCXX_nolib,


ARCXX_libcxx,


ARCXX_libstdcxx
};



enum class DisableValidationForModuleKind {

None = 0,


PCH = 0x1,


Module = 0x2,


All = PCH | Module,

LLVM_MARK_AS_BITMASK_ENUM(Module)
};



class PreprocessorOptions {
public:
std::vector<std::pair<std::string, bool>> Macros;
std::vector<std::string> Includes;
std::vector<std::string> MacroIncludes;



bool UsePredefines = true;



bool DetailedRecord = false;



bool PCHWithHdrStop = false;





bool PCHWithHdrStopCreate = false;






std::string PCHThroughHeader;


std::string ImplicitPCHInclude;


std::vector<std::string> ChainedIncludes;



DisableValidationForModuleKind DisablePCHOrModuleValidation =
DisableValidationForModuleKind::None;


bool AllowPCHWithCompilerErrors = false;



bool AllowPCHWithDifferentModulesCachePath = false;


bool DumpDeserializedPCHDecls = false;



std::set<std::string> DeserializedPCHDeclsToErrorOn;






std::pair<unsigned, bool> PrecompiledPreambleBytes;






bool GeneratePreamble = false;




bool WriteCommentListToPCH = true;






bool SingleFileParseMode = false;


bool LexEditorPlaceholders = true;



bool RemappedFilesKeepOriginalName = true;





std::vector<std::pair<std::string, std::string>> RemappedFiles;




std::vector<std::pair<std::string, llvm::MemoryBuffer *>> RemappedFileBuffers;







bool RetainRemappedFileBuffers = false;


bool RetainExcludedConditionalBlocks = false;




ObjCXXARCStandardLibraryKind ObjCXXARCStandardLibrary = ARCXX_nolib;


class FailedModulesSet {
llvm::StringSet<> Failed;

public:
bool hasAlreadyFailed(StringRef module) {
return Failed.count(module) > 0;
}

void addFailed(StringRef module) {
Failed.insert(module);
}
};







std::shared_ptr<FailedModulesSet> FailedModules;






ExcludedPreprocessorDirectiveSkipMapping
*ExcludedConditionalDirectiveSkipMappings = nullptr;


bool SetUpStaticAnalyzer = false;


bool DisablePragmaDebugCrash = false;

public:
PreprocessorOptions() : PrecompiledPreambleBytes(0, false) {}

void addMacroDef(StringRef Name) {
Macros.emplace_back(std::string(Name), false);
}
void addMacroUndef(StringRef Name) {
Macros.emplace_back(std::string(Name), true);
}

void addRemappedFile(StringRef From, StringRef To) {
RemappedFiles.emplace_back(std::string(From), std::string(To));
}

void addRemappedFile(StringRef From, llvm::MemoryBuffer *To) {
RemappedFileBuffers.emplace_back(std::string(From), To);
}

void clearRemappedFiles() {
RemappedFiles.clear();
RemappedFileBuffers.clear();
}



void resetNonModularOptions() {
Includes.clear();
MacroIncludes.clear();
ChainedIncludes.clear();
DumpDeserializedPCHDecls = false;
ImplicitPCHInclude.clear();
SingleFileParseMode = false;
LexEditorPlaceholders = true;
RetainRemappedFileBuffers = true;
PrecompiledPreambleBytes.first = 0;
PrecompiledPreambleBytes.second = false;
RetainExcludedConditionalBlocks = false;
}
};

}

#endif
