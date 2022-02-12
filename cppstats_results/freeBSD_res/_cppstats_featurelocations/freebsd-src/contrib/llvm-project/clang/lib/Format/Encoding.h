













#if !defined(LLVM_CLANG_LIB_FORMAT_ENCODING_H)
#define LLVM_CLANG_LIB_FORMAT_ENCODING_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ConvertUTF.h"
#include "llvm/Support/Unicode.h"

namespace clang {
namespace format {
namespace encoding {

enum Encoding {
Encoding_UTF8,
Encoding_Unknown
};



inline Encoding detectEncoding(StringRef Text) {
const llvm::UTF8 *Ptr = reinterpret_cast<const llvm::UTF8 *>(Text.begin());
const llvm::UTF8 *BufEnd = reinterpret_cast<const llvm::UTF8 *>(Text.end());
if (llvm::isLegalUTF8String(&Ptr, BufEnd))
return Encoding_UTF8;
return Encoding_Unknown;
}




inline unsigned columnWidth(StringRef Text, Encoding Encoding) {
if (Encoding == Encoding_UTF8) {
int ContentWidth = llvm::sys::unicode::columnWidthUTF8(Text);




if (ContentWidth >= 0)
return ContentWidth;
}
return Text.size();
}




inline unsigned columnWidthWithTabs(StringRef Text, unsigned StartColumn,
unsigned TabWidth, Encoding Encoding) {
unsigned TotalWidth = 0;
StringRef Tail = Text;
for (;;) {
StringRef::size_type TabPos = Tail.find('\t');
if (TabPos == StringRef::npos)
return TotalWidth + columnWidth(Tail, Encoding);
TotalWidth += columnWidth(Tail.substr(0, TabPos), Encoding);
if (TabWidth)
TotalWidth += TabWidth - (TotalWidth + StartColumn) % TabWidth;
Tail = Tail.substr(TabPos + 1);
}
}



inline unsigned getCodePointNumBytes(char FirstChar, Encoding Encoding) {
switch (Encoding) {
case Encoding_UTF8:
return llvm::getNumBytesForUTF8(FirstChar);
default:
return 1;
}
}

inline bool isOctDigit(char c) { return '0' <= c && c <= '7'; }

inline bool isHexDigit(char c) {
return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') ||
('A' <= c && c <= 'F');
}




inline unsigned getEscapeSequenceLength(StringRef Text) {
assert(Text[0] == '\\');
if (Text.size() < 2)
return 1;

switch (Text[1]) {
case 'u':
return 6;
case 'U':
return 10;
case 'x': {
unsigned I = 2;
while (I < Text.size() && isHexDigit(Text[I]))
++I;
return I;
}
default:
if (isOctDigit(Text[1])) {
unsigned I = 1;
while (I < Text.size() && I < 4 && isOctDigit(Text[I]))
++I;
return I;
}
return 1 + llvm::getNumBytesForUTF8(Text[1]);
}
}

}
}
}

#endif
