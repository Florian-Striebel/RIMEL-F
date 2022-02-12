












#if !defined(LLVM_CLANG_LEX_LITERALSUPPORT_H)
#define LLVM_CLANG_LEX_LITERALSUPPORT_H

#include "clang/Basic/CharInfo.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/TokenKinds.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/DataTypes.h"

namespace clang {

class DiagnosticsEngine;
class Preprocessor;
class Token;
class SourceLocation;
class TargetInfo;
class SourceManager;
class LangOptions;


void expandUCNs(SmallVectorImpl<char> &Buf, StringRef Input);




class NumericLiteralParser {
const SourceManager &SM;
const LangOptions &LangOpts;
DiagnosticsEngine &Diags;

const char *const ThisTokBegin;
const char *const ThisTokEnd;
const char *DigitsBegin, *SuffixBegin;
const char *s;

unsigned radix;

bool saw_exponent, saw_period, saw_ud_suffix, saw_fixed_point_suffix;

SmallString<32> UDSuffixBuf;

public:
NumericLiteralParser(StringRef TokSpelling, SourceLocation TokLoc,
const SourceManager &SM, const LangOptions &LangOpts,
const TargetInfo &Target, DiagnosticsEngine &Diags);
bool hadError : 1;
bool isUnsigned : 1;
bool isLong : 1;
bool isLongLong : 1;
bool isSizeT : 1;
bool isHalf : 1;
bool isFloat : 1;
bool isImaginary : 1;
bool isFloat16 : 1;
bool isFloat128 : 1;
uint8_t MicrosoftInteger;

bool isFract : 1;
bool isAccum : 1;

bool isFixedPointLiteral() const {
return (saw_period || saw_exponent) && saw_fixed_point_suffix;
}

bool isIntegerLiteral() const {
return !saw_period && !saw_exponent && !isFixedPointLiteral();
}
bool isFloatingLiteral() const {
return (saw_period || saw_exponent) && !isFixedPointLiteral();
}

bool hasUDSuffix() const {
return saw_ud_suffix;
}
StringRef getUDSuffix() const {
assert(saw_ud_suffix);
return UDSuffixBuf;
}
unsigned getUDSuffixOffset() const {
assert(saw_ud_suffix);
return SuffixBegin - ThisTokBegin;
}

static bool isValidUDSuffix(const LangOptions &LangOpts, StringRef Suffix);

unsigned getRadix() const { return radix; }





bool GetIntegerValue(llvm::APInt &Val);






llvm::APFloat::opStatus GetFloatValue(llvm::APFloat &Result);





bool GetFixedPointValue(llvm::APInt &StoreVal, unsigned Scale);

private:

void ParseNumberStartingWithZero(SourceLocation TokLoc);
void ParseDecimalOrOctalCommon(SourceLocation TokLoc);

static bool isDigitSeparator(char C) { return C == '\''; }



bool containsDigits(const char *Start, const char *End) {
return Start != End && (Start + 1 != End || !isDigitSeparator(Start[0]));
}

enum CheckSeparatorKind { CSK_BeforeDigits, CSK_AfterDigits };


void checkSeparator(SourceLocation TokLoc, const char *Pos,
CheckSeparatorKind IsAfterDigits);



const char *SkipHexDigits(const char *ptr) {
while (ptr != ThisTokEnd && (isHexDigit(*ptr) || isDigitSeparator(*ptr)))
ptr++;
return ptr;
}



const char *SkipOctalDigits(const char *ptr) {
while (ptr != ThisTokEnd &&
((*ptr >= '0' && *ptr <= '7') || isDigitSeparator(*ptr)))
ptr++;
return ptr;
}



const char *SkipDigits(const char *ptr) {
while (ptr != ThisTokEnd && (isDigit(*ptr) || isDigitSeparator(*ptr)))
ptr++;
return ptr;
}



const char *SkipBinaryDigits(const char *ptr) {
while (ptr != ThisTokEnd &&
(*ptr == '0' || *ptr == '1' || isDigitSeparator(*ptr)))
ptr++;
return ptr;
}

};



class CharLiteralParser {
uint64_t Value;
tok::TokenKind Kind;
bool IsMultiChar;
bool HadError;
SmallString<32> UDSuffixBuf;
unsigned UDSuffixOffset;
public:
CharLiteralParser(const char *begin, const char *end,
SourceLocation Loc, Preprocessor &PP,
tok::TokenKind kind);

bool hadError() const { return HadError; }
bool isAscii() const { return Kind == tok::char_constant; }
bool isWide() const { return Kind == tok::wide_char_constant; }
bool isUTF8() const { return Kind == tok::utf8_char_constant; }
bool isUTF16() const { return Kind == tok::utf16_char_constant; }
bool isUTF32() const { return Kind == tok::utf32_char_constant; }
bool isMultiChar() const { return IsMultiChar; }
uint64_t getValue() const { return Value; }
StringRef getUDSuffix() const { return UDSuffixBuf; }
unsigned getUDSuffixOffset() const {
assert(!UDSuffixBuf.empty() && "no ud-suffix");
return UDSuffixOffset;
}
};




class StringLiteralParser {
const SourceManager &SM;
const LangOptions &Features;
const TargetInfo &Target;
DiagnosticsEngine *Diags;

unsigned MaxTokenLength;
unsigned SizeBound;
unsigned CharByteWidth;
tok::TokenKind Kind;
SmallString<512> ResultBuf;
char *ResultPtr;
SmallString<32> UDSuffixBuf;
unsigned UDSuffixToken;
unsigned UDSuffixOffset;
public:
StringLiteralParser(ArrayRef<Token> StringToks,
Preprocessor &PP, bool Complain = true);
StringLiteralParser(ArrayRef<Token> StringToks,
const SourceManager &sm, const LangOptions &features,
const TargetInfo &target,
DiagnosticsEngine *diags = nullptr)
: SM(sm), Features(features), Target(target), Diags(diags),
MaxTokenLength(0), SizeBound(0), CharByteWidth(0), Kind(tok::unknown),
ResultPtr(ResultBuf.data()), hadError(false), Pascal(false) {
init(StringToks);
}


bool hadError;
bool Pascal;

StringRef GetString() const {
return StringRef(ResultBuf.data(), GetStringLength());
}
unsigned GetStringLength() const { return ResultPtr-ResultBuf.data(); }

unsigned GetNumStringChars() const {
return GetStringLength() / CharByteWidth;
}






unsigned getOffsetOfStringByte(const Token &TheTok, unsigned ByteNo) const;

bool isAscii() const { return Kind == tok::string_literal; }
bool isWide() const { return Kind == tok::wide_string_literal; }
bool isUTF8() const { return Kind == tok::utf8_string_literal; }
bool isUTF16() const { return Kind == tok::utf16_string_literal; }
bool isUTF32() const { return Kind == tok::utf32_string_literal; }
bool isPascal() const { return Pascal; }

StringRef getUDSuffix() const { return UDSuffixBuf; }


unsigned getUDSuffixToken() const {
assert(!UDSuffixBuf.empty() && "no ud-suffix");
return UDSuffixToken;
}

unsigned getUDSuffixOffset() const {
assert(!UDSuffixBuf.empty() && "no ud-suffix");
return UDSuffixOffset;
}

static bool isValidUDSuffix(const LangOptions &LangOpts, StringRef Suffix);

private:
void init(ArrayRef<Token> StringToks);
bool CopyStringFragment(const Token &Tok, const char *TokBegin,
StringRef Fragment);
void DiagnoseLexingError(SourceLocation Loc);
};

}

#endif
