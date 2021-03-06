














#if !defined(XRAY_XRAY_RECORDS_H)
#define XRAY_XRAY_RECORDS_H

#include <cstdint>

namespace __xray {

enum FileTypes {
NAIVE_LOG = 0,
FDR_LOG = 1,
};


struct alignas(16) FdrAdditionalHeaderData {
uint64_t ThreadBufferSize;
};

static_assert(sizeof(FdrAdditionalHeaderData) == 16,
"FdrAdditionalHeaderData != 16 bytes");



struct alignas(32) XRayFileHeader {
uint16_t Version = 0;




uint16_t Type = 0;



bool ConstantTSC : 1;
bool NonstopTSC : 1;


alignas(8) uint64_t CycleFrequency = 0;

union {
char FreeForm[16];



struct timespec TS;

struct FdrAdditionalHeaderData FdrData;
};
} __attribute__((packed));

static_assert(sizeof(XRayFileHeader) == 32, "XRayFileHeader != 32 bytes");

enum RecordTypes {
NORMAL = 0,
ARG_PAYLOAD = 1,
};

struct alignas(32) XRayRecord {



uint16_t RecordType = RecordTypes::NORMAL;


uint8_t CPU = 0;






uint8_t Type = 0;


int32_t FuncId = 0;


uint64_t TSC = 0;


uint32_t TId = 0;


uint32_t PId = 0;


char Buffer[8] = {};
} __attribute__((packed));

static_assert(sizeof(XRayRecord) == 32, "XRayRecord != 32 bytes");

struct alignas(32) XRayArgPayload {


uint16_t RecordType = RecordTypes::ARG_PAYLOAD;


uint8_t Padding[2] = {};


int32_t FuncId = 0;


uint32_t TId = 0;


uint32_t PId = 0;


uint64_t Arg = 0;


uint8_t TailPadding[8] = {};
} __attribute__((packed));

static_assert(sizeof(XRayArgPayload) == 32, "XRayArgPayload != 32 bytes");

}

#endif
