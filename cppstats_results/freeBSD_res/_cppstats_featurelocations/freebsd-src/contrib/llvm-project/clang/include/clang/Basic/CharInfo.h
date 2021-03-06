







#if !defined(LLVM_CLANG_BASIC_CHARINFO_H)
#define LLVM_CLANG_BASIC_CHARINFO_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/DataTypes.h"

namespace clang {
namespace charinfo {
extern const uint16_t InfoTable[256];

enum {
CHAR_HORZ_WS = 0x0001,
CHAR_VERT_WS = 0x0002,
CHAR_SPACE = 0x0004,
CHAR_DIGIT = 0x0008,
CHAR_XLETTER = 0x0010,
CHAR_UPPER = 0x0020,
CHAR_LOWER = 0x0040,
CHAR_UNDER = 0x0080,
CHAR_PERIOD = 0x0100,
CHAR_RAWDEL = 0x0200,
CHAR_PUNCT = 0x0400
};

enum {
CHAR_XUPPER = CHAR_XLETTER | CHAR_UPPER,
CHAR_XLOWER = CHAR_XLETTER | CHAR_LOWER
};
}


LLVM_READNONE inline bool isASCII(char c) {
return static_cast<unsigned char>(c) <= 127;
}



LLVM_READONLY inline bool isIdentifierHead(unsigned char c,
bool AllowDollar = false) {
using namespace charinfo;
if (InfoTable[c] & (CHAR_UPPER|CHAR_LOWER|CHAR_UNDER))
return true;
return AllowDollar && c == '$';
}



LLVM_READONLY inline bool isIdentifierBody(unsigned char c,
bool AllowDollar = false) {
using namespace charinfo;
if (InfoTable[c] & (CHAR_UPPER|CHAR_LOWER|CHAR_DIGIT|CHAR_UNDER))
return true;
return AllowDollar && c == '$';
}





LLVM_READONLY inline bool isHorizontalWhitespace(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] & (CHAR_HORZ_WS|CHAR_SPACE)) != 0;
}




LLVM_READONLY inline bool isVerticalWhitespace(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] & CHAR_VERT_WS) != 0;
}





LLVM_READONLY inline bool isWhitespace(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] & (CHAR_HORZ_WS|CHAR_VERT_WS|CHAR_SPACE)) != 0;
}


LLVM_READONLY inline bool isDigit(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] & CHAR_DIGIT) != 0;
}


LLVM_READONLY inline bool isLowercase(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] & CHAR_LOWER) != 0;
}


LLVM_READONLY inline bool isUppercase(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] & CHAR_UPPER) != 0;
}


LLVM_READONLY inline bool isLetter(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] & (CHAR_UPPER|CHAR_LOWER)) != 0;
}


LLVM_READONLY inline bool isAlphanumeric(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] & (CHAR_DIGIT|CHAR_UPPER|CHAR_LOWER)) != 0;
}


LLVM_READONLY inline bool isHexDigit(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] & (CHAR_DIGIT|CHAR_XLETTER)) != 0;
}




LLVM_READONLY inline bool isPunctuation(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] & (CHAR_UNDER|CHAR_PERIOD|CHAR_RAWDEL|CHAR_PUNCT)) != 0;
}




LLVM_READONLY inline bool isPrintable(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] & (CHAR_UPPER|CHAR_LOWER|CHAR_PERIOD|CHAR_PUNCT|
CHAR_DIGIT|CHAR_UNDER|CHAR_RAWDEL|CHAR_SPACE)) != 0;
}



LLVM_READONLY inline bool isPreprocessingNumberBody(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] &
(CHAR_UPPER|CHAR_LOWER|CHAR_DIGIT|CHAR_UNDER|CHAR_PERIOD)) != 0;
}


LLVM_READONLY inline bool isRawStringDelimBody(unsigned char c) {
using namespace charinfo;
return (InfoTable[c] & (CHAR_UPPER|CHAR_LOWER|CHAR_PERIOD|
CHAR_DIGIT|CHAR_UNDER|CHAR_RAWDEL)) != 0;
}





LLVM_READONLY inline char toLowercase(char c) {
if (isUppercase(c))
return c + 'a' - 'A';
return c;
}




LLVM_READONLY inline char toUppercase(char c) {
if (isLowercase(c))
return c + 'A' - 'a';
return c;
}






LLVM_READONLY inline bool isValidIdentifier(StringRef S,
bool AllowDollar = false) {
if (S.empty() || !isIdentifierHead(S[0], AllowDollar))
return false;

for (StringRef::iterator I = S.begin(), E = S.end(); I != E; ++I)
if (!isIdentifierBody(*I, AllowDollar))
return false;

return true;
}

}

#endif
