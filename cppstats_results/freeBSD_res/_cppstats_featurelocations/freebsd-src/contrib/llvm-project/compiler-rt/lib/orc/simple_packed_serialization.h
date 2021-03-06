
































#if !defined(ORC_RT_SIMPLE_PACKED_SERIALIZATION_H)
#define ORC_RT_SIMPLE_PACKED_SERIALIZATION_H

#include "adt.h"
#include "endianness.h"
#include "error.h"
#include "stl_extras.h"

#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace __orc_rt {


class SPSOutputBuffer {
public:
SPSOutputBuffer(char *Buffer, size_t Remaining)
: Buffer(Buffer), Remaining(Remaining) {}
bool write(const char *Data, size_t Size) {
if (Size > Remaining)
return false;
memcpy(Buffer, Data, Size);
Buffer += Size;
Remaining -= Size;
return true;
}

private:
char *Buffer = nullptr;
size_t Remaining = 0;
};


class SPSInputBuffer {
public:
SPSInputBuffer() = default;
SPSInputBuffer(const char *Buffer, size_t Remaining)
: Buffer(Buffer), Remaining(Remaining) {}
bool read(char *Data, size_t Size) {
if (Size > Remaining)
return false;
memcpy(Data, Buffer, Size);
Buffer += Size;
Remaining -= Size;
return true;
}

const char *data() const { return Buffer; }
bool skip(size_t Size) {
if (Size > Remaining)
return false;
Buffer += Size;
Remaining -= Size;
return true;
}

private:
const char *Buffer = nullptr;
size_t Remaining = 0;
};



template <typename SPSTagT, typename ConcreteT, typename _ = void>
class SPSSerializationTraits;


template <typename... ArgTs> class SPSArgList;


template <> class SPSArgList<> {
public:
static size_t size() { return 0; }

static bool serialize(SPSOutputBuffer &OB) { return true; }
static bool deserialize(SPSInputBuffer &IB) { return true; }
};


template <typename SPSTagT, typename... SPSTagTs>
class SPSArgList<SPSTagT, SPSTagTs...> {
public:
template <typename ArgT, typename... ArgTs>
static size_t size(const ArgT &Arg, const ArgTs &...Args) {
return SPSSerializationTraits<SPSTagT, ArgT>::size(Arg) +
SPSArgList<SPSTagTs...>::size(Args...);
}

template <typename ArgT, typename... ArgTs>
static bool serialize(SPSOutputBuffer &OB, const ArgT &Arg,
const ArgTs &...Args) {
return SPSSerializationTraits<SPSTagT, ArgT>::serialize(OB, Arg) &&
SPSArgList<SPSTagTs...>::serialize(OB, Args...);
}

template <typename ArgT, typename... ArgTs>
static bool deserialize(SPSInputBuffer &IB, ArgT &Arg, ArgTs &...Args) {
return SPSSerializationTraits<SPSTagT, ArgT>::deserialize(IB, Arg) &&
SPSArgList<SPSTagTs...>::deserialize(IB, Args...);
}
};


template <typename SPSTagT>
class SPSSerializationTraits<
SPSTagT, SPSTagT,
std::enable_if_t<std::is_same<SPSTagT, bool>::value ||
std::is_same<SPSTagT, char>::value ||
std::is_same<SPSTagT, int8_t>::value ||
std::is_same<SPSTagT, int16_t>::value ||
std::is_same<SPSTagT, int32_t>::value ||
std::is_same<SPSTagT, int64_t>::value ||
std::is_same<SPSTagT, uint8_t>::value ||
std::is_same<SPSTagT, uint16_t>::value ||
std::is_same<SPSTagT, uint32_t>::value ||
std::is_same<SPSTagT, uint64_t>::value>> {
public:
static size_t size(const SPSTagT &Value) { return sizeof(SPSTagT); }

static bool serialize(SPSOutputBuffer &OB, const SPSTagT &Value) {
SPSTagT Tmp = Value;
if (IsBigEndianHost)
swapByteOrder(Tmp);
return OB.write(reinterpret_cast<const char *>(&Tmp), sizeof(Tmp));
}

static bool deserialize(SPSInputBuffer &IB, SPSTagT &Value) {
SPSTagT Tmp;
if (!IB.read(reinterpret_cast<char *>(&Tmp), sizeof(Tmp)))
return false;
if (IsBigEndianHost)
swapByteOrder(Tmp);
Value = Tmp;
return true;
}
};


class SPSEmpty {};


class SPSExecutorAddress {};





template <typename... SPSTagTs> class SPSTuple {
public:

typedef SPSArgList<SPSTagTs...> AsArgList;
};





template <typename SPSElementTagT> class SPSSequence;


using SPSString = SPSSequence<char>;




template <typename SPSTagT1, typename SPSTagT2>
using SPSMap = SPSSequence<SPSTuple<SPSTagT1, SPSTagT2>>;


