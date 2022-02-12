







#if !defined(LLVM_CLANG_ARCMIGRATE_ARCMT_H)
#define LLVM_CLANG_ARCMIGRATE_ARCMT_H

#include "clang/ARCMigrate/FileRemapper.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Frontend/CompilerInvocation.h"

namespace clang {
class ASTContext;
class DiagnosticConsumer;
class PCHContainerOperations;

namespace arcmt {
class MigrationPass;

















bool
checkForManualIssues(CompilerInvocation &CI, const FrontendInputFile &Input,
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
DiagnosticConsumer *DiagClient,
bool emitPremigrationARCErrors = false,
StringRef plistOut = StringRef());





bool
applyTransformations(CompilerInvocation &origCI,
const FrontendInputFile &Input,
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
DiagnosticConsumer *DiagClient);












bool migrateWithTemporaryFiles(
CompilerInvocation &origCI, const FrontendInputFile &Input,
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
DiagnosticConsumer *DiagClient, StringRef outputDir,
bool emitPremigrationARCErrors, StringRef plistOut);





bool getFileRemappings(std::vector<std::pair<std::string,std::string> > &remap,
StringRef outputDir,
DiagnosticConsumer *DiagClient);





bool getFileRemappingsFromFileList(
std::vector<std::pair<std::string,std::string> > &remap,
ArrayRef<StringRef> remapFiles,
DiagnosticConsumer *DiagClient);

typedef void (*TransformFn)(MigrationPass &pass);

std::vector<TransformFn> getAllTransformations(LangOptions::GCMode OrigGCMode,
bool NoFinalizeRemoval);

class MigrationProcess {
CompilerInvocation OrigCI;
std::shared_ptr<PCHContainerOperations> PCHContainerOps;
DiagnosticConsumer *DiagClient;
FileRemapper Remapper;

public:
bool HadARCErrors;

MigrationProcess(const CompilerInvocation &CI,
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
DiagnosticConsumer *diagClient,
StringRef outputDir = StringRef());

class RewriteListener {
public:
virtual ~RewriteListener();

virtual void start(ASTContext &Ctx) { }
virtual void finish() { }

virtual void insert(SourceLocation loc, StringRef text) { }
virtual void remove(CharSourceRange range) { }
};

bool applyTransform(TransformFn trans, RewriteListener *listener = nullptr);

FileRemapper &getRemapper() { return Remapper; }
};

}

}

#endif
