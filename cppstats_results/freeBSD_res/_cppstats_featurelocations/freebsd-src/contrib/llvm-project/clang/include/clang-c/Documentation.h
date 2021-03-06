













#if !defined(LLVM_CLANG_C_DOCUMENTATION_H)
#define LLVM_CLANG_C_DOCUMENTATION_H

#include "clang-c/ExternC.h"
#include "clang-c/Index.h"

LLVM_CLANG_C_EXTERN_C_BEGIN














typedef struct {
const void *ASTNode;
CXTranslationUnit TranslationUnit;
} CXComment;






CINDEX_LINKAGE CXComment clang_Cursor_getParsedComment(CXCursor C);






enum CXCommentKind {




CXComment_Null = 0,




CXComment_Text = 1,






CXComment_InlineCommand = 2,










CXComment_HTMLStartTag = 3,









CXComment_HTMLEndTag = 4,





CXComment_Paragraph = 5,













CXComment_BlockCommand = 6,







CXComment_ParamCommand = 7,







CXComment_TParamCommand = 8,











CXComment_VerbatimBlockCommand = 9,





CXComment_VerbatimBlockLine = 10,






CXComment_VerbatimLine = 11,




CXComment_FullComment = 12
};





enum CXCommentInlineCommandRenderKind {



CXCommentInlineCommandRenderKind_Normal,




CXCommentInlineCommandRenderKind_Bold,




CXCommentInlineCommandRenderKind_Monospaced,





CXCommentInlineCommandRenderKind_Emphasized,




CXCommentInlineCommandRenderKind_Anchor
};




enum CXCommentParamPassDirection {



CXCommentParamPassDirection_In,




CXCommentParamPassDirection_Out,




CXCommentParamPassDirection_InOut
};






CINDEX_LINKAGE enum CXCommentKind clang_Comment_getKind(CXComment Comment);






CINDEX_LINKAGE unsigned clang_Comment_getNumChildren(CXComment Comment);








CINDEX_LINKAGE
CXComment clang_Comment_getChild(CXComment Comment, unsigned ChildIdx);










CINDEX_LINKAGE unsigned clang_Comment_isWhitespace(CXComment Comment);






CINDEX_LINKAGE
unsigned clang_InlineContentComment_hasTrailingNewline(CXComment Comment);






CINDEX_LINKAGE CXString clang_TextComment_getText(CXComment Comment);






CINDEX_LINKAGE
CXString clang_InlineCommandComment_getCommandName(CXComment Comment);







CINDEX_LINKAGE enum CXCommentInlineCommandRenderKind
clang_InlineCommandComment_getRenderKind(CXComment Comment);






CINDEX_LINKAGE
unsigned clang_InlineCommandComment_getNumArgs(CXComment Comment);








CINDEX_LINKAGE
CXString clang_InlineCommandComment_getArgText(CXComment Comment,
unsigned ArgIdx);







CINDEX_LINKAGE CXString clang_HTMLTagComment_getTagName(CXComment Comment);






CINDEX_LINKAGE
unsigned clang_HTMLStartTagComment_isSelfClosing(CXComment Comment);






CINDEX_LINKAGE unsigned clang_HTMLStartTag_getNumAttrs(CXComment Comment);








CINDEX_LINKAGE
CXString clang_HTMLStartTag_getAttrName(CXComment Comment, unsigned AttrIdx);








CINDEX_LINKAGE
CXString clang_HTMLStartTag_getAttrValue(CXComment Comment, unsigned AttrIdx);






CINDEX_LINKAGE
CXString clang_BlockCommandComment_getCommandName(CXComment Comment);






CINDEX_LINKAGE
unsigned clang_BlockCommandComment_getNumArgs(CXComment Comment);








CINDEX_LINKAGE
CXString clang_BlockCommandComment_getArgText(CXComment Comment,
unsigned ArgIdx);







CINDEX_LINKAGE
CXComment clang_BlockCommandComment_getParagraph(CXComment Comment);






CINDEX_LINKAGE
CXString clang_ParamCommandComment_getParamName(CXComment Comment);








CINDEX_LINKAGE
unsigned clang_ParamCommandComment_isParamIndexValid(CXComment Comment);






CINDEX_LINKAGE
unsigned clang_ParamCommandComment_getParamIndex(CXComment Comment);







CINDEX_LINKAGE
unsigned clang_ParamCommandComment_isDirectionExplicit(CXComment Comment);






CINDEX_LINKAGE
enum CXCommentParamPassDirection clang_ParamCommandComment_getDirection(
CXComment Comment);






CINDEX_LINKAGE
CXString clang_TParamCommandComment_getParamName(CXComment Comment);










CINDEX_LINKAGE
unsigned clang_TParamCommandComment_isParamPositionValid(CXComment Comment);














CINDEX_LINKAGE
unsigned clang_TParamCommandComment_getDepth(CXComment Comment);



















CINDEX_LINKAGE
unsigned clang_TParamCommandComment_getIndex(CXComment Comment, unsigned Depth);






CINDEX_LINKAGE
CXString clang_VerbatimBlockLineComment_getText(CXComment Comment);






CINDEX_LINKAGE CXString clang_VerbatimLineComment_getText(CXComment Comment);









CINDEX_LINKAGE CXString clang_HTMLTagComment_getAsString(CXComment Comment);
































CINDEX_LINKAGE CXString clang_FullComment_getAsHTML(CXComment Comment);











CINDEX_LINKAGE CXString clang_FullComment_getAsXML(CXComment Comment);





LLVM_CLANG_C_EXTERN_C_END

#endif

