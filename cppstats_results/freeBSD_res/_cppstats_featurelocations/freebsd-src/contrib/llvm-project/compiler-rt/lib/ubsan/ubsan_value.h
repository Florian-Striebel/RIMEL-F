











#if !defined(UBSAN_VALUE_H)
#define UBSAN_VALUE_H

#include "sanitizer_common/sanitizer_atomic.h"
#include "sanitizer_common/sanitizer_common.h"


#if __SIZEOF_INT128__
__extension__ typedef __int128 s128;
__extension__ typedef unsigned __int128 u128;
#define HAVE_INT128_T 1
#else
#define HAVE_INT128_T 0
#endif

namespace __ubsan {


#if HAVE_INT128_T
typedef s128 SIntMax;
typedef u128 UIntMax;
#else
typedef s64 SIntMax;
typedef u64 UIntMax;
#endif


typedef long double FloatMax;



class SourceLocation {
const char *Filename;
u32 Line;
u32 Column;

public:
SourceLocation() : Filename(), Line(), Column() {}
SourceLocation(const char *Filename, unsigned Line, unsigned Column)
: Filename(Filename), Line(Line), Column(Column) {}


bool isInvalid() const { return !Filename; }



SourceLocation acquire() {
u32 OldColumn = __sanitizer::atomic_exchange(
(__sanitizer::atomic_uint32_t *)&Column, ~u32(0),
__sanitizer::memory_order_relaxed);
return SourceLocation(Filename, Line, OldColumn);
}



bool isDisabled() {
return Column == ~u32(0);
}


const char *getFilename() const { return Filename; }

unsigned getLine() const { return Line; }

unsigned getColumn() const { return Column; }
};



class TypeDescriptor {


u16 TypeKind;



u16 TypeInfo;



char TypeName[1];

public:
enum Kind {




TK_Integer = 0x0000,



TK_Float = 0x0001,

TK_Unknown = 0xffff
};

const char *getTypeName() const { return TypeName; }

Kind getKind() const {
return static_cast<Kind>(TypeKind);
}

bool isIntegerTy() const { return getKind() == TK_Integer; }
bool isSignedIntegerTy() const {
return isIntegerTy() && (TypeInfo & 1);
}
bool isUnsignedIntegerTy() const {
return isIntegerTy() && !(TypeInfo & 1);
}
unsigned getIntegerBitWidth() const {
CHECK(isIntegerTy());
return 1 << (TypeInfo >> 1);
}

bool isFloatTy() const { return getKind() == TK_Float; }
unsigned getFloatBitWidth() const {
CHECK(isFloatTy());
return TypeInfo;
}
};


typedef uptr ValueHandle;



const char *getObjCClassName(ValueHandle Pointer);






class Value {

const TypeDescriptor &Type;

ValueHandle Val;


bool isInlineInt() const {
CHECK(getType().isIntegerTy());
const unsigned InlineBits = sizeof(ValueHandle) * 8;
const unsigned Bits = getType().getIntegerBitWidth();
return Bits <= InlineBits;
}


bool isInlineFloat() const {
CHECK(getType().isFloatTy());
const unsigned InlineBits = sizeof(ValueHandle) * 8;
const unsigned Bits = getType().getFloatBitWidth();
return Bits <= InlineBits;
}

public:
Value(const TypeDescriptor &Type, ValueHandle Val) : Type(Type), Val(Val) {}

const TypeDescriptor &getType() const { return Type; }


SIntMax getSIntValue() const;


UIntMax getUIntValue() const;


UIntMax getPositiveIntValue() const;


bool isMinusOne() const {
return getType().isSignedIntegerTy() && getSIntValue() == -1;
}


bool isNegative() const {
return getType().isSignedIntegerTy() && getSIntValue() < 0;
}


FloatMax getFloatValue() const;
};

}

#endif
