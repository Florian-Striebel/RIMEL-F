












#if !defined(LLVM_CLANG_REWRITE_CORE_REWRITER_H)
#define LLVM_CLANG_REWRITE_CORE_REWRITER_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Rewrite/Core/RewriteBuffer.h"
#include "llvm/ADT/StringRef.h"
#include <map>
#include <string>

namespace clang {

class LangOptions;
class SourceManager;




class Rewriter {
SourceManager *SourceMgr = nullptr;
const LangOptions *LangOpts = nullptr;
std::map<FileID, RewriteBuffer> RewriteBuffers;

public:
struct RewriteOptions {


bool IncludeInsertsAtBeginOfRange = true;



bool IncludeInsertsAtEndOfRange = true;














bool RemoveLineIfEmpty = false;

RewriteOptions() {}
};

using buffer_iterator = std::map<FileID, RewriteBuffer>::iterator;
using const_buffer_iterator = std::map<FileID, RewriteBuffer>::const_iterator;

explicit Rewriter() = default;
explicit Rewriter(SourceManager &SM, const LangOptions &LO)
: SourceMgr(&SM), LangOpts(&LO) {}

void setSourceMgr(SourceManager &SM, const LangOptions &LO) {
SourceMgr = &SM;
LangOpts = &LO;
}

SourceManager &getSourceMgr() const { return *SourceMgr; }
const LangOptions &getLangOpts() const { return *LangOpts; }



static bool isRewritable(SourceLocation Loc) {
return Loc.isFileID();
}



int getRangeSize(SourceRange Range,
RewriteOptions opts = RewriteOptions()) const;
int getRangeSize(const CharSourceRange &Range,
RewriteOptions opts = RewriteOptions()) const;






std::string getRewrittenText(CharSourceRange Range) const;






std::string getRewrittenText(SourceRange Range) const {
return getRewrittenText(CharSourceRange::getTokenRange(Range));
}







bool InsertText(SourceLocation Loc, StringRef Str,
bool InsertAfter = true, bool indentNewLines = false);






bool InsertTextAfter(SourceLocation Loc, StringRef Str) {
return InsertText(Loc, Str);
}



bool InsertTextAfterToken(SourceLocation Loc, StringRef Str);






bool InsertTextBefore(SourceLocation Loc, StringRef Str) {
return InsertText(Loc, Str, false);
}


bool RemoveText(SourceLocation Start, unsigned Length,
RewriteOptions opts = RewriteOptions());


bool RemoveText(CharSourceRange range,
RewriteOptions opts = RewriteOptions()) {
return RemoveText(range.getBegin(), getRangeSize(range, opts), opts);
}


bool RemoveText(SourceRange range, RewriteOptions opts = RewriteOptions()) {
return RemoveText(range.getBegin(), getRangeSize(range, opts), opts);
}




bool ReplaceText(SourceLocation Start, unsigned OrigLength,
StringRef NewStr);




bool ReplaceText(CharSourceRange range, StringRef NewStr) {
return ReplaceText(range.getBegin(), getRangeSize(range), NewStr);
}




bool ReplaceText(SourceRange range, StringRef NewStr) {
return ReplaceText(range.getBegin(), getRangeSize(range), NewStr);
}




bool ReplaceText(SourceRange range, SourceRange replacementRange);





bool IncreaseIndentation(CharSourceRange range, SourceLocation parentIndent);
bool IncreaseIndentation(SourceRange range, SourceLocation parentIndent) {
return IncreaseIndentation(CharSourceRange::getTokenRange(range),
parentIndent);
}





RewriteBuffer &getEditBuffer(FileID FID);



const RewriteBuffer *getRewriteBufferFor(FileID FID) const {
std::map<FileID, RewriteBuffer>::const_iterator I =
RewriteBuffers.find(FID);
return I == RewriteBuffers.end() ? nullptr : &I->second;
}


buffer_iterator buffer_begin() { return RewriteBuffers.begin(); }
buffer_iterator buffer_end() { return RewriteBuffers.end(); }
const_buffer_iterator buffer_begin() const { return RewriteBuffers.begin(); }
const_buffer_iterator buffer_end() const { return RewriteBuffers.end(); }






bool overwriteChangedFiles();

private:
unsigned getLocationOffsetAndFileID(SourceLocation Loc, FileID &FID) const;
};

}

#endif
