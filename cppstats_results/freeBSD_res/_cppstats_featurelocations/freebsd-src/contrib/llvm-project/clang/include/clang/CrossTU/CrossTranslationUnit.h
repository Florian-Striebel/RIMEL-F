












#if !defined(LLVM_CLANG_CROSSTU_CROSSTRANSLATIONUNIT_H)
#define LLVM_CLANG_CROSSTU_CROSSTRANSLATIONUNIT_H

#include "clang/AST/ASTImporterSharedState.h"
#include "clang/Analysis/MacroExpansionContext.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/Path.h"

namespace clang {
class CompilerInstance;
class ASTContext;
class ASTImporter;
class ASTUnit;
class DeclContext;
class FunctionDecl;
class VarDecl;
class NamedDecl;
class TranslationUnitDecl;

namespace cross_tu {

enum class index_error_code {
success = 0,
unspecified = 1,
missing_index_file,
invalid_index_format,
multiple_definitions,
missing_definition,
failed_import,
failed_to_get_external_ast,
failed_to_generate_usr,
triple_mismatch,
lang_mismatch,
lang_dialect_mismatch,
load_threshold_reached,
invocation_list_ambiguous,
invocation_list_file_not_found,
invocation_list_empty,
invocation_list_wrong_format,
invocation_list_lookup_unsuccessful
};

class IndexError : public llvm::ErrorInfo<IndexError> {
public:
static char ID;
IndexError(index_error_code C) : Code(C), LineNo(0) {}
IndexError(index_error_code C, std::string FileName, int LineNo = 0)
: Code(C), FileName(std::move(FileName)), LineNo(LineNo) {}
IndexError(index_error_code C, std::string FileName, std::string TripleToName,
std::string TripleFromName)
: Code(C), FileName(std::move(FileName)),
TripleToName(std::move(TripleToName)),
TripleFromName(std::move(TripleFromName)) {}
void log(raw_ostream &OS) const override;
std::error_code convertToErrorCode() const override;
index_error_code getCode() const { return Code; }
int getLineNum() const { return LineNo; }
std::string getFileName() const { return FileName; }
std::string getTripleToName() const { return TripleToName; }
std::string getTripleFromName() const { return TripleFromName; }

private:
index_error_code Code;
std::string FileName;
int LineNo;
std::string TripleToName;
std::string TripleFromName;
};










llvm::Expected<llvm::StringMap<std::string>>
parseCrossTUIndex(StringRef IndexPath);

std::string createCrossTUIndexString(const llvm::StringMap<std::string> &Index);

using InvocationListTy = llvm::StringMap<llvm::SmallVector<std::string, 32>>;





llvm::Expected<InvocationListTy> parseInvocationList(
StringRef FileContent,
llvm::sys::path::Style PathStyle = llvm::sys::path::Style::posix);


bool containsConst(const VarDecl *VD, const ASTContext &ACtx);











class CrossTranslationUnitContext {
public:
CrossTranslationUnitContext(CompilerInstance &CI);
~CrossTranslationUnitContext();

















llvm::Expected<const FunctionDecl *>
getCrossTUDefinition(const FunctionDecl *FD, StringRef CrossTUDir,
StringRef IndexName, bool DisplayCTUProgress = false);
llvm::Expected<const VarDecl *>
getCrossTUDefinition(const VarDecl *VD, StringRef CrossTUDir,
StringRef IndexName, bool DisplayCTUProgress = false);














llvm::Expected<ASTUnit *> loadExternalAST(StringRef LookupName,
StringRef CrossTUDir,
StringRef IndexName,
bool DisplayCTUProgress = false);






llvm::Expected<const FunctionDecl *> importDefinition(const FunctionDecl *FD,
ASTUnit *Unit);
llvm::Expected<const VarDecl *> importDefinition(const VarDecl *VD,
ASTUnit *Unit);


static llvm::Optional<std::string> getLookupName(const NamedDecl *ND);


void emitCrossTUDiagnostics(const IndexError &IE);








llvm::Optional<clang::MacroExpansionContext>
getMacroExpansionContextForSourceLocation(
const clang::SourceLocation &ToLoc) const;

private:
void lazyInitImporterSharedSt(TranslationUnitDecl *ToTU);
ASTImporter &getOrCreateASTImporter(ASTUnit *Unit);
template <typename T>
llvm::Expected<const T *> getCrossTUDefinitionImpl(const T *D,
StringRef CrossTUDir,
StringRef IndexName,
bool DisplayCTUProgress);
template <typename T>
const T *findDefInDeclContext(const DeclContext *DC,
StringRef LookupName);
template <typename T>
llvm::Expected<const T *> importDefinitionImpl(const T *D, ASTUnit *Unit);

using ImporterMapTy =
llvm::DenseMap<TranslationUnitDecl *, std::unique_ptr<ASTImporter>>;

ImporterMapTy ASTUnitImporterMap;

ASTContext &Context;
std::shared_ptr<ASTImporterSharedState> ImporterSharedSt;

using LoadResultTy = llvm::Expected<std::unique_ptr<ASTUnit>>;


class ASTLoader {
public:
ASTLoader(CompilerInstance &CI, StringRef CTUDir,
StringRef InvocationListFilePath);





LoadResultTy load(StringRef Identifier);



llvm::Error lazyInitInvocationList();

private:


const llvm::sys::path::Style PathStyle = llvm::sys::path::Style::posix;


LoadResultTy loadFromDump(StringRef Identifier);

LoadResultTy loadFromSource(StringRef Identifier);

CompilerInstance &CI;
StringRef CTUDir;



StringRef InvocationListFilePath;


llvm::Optional<InvocationListTy> InvocationList;
index_error_code PreviousParsingResult = index_error_code::success;
};


class ASTLoadGuard {
public:
ASTLoadGuard(unsigned Limit) : Limit(Limit) {}



operator bool() const { return Count < Limit; }


void indicateLoadSuccess() { ++Count; }

private:

unsigned Count{0u};

const unsigned Limit;
};



class ASTUnitStorage {
public:
ASTUnitStorage(CompilerInstance &CI);











llvm::Expected<ASTUnit *> getASTUnitForFunction(StringRef FunctionName,
StringRef CrossTUDir,
StringRef IndexName,
bool DisplayCTUProgress);










llvm::Expected<std::string> getFileForFunction(StringRef FunctionName,
StringRef CrossTUDir,
StringRef IndexName);

private:
llvm::Error ensureCTUIndexLoaded(StringRef CrossTUDir, StringRef IndexName);
llvm::Expected<ASTUnit *> getASTUnitForFile(StringRef FileName,
bool DisplayCTUProgress);

template <typename... T> using BaseMapTy = llvm::StringMap<T...>;
using OwningMapTy = BaseMapTy<std::unique_ptr<clang::ASTUnit>>;
using NonOwningMapTy = BaseMapTy<clang::ASTUnit *>;

OwningMapTy FileASTUnitMap;
NonOwningMapTy NameASTUnitMap;

using IndexMapTy = BaseMapTy<std::string>;
IndexMapTy NameFileMap;


ASTLoader Loader;





ASTLoadGuard LoadGuard;
};

ASTUnitStorage ASTStorage;
};

}
}

#endif
