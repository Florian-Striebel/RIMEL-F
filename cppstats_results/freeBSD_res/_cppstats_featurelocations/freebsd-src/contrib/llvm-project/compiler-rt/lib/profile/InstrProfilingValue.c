







#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "InstrProfiling.h"
#include "InstrProfilingInternal.h"
#include "InstrProfilingUtil.h"

#define INSTR_PROF_VALUE_PROF_DATA
#define INSTR_PROF_COMMON_API_IMPL
#define INSTR_PROF_VALUE_PROF_MEMOP_API
#include "profile/InstrProfData.inc"

static int hasStaticCounters = 1;
static int OutOfNodesWarnings = 0;
static int hasNonDefaultValsPerSite = 0;
#define INSTR_PROF_MAX_VP_WARNS 10
#define INSTR_PROF_DEFAULT_NUM_VAL_PER_SITE 24
#define INSTR_PROF_VNODE_POOL_SIZE 1024

#if !defined(_MSC_VER)


COMPILER_RT_VISIBILITY ValueProfNode
lprofValueProfNodes[INSTR_PROF_VNODE_POOL_SIZE] COMPILER_RT_SECTION(
COMPILER_RT_SEG INSTR_PROF_VNODES_SECT_NAME);
#endif

COMPILER_RT_VISIBILITY uint32_t VPMaxNumValsPerSite =
INSTR_PROF_DEFAULT_NUM_VAL_PER_SITE;

COMPILER_RT_VISIBILITY void lprofSetupValueProfiler() {
const char *Str = 0;
Str = getenv("LLVM_VP_MAX_NUM_VALS_PER_SITE");
if (Str && Str[0]) {
VPMaxNumValsPerSite = atoi(Str);
hasNonDefaultValsPerSite = 1;
}
if (VPMaxNumValsPerSite > INSTR_PROF_MAX_NUM_VAL_PER_SITE)
VPMaxNumValsPerSite = INSTR_PROF_MAX_NUM_VAL_PER_SITE;
}

COMPILER_RT_VISIBILITY void lprofSetMaxValsPerSite(uint32_t MaxVals) {
VPMaxNumValsPerSite = MaxVals;
hasNonDefaultValsPerSite = 1;
}


COMPILER_RT_VISIBILITY void
__llvm_profile_set_num_value_sites(__llvm_profile_data *Data,
uint32_t ValueKind, uint16_t NumValueSites) {
*((uint16_t *)&Data->NumValueSites[ValueKind]) = NumValueSites;
}


COMPILER_RT_VISIBILITY const __llvm_profile_data *
__llvm_profile_iterate_data(const __llvm_profile_data *Data) {
return Data + 1;
}


COMPILER_RT_VISIBILITY void *
__llvm_get_function_addr(const __llvm_profile_data *Data) {
return Data->FunctionPointer;
}







static int allocateValueProfileCounters(__llvm_profile_data *Data) {
uint64_t NumVSites = 0;
uint32_t VKI;



hasStaticCounters = 0;


if (!hasNonDefaultValsPerSite)
VPMaxNumValsPerSite = INSTR_PROF_MAX_NUM_VAL_PER_SITE;

for (VKI = IPVK_First; VKI <= IPVK_Last; ++VKI)
NumVSites += Data->NumValueSites[VKI];


assert(NumVSites > 0 && "NumVSites can't be zero");
ValueProfNode **Mem =
(ValueProfNode **)calloc(NumVSites, sizeof(ValueProfNode *));
if (!Mem)
return 0;
if (!COMPILER_RT_BOOL_CMPXCHG(&Data->Values, 0, Mem)) {
free(Mem);
return 0;
}
return 1;
}

static ValueProfNode *allocateOneNode(void) {
ValueProfNode *Node;

if (!hasStaticCounters)
return (ValueProfNode *)calloc(1, sizeof(ValueProfNode));


if (CurrentVNode + 1 > EndVNode) {
if (OutOfNodesWarnings++ < INSTR_PROF_MAX_VP_WARNS) {
PROF_WARN("Unable to track new values: %s. "
" Consider using option -mllvm -vp-counters-per-site=<n> to "
"allocate more"
" value profile counters at compile time. \n",
"Running out of static counters");
}
return 0;
}
Node = COMPILER_RT_PTR_FETCH_ADD(ValueProfNode, CurrentVNode, 1);


if (Node + 1 > EndVNode)
return 0;

return Node;
}

static COMPILER_RT_ALWAYS_INLINE void
instrumentTargetValueImpl(uint64_t TargetValue, void *Data,
uint32_t CounterIndex, uint64_t CountValue) {
__llvm_profile_data *PData = (__llvm_profile_data *)Data;
if (!PData)
return;
if (!CountValue)
return;
if (!PData->Values) {
if (!allocateValueProfileCounters(PData))
return;
}

ValueProfNode **ValueCounters = (ValueProfNode **)PData->Values;
ValueProfNode *PrevVNode = NULL;
ValueProfNode *MinCountVNode = NULL;
ValueProfNode *CurVNode = ValueCounters[CounterIndex];
uint64_t MinCount = UINT64_MAX;

uint8_t VDataCount = 0;
while (CurVNode) {
if (TargetValue == CurVNode->Value) {
CurVNode->Count += CountValue;
return;
}
if (CurVNode->Count < MinCount) {
MinCount = CurVNode->Count;
MinCountVNode = CurVNode;
}
PrevVNode = CurVNode;
CurVNode = CurVNode->Next;
++VDataCount;
}

if (VDataCount >= VPMaxNumValsPerSite) {




























if (MinCountVNode->Count <= CountValue) {
CurVNode = MinCountVNode;
CurVNode->Value = TargetValue;
CurVNode->Count = CountValue;
} else
MinCountVNode->Count -= CountValue;

return;
}

CurVNode = allocateOneNode();
if (!CurVNode)
return;
CurVNode->Value = TargetValue;
CurVNode->Count += CountValue;

uint32_t Success = 0;
if (!ValueCounters[CounterIndex])
Success =
COMPILER_RT_BOOL_CMPXCHG(&ValueCounters[CounterIndex], 0, CurVNode);
else if (PrevVNode && !PrevVNode->Next)
Success = COMPILER_RT_BOOL_CMPXCHG(&(PrevVNode->Next), 0, CurVNode);

if (!Success && !hasStaticCounters) {
free(CurVNode);
return;
}
}

