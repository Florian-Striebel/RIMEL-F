














#if !defined(LLVM_CLANG_LIB_FORMAT_TOKENANALYZER_H)
#define LLVM_CLANG_LIB_FORMAT_TOKENANALYZER_H

#include "AffectedRangeManager.h"
#include "Encoding.h"
#include "FormatToken.h"
#include "FormatTokenLexer.h"
#include "TokenAnnotator.h"
#include "UnwrappedLineParser.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Format/Format.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/Debug.h"

namespace clang {
namespace format {

class Environment {
public:





Environment(StringRef Code, StringRef FileName,
ArrayRef<tooling::Range> Ranges, unsigned FirstStartColumn = 0,
unsigned NextStartColumn = 0, unsigned LastStartColumn = 0);

FileID getFileID() const { return ID; }

const SourceManager &getSourceManager() const { return SM; }

ArrayRef<CharSourceRange> getCharRanges() const { return CharRanges; }



unsigned getFirstStartColumn() const { return FirstStartColumn; }



unsigned getNextStartColumn() const { return NextStartColumn; }



unsigned getLastStartColumn() const { return LastStartColumn; }

private:

std::unique_ptr<SourceManagerForFile> VirtualSM;



SourceManager &SM;
FileID ID;

SmallVector<CharSourceRange, 8> CharRanges;
unsigned FirstStartColumn;
unsigned NextStartColumn;
unsigned LastStartColumn;
};

class TokenAnalyzer : public UnwrappedLineConsumer {
public:
TokenAnalyzer(const Environment &Env, const FormatStyle &Style);

std::pair<tooling::Replacements, unsigned> process();

protected:
virtual std::pair<tooling::Replacements, unsigned>
analyze(TokenAnnotator &Annotator,
SmallVectorImpl<AnnotatedLine *> &AnnotatedLines,
FormatTokenLexer &Tokens) = 0;

void consumeUnwrappedLine(const UnwrappedLine &TheLine) override;

void finishRun() override;

FormatStyle Style;

const Environment &Env;

AffectedRangeManager AffectedRangeMgr;
SmallVector<SmallVector<UnwrappedLine, 16>, 2> UnwrappedLines;
encoding::Encoding Encoding;
};

}
}

#endif