template <> class SPSSerializationTraits<SPSEmpty, SPSEmpty> {
public:
static size_t size(const SPSEmpty &EP) { return 0; }
static bool serialize(SPSOutputBuffer &OB, const SPSEmpty &BE) {
return true;
}
static bool deserialize(SPSInputBuffer &IB, SPSEmpty &BE) { return true; }
};










template <typename SPSElementTagT, typename ConcreteSequenceT>
class TrivialSPSSequenceSerialization {
public:
static constexpr bool available = false;
};











template <typename SPSElementTagT, typename ConcreteSequenceT>
class TrivialSPSSequenceDeserialization {
public:
static constexpr bool available = false;
};


template <> class TrivialSPSSequenceSerialization<char, std::string> {
public:
static constexpr bool available = true;
};


template <> class TrivialSPSSequenceDeserialization<char, std::string> {
public:
static constexpr bool available = true;

using element_type = char;

static void reserve(std::string &S, uint64_t Size) { S.reserve(Size); }
static bool append(std::string &S, char C) {
S.push_back(C);
return true;
}
};


template <typename SPSElementTagT, typename T>
class TrivialSPSSequenceSerialization<SPSElementTagT, std::vector<T>> {
public:
static constexpr bool available = true;
};


template <typename SPSElementTagT, typename T>
class TrivialSPSSequenceDeserialization<SPSElementTagT, std::vector<T>> {
public:
static constexpr bool available = true;

using element_type = typename std::vector<T>::value_type;

static void reserve(std::vector<T> &V, uint64_t Size) { V.reserve(Size); }
static bool append(std::vector<T> &V, T E) {
V.push_back(std::move(E));
return true;
}
};



template <typename SPSKeyTagT, typename SPSValueTagT, typename K, typename V>
class TrivialSPSSequenceSerialization<SPSTuple<SPSKeyTagT, SPSValueTagT>,
std::unordered_map<K, V>> {
public:
static constexpr bool available = true;
};



template <typename SPSKeyTagT, typename SPSValueTagT, typename K, typename V>
class TrivialSPSSequenceDeserialization<SPSTuple<SPSKeyTagT, SPSValueTagT>,
std::unordered_map<K, V>> {
public:
static constexpr bool available = true;

using element_type = std::pair<K, V>;

static void reserve(std::unordered_map<K, V> &M, uint64_t Size) {
M.reserve(Size);
}
static bool append(std::unordered_map<K, V> &M, element_type E) {
return M.insert(std::move(E)).second;
}
};




template <typename SPSElementTagT, typename SequenceT>
class SPSSerializationTraits<SPSSequence<SPSElementTagT>, SequenceT,
std::enable_if_t<TrivialSPSSequenceSerialization<
SPSElementTagT, SequenceT>::available>> {
public:
static size_t size(const SequenceT &S) {
size_t Size = SPSArgList<uint64_t>::size(static_cast<uint64_t>(S.size()));
for (const auto &E : S)
Size += SPSArgList<SPSElementTagT>::size(E);
return Size;
}

static bool serialize(SPSOutputBuffer &OB, const SequenceT &S) {
if (!SPSArgList<uint64_t>::serialize(OB, static_cast<uint64_t>(S.size())))
return false;
for (const auto &E : S)
if (!SPSArgList<SPSElementTagT>::serialize(OB, E))
return false;
return true;
}

static bool deserialize(SPSInputBuffer &IB, SequenceT &S) {
using TBSD = TrivialSPSSequenceDeserialization<SPSElementTagT, SequenceT>;
uint64_t Size;
if (!SPSArgList<uint64_t>::deserialize(IB, Size))
return false;
TBSD::reserve(S, Size);
for (size_t I = 0; I != Size; ++I) {
typename TBSD::element_type E;
if (!SPSArgList<SPSElementTagT>::deserialize(IB, E))
return false;
if (!TBSD::append(S, std::move(E)))
return false;
}
return true;
}
};


template <typename SPSTagT1, typename SPSTagT2, typename T1, typename T2>
class SPSSerializationTraits<SPSTuple<SPSTagT1, SPSTagT2>, std::pair<T1, T2>> {
public:
static size_t size(const std::pair<T1, T2> &P) {
return SPSArgList<SPSTagT1>::size(P.first) +
SPSArgList<SPSTagT2>::size(P.second);
}

static bool serialize(SPSOutputBuffer &OB, const std::pair<T1, T2> &P) {
return SPSArgList<SPSTagT1>::serialize(OB, P.first) &&
SPSArgList<SPSTagT2>::serialize(OB, P.second);
}

static bool deserialize(SPSInputBuffer &IB, std::pair<T1, T2> &P) {
return SPSArgList<SPSTagT1>::deserialize(IB, P.first) &&
SPSArgList<SPSTagT2>::deserialize(IB, P.second);
}
};





