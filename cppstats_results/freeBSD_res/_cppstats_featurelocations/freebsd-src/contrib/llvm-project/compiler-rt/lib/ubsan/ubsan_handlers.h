










#if !defined(UBSAN_HANDLERS_H)
#define UBSAN_HANDLERS_H

#include "ubsan_value.h"

namespace __ubsan {

struct TypeMismatchData {
SourceLocation Loc;
const TypeDescriptor &Type;
unsigned char LogAlignment;
unsigned char TypeCheckKind;
};

#define UNRECOVERABLE(checkname, ...) extern "C" SANITIZER_INTERFACE_ATTRIBUTE NORETURN void __ubsan_handle_ ##checkname( __VA_ARGS__ );



#define RECOVERABLE(checkname, ...) extern "C" SANITIZER_INTERFACE_ATTRIBUTE void __ubsan_handle_ ##checkname( __VA_ARGS__ ); extern "C" SANITIZER_INTERFACE_ATTRIBUTE NORETURN void __ubsan_handle_ ##checkname ##_abort( __VA_ARGS__ );








RECOVERABLE(type_mismatch_v1, TypeMismatchData *Data, ValueHandle Pointer)

struct AlignmentAssumptionData {
SourceLocation Loc;
SourceLocation AssumptionLoc;
const TypeDescriptor &Type;
};



RECOVERABLE(alignment_assumption, AlignmentAssumptionData *Data,
ValueHandle Pointer, ValueHandle Alignment, ValueHandle Offset)

struct OverflowData {
SourceLocation Loc;
const TypeDescriptor &Type;
};


RECOVERABLE(add_overflow, OverflowData *Data, ValueHandle LHS, ValueHandle RHS)


RECOVERABLE(sub_overflow, OverflowData *Data, ValueHandle LHS, ValueHandle RHS)


RECOVERABLE(mul_overflow, OverflowData *Data, ValueHandle LHS, ValueHandle RHS)


RECOVERABLE(negate_overflow, OverflowData *Data, ValueHandle OldVal)


RECOVERABLE(divrem_overflow, OverflowData *Data,
ValueHandle LHS, ValueHandle RHS)

struct ShiftOutOfBoundsData {
SourceLocation Loc;
const TypeDescriptor &LHSType;
const TypeDescriptor &RHSType;
};



RECOVERABLE(shift_out_of_bounds, ShiftOutOfBoundsData *Data,
ValueHandle LHS, ValueHandle RHS)

struct OutOfBoundsData {
SourceLocation Loc;
const TypeDescriptor &ArrayType;
const TypeDescriptor &IndexType;
};


RECOVERABLE(out_of_bounds, OutOfBoundsData *Data, ValueHandle Index)

struct UnreachableData {
SourceLocation Loc;
};


UNRECOVERABLE(builtin_unreachable, UnreachableData *Data)

UNRECOVERABLE(missing_return, UnreachableData *Data)

struct VLABoundData {
SourceLocation Loc;
const TypeDescriptor &Type;
};


RECOVERABLE(vla_bound_not_positive, VLABoundData *Data, ValueHandle Bound)



struct FloatCastOverflowData {
const TypeDescriptor &FromType;
const TypeDescriptor &ToType;
};

struct FloatCastOverflowDataV2 {
SourceLocation Loc;
const TypeDescriptor &FromType;
const TypeDescriptor &ToType;
};



RECOVERABLE(float_cast_overflow, void *Data, ValueHandle From)

struct InvalidValueData {
SourceLocation Loc;
const TypeDescriptor &Type;
};


RECOVERABLE(load_invalid_value, InvalidValueData *Data, ValueHandle Val)



enum ImplicitConversionCheckKind : unsigned char {
ICCK_IntegerTruncation = 0,
ICCK_UnsignedIntegerTruncation = 1,
ICCK_SignedIntegerTruncation = 2,
ICCK_IntegerSignChange = 3,
ICCK_SignedIntegerTruncationOrSignChange = 4,
};

struct ImplicitConversionData {
SourceLocation Loc;
const TypeDescriptor &FromType;
const TypeDescriptor &ToType;
unsigned char Kind;
};


RECOVERABLE(implicit_conversion, ImplicitConversionData *Data, ValueHandle Src,
ValueHandle Dst)



enum BuiltinCheckKind : unsigned char {
BCK_CTZPassedZero,
BCK_CLZPassedZero,
};

struct InvalidBuiltinData {
SourceLocation Loc;
unsigned char Kind;
};


RECOVERABLE(invalid_builtin, InvalidBuiltinData *Data)

struct InvalidObjCCast {
SourceLocation Loc;
const TypeDescriptor &ExpectedType;
};


RECOVERABLE(invalid_objc_cast, InvalidObjCCast *Data, ValueHandle Pointer)

struct NonNullReturnData {
SourceLocation AttrLoc;
};



RECOVERABLE(nonnull_return_v1, NonNullReturnData *Data, SourceLocation *Loc)
RECOVERABLE(nullability_return_v1, NonNullReturnData *Data, SourceLocation *Loc)

struct NonNullArgData {
SourceLocation Loc;
SourceLocation AttrLoc;
int ArgIndex;
};



RECOVERABLE(nonnull_arg, NonNullArgData *Data)
RECOVERABLE(nullability_arg, NonNullArgData *Data)

struct PointerOverflowData {
SourceLocation Loc;
};

RECOVERABLE(pointer_overflow, PointerOverflowData *Data, ValueHandle Base,
ValueHandle Result)



enum CFITypeCheckKind : unsigned char {
CFITCK_VCall,
CFITCK_NVCall,
CFITCK_DerivedCast,
CFITCK_UnrelatedCast,
CFITCK_ICall,
CFITCK_NVMFCall,
CFITCK_VMFCall,
};

struct CFICheckFailData {
CFITypeCheckKind CheckKind;
SourceLocation Loc;
const TypeDescriptor &Type;
};


RECOVERABLE(cfi_check_fail, CFICheckFailData *Data, ValueHandle Function,
uptr VtableIsValid)

struct ReportOptions;

extern "C" SANITIZER_INTERFACE_ATTRIBUTE void __ubsan_handle_cfi_bad_type(
CFICheckFailData *Data, ValueHandle Vtable, bool ValidVtable,
ReportOptions Opts);

}

#endif
