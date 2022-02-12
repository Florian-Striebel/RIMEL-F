












#if !defined(LLVM_CLANG_LEX_PREPROCESSORLEXER_H)
#define LLVM_CLANG_LEX_PREPROCESSORLEXER_H

#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/MultipleIncludeOpt.h"
#include "clang/Lex/Token.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include <cassert>

namespace clang {

class FileEntry;
class Preprocessor;

class PreprocessorLexer {
virtual void anchor();

protected:
friend class Preprocessor;


Preprocessor *PP = nullptr;


const FileID FID;


unsigned InitialNumSLocEntries = 0;






bool ParsingPreprocessorDirective = false;


bool ParsingFilename = false;














bool LexingRawMode = false;



MultipleIncludeOpt MIOpt;



SmallVector<PPConditionalInfo, 4> ConditionalStack;

struct IncludeInfo {
const FileEntry *File;
SourceLocation Location;
};

llvm::StringMap<IncludeInfo> IncludeHistory;

PreprocessorLexer() : FID() {}
PreprocessorLexer(Preprocessor *pp, FileID fid);
virtual ~PreprocessorLexer() = default;

virtual void IndirectLex(Token& Result) = 0;


virtual SourceLocation getSourceLocation() = 0;







void pushConditionalLevel(SourceLocation DirectiveStart, bool WasSkipping,
bool FoundNonSkip, bool FoundElse) {
PPConditionalInfo CI;
CI.IfLoc = DirectiveStart;
CI.WasSkipping = WasSkipping;
CI.FoundNonSkip = FoundNonSkip;
CI.FoundElse = FoundElse;
ConditionalStack.push_back(CI);
}
void pushConditionalLevel(const PPConditionalInfo &CI) {
ConditionalStack.push_back(CI);
}




bool popConditionalLevel(PPConditionalInfo &CI) {
if (ConditionalStack.empty())
return true;
CI = ConditionalStack.pop_back_val();
return false;
}



PPConditionalInfo &peekConditionalLevel() {
assert(!ConditionalStack.empty() && "No conditionals active!");
return ConditionalStack.back();
}

unsigned getConditionalStackDepth() const { return ConditionalStack.size(); }

public:
PreprocessorLexer(const PreprocessorLexer &) = delete;
PreprocessorLexer &operator=(const PreprocessorLexer &) = delete;





void LexIncludeFilename(Token &FilenameTok);



void setParsingPreprocessorDirective(bool f) {
ParsingPreprocessorDirective = f;
}


bool isLexingRawMode() const { return LexingRawMode; }


Preprocessor *getPP() const { return PP; }

FileID getFileID() const {
assert(PP &&
"PreprocessorLexer::getFileID() should only be used with a Preprocessor");
return FID;
}


unsigned getInitialNumSLocEntries() const {
return InitialNumSLocEntries;
}



const FileEntry *getFileEntry() const;



using conditional_iterator =
SmallVectorImpl<PPConditionalInfo>::const_iterator;

conditional_iterator conditional_begin() const {
return ConditionalStack.begin();
}

conditional_iterator conditional_end() const {
return ConditionalStack.end();
}

void setConditionalLevels(ArrayRef<PPConditionalInfo> CL) {
ConditionalStack.clear();
ConditionalStack.append(CL.begin(), CL.end());
}

void addInclude(StringRef Filename, const FileEntry &File,
SourceLocation Location) {
IncludeHistory.insert({Filename, {&File, Location}});
}

const llvm::StringMap<IncludeInfo> &getIncludeHistory() const {
return IncludeHistory;
}
};

}

#endif
