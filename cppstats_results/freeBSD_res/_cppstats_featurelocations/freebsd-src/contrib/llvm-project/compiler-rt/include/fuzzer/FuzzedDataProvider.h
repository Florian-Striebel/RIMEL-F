











#if !defined(LLVM_FUZZER_FUZZED_DATA_PROVIDER_H_)
#define LLVM_FUZZER_FUZZED_DATA_PROVIDER_H_

#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>



class FuzzedDataProvider {
public:


FuzzedDataProvider(const uint8_t *data, size_t size)
: data_ptr_(data), remaining_bytes_(size) {}
~FuzzedDataProvider() = default;








template <typename T> std::vector<T> ConsumeBytes(size_t num_bytes);
template <typename T>
std::vector<T> ConsumeBytesWithTerminator(size_t num_bytes, T terminator = 0);
template <typename T> std::vector<T> ConsumeRemainingBytes();



std::string ConsumeBytesAsString(size_t num_bytes);
std::string ConsumeRandomLengthString(size_t max_length);
std::string ConsumeRandomLengthString();
std::string ConsumeRemainingBytesAsString();


template <typename T> T ConsumeIntegral();
template <typename T> T ConsumeIntegralInRange(T min, T max);


template <typename T> T ConsumeFloatingPoint();
template <typename T> T ConsumeFloatingPointInRange(T min, T max);


template <typename T> T ConsumeProbability();

bool ConsumeBool();


template <typename T> T ConsumeEnum();


template <typename T, size_t size> T PickValueInArray(const T (&array)[size]);
template <typename T, size_t size>
T PickValueInArray(const std::array<T, size> &array);
template <typename T> T PickValueInArray(std::initializer_list<const T> list);


size_t ConsumeData(void *destination, size_t num_bytes);


size_t remaining_bytes() { return remaining_bytes_; }

private:
FuzzedDataProvider(const FuzzedDataProvider &) = delete;
FuzzedDataProvider &operator=(const FuzzedDataProvider &) = delete;

void CopyAndAdvance(void *destination, size_t num_bytes);

void Advance(size_t num_bytes);

template <typename T>
std::vector<T> ConsumeBytes(size_t size, size_t num_bytes);

template <typename TS, typename TU> TS ConvertUnsignedToSigned(TU value);

const uint8_t *data_ptr_;
size_t remaining_bytes_;
};





template <typename T>
std::vector<T> FuzzedDataProvider::ConsumeBytes(size_t num_bytes) {
num_bytes = std::min(num_bytes, remaining_bytes_);
return ConsumeBytes<T>(num_bytes, num_bytes);
}





template <typename T>
std::vector<T> FuzzedDataProvider::ConsumeBytesWithTerminator(size_t num_bytes,
T terminator) {
num_bytes = std::min(num_bytes, remaining_bytes_);
std::vector<T> result = ConsumeBytes<T>(num_bytes + 1, num_bytes);
result.back() = terminator;
return result;
}


template <typename T>
std::vector<T> FuzzedDataProvider::ConsumeRemainingBytes() {
return ConsumeBytes<T>(remaining_bytes_);
}





inline std::string FuzzedDataProvider::ConsumeBytesAsString(size_t num_bytes) {
static_assert(sizeof(std::string::value_type) == sizeof(uint8_t),
"ConsumeBytesAsString cannot convert the data to a string.");

num_bytes = std::min(num_bytes, remaining_bytes_);
std::string result(
reinterpret_cast<const std::string::value_type *>(data_ptr_), num_bytes);
Advance(num_bytes);
return result;
}





inline std::string
FuzzedDataProvider::ConsumeRandomLengthString(size_t max_length) {






std::string result;


result.reserve(std::min(max_length, remaining_bytes_));
for (size_t i = 0; i < max_length && remaining_bytes_ != 0; ++i) {
char next = ConvertUnsignedToSigned<char>(data_ptr_[0]);
Advance(1);
if (next == '\\' && remaining_bytes_ != 0) {
next = ConvertUnsignedToSigned<char>(data_ptr_[0]);
Advance(1);
if (next != '\\')
break;
}
result += next;
}

result.shrink_to_fit();
return result;
}


inline std::string FuzzedDataProvider::ConsumeRandomLengthString() {
return ConsumeRandomLengthString(remaining_bytes_);
}




inline std::string FuzzedDataProvider::ConsumeRemainingBytesAsString() {
return ConsumeBytesAsString(remaining_bytes_);
}




template <typename T> T FuzzedDataProvider::ConsumeIntegral() {
return ConsumeIntegralInRange(std::numeric_limits<T>::min(),
std::numeric_limits<T>::max());
}





