










#if !defined(UBSAN_TYPE_HASH_H)
#define UBSAN_TYPE_HASH_H

#include "sanitizer_common/sanitizer_common.h"

namespace __ubsan {

typedef uptr HashValue;



class DynamicTypeInfo {
const char *MostDerivedTypeName;
sptr Offset;
const char *SubobjectTypeName;

public:
DynamicTypeInfo(const char *MDTN, sptr Offset, const char *STN)
: MostDerivedTypeName(MDTN), Offset(Offset), SubobjectTypeName(STN) {}


bool isValid() const { return MostDerivedTypeName; }

const char *getMostDerivedTypeName() const { return MostDerivedTypeName; }

sptr getOffset() const { return Offset; }

const char *getSubobjectTypeName() const { return SubobjectTypeName; }
};


DynamicTypeInfo getDynamicTypeInfoFromObject(void *Object);


DynamicTypeInfo getDynamicTypeInfoFromVtable(void *Vtable);




bool checkDynamicType(void *Object, void *Type, HashValue Hash);

const unsigned VptrTypeCacheSize = 128;



const int VptrMaxOffsetToTop = 1<<20;






extern "C" SANITIZER_INTERFACE_ATTRIBUTE
HashValue __ubsan_vptr_type_cache[VptrTypeCacheSize];



bool checkTypeInfoEquality(const void *TypeInfo1, const void *TypeInfo2);

}

#endif
