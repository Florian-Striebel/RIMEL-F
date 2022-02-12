











#if !defined(LLVM_CLANG_LEX_TOKENLEXER_H)
#define LLVM_CLANG_LEX_TOKENLEXER_H

#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"

namespace clang {

class MacroArgs;
class MacroInfo;
class Preprocessor;
class Token;
class VAOptExpansionContext;




class TokenLexer {
friend class Preprocessor;


MacroInfo *Macro = nullptr;



MacroArgs *ActualArgs = nullptr;


Preprocessor &PP;









const Token *Tokens;


unsigned NumTokens;


unsigned CurTokenIdx;


SourceLocation ExpandLocStart, ExpandLocEnd;



SourceLocation MacroExpansionStart;



unsigned MacroStartSLocOffset;


SourceLocation MacroDefStart;


unsigned MacroDefLength;



bool AtStartOfLine : 1;
bool HasLeadingSpace : 1;







bool NextTokGetsSpace : 1;





bool OwnsTokens : 1;



bool DisableMacroExpansion : 1;



bool IsReinject : 1;

public:




TokenLexer(Token &Tok, SourceLocation ILEnd, MacroInfo *MI,
MacroArgs *ActualArgs, Preprocessor &pp)
: PP(pp), OwnsTokens(false) {
Init(Tok, ILEnd, MI, ActualArgs);
}




TokenLexer(const Token *TokArray, unsigned NumToks, bool DisableExpansion,
bool ownsTokens, bool isReinject, Preprocessor &pp)
: PP(pp), OwnsTokens(false) {
Init(TokArray, NumToks, DisableExpansion, ownsTokens, isReinject);
}

TokenLexer(const TokenLexer &) = delete;
TokenLexer &operator=(const TokenLexer &) = delete;
~TokenLexer() { destroy(); }





void Init(Token &Tok, SourceLocation ELEnd, MacroInfo *MI,
MacroArgs *Actuals);






void Init(const Token *TokArray, unsigned NumToks, bool DisableMacroExpansion,
bool OwnsTokens, bool IsReinject);




unsigned isNextTokenLParen() const;


bool Lex(Token &Tok);



bool isParsingPreprocessorDirective() const;

private:
void destroy();



bool isAtEnd() const {
return CurTokenIdx == NumTokens;
}






















bool pasteTokens(Token &LHSTok, ArrayRef<Token> TokenStream,
unsigned int &CurIdx);



bool pasteTokens(Token &Tok);















void stringifyVAOPTContents(SmallVectorImpl<Token> &ResultToks,
const VAOptExpansionContext &VCtx,
SourceLocation VAOPTClosingParenLoc);



void ExpandFunctionArguments();






void HandleMicrosoftCommentPaste(Token &Tok, SourceLocation OpLoc);




SourceLocation getExpansionLocForMacroDefLoc(SourceLocation loc) const;






void updateLocForMacroArgTokens(SourceLocation ArgIdSpellLoc,
Token *begin_tokens, Token *end_tokens);



bool MaybeRemoveCommaBeforeVaArgs(SmallVectorImpl<Token> &ResultToks,
bool HasPasteOperator,
MacroInfo *Macro, unsigned MacroArgNo,
Preprocessor &PP);

void PropagateLineStartLeadingSpaceInfo(Token &Result);
};

}

#endif
