














#if !defined(ORC_RT_EXECUTOR_ADDRESS_H)
#define ORC_RT_EXECUTOR_ADDRESS_H

#include "adt.h"
#include "simple_packed_serialization.h"

#include <cassert>
#include <type_traits>

namespace __orc_rt {


class ExecutorAddrDiff {
public:
ExecutorAddrDiff() = default;
explicit ExecutorAddrDiff(uint64_t Value) : Value(Value) {}

uint64_t getValue() const { return Value; }

private:
int64_t Value = 0;
};


class ExecutorAddress {
public:
ExecutorAddress() = default;
explicit ExecutorAddress(uint64_t Addr) : Addr(Addr) {}



template <typename T> static ExecutorAddress fromPtr(T *Value) {
return ExecutorAddress(
static_cast<uint64_t>(reinterpret_cast<uintptr_t>(Value)));
}



template <typename T> T toPtr() const {
static_assert(std::is_pointer<T>::value, "T must be a pointer type");
uintptr_t IntPtr = static_cast<uintptr_t>(Addr);
assert(IntPtr == Addr &&
"JITTargetAddress value out of range for uintptr_t");
return reinterpret_cast<T>(IntPtr);
}

uint64_t getValue() const { return Addr; }
void setValue(uint64_t Addr) { this->Addr = Addr; }
bool isNull() const { return Addr == 0; }

explicit operator bool() const { return Addr != 0; }

friend bool operator==(const ExecutorAddress &LHS,
const ExecutorAddress &RHS) {
return LHS.Addr == RHS.Addr;
}

friend bool operator!=(const ExecutorAddress &LHS,
const ExecutorAddress &RHS) {
return LHS.Addr != RHS.Addr;
}

friend bool operator<(const ExecutorAddress &LHS,
const ExecutorAddress &RHS) {
return LHS.Addr < RHS.Addr;
}

friend bool operator<=(const ExecutorAddress &LHS,
const ExecutorAddress &RHS) {
return LHS.Addr <= RHS.Addr;
}

friend bool operator>(const ExecutorAddress &LHS,
const ExecutorAddress &RHS) {
return LHS.Addr > RHS.Addr;
}

friend bool operator>=(const ExecutorAddress &LHS,
const ExecutorAddress &RHS) {
return LHS.Addr >= RHS.Addr;
}

ExecutorAddress &operator++() {
++Addr;
return *this;
}
ExecutorAddress &operator--() {
--Addr;
return *this;
}
ExecutorAddress operator++(int) { return ExecutorAddress(Addr++); }
ExecutorAddress operator--(int) { return ExecutorAddress(Addr++); }

ExecutorAddress &operator+=(const ExecutorAddrDiff Delta) {
Addr += Delta.getValue();
return *this;
}

ExecutorAddress &operator-=(const ExecutorAddrDiff Delta) {
Addr -= Delta.getValue();
return *this;
}

private:
uint64_t Addr = 0;
};


inline ExecutorAddrDiff operator-(const ExecutorAddress &LHS,
const ExecutorAddress &RHS) {
return ExecutorAddrDiff(LHS.getValue() - RHS.getValue());
}


inline ExecutorAddress operator+(const ExecutorAddress &LHS,
const ExecutorAddrDiff &RHS) {
return ExecutorAddress(LHS.getValue() + RHS.getValue());
}


inline ExecutorAddress operator+(const ExecutorAddrDiff &LHS,
const ExecutorAddress &RHS) {
return ExecutorAddress(LHS.getValue() + RHS.getValue());
}


struct ExecutorAddressRange {
ExecutorAddressRange() = default;
ExecutorAddressRange(ExecutorAddress StartAddress, ExecutorAddress EndAddress)
: StartAddress(StartAddress), EndAddress(EndAddress) {}

bool empty() const { return StartAddress == EndAddress; }
ExecutorAddrDiff size() const { return EndAddress - StartAddress; }

template <typename T> span<T> toSpan() const {
assert(size().getValue() % sizeof(T) == 0 &&
"AddressRange is not a multiple of sizeof(T)");
return span<T>(StartAddress.toPtr<T *>(), size().getValue() / sizeof(T));
}

ExecutorAddress StartAddress;
ExecutorAddress EndAddress;
};


template <> class SPSSerializationTraits<SPSExecutorAddress, ExecutorAddress> {
public:
static size_t size(const ExecutorAddress &EA) {
return SPSArgList<uint64_t>::size(EA.getValue());
}

static bool serialize(SPSOutputBuffer &BOB, const ExecutorAddress &EA) {
return SPSArgList<uint64_t>::serialize(BOB, EA.getValue());
}

static bool deserialize(SPSInputBuffer &BIB, ExecutorAddress &EA) {
uint64_t Tmp;
if (!SPSArgList<uint64_t>::deserialize(BIB, Tmp))
return false;
EA = ExecutorAddress(Tmp);
return true;
}
};

using SPSExecutorAddressRange =
SPSTuple<SPSExecutorAddress, SPSExecutorAddress>;


template <>
class SPSSerializationTraits<SPSExecutorAddressRange, ExecutorAddressRange> {
public:
static size_t size(const ExecutorAddressRange &Value) {
return SPSArgList<SPSExecutorAddress, SPSExecutorAddress>::size(
Value.StartAddress, Value.EndAddress);
}

static bool serialize(SPSOutputBuffer &BOB,
const ExecutorAddressRange &Value) {
return SPSArgList<SPSExecutorAddress, SPSExecutorAddress>::serialize(
BOB, Value.StartAddress, Value.EndAddress);
}

static bool deserialize(SPSInputBuffer &BIB, ExecutorAddressRange &Value) {
return SPSArgList<SPSExecutorAddress, SPSExecutorAddress>::deserialize(
BIB, Value.StartAddress, Value.EndAddress);
}
};

using SPSExecutorAddressRangeSequence = SPSSequence<SPSExecutorAddressRange>;

}

#endif