template <typename T>
T FuzzedDataProvider::ConsumeIntegralInRange(T min, T max) {
static_assert(std::is_integral<T>::value, "An integral type is required.");
static_assert(sizeof(T) <= sizeof(uint64_t), "Unsupported integral type.");

if (min > max)
abort();


uint64_t range = static_cast<uint64_t>(max) - min;
uint64_t result = 0;
size_t offset = 0;

while (offset < sizeof(T) * CHAR_BIT && (range >> offset) > 0 &&
remaining_bytes_ != 0) {






--remaining_bytes_;
result = (result << CHAR_BIT) | data_ptr_[remaining_bytes_];
offset += CHAR_BIT;
}


if (range != std::numeric_limits<decltype(range)>::max())
result = result % (range + 1);

return static_cast<T>(min + result);
}




template <typename T> T FuzzedDataProvider::ConsumeFloatingPoint() {
return ConsumeFloatingPointInRange<T>(std::numeric_limits<T>::lowest(),
std::numeric_limits<T>::max());
}




template <typename T>
T FuzzedDataProvider::ConsumeFloatingPointInRange(T min, T max) {
if (min > max)
abort();

T range = .0;
T result = min;
constexpr T zero(.0);
if (max > zero && min < zero && max > min + std::numeric_limits<T>::max()) {



range = (max / 2.0) - (min / 2.0);
if (ConsumeBool()) {
result += range;
}
} else {
range = max - min;
}

return result + range * ConsumeProbability<T>();
}



template <typename T> T FuzzedDataProvider::ConsumeProbability() {
static_assert(std::is_floating_point<T>::value,
"A floating point type is required.");



using IntegralType =
typename std::conditional<(sizeof(T) <= sizeof(uint32_t)), uint32_t,
uint64_t>::type;

T result = static_cast<T>(ConsumeIntegral<IntegralType>());
result /= static_cast<T>(std::numeric_limits<IntegralType>::max());
return result;
}


inline bool FuzzedDataProvider::ConsumeBool() {
return 1 & ConsumeIntegral<uint8_t>();
}




template <typename T> T FuzzedDataProvider::ConsumeEnum() {
static_assert(std::is_enum<T>::value, "|T| must be an enum type.");
return static_cast<T>(
ConsumeIntegralInRange<uint32_t>(0, static_cast<uint32_t>(T::kMaxValue)));
}


template <typename T, size_t size>
T FuzzedDataProvider::PickValueInArray(const T (&array)[size]) {
static_assert(size > 0, "The array must be non empty.");
return array[ConsumeIntegralInRange<size_t>(0, size - 1)];
}

template <typename T, size_t size>
T FuzzedDataProvider::PickValueInArray(const std::array<T, size> &array) {
static_assert(size > 0, "The array must be non empty.");
return array[ConsumeIntegralInRange<size_t>(0, size - 1)];
}

template <typename T>
T FuzzedDataProvider::PickValueInArray(std::initializer_list<const T> list) {

if (!list.size())
abort();

return *(list.begin() + ConsumeIntegralInRange<size_t>(0, list.size() - 1));
}







inline size_t FuzzedDataProvider::ConsumeData(void *destination,
size_t num_bytes) {
num_bytes = std::min(num_bytes, remaining_bytes_);
CopyAndAdvance(destination, num_bytes);
return num_bytes;
}


inline void FuzzedDataProvider::CopyAndAdvance(void *destination,
size_t num_bytes) {
std::memcpy(destination, data_ptr_, num_bytes);
Advance(num_bytes);
}

inline void FuzzedDataProvider::Advance(size_t num_bytes) {
if (num_bytes > remaining_bytes_)
abort();

data_ptr_ += num_bytes;
remaining_bytes_ -= num_bytes;
}

template <typename T>
std::vector<T> FuzzedDataProvider::ConsumeBytes(size_t size, size_t num_bytes) {
static_assert(sizeof(T) == sizeof(uint8_t), "Incompatible data type.");







std::vector<T> result(size);
if (size == 0) {
if (num_bytes != 0)
abort();
return result;
}

CopyAndAdvance(result.data(), num_bytes);




result.shrink_to_fit();
return result;
}

template <typename TS, typename TU>
TS FuzzedDataProvider::ConvertUnsignedToSigned(TU value) {
static_assert(sizeof(TS) == sizeof(TU), "Incompatible data types.");
static_assert(!std::numeric_limits<TU>::is_signed,
"Source type must be unsigned.");


if (std::numeric_limits<TS>::is_modulo)
return static_cast<TS>(value);



if (value <= std::numeric_limits<TS>::max()) {
return static_cast<TS>(value);
} else {
constexpr auto TS_min = std::numeric_limits<TS>::min();
return TS_min + static_cast<TS>(value - TS_min);
}
}

#endif
