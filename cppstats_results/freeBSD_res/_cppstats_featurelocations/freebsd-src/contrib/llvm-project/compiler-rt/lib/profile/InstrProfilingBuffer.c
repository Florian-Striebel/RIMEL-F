










#include "InstrProfiling.h"
#include "InstrProfilingInternal.h"
#include "InstrProfilingPort.h"







static int ContinuouslySyncProfile = 0;



static unsigned PageSize = 0;

COMPILER_RT_VISIBILITY int __llvm_profile_is_continuous_mode_enabled(void) {
return ContinuouslySyncProfile && PageSize;
}

COMPILER_RT_VISIBILITY void __llvm_profile_enable_continuous_mode(void) {
ContinuouslySyncProfile = 1;
}

COMPILER_RT_VISIBILITY void __llvm_profile_set_page_size(unsigned PS) {
PageSize = PS;
}

COMPILER_RT_VISIBILITY
uint64_t __llvm_profile_get_size_for_buffer(void) {
const __llvm_profile_data *DataBegin = __llvm_profile_begin_data();
const __llvm_profile_data *DataEnd = __llvm_profile_end_data();
const uint64_t *CountersBegin = __llvm_profile_begin_counters();
const uint64_t *CountersEnd = __llvm_profile_end_counters();
const char *NamesBegin = __llvm_profile_begin_names();
const char *NamesEnd = __llvm_profile_end_names();

return __llvm_profile_get_size_for_buffer_internal(
DataBegin, DataEnd, CountersBegin, CountersEnd, NamesBegin, NamesEnd);
}

COMPILER_RT_VISIBILITY
uint64_t __llvm_profile_get_data_size(const __llvm_profile_data *Begin,
const __llvm_profile_data *End) {
intptr_t BeginI = (intptr_t)Begin, EndI = (intptr_t)End;
return ((EndI + sizeof(__llvm_profile_data) - 1) - BeginI) /
sizeof(__llvm_profile_data);
}



static uint64_t calculateBytesNeededToPageAlign(uint64_t Offset) {
uint64_t OffsetModPage = Offset % PageSize;
if (OffsetModPage > 0)
return PageSize - OffsetModPage;
return 0;
}

static int needsCounterPadding(void) {
#if defined(__APPLE__)
return __llvm_profile_is_continuous_mode_enabled();
#else
return 0;
#endif
}

COMPILER_RT_VISIBILITY
void __llvm_profile_get_padding_sizes_for_counters(
uint64_t DataSize, uint64_t CountersSize, uint64_t NamesSize,
uint64_t *PaddingBytesBeforeCounters, uint64_t *PaddingBytesAfterCounters,
uint64_t *PaddingBytesAfterNames) {
if (!needsCounterPadding()) {
*PaddingBytesBeforeCounters = 0;
*PaddingBytesAfterCounters = 0;
*PaddingBytesAfterNames = __llvm_profile_get_num_padding_bytes(NamesSize);
return;
}



uint64_t DataSizeInBytes = DataSize * sizeof(__llvm_profile_data);
uint64_t CountersSizeInBytes = CountersSize * sizeof(uint64_t);
*PaddingBytesBeforeCounters = calculateBytesNeededToPageAlign(
sizeof(__llvm_profile_header) + DataSizeInBytes);
*PaddingBytesAfterCounters =
calculateBytesNeededToPageAlign(CountersSizeInBytes);
*PaddingBytesAfterNames = calculateBytesNeededToPageAlign(NamesSize);
}

COMPILER_RT_VISIBILITY
uint64_t __llvm_profile_get_size_for_buffer_internal(
const __llvm_profile_data *DataBegin, const __llvm_profile_data *DataEnd,
const uint64_t *CountersBegin, const uint64_t *CountersEnd,
const char *NamesBegin, const char *NamesEnd) {

const uint64_t NamesSize = (NamesEnd - NamesBegin) * sizeof(char);
uint64_t DataSize = __llvm_profile_get_data_size(DataBegin, DataEnd);
uint64_t CountersSize = CountersEnd - CountersBegin;



uint64_t PaddingBytesBeforeCounters, PaddingBytesAfterCounters,
PaddingBytesAfterNames;
__llvm_profile_get_padding_sizes_for_counters(
DataSize, CountersSize, NamesSize, &PaddingBytesBeforeCounters,
&PaddingBytesAfterCounters, &PaddingBytesAfterNames);

return sizeof(__llvm_profile_header) + __llvm_write_binary_ids(NULL) +
(DataSize * sizeof(__llvm_profile_data)) + PaddingBytesBeforeCounters +
(CountersSize * sizeof(uint64_t)) + PaddingBytesAfterCounters +
NamesSize + PaddingBytesAfterNames;
}

COMPILER_RT_VISIBILITY
void initBufferWriter(ProfDataWriter *BufferWriter, char *Buffer) {
BufferWriter->Write = lprofBufferWriter;
BufferWriter->WriterCtx = Buffer;
}

COMPILER_RT_VISIBILITY int __llvm_profile_write_buffer(char *Buffer) {
ProfDataWriter BufferWriter;
initBufferWriter(&BufferWriter, Buffer);
return lprofWriteData(&BufferWriter, 0, 0);
}

COMPILER_RT_VISIBILITY int __llvm_profile_write_buffer_internal(
char *Buffer, const __llvm_profile_data *DataBegin,
const __llvm_profile_data *DataEnd, const uint64_t *CountersBegin,
const uint64_t *CountersEnd, const char *NamesBegin, const char *NamesEnd) {
ProfDataWriter BufferWriter;
initBufferWriter(&BufferWriter, Buffer);
return lprofWriteDataImpl(&BufferWriter, DataBegin, DataEnd, CountersBegin,
CountersEnd, 0, NamesBegin, NamesEnd, 0);
}
