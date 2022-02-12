











#if !defined(LLVM_CLANG_AST_COMMENTSEMA_H)
#define LLVM_CLANG_AST_COMMENTSEMA_H

#include "clang/AST/Comment.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"

namespace clang {
class Decl;
class SourceMgr;
class Preprocessor;

namespace comments {
class CommandTraits;

class Sema {
Sema(const Sema &) = delete;
void operator=(const Sema &) = delete;


llvm::BumpPtrAllocator &Allocator;


const SourceManager &SourceMgr;

DiagnosticsEngine &Diags;

CommandTraits &Traits;

const Preprocessor *PP;


DeclInfo *ThisDeclInfo;





llvm::StringMap<TParamCommandComment *> TemplateParameterDocs;


const BlockCommandComment *BriefCommand;


const BlockCommandComment *HeaderfileCommand;

DiagnosticBuilder Diag(SourceLocation Loc, unsigned DiagID) {
return Diags.Report(Loc, DiagID);
}



SmallVector<HTMLStartTagComment *, 8> HTMLOpenTags;

public:
Sema(llvm::BumpPtrAllocator &Allocator, const SourceManager &SourceMgr,
DiagnosticsEngine &Diags, CommandTraits &Traits,
const Preprocessor *PP);

void setDecl(const Decl *D);


template<typename T>
ArrayRef<T> copyArray(ArrayRef<T> Source) {
if (!Source.empty())
return Source.copy(Allocator);
return None;
}

ParagraphComment *actOnParagraphComment(
ArrayRef<InlineContentComment *> Content);

BlockCommandComment *actOnBlockCommandStart(SourceLocation LocBegin,
SourceLocation LocEnd,
unsigned CommandID,
CommandMarkerKind CommandMarker);

void actOnBlockCommandArgs(BlockCommandComment *Command,
ArrayRef<BlockCommandComment::Argument> Args);

void actOnBlockCommandFinish(BlockCommandComment *Command,
ParagraphComment *Paragraph);

ParamCommandComment *actOnParamCommandStart(SourceLocation LocBegin,
SourceLocation LocEnd,
unsigned CommandID,
CommandMarkerKind CommandMarker);

void actOnParamCommandDirectionArg(ParamCommandComment *Command,
SourceLocation ArgLocBegin,
SourceLocation ArgLocEnd,
StringRef Arg);

void actOnParamCommandParamNameArg(ParamCommandComment *Command,
SourceLocation ArgLocBegin,
SourceLocation ArgLocEnd,
StringRef Arg);

void actOnParamCommandFinish(ParamCommandComment *Command,
ParagraphComment *Paragraph);

TParamCommandComment *actOnTParamCommandStart(SourceLocation LocBegin,
SourceLocation LocEnd,
unsigned CommandID,
CommandMarkerKind CommandMarker);

void actOnTParamCommandParamNameArg(TParamCommandComment *Command,
SourceLocation ArgLocBegin,
SourceLocation ArgLocEnd,
StringRef Arg);

void actOnTParamCommandFinish(TParamCommandComment *Command,
ParagraphComment *Paragraph);

InlineCommandComment *actOnInlineCommand(SourceLocation CommandLocBegin,
SourceLocation CommandLocEnd,
unsigned CommandID);

InlineCommandComment *actOnInlineCommand(SourceLocation CommandLocBegin,
SourceLocation CommandLocEnd,
unsigned CommandID,
SourceLocation ArgLocBegin,
SourceLocation ArgLocEnd,
StringRef Arg);

InlineContentComment *actOnUnknownCommand(SourceLocation LocBegin,
SourceLocation LocEnd,
StringRef CommandName);

InlineContentComment *actOnUnknownCommand(SourceLocation LocBegin,
SourceLocation LocEnd,
unsigned CommandID);

TextComment *actOnText(SourceLocation LocBegin,
SourceLocation LocEnd,
StringRef Text);

VerbatimBlockComment *actOnVerbatimBlockStart(SourceLocation Loc,
unsigned CommandID);

VerbatimBlockLineComment *actOnVerbatimBlockLine(SourceLocation Loc,
StringRef Text);

void actOnVerbatimBlockFinish(VerbatimBlockComment *Block,
SourceLocation CloseNameLocBegin,
StringRef CloseName,
ArrayRef<VerbatimBlockLineComment *> Lines);

VerbatimLineComment *actOnVerbatimLine(SourceLocation LocBegin,
unsigned CommandID,
SourceLocation TextBegin,
StringRef Text);

HTMLStartTagComment *actOnHTMLStartTagStart(SourceLocation LocBegin,
StringRef TagName);

void actOnHTMLStartTagFinish(HTMLStartTagComment *Tag,
ArrayRef<HTMLStartTagComment::Attribute> Attrs,
SourceLocation GreaterLoc,
bool IsSelfClosing);

HTMLEndTagComment *actOnHTMLEndTag(SourceLocation LocBegin,
SourceLocation LocEnd,
StringRef TagName);

FullComment *actOnFullComment(ArrayRef<BlockContentComment *> Blocks);

void checkBlockCommandEmptyParagraph(BlockCommandComment *Command);

void checkReturnsCommand(const BlockCommandComment *Command);



void checkBlockCommandDuplicate(const BlockCommandComment *Command);

void checkDeprecatedCommand(const BlockCommandComment *Comment);

void checkFunctionDeclVerbatimLine(const BlockCommandComment *Comment);

void checkContainerDeclVerbatimLine(const BlockCommandComment *Comment);

void checkContainerDecl(const BlockCommandComment *Comment);



void resolveParamCommandIndexes(const FullComment *FC);

bool isFunctionDecl();
bool isAnyFunctionDecl();



bool isFunctionPointerVarDecl();



bool isFunctionOrBlockPointerVarLikeDecl();
bool isFunctionOrMethodVariadic();
bool isObjCMethodDecl();
bool isObjCPropertyDecl();
bool isTemplateOrSpecialization();
bool isRecordLikeDecl();
bool isClassOrStructDecl();


bool isClassOrStructOrTagTypedefDecl();
bool isUnionDecl();
bool isObjCInterfaceDecl();
bool isObjCProtocolDecl();
bool isClassTemplateDecl();
bool isFunctionTemplateDecl();

ArrayRef<const ParmVarDecl *> getParamVars();



void inspectThisDecl();


unsigned resolveParmVarReference(StringRef Name,
ArrayRef<const ParmVarDecl *> ParamVars);



unsigned correctTypoInParmVarReference(StringRef Typo,
ArrayRef<const ParmVarDecl *> ParamVars);

bool resolveTParamReference(StringRef Name,
const TemplateParameterList *TemplateParameters,
SmallVectorImpl<unsigned> *Position);

StringRef correctTypoInTParamReference(
StringRef Typo,
const TemplateParameterList *TemplateParameters);

InlineCommandComment::RenderKind
getInlineCommandRenderKind(StringRef Name) const;
};

}
}

#endif

