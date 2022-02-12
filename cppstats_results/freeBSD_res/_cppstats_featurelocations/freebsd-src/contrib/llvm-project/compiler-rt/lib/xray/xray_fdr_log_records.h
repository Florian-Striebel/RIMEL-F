










#if !defined(XRAY_XRAY_FDR_LOG_RECORDS_H)
#define XRAY_XRAY_FDR_LOG_RECORDS_H
#include <cstdint>

namespace __xray {

enum class RecordType : uint8_t { Function, Metadata };



struct alignas(16) MetadataRecord {

uint8_t Type : 1;



enum class RecordKinds : uint8_t {
NewBuffer,
EndOfBuffer,
NewCPUId,
TSCWrap,
WalltimeMarker,
CustomEventMarker,
CallArgument,
BufferExtents,
TypedEventMarker,
Pid,
};


uint8_t RecordKind : 7;
char Data[15];
} __attribute__((packed));

static_assert(sizeof(MetadataRecord) == 16, "Wrong size for MetadataRecord.");

struct alignas(8) FunctionRecord {

uint8_t Type : 1;
enum class RecordKinds {
FunctionEnter = 0x00,
FunctionExit = 0x01,
FunctionTailExit = 0x02,
};
uint8_t RecordKind : 3;




int FuncId : 28;






uint32_t TSCDelta;
} __attribute__((packed));

static_assert(sizeof(FunctionRecord) == 8, "Wrong size for FunctionRecord.");

}

#endif
