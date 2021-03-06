







#if !defined(LLVM_CLANG_FRONTEND_VERIFYDIAGNOSTICCONSUMER_H)
#define LLVM_CLANG_FRONTEND_VERIFYDIAGNOSTICCONSUMER_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/StringRef.h"
#include <cassert>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace clang {

class FileEntry;
class LangOptions;
class SourceManager;
class TextDiagnosticBuffer;


























































































































































class VerifyDiagnosticConsumer: public DiagnosticConsumer,
public CommentHandler {
public:


class Directive {
public:
static std::unique_ptr<Directive>
create(bool RegexKind, SourceLocation DirectiveLoc,
SourceLocation DiagnosticLoc, bool MatchAnyFileAndLine,
bool MatchAnyLine, StringRef Text, unsigned Min, unsigned Max);

public:

static const unsigned MaxCount = std::numeric_limits<unsigned>::max();

SourceLocation DirectiveLoc;
SourceLocation DiagnosticLoc;
const std::string Text;
unsigned Min, Max;
bool MatchAnyLine;
bool MatchAnyFileAndLine;

Directive(const Directive &) = delete;
Directive &operator=(const Directive &) = delete;
virtual ~Directive() = default;



virtual bool isValid(std::string &Error) = 0;


virtual bool match(StringRef S) = 0;

protected:
Directive(SourceLocation DirectiveLoc, SourceLocation DiagnosticLoc,
bool MatchAnyFileAndLine, bool MatchAnyLine, StringRef Text,
unsigned Min, unsigned Max)
: DirectiveLoc(DirectiveLoc), DiagnosticLoc(DiagnosticLoc), Text(Text),
Min(Min), Max(Max), MatchAnyLine(MatchAnyLine || MatchAnyFileAndLine),
MatchAnyFileAndLine(MatchAnyFileAndLine) {
assert(!DirectiveLoc.isInvalid() && "DirectiveLoc is invalid!");
assert((!DiagnosticLoc.isInvalid() || MatchAnyLine) &&
"DiagnosticLoc is invalid!");
}
};

using DirectiveList = std::vector<std::unique_ptr<Directive>>;


struct ExpectedData {
DirectiveList Errors;
DirectiveList Warnings;
DirectiveList Remarks;
DirectiveList Notes;

void Reset() {
Errors.clear();
Warnings.clear();
Remarks.clear();
Notes.clear();
}
};

enum DirectiveStatus {
HasNoDirectives,
HasNoDirectivesReported,
HasExpectedNoDiagnostics,
HasOtherExpectedDirectives
};

class MarkerTracker;

private:
DiagnosticsEngine &Diags;
DiagnosticConsumer *PrimaryClient;
std::unique_ptr<DiagnosticConsumer> PrimaryClientOwner;
std::unique_ptr<TextDiagnosticBuffer> Buffer;
std::unique_ptr<MarkerTracker> Markers;
const Preprocessor *CurrentPreprocessor = nullptr;
const LangOptions *LangOpts = nullptr;
SourceManager *SrcManager = nullptr;
unsigned ActiveSourceFiles = 0;
DirectiveStatus Status;
ExpectedData ED;

void CheckDiagnostics();

void setSourceManager(SourceManager &SM) {
assert((!SrcManager || SrcManager == &SM) && "SourceManager changed!");
SrcManager = &SM;
}


class UnparsedFileStatus {
llvm::PointerIntPair<const FileEntry *, 1, bool> Data;

public:
UnparsedFileStatus(const FileEntry *File, bool FoundDirectives)
: Data(File, FoundDirectives) {}

const FileEntry *getFile() const { return Data.getPointer(); }
bool foundDirectives() const { return Data.getInt(); }
};

using ParsedFilesMap = llvm::DenseMap<FileID, const FileEntry *>;
using UnparsedFilesMap = llvm::DenseMap<FileID, UnparsedFileStatus>;

ParsedFilesMap ParsedFiles;
UnparsedFilesMap UnparsedFiles;

public:



VerifyDiagnosticConsumer(DiagnosticsEngine &Diags);
~VerifyDiagnosticConsumer() override;

void BeginSourceFile(const LangOptions &LangOpts,
const Preprocessor *PP) override;

void EndSourceFile() override;

enum ParsedStatus {

IsParsed,


IsUnparsed,


IsUnparsedNoDirectives
};


void UpdateParsedFileStatus(SourceManager &SM, FileID FID, ParsedStatus PS);

bool HandleComment(Preprocessor &PP, SourceRange Comment) override;

void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
const Diagnostic &Info) override;
};

}

#endif