template <> class SPSSerializationTraits<SPSString, __orc_rt::string_view> {
public:
static size_t size(const __orc_rt::string_view &S) {
return SPSArgList<uint64_t>::size(static_cast<uint64_t>(S.size())) +
S.size();
}

static bool serialize(SPSOutputBuffer &OB, const __orc_rt::string_view &S) {
if (!SPSArgList<uint64_t>::serialize(OB, static_cast<uint64_t>(S.size())))
return false;
return OB.write(S.data(), S.size());
}

static bool deserialize(SPSInputBuffer &IB, __orc_rt::string_view &S) {
const char *Data = nullptr;
uint64_t Size;
if (!SPSArgList<uint64_t>::deserialize(IB, Size))
return false;
Data = IB.data();
if (!IB.skip(Size))
return false;
S = {Data, Size};
return true;
}
};


class SPSError;



template <typename SPSTagT> class SPSExpected;

namespace detail {










struct SPSSerializableError {
bool HasError = false;
std::string ErrMsg;
};






template <typename T> struct SPSSerializableExpected {
bool HasValue = false;
T Value{};
std::string ErrMsg;
};

inline SPSSerializableError toSPSSerializable(Error Err) {
if (Err)
return {true, toString(std::move(Err))};
return {false, {}};
}

inline Error fromSPSSerializable(SPSSerializableError BSE) {
if (BSE.HasError)
return make_error<StringError>(BSE.ErrMsg);
return Error::success();
}

template <typename T>
SPSSerializableExpected<T> toSPSSerializable(Expected<T> E) {
if (E)
return {true, std::move(*E), {}};
else
return {false, {}, toString(E.takeError())};
}

template <typename T>
Expected<T> fromSPSSerializable(SPSSerializableExpected<T> BSE) {
if (BSE.HasValue)
return std::move(BSE.Value);
else
return make_error<StringError>(BSE.ErrMsg);
}

}


template <>
class SPSSerializationTraits<SPSError, detail::SPSSerializableError> {
public:
static size_t size(const detail::SPSSerializableError &BSE) {
size_t Size = SPSArgList<bool>::size(BSE.HasError);
if (BSE.HasError)
Size += SPSArgList<SPSString>::size(BSE.ErrMsg);
return Size;
}

static bool serialize(SPSOutputBuffer &OB,
const detail::SPSSerializableError &BSE) {
if (!SPSArgList<bool>::serialize(OB, BSE.HasError))
return false;
if (BSE.HasError)
if (!SPSArgList<SPSString>::serialize(OB, BSE.ErrMsg))
return false;
return true;
}

static bool deserialize(SPSInputBuffer &IB,
detail::SPSSerializableError &BSE) {
if (!SPSArgList<bool>::deserialize(IB, BSE.HasError))
return false;

if (!BSE.HasError)
return true;

return SPSArgList<SPSString>::deserialize(IB, BSE.ErrMsg);
}
};



template <typename SPSTagT, typename T>
class SPSSerializationTraits<SPSExpected<SPSTagT>,
detail::SPSSerializableExpected<T>> {
public:
static size_t size(const detail::SPSSerializableExpected<T> &BSE) {
size_t Size = SPSArgList<bool>::size(BSE.HasValue);
if (BSE.HasValue)
Size += SPSArgList<SPSTagT>::size(BSE.Value);
else
Size += SPSArgList<SPSString>::size(BSE.ErrMsg);
return Size;
}

static bool serialize(SPSOutputBuffer &OB,
const detail::SPSSerializableExpected<T> &BSE) {
if (!SPSArgList<bool>::serialize(OB, BSE.HasValue))
return false;

if (BSE.HasValue)
return SPSArgList<SPSTagT>::serialize(OB, BSE.Value);

return SPSArgList<SPSString>::serialize(OB, BSE.ErrMsg);
}

static bool deserialize(SPSInputBuffer &IB,
detail::SPSSerializableExpected<T> &BSE) {
if (!SPSArgList<bool>::deserialize(IB, BSE.HasValue))
return false;

if (BSE.HasValue)
return SPSArgList<SPSTagT>::deserialize(IB, BSE.Value);

return SPSArgList<SPSString>::deserialize(IB, BSE.ErrMsg);
}
};


template <typename SPSTagT>
class SPSSerializationTraits<SPSExpected<SPSTagT>,
detail::SPSSerializableError> {
public:
static size_t size(const detail::SPSSerializableError &BSE) {
assert(BSE.HasError && "Cannot serialize expected from a success value");
return SPSArgList<bool>::size(false) +
SPSArgList<SPSString>::size(BSE.ErrMsg);
}

static bool serialize(SPSOutputBuffer &OB,
const detail::SPSSerializableError &BSE) {
assert(BSE.HasError && "Cannot serialize expected from a success value");
if (!SPSArgList<bool>::serialize(OB, false))
return false;
return SPSArgList<SPSString>::serialize(OB, BSE.ErrMsg);
}
};


template <typename SPSTagT, typename T>
class SPSSerializationTraits<SPSExpected<SPSTagT>, T> {
public:
static size_t size(const T &Value) {
return SPSArgList<bool>::size(true) + SPSArgList<SPSTagT>::size(Value);
}

static bool serialize(SPSOutputBuffer &OB, const T &Value) {
if (!SPSArgList<bool>::serialize(OB, true))
return false;
return SPSArgList<SPSTagT>::serialize(Value);
}
};

}

#endif
