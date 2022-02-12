











#if !defined(LLVM_CLANG_LEX_MACROARGS_H)
#define LLVM_CLANG_LEX_MACROARGS_H

#include "clang/Basic/LLVM.h"
#include "clang/Lex/Token.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/TrailingObjects.h"
#include <vector>

namespace clang {
class MacroInfo;
class Preprocessor;
class SourceLocation;



class MacroArgs final
: private llvm::TrailingObjects<MacroArgs, Token> {

friend TrailingObjects;




unsigned NumUnexpArgTokens;






bool VarargsElided;




std::vector<std::vector<Token> > PreExpArgTokens;



MacroArgs *ArgCache;


unsigned NumMacroArgs;

MacroArgs(unsigned NumToks, bool varargsElided, unsigned MacroArgs)
: NumUnexpArgTokens(NumToks), VarargsElided(varargsElided),
ArgCache(nullptr), NumMacroArgs(MacroArgs) {}
~MacroArgs() = default;

public:


static MacroArgs *create(const MacroInfo *MI,
ArrayRef<Token> UnexpArgTokens,
bool VarargsElided, Preprocessor &PP);



void destroy(Preprocessor &PP);



bool ArgNeedsPreexpansion(const Token *ArgTok, Preprocessor &PP) const;




const Token *getUnexpArgument(unsigned Arg) const;




static unsigned getArgLength(const Token *ArgPtr);



const std::vector<Token> &
getPreExpArgument(unsigned Arg, Preprocessor &PP);



unsigned getNumMacroArguments() const { return NumMacroArgs; }






bool isVarargsElidedUse() const { return VarargsElided; }














bool invokedWithVariadicArgument(const MacroInfo *const MI, Preprocessor &PP);






static Token StringifyArgument(const Token *ArgToks,
Preprocessor &PP, bool Charify,
SourceLocation ExpansionLocStart,
SourceLocation ExpansionLocEnd);




MacroArgs *deallocate();
};

}

#endif
