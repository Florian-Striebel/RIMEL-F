










#if !defined(__Fuchsia__)

#include "InstrProfiling.h"
#include "InstrProfilingInternal.h"
#include "InstrProfilingUtil.h"

#define INSTR_PROF_VALUE_PROF_DATA
#include "profile/InstrProfData.inc"



COMPILER_RT_VISIBILITY
void lprofMergeValueProfData(ValueProfData *SrcValueProfData,
__llvm_profile_data *DstData) {
unsigned I, S, V, DstIndex = 0;
InstrProfValueData *VData;
ValueProfRecord *VR = getFirstValueProfRecord(SrcValueProfData);
for (I = 0; I < SrcValueProfData->NumValueKinds; I++) {
VData = getValueProfRecordValueData(VR);
unsigned SrcIndex = 0;
for (S = 0; S < VR->NumValueSites; S++) {
uint8_t NV = VR->SiteCountArray[S];
for (V = 0; V < NV; V++) {
__llvm_profile_instrument_target_value(VData[SrcIndex].Value, DstData,
DstIndex, VData[SrcIndex].Count);
++SrcIndex;
}
++DstIndex;
}
VR = getValueProfRecordNext(VR);
}
}

#endif
