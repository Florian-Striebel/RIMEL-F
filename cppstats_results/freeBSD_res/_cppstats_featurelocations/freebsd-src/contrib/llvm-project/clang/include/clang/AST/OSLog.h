












#if !defined(LLVM_CLANG_ANALYSIS_ANALYSES_OSLOG_H)
#define LLVM_CLANG_ANALYSIS_ANALYSES_OSLOG_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"

namespace clang {
namespace analyze_os_log {



class OSLogBufferItem {
public:
enum Kind {


ScalarKind = 0,




CountKind,



StringKind,



PointerKind,



ObjCObjKind,


WideStringKind,



ErrnoKind,


MaskKind
};

enum {

IsPrivate = 0x1,


IsPublic = 0x2,


IsSensitive = 0x4 | IsPrivate
};

private:
Kind TheKind = ScalarKind;
const Expr *TheExpr = nullptr;
CharUnits ConstValue;
CharUnits Size;
unsigned Flags = 0;
StringRef MaskType;

public:
OSLogBufferItem(Kind kind, const Expr *expr, CharUnits size, unsigned flags,
StringRef maskType = StringRef())
: TheKind(kind), TheExpr(expr), Size(size), Flags(flags),
MaskType(maskType) {
assert(((Flags == 0) || (Flags == IsPrivate) || (Flags == IsPublic) ||
(Flags == IsSensitive)) &&
"unexpected privacy flag");
}

OSLogBufferItem(ASTContext &Ctx, CharUnits value, unsigned flags)
: TheKind(CountKind), ConstValue(value),
Size(Ctx.getTypeSizeInChars(Ctx.IntTy)), Flags(flags) {}

unsigned char getDescriptorByte() const {
unsigned char result = Flags;
result |= ((unsigned)getKind()) << 4;
return result;
}

unsigned char getSizeByte() const { return size().getQuantity(); }

Kind getKind() const { return TheKind; }
bool getIsPrivate() const { return (Flags & IsPrivate) != 0; }

const Expr *getExpr() const { return TheExpr; }
CharUnits getConstValue() const { return ConstValue; }
CharUnits size() const { return Size; }

StringRef getMaskType() const { return MaskType; }
};

class OSLogBufferLayout {
public:
SmallVector<OSLogBufferItem, 4> Items;

enum Flags { HasPrivateItems = 1, HasNonScalarItems = 1 << 1 };

CharUnits size() const {
CharUnits result;
result += CharUnits::fromQuantity(2);
for (auto &item : Items) {

result += item.size() + CharUnits::fromQuantity(2);
}
return result;
}

bool hasPrivateItems() const {
return llvm::any_of(
Items, [](const OSLogBufferItem &Item) { return Item.getIsPrivate(); });
}

bool hasNonScalarOrMask() const {
return llvm::any_of(Items, [](const OSLogBufferItem &Item) {
return Item.getKind() != OSLogBufferItem::ScalarKind ||
!Item.getMaskType().empty();
});
}

unsigned char getSummaryByte() const {
unsigned char result = 0;
if (hasPrivateItems())
result |= HasPrivateItems;
if (hasNonScalarOrMask())
result |= HasNonScalarItems;
return result;
}

unsigned char getNumArgsByte() const { return Items.size(); }
};





bool computeOSLogBufferLayout(clang::ASTContext &Ctx, const clang::CallExpr *E,
OSLogBufferLayout &layout);

}
}
#endif
