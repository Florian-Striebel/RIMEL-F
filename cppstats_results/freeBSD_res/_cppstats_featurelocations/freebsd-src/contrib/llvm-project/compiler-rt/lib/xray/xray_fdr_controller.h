










#if !defined(COMPILER_RT_LIB_XRAY_XRAY_FDR_CONTROLLER_H_)
#define COMPILER_RT_LIB_XRAY_XRAY_FDR_CONTROLLER_H_

#include <limits>
#include <time.h>

#include "xray/xray_interface.h"
#include "xray/xray_records.h"
#include "xray_buffer_queue.h"
#include "xray_fdr_log_writer.h"

namespace __xray {

template <size_t Version = 5> class FDRController {
BufferQueue *BQ;
BufferQueue::Buffer &B;
FDRLogWriter &W;
int (*WallClockReader)(clockid_t, struct timespec *) = 0;
uint64_t CycleThreshold = 0;

uint64_t LastFunctionEntryTSC = 0;
uint64_t LatestTSC = 0;
uint16_t LatestCPU = 0;
tid_t TId = 0;
pid_t PId = 0;
bool First = true;

uint32_t UndoableFunctionEnters = 0;
uint32_t UndoableTailExits = 0;

bool finalized() const XRAY_NEVER_INSTRUMENT {
return BQ == nullptr || BQ->finalizing();
}

bool hasSpace(size_t S) XRAY_NEVER_INSTRUMENT {
return B.Data != nullptr && B.Generation == BQ->generation() &&
W.getNextRecord() + S <= reinterpret_cast<char *>(B.Data) + B.Size;
}

constexpr int32_t mask(int32_t FuncId) const XRAY_NEVER_INSTRUMENT {
return FuncId & ((1 << 29) - 1);
}

bool getNewBuffer() XRAY_NEVER_INSTRUMENT {
if (BQ->getBuffer(B) != BufferQueue::ErrorCode::Ok)
return false;

W.resetRecord();
DCHECK_EQ(W.getNextRecord(), B.Data);
LatestTSC = 0;
LatestCPU = 0;
First = true;
UndoableFunctionEnters = 0;
UndoableTailExits = 0;
atomic_store(B.Extents, 0, memory_order_release);
return true;
}

bool setupNewBuffer() XRAY_NEVER_INSTRUMENT {
if (finalized())
return false;

DCHECK(hasSpace(sizeof(MetadataRecord) * 3));
TId = GetTid();
PId = internal_getpid();
struct timespec TS {
0, 0
};
WallClockReader(CLOCK_MONOTONIC, &TS);

MetadataRecord Metadata[] = {






createMetadataRecord<MetadataRecord::RecordKinds::NewBuffer>(
static_cast<int32_t>(TId)),




createMetadataRecord<MetadataRecord::RecordKinds::WalltimeMarker>(
static_cast<int64_t>(TS.tv_sec),
static_cast<int32_t>(TS.tv_nsec / 1000)),


createMetadataRecord<MetadataRecord::RecordKinds::Pid>(
static_cast<int32_t>(PId)),
};

if (finalized())
return false;
return W.writeMetadataRecords(Metadata);
}

bool prepareBuffer(size_t S) XRAY_NEVER_INSTRUMENT {
if (finalized())
return returnBuffer();

if (UNLIKELY(!hasSpace(S))) {
if (!returnBuffer())
return false;
if (!getNewBuffer())
return false;
if (!setupNewBuffer())
return false;
}

if (First) {
First = false;
W.resetRecord();
atomic_store(B.Extents, 0, memory_order_release);
return setupNewBuffer();
}

return true;
}

bool returnBuffer() XRAY_NEVER_INSTRUMENT {
if (BQ == nullptr)
return false;

First = true;
if (finalized()) {
BQ->releaseBuffer(B);
return false;
}

return BQ->releaseBuffer(B) == BufferQueue::ErrorCode::Ok;
}

enum class PreambleResult { NoChange, WroteMetadata, InvalidBuffer };
PreambleResult recordPreamble(uint64_t TSC,
uint16_t CPU) XRAY_NEVER_INSTRUMENT {
if (UNLIKELY(LatestCPU != CPU || LatestTSC == 0)) {


LatestTSC = TSC;
LatestCPU = CPU;

if (B.Generation != BQ->generation())
return PreambleResult::InvalidBuffer;

W.writeMetadata<MetadataRecord::RecordKinds::NewCPUId>(CPU, TSC);
return PreambleResult::WroteMetadata;
}

DCHECK_EQ(LatestCPU, CPU);

if (UNLIKELY(LatestTSC > TSC ||
TSC - LatestTSC >
uint64_t{std::numeric_limits<int32_t>::max()})) {



LatestTSC = TSC;

if (B.Generation != BQ->generation())
return PreambleResult::InvalidBuffer;

W.writeMetadata<MetadataRecord::RecordKinds::TSCWrap>(TSC);
return PreambleResult::WroteMetadata;
}

return PreambleResult::NoChange;
}

bool rewindRecords(int32_t FuncId, uint64_t TSC,
uint16_t CPU) XRAY_NEVER_INSTRUMENT {






FunctionRecord F;
W.undoWrites(sizeof(FunctionRecord));
if (B.Generation != BQ->generation())
return false;
internal_memcpy(&F, W.getNextRecord(), sizeof(FunctionRecord));

DCHECK(F.RecordKind ==
uint8_t(FunctionRecord::RecordKinds::FunctionEnter) &&
"Expected to find function entry recording when rewinding.");
DCHECK_EQ(F.FuncId, FuncId & ~(0x0F << 28));

LatestTSC -= F.TSCDelta;
if (--UndoableFunctionEnters != 0) {
LastFunctionEntryTSC -= F.TSCDelta;
return true;
}

LastFunctionEntryTSC = 0;
auto RewindingTSC = LatestTSC;
auto RewindingRecordPtr = W.getNextRecord() - sizeof(FunctionRecord);
while (UndoableTailExits) {
if (B.Generation != BQ->generation())
return false;
internal_memcpy(&F, RewindingRecordPtr, sizeof(FunctionRecord));
DCHECK_EQ(F.RecordKind,
uint8_t(FunctionRecord::RecordKinds::FunctionTailExit));
RewindingTSC -= F.TSCDelta;
RewindingRecordPtr -= sizeof(FunctionRecord);
if (B.Generation != BQ->generation())
return false;
internal_memcpy(&F, RewindingRecordPtr, sizeof(FunctionRecord));


if ((TSC - RewindingTSC) >= CycleThreshold) {
UndoableTailExits = 0;
return true;
}

--UndoableTailExits;
W.undoWrites(sizeof(FunctionRecord) * 2);
LatestTSC = RewindingTSC;
}
return true;
}

public:
template <class WallClockFunc>
FDRController(BufferQueue *BQ, BufferQueue::Buffer &B, FDRLogWriter &W,
WallClockFunc R, uint64_t C) XRAY_NEVER_INSTRUMENT
: BQ(BQ),
B(B),
W(W),
WallClockReader(R),
CycleThreshold(C) {}

bool functionEnter(int32_t FuncId, uint64_t TSC,
uint16_t CPU) XRAY_NEVER_INSTRUMENT {
if (finalized() ||
!prepareBuffer(sizeof(MetadataRecord) + sizeof(FunctionRecord)))
return returnBuffer();

auto PreambleStatus = recordPreamble(TSC, CPU);
if (PreambleStatus == PreambleResult::InvalidBuffer)
return returnBuffer();

if (PreambleStatus == PreambleResult::WroteMetadata) {
UndoableFunctionEnters = 1;
UndoableTailExits = 0;
} else {
++UndoableFunctionEnters;
}

auto Delta = TSC - LatestTSC;
LastFunctionEntryTSC = TSC;
LatestTSC = TSC;
return W.writeFunction(FDRLogWriter::FunctionRecordKind::Enter,
mask(FuncId), Delta);
}

bool functionTailExit(int32_t FuncId, uint64_t TSC,
uint16_t CPU) XRAY_NEVER_INSTRUMENT {
if (finalized())
return returnBuffer();

if (!prepareBuffer(sizeof(MetadataRecord) + sizeof(FunctionRecord)))
return returnBuffer();

auto PreambleStatus = recordPreamble(TSC, CPU);
if (PreambleStatus == PreambleResult::InvalidBuffer)
return returnBuffer();

if (PreambleStatus == PreambleResult::NoChange &&
UndoableFunctionEnters != 0 &&
TSC - LastFunctionEntryTSC < CycleThreshold)
return rewindRecords(FuncId, TSC, CPU);

UndoableTailExits = UndoableFunctionEnters ? UndoableTailExits + 1 : 0;
UndoableFunctionEnters = 0;
auto Delta = TSC - LatestTSC;
LatestTSC = TSC;
return W.writeFunction(FDRLogWriter::FunctionRecordKind::TailExit,
mask(FuncId), Delta);
}

bool functionEnterArg(int32_t FuncId, uint64_t TSC, uint16_t CPU,
uint64_t Arg) XRAY_NEVER_INSTRUMENT {
if (finalized() ||
!prepareBuffer((2 * sizeof(MetadataRecord)) + sizeof(FunctionRecord)) ||
recordPreamble(TSC, CPU) == PreambleResult::InvalidBuffer)
return returnBuffer();

auto Delta = TSC - LatestTSC;
LatestTSC = TSC;
LastFunctionEntryTSC = 0;
UndoableFunctionEnters = 0;
UndoableTailExits = 0;

return W.writeFunctionWithArg(FDRLogWriter::FunctionRecordKind::EnterArg,
mask(FuncId), Delta, Arg);
}

bool functionExit(int32_t FuncId, uint64_t TSC,
uint16_t CPU) XRAY_NEVER_INSTRUMENT {
if (finalized() ||
!prepareBuffer(sizeof(MetadataRecord) + sizeof(FunctionRecord)))
return returnBuffer();

auto PreambleStatus = recordPreamble(TSC, CPU);
if (PreambleStatus == PreambleResult::InvalidBuffer)
return returnBuffer();

if (PreambleStatus == PreambleResult::NoChange &&
UndoableFunctionEnters != 0 &&
TSC - LastFunctionEntryTSC < CycleThreshold)
return rewindRecords(FuncId, TSC, CPU);

auto Delta = TSC - LatestTSC;
LatestTSC = TSC;
UndoableFunctionEnters = 0;
UndoableTailExits = 0;
return W.writeFunction(FDRLogWriter::FunctionRecordKind::Exit, mask(FuncId),
Delta);
}

bool customEvent(uint64_t TSC, uint16_t CPU, const void *Event,
int32_t EventSize) XRAY_NEVER_INSTRUMENT {
if (finalized() ||
!prepareBuffer((2 * sizeof(MetadataRecord)) + EventSize) ||
recordPreamble(TSC, CPU) == PreambleResult::InvalidBuffer)
return returnBuffer();

auto Delta = TSC - LatestTSC;
LatestTSC = TSC;
UndoableFunctionEnters = 0;
UndoableTailExits = 0;
return W.writeCustomEvent(Delta, Event, EventSize);
}

bool typedEvent(uint64_t TSC, uint16_t CPU, uint16_t EventType,
const void *Event, int32_t EventSize) XRAY_NEVER_INSTRUMENT {
if (finalized() ||
!prepareBuffer((2 * sizeof(MetadataRecord)) + EventSize) ||
recordPreamble(TSC, CPU) == PreambleResult::InvalidBuffer)
return returnBuffer();

auto Delta = TSC - LatestTSC;
LatestTSC = TSC;
UndoableFunctionEnters = 0;
UndoableTailExits = 0;
return W.writeTypedEvent(Delta, EventType, Event, EventSize);
}

bool flush() XRAY_NEVER_INSTRUMENT {
if (finalized()) {
returnBuffer();
return true;
}
return returnBuffer();
}
};

}

#endif
