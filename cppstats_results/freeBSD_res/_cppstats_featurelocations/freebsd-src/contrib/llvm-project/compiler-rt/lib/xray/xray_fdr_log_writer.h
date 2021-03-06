










#if !defined(COMPILER_RT_LIB_XRAY_XRAY_FDR_LOG_WRITER_H_)
#define COMPILER_RT_LIB_XRAY_XRAY_FDR_LOG_WRITER_H_

#include "xray_buffer_queue.h"
#include "xray_fdr_log_records.h"
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace __xray {

template <size_t Index> struct SerializerImpl {
template <class Tuple,
typename std::enable_if<
Index<std::tuple_size<
typename std::remove_reference<Tuple>::type>::value,
int>::type = 0> static void serializeTo(char *Buffer,
Tuple &&T) {
auto P = reinterpret_cast<const char *>(&std::get<Index>(T));
constexpr auto Size = sizeof(std::get<Index>(T));
internal_memcpy(Buffer, P, Size);
SerializerImpl<Index + 1>::serializeTo(Buffer + Size,
std::forward<Tuple>(T));
}

template <class Tuple,
typename std::enable_if<
Index >= std::tuple_size<typename std::remove_reference<
Tuple>::type>::value,
int>::type = 0>
static void serializeTo(char *, Tuple &&) {}
};

using Serializer = SerializerImpl<0>;

template <class Tuple, size_t Index> struct AggregateSizesImpl {
static constexpr size_t value =
sizeof(typename std::tuple_element<Index, Tuple>::type) +
AggregateSizesImpl<Tuple, Index - 1>::value;
};

template <class Tuple> struct AggregateSizesImpl<Tuple, 0> {
static constexpr size_t value =
sizeof(typename std::tuple_element<0, Tuple>::type);
};

template <class Tuple> struct AggregateSizes {
static constexpr size_t value =
AggregateSizesImpl<Tuple, std::tuple_size<Tuple>::value - 1>::value;
};

template <MetadataRecord::RecordKinds Kind, class... DataTypes>
MetadataRecord createMetadataRecord(DataTypes &&... Ds) {
static_assert(AggregateSizes<std::tuple<DataTypes...>>::value <=
sizeof(MetadataRecord) - 1,
"Metadata payload longer than metadata buffer!");
MetadataRecord R;
R.Type = 1;
R.RecordKind = static_cast<uint8_t>(Kind);
Serializer::serializeTo(R.Data,
std::make_tuple(std::forward<DataTypes>(Ds)...));
return R;
}

class FDRLogWriter {
BufferQueue::Buffer &Buffer;
char *NextRecord = nullptr;

template <class T> void writeRecord(const T &R) {
internal_memcpy(NextRecord, reinterpret_cast<const char *>(&R), sizeof(T));
NextRecord += sizeof(T);



atomic_thread_fence(memory_order_release);
atomic_fetch_add(Buffer.Extents, sizeof(T), memory_order_acq_rel);
}

public:
explicit FDRLogWriter(BufferQueue::Buffer &B, char *P)
: Buffer(B), NextRecord(P) {
DCHECK_NE(Buffer.Data, nullptr);
DCHECK_NE(NextRecord, nullptr);
}

explicit FDRLogWriter(BufferQueue::Buffer &B)
: FDRLogWriter(B, static_cast<char *>(B.Data)) {}

template <MetadataRecord::RecordKinds Kind, class... Data>
bool writeMetadata(Data &&... Ds) {



writeRecord(createMetadataRecord<Kind>(std::forward<Data>(Ds)...));
return true;
}

template <size_t N> size_t writeMetadataRecords(MetadataRecord (&Recs)[N]) {
constexpr auto Size = sizeof(MetadataRecord) * N;
internal_memcpy(NextRecord, reinterpret_cast<const char *>(Recs), Size);
NextRecord += Size;



atomic_thread_fence(memory_order_release);
atomic_fetch_add(Buffer.Extents, Size, memory_order_acq_rel);
return Size;
}

enum class FunctionRecordKind : uint8_t {
Enter = 0x00,
Exit = 0x01,
TailExit = 0x02,
EnterArg = 0x03,
};

bool writeFunction(FunctionRecordKind Kind, int32_t FuncId, int32_t Delta) {
FunctionRecord R;
R.Type = 0;
R.RecordKind = uint8_t(Kind);
R.FuncId = FuncId;
R.TSCDelta = Delta;
writeRecord(R);
return true;
}

bool writeFunctionWithArg(FunctionRecordKind Kind, int32_t FuncId,
int32_t Delta, uint64_t Arg) {




FunctionRecord R;
R.Type = 0;
R.RecordKind = uint8_t(Kind);
R.FuncId = FuncId;
R.TSCDelta = Delta;
MetadataRecord A =
createMetadataRecord<MetadataRecord::RecordKinds::CallArgument>(Arg);
NextRecord = reinterpret_cast<char *>(internal_memcpy(
NextRecord, reinterpret_cast<char *>(&R), sizeof(R))) +
sizeof(R);
NextRecord = reinterpret_cast<char *>(internal_memcpy(
NextRecord, reinterpret_cast<char *>(&A), sizeof(A))) +
sizeof(A);



atomic_thread_fence(memory_order_release);
atomic_fetch_add(Buffer.Extents, sizeof(R) + sizeof(A),
memory_order_acq_rel);
return true;
}

bool writeCustomEvent(int32_t Delta, const void *Event, int32_t EventSize) {





MetadataRecord R =
createMetadataRecord<MetadataRecord::RecordKinds::CustomEventMarker>(
EventSize, Delta);
NextRecord = reinterpret_cast<char *>(internal_memcpy(
NextRecord, reinterpret_cast<char *>(&R), sizeof(R))) +
sizeof(R);
NextRecord = reinterpret_cast<char *>(
internal_memcpy(NextRecord, Event, EventSize)) +
EventSize;




atomic_thread_fence(memory_order_release);
atomic_fetch_add(Buffer.Extents, sizeof(R) + EventSize,
memory_order_acq_rel);
return true;
}

bool writeTypedEvent(int32_t Delta, uint16_t EventType, const void *Event,
int32_t EventSize) {


MetadataRecord R =
createMetadataRecord<MetadataRecord::RecordKinds::TypedEventMarker>(
EventSize, Delta, EventType);
NextRecord = reinterpret_cast<char *>(internal_memcpy(
NextRecord, reinterpret_cast<char *>(&R), sizeof(R))) +
sizeof(R);
NextRecord = reinterpret_cast<char *>(
internal_memcpy(NextRecord, Event, EventSize)) +
EventSize;




atomic_thread_fence(memory_order_release);
atomic_fetch_add(Buffer.Extents, EventSize, memory_order_acq_rel);
return true;
}

char *getNextRecord() const { return NextRecord; }

void resetRecord() {
NextRecord = reinterpret_cast<char *>(Buffer.Data);
atomic_store(Buffer.Extents, 0, memory_order_release);
}

void undoWrites(size_t B) {
DCHECK_GE(NextRecord - B, reinterpret_cast<char *>(Buffer.Data));
NextRecord -= B;
atomic_fetch_sub(Buffer.Extents, B, memory_order_acq_rel);
}

};

}

#endif