COMPILER_RT_VISIBILITY void
__llvm_profile_instrument_target(uint64_t TargetValue, void *Data,
uint32_t CounterIndex) {
instrumentTargetValueImpl(TargetValue, Data, CounterIndex, 1);
}
COMPILER_RT_VISIBILITY void
__llvm_profile_instrument_target_value(uint64_t TargetValue, void *Data,
uint32_t CounterIndex,
uint64_t CountValue) {
instrumentTargetValueImpl(TargetValue, Data, CounterIndex, CountValue);
}





COMPILER_RT_VISIBILITY void
__llvm_profile_instrument_memop(uint64_t TargetValue, void *Data,
uint32_t CounterIndex) {

uint64_t RepValue = InstrProfGetRangeRepValue(TargetValue);
__llvm_profile_instrument_target(RepValue, Data, CounterIndex);
}








typedef struct ValueProfRuntimeRecord {
const __llvm_profile_data *Data;
ValueProfNode **NodesKind[IPVK_Last + 1];
uint8_t **SiteCountArray;
} ValueProfRuntimeRecord;



static uint32_t getNumValueSitesRT(const void *R, uint32_t VK) {
return ((const ValueProfRuntimeRecord *)R)->Data->NumValueSites[VK];
}

static uint32_t getNumValueDataRT(const void *R, uint32_t VK) {
uint32_t S = 0, I;
const ValueProfRuntimeRecord *Record = (const ValueProfRuntimeRecord *)R;
if (Record->SiteCountArray[VK] == INSTR_PROF_NULLPTR)
return 0;
for (I = 0; I < Record->Data->NumValueSites[VK]; I++)
S += Record->SiteCountArray[VK][I];
return S;
}

static uint32_t getNumValueDataForSiteRT(const void *R, uint32_t VK,
uint32_t S) {
const ValueProfRuntimeRecord *Record = (const ValueProfRuntimeRecord *)R;
return Record->SiteCountArray[VK][S];
}

static ValueProfRuntimeRecord RTRecord;
static ValueProfRecordClosure RTRecordClosure = {
&RTRecord, INSTR_PROF_NULLPTR,
getNumValueSitesRT, getNumValueDataRT, getNumValueDataForSiteRT,
INSTR_PROF_NULLPTR,
INSTR_PROF_NULLPTR,
INSTR_PROF_NULLPTR
};

static uint32_t
initializeValueProfRuntimeRecord(const __llvm_profile_data *Data,
uint8_t *SiteCountArray[]) {
unsigned I, J, S = 0, NumValueKinds = 0;
ValueProfNode **Nodes = (ValueProfNode **)Data->Values;
RTRecord.Data = Data;
RTRecord.SiteCountArray = SiteCountArray;
for (I = 0; I <= IPVK_Last; I++) {
uint16_t N = Data->NumValueSites[I];
if (!N)
continue;

NumValueKinds++;

RTRecord.NodesKind[I] = Nodes ? &Nodes[S] : INSTR_PROF_NULLPTR;
for (J = 0; J < N; J++) {

uint32_t C = 0;
ValueProfNode *Site =
Nodes ? RTRecord.NodesKind[I][J] : INSTR_PROF_NULLPTR;
while (Site) {
C++;
Site = Site->Next;
}
if (C > UCHAR_MAX)
C = UCHAR_MAX;
RTRecord.SiteCountArray[I][J] = C;
}
S += N;
}
return NumValueKinds;
}

static ValueProfNode *getNextNValueData(uint32_t VK, uint32_t Site,
InstrProfValueData *Dst,
ValueProfNode *StartNode, uint32_t N) {
unsigned I;
ValueProfNode *VNode = StartNode ? StartNode : RTRecord.NodesKind[VK][Site];
for (I = 0; I < N; I++) {
Dst[I].Value = VNode->Value;
Dst[I].Count = VNode->Count;
VNode = VNode->Next;
}
return VNode;
}

static uint32_t getValueProfDataSizeWrapper(void) {
return getValueProfDataSize(&RTRecordClosure);
}

static uint32_t getNumValueDataForSiteWrapper(uint32_t VK, uint32_t S) {
return getNumValueDataForSiteRT(&RTRecord, VK, S);
}

static VPDataReaderType TheVPDataReader = {
initializeValueProfRuntimeRecord, getValueProfRecordHeaderSize,
getFirstValueProfRecord, getNumValueDataForSiteWrapper,
getValueProfDataSizeWrapper, getNextNValueData};

COMPILER_RT_VISIBILITY VPDataReaderType *lprofGetVPDataReader() {
return &TheVPDataReader;
}
