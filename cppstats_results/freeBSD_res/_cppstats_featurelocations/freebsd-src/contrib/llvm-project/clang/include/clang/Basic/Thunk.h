













#if !defined(LLVM_CLANG_BASIC_THUNK_H)
#define LLVM_CLANG_BASIC_THUNK_H

#include <cstdint>
#include <cstring>

namespace clang {

class CXXMethodDecl;


struct ReturnAdjustment {


int64_t NonVirtual;



union VirtualAdjustment {

struct {


int64_t VBaseOffsetOffset;
} Itanium;


struct {


uint32_t VBPtrOffset;


uint32_t VBIndex;
} Microsoft;

VirtualAdjustment() { memset(this, 0, sizeof(*this)); }

bool Equals(const VirtualAdjustment &Other) const {
return memcmp(this, &Other, sizeof(Other)) == 0;
}

bool isEmpty() const {
VirtualAdjustment Zero;
return Equals(Zero);
}

bool Less(const VirtualAdjustment &RHS) const {
return memcmp(this, &RHS, sizeof(RHS)) < 0;
}
} Virtual;

ReturnAdjustment() : NonVirtual(0) {}

bool isEmpty() const { return !NonVirtual && Virtual.isEmpty(); }

friend bool operator==(const ReturnAdjustment &LHS,
const ReturnAdjustment &RHS) {
return LHS.NonVirtual == RHS.NonVirtual && LHS.Virtual.Equals(RHS.Virtual);
}

friend bool operator!=(const ReturnAdjustment &LHS,
const ReturnAdjustment &RHS) {
return !(LHS == RHS);
}

friend bool operator<(const ReturnAdjustment &LHS,
const ReturnAdjustment &RHS) {
if (LHS.NonVirtual < RHS.NonVirtual)
return true;

return LHS.NonVirtual == RHS.NonVirtual && LHS.Virtual.Less(RHS.Virtual);
}
};


struct ThisAdjustment {


int64_t NonVirtual;



union VirtualAdjustment {

struct {


int64_t VCallOffsetOffset;
} Itanium;

struct {

int32_t VtordispOffset;



int32_t VBPtrOffset;


int32_t VBOffsetOffset;
} Microsoft;

VirtualAdjustment() { memset(this, 0, sizeof(*this)); }

bool Equals(const VirtualAdjustment &Other) const {
return memcmp(this, &Other, sizeof(Other)) == 0;
}

bool isEmpty() const {
VirtualAdjustment Zero;
return Equals(Zero);
}

bool Less(const VirtualAdjustment &RHS) const {
return memcmp(this, &RHS, sizeof(RHS)) < 0;
}
} Virtual;

ThisAdjustment() : NonVirtual(0) {}

bool isEmpty() const { return !NonVirtual && Virtual.isEmpty(); }

friend bool operator==(const ThisAdjustment &LHS, const ThisAdjustment &RHS) {
return LHS.NonVirtual == RHS.NonVirtual && LHS.Virtual.Equals(RHS.Virtual);
}

friend bool operator!=(const ThisAdjustment &LHS, const ThisAdjustment &RHS) {
return !(LHS == RHS);
}

friend bool operator<(const ThisAdjustment &LHS, const ThisAdjustment &RHS) {
if (LHS.NonVirtual < RHS.NonVirtual)
return true;

return LHS.NonVirtual == RHS.NonVirtual && LHS.Virtual.Less(RHS.Virtual);
}
};



struct ThunkInfo {

ThisAdjustment This;


ReturnAdjustment Return;






const CXXMethodDecl *Method;

ThunkInfo() : Method(nullptr) {}

ThunkInfo(const ThisAdjustment &This, const ReturnAdjustment &Return,
const CXXMethodDecl *Method = nullptr)
: This(This), Return(Return), Method(Method) {}

friend bool operator==(const ThunkInfo &LHS, const ThunkInfo &RHS) {
return LHS.This == RHS.This && LHS.Return == RHS.Return &&
LHS.Method == RHS.Method;
}

bool isEmpty() const {
return This.isEmpty() && Return.isEmpty() && Method == nullptr;
}
};

}

#endif
