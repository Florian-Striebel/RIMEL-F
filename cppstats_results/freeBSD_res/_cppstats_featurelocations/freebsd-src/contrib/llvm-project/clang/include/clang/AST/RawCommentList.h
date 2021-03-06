







#if !defined(LLVM_CLANG_AST_RAWCOMMENTLIST_H)
#define LLVM_CLANG_AST_RAWCOMMENTLIST_H

#include "clang/Basic/CommentOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Allocator.h"
#include <map>

namespace clang {

class ASTContext;
class ASTReader;
class Decl;
class DiagnosticsEngine;
class Preprocessor;
class SourceManager;

namespace comments {
class FullComment;
}

class RawComment {
public:
enum CommentKind {
RCK_Invalid,
RCK_OrdinaryBCPL,
RCK_OrdinaryC,
RCK_BCPLSlash,
RCK_BCPLExcl,
RCK_JavaDoc,
RCK_Qt,
RCK_Merged
};

RawComment() : Kind(RCK_Invalid), IsAlmostTrailingComment(false) { }

RawComment(const SourceManager &SourceMgr, SourceRange SR,
const CommentOptions &CommentOpts, bool Merged);

CommentKind getKind() const LLVM_READONLY {
return (CommentKind) Kind;
}

bool isInvalid() const LLVM_READONLY {
return Kind == RCK_Invalid;
}

bool isMerged() const LLVM_READONLY {
return Kind == RCK_Merged;
}


bool isAttached() const LLVM_READONLY {
return IsAttached;
}

void setAttached() {
IsAttached = true;
}






bool isTrailingComment() const LLVM_READONLY {
return IsTrailingComment;
}




bool isAlmostTrailingComment() const LLVM_READONLY {
return IsAlmostTrailingComment;
}


bool isOrdinary() const LLVM_READONLY {
return ((Kind == RCK_OrdinaryBCPL) || (Kind == RCK_OrdinaryC));
}


bool isDocumentation() const LLVM_READONLY {
return !isInvalid() && !isOrdinary();
}


StringRef getRawText(const SourceManager &SourceMgr) const {
if (RawTextValid)
return RawText;

RawText = getRawTextSlow(SourceMgr);
RawTextValid = true;
return RawText;
}

SourceRange getSourceRange() const LLVM_READONLY { return Range; }
SourceLocation getBeginLoc() const LLVM_READONLY { return Range.getBegin(); }
SourceLocation getEndLoc() const LLVM_READONLY { return Range.getEnd(); }

const char *getBriefText(const ASTContext &Context) const {
if (BriefTextValid)
return BriefText;

return extractBriefText(Context);
}






















std::string getFormattedText(const SourceManager &SourceMgr,
DiagnosticsEngine &Diags) const;


comments::FullComment *parse(const ASTContext &Context,
const Preprocessor *PP, const Decl *D) const;

private:
SourceRange Range;

mutable StringRef RawText;
mutable const char *BriefText;

mutable bool RawTextValid : 1;
mutable bool BriefTextValid : 1;

unsigned Kind : 3;


bool IsAttached : 1;

bool IsTrailingComment : 1;
bool IsAlmostTrailingComment : 1;


RawComment(SourceRange SR, CommentKind K, bool IsTrailingComment,
bool IsAlmostTrailingComment) :
Range(SR), RawTextValid(false), BriefTextValid(false), Kind(K),
IsAttached(false), IsTrailingComment(IsTrailingComment),
IsAlmostTrailingComment(IsAlmostTrailingComment)
{ }

StringRef getRawTextSlow(const SourceManager &SourceMgr) const;

const char *extractBriefText(const ASTContext &Context) const;

friend class ASTReader;
};



class RawCommentList {
public:
RawCommentList(SourceManager &SourceMgr) : SourceMgr(SourceMgr) {}

void addComment(const RawComment &RC, const CommentOptions &CommentOpts,
llvm::BumpPtrAllocator &Allocator);



const std::map<unsigned, RawComment *> *getCommentsInFile(FileID File) const;

bool empty() const;

unsigned getCommentBeginLine(RawComment *C, FileID File,
unsigned Offset) const;
unsigned getCommentEndOffset(RawComment *C) const;

private:
SourceManager &SourceMgr;

llvm::DenseMap<FileID, std::map<unsigned, RawComment *>> OrderedComments;
mutable llvm::DenseMap<RawComment *, unsigned> CommentBeginLine;
mutable llvm::DenseMap<RawComment *, unsigned> CommentEndOffset;

friend class ASTReader;
friend class ASTWriter;
};

}

#endif
