













#if !defined(LLVM_CLANG_REWRITE_FRONTEND_FIXITREWRITER_H)
#define LLVM_CLANG_REWRITE_FRONTEND_FIXITREWRITER_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Edit/EditedSource.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace clang {

class LangOptions;
class SourceManager;

class FixItOptions {
public:
FixItOptions() = default;
virtual ~FixItOptions();







virtual std::string RewriteFilename(const std::string &Filename, int &fd) = 0;



bool InPlace = false;


bool FixWhatYouCan = false;


bool FixOnlyWarnings = false;




bool Silent = false;
};

class FixItRewriter : public DiagnosticConsumer {

DiagnosticsEngine &Diags;

edit::EditedSource Editor;



Rewriter Rewrite;



DiagnosticConsumer *Client;
std::unique_ptr<DiagnosticConsumer> Owner;



FixItOptions *FixItOpts;


unsigned NumFailures = 0;


bool PrevDiagSilenced = false;

public:

FixItRewriter(DiagnosticsEngine &Diags, SourceManager &SourceMgr,
const LangOptions &LangOpts, FixItOptions *FixItOpts);


~FixItRewriter() override;


bool IsModified(FileID ID) const {
return Rewrite.getRewriteBufferFor(ID) != nullptr;
}

using iterator = Rewriter::buffer_iterator;


iterator buffer_begin() { return Rewrite.buffer_begin(); }
iterator buffer_end() { return Rewrite.buffer_end(); }




bool WriteFixedFile(FileID ID, raw_ostream &OS);




bool WriteFixedFiles(
std::vector<std::pair<std::string, std::string>> *RewrittenFiles = nullptr);





bool IncludeInDiagnosticCounts() const override;



void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
const Diagnostic &Info) override;


void Diag(SourceLocation Loc, unsigned DiagID);
};

}

#endif
