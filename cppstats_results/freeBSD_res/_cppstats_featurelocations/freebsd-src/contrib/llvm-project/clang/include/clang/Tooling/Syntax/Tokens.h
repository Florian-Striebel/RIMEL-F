

























#if !defined(LLVM_CLANG_TOOLING_SYNTAX_TOKENS_H)
#define LLVM_CLANG_TOOLING_SYNTAX_TOKENS_H

#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Lex/Token.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdint>
#include <tuple>

namespace clang {
class Preprocessor;

namespace syntax {



struct FileRange {

FileRange(FileID File, unsigned BeginOffset, unsigned EndOffset);

FileRange(const SourceManager &SM, SourceLocation BeginLoc, unsigned Length);


FileRange(const SourceManager &SM, SourceLocation BeginLoc,
SourceLocation EndLoc);

FileID file() const { return File; }

unsigned beginOffset() const { return Begin; }

unsigned endOffset() const { return End; }

unsigned length() const { return End - Begin; }


bool contains(unsigned Offset) const {
return Begin <= Offset && Offset < End;
}

bool touches(unsigned Offset) const {
return Begin <= Offset && Offset <= End;
}


llvm::StringRef text(const SourceManager &SM) const;



CharSourceRange toCharRange(const SourceManager &SM) const;

friend bool operator==(const FileRange &L, const FileRange &R) {
return std::tie(L.File, L.Begin, L.End) == std::tie(R.File, R.Begin, R.End);
}
friend bool operator!=(const FileRange &L, const FileRange &R) {
return !(L == R);
}

private:
FileID File;
unsigned Begin;
unsigned End;
};


llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const FileRange &R);




class Token {
public:
Token(SourceLocation Location, unsigned Length, tok::TokenKind Kind);

explicit Token(const clang::Token &T);

tok::TokenKind kind() const { return Kind; }

SourceLocation location() const { return Location; }

SourceLocation endLocation() const {
return Location.getLocWithOffset(Length);
}
unsigned length() const { return Length; }






llvm::StringRef text(const SourceManager &SM) const;



FileRange range(const SourceManager &SM) const;





static FileRange range(const SourceManager &SM, const syntax::Token &First,
const syntax::Token &Last);

std::string dumpForTests(const SourceManager &SM) const;

std::string str() const;

private:
SourceLocation Location;
unsigned Length;
tok::TokenKind Kind;
};

llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const Token &T);



























class TokenBuffer {
public:
TokenBuffer(const SourceManager &SourceMgr) : SourceMgr(&SourceMgr) {}

TokenBuffer(TokenBuffer &&) = default;
TokenBuffer(const TokenBuffer &) = delete;
TokenBuffer &operator=(TokenBuffer &&) = default;
TokenBuffer &operator=(const TokenBuffer &) = delete;








llvm::ArrayRef<syntax::Token> expandedTokens() const {
return ExpandedTokens;
}



void indexExpandedTokens();




llvm::ArrayRef<syntax::Token> expandedTokens(SourceRange R) const;





























llvm::Optional<llvm::ArrayRef<syntax::Token>>
spelledForExpanded(llvm::ArrayRef<syntax::Token> Expanded) const;




























llvm::SmallVector<llvm::ArrayRef<syntax::Token>, 1>
expandedForSpelled(llvm::ArrayRef<syntax::Token> Spelled) const;










struct Expansion {
llvm::ArrayRef<syntax::Token> Spelled;
llvm::ArrayRef<syntax::Token> Expanded;
};



llvm::Optional<Expansion>
expansionStartingAt(const syntax::Token *Spelled) const;


std::vector<Expansion>
expansionsOverlapping(llvm::ArrayRef<syntax::Token> Spelled) const;







llvm::ArrayRef<syntax::Token> spelledTokens(FileID FID) const;



const syntax::Token *spelledTokenAt(SourceLocation Loc) const;









std::vector<const syntax::Token *> macroExpansions(FileID FID) const;

const SourceManager &sourceManager() const { return *SourceMgr; }

std::string dumpForTests() const;

private:








struct Mapping {


unsigned BeginSpelled = 0;
unsigned EndSpelled = 0;


unsigned BeginExpanded = 0;
unsigned EndExpanded = 0;


std::string str() const;
};

struct MarkedFile {





std::vector<syntax::Token> SpelledTokens;

std::vector<Mapping> Mappings;

unsigned BeginExpanded = 0;
unsigned EndExpanded = 0;
};

friend class TokenCollector;



std::pair<const syntax::Token *, const Mapping *>
spelledForExpandedToken(const syntax::Token *Expanded) const;



static const Mapping *
mappingStartingBeforeSpelled(const MarkedFile &F,
const syntax::Token *Spelled);


Expansion makeExpansion(const MarkedFile &, const Mapping &) const;


const MarkedFile &fileForSpelled(llvm::ArrayRef<syntax::Token> Spelled) const;




std::vector<syntax::Token> ExpandedTokens;

llvm::DenseMap<SourceLocation, unsigned> ExpandedTokIndex;
llvm::DenseMap<FileID, MarkedFile> Files;


const SourceManager *SourceMgr;
};



llvm::ArrayRef<syntax::Token>
spelledTokensTouching(SourceLocation Loc, const syntax::TokenBuffer &Tokens);
llvm::ArrayRef<syntax::Token>
spelledTokensTouching(SourceLocation Loc, llvm::ArrayRef<syntax::Token> Tokens);



const syntax::Token *
spelledIdentifierTouching(SourceLocation Loc,
llvm::ArrayRef<syntax::Token> Tokens);
const syntax::Token *
spelledIdentifierTouching(SourceLocation Loc,
const syntax::TokenBuffer &Tokens);









std::vector<syntax::Token> tokenize(FileID FID, const SourceManager &SM,
const LangOptions &LO);




std::vector<syntax::Token>
tokenize(const FileRange &FR, const SourceManager &SM, const LangOptions &LO);





class TokenCollector {
public:



TokenCollector(Preprocessor &P);



LLVM_NODISCARD TokenBuffer consume() &&;

private:

















using PPExpansions = llvm::DenseMap<SourceLocation, SourceLocation>;
class Builder;
class CollectPPExpansions;

std::vector<syntax::Token> Expanded;

PPExpansions Expansions;
Preprocessor &PP;
CollectPPExpansions *Collector;
};

}
}

#endif
