












#if !defined(LLVM_CLANG_REWRITE_CORE_TOKENREWRITER_H)
#define LLVM_CLANG_REWRITE_CORE_TOKENREWRITER_H

#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Token.h"
#include <cassert>
#include <list>
#include <map>
#include <memory>

namespace clang {

class LangOptions;
class ScratchBuffer;
class SourceManager;

class TokenRewriter {


std::list<Token> TokenList;


using TokenRefTy = std::list<Token>::iterator;





std::map<SourceLocation, TokenRefTy> TokenAtLoc;


std::unique_ptr<ScratchBuffer> ScratchBuf;

public:


TokenRewriter(FileID FID, SourceManager &SM, const LangOptions &LO);

TokenRewriter(const TokenRewriter &) = delete;
TokenRewriter &operator=(const TokenRewriter &) = delete;
~TokenRewriter();

using token_iterator = std::list<Token>::const_iterator;

token_iterator token_begin() const { return TokenList.begin(); }
token_iterator token_end() const { return TokenList.end(); }

token_iterator AddTokenBefore(token_iterator I, const char *Val);

token_iterator AddTokenAfter(token_iterator I, const char *Val) {
assert(I != token_end() && "Cannot insert after token_end()!");
return AddTokenBefore(++I, Val);
}

private:


TokenRefTy RemapIterator(token_iterator I);



TokenRefTy AddToken(const Token &T, TokenRefTy Where);
};

}

#endif
