







#include "InstrProfiling.h"
#include "InstrProfilingInternal.h"

#if defined(_WIN32)

#if defined(_MSC_VER)

#pragma comment(linker, "/MERGE:.lprfc=.data")
#pragma comment(linker, "/MERGE:.lprfd=.data")
#pragma comment(linker, "/MERGE:.lprfv=.data")
#pragma comment(linker, "/MERGE:.lprfnd=.data")





#pragma section(".lprfn$A", read)
#pragma section(".lprfn$Z", read)


#pragma section(".lprfd$A", read, write)
#pragma section(".lprfd$Z", read, write)
#pragma section(".lprfc$A", read, write)
#pragma section(".lprfc$Z", read, write)
#pragma section(".lorderfile$A", read, write)
#pragma section(".lprfnd$A", read, write)
#pragma section(".lprfnd$Z", read, write)
#endif

__llvm_profile_data COMPILER_RT_SECTION(".lprfd$A") DataStart = {0};
__llvm_profile_data COMPILER_RT_SECTION(".lprfd$Z") DataEnd = {0};

const char COMPILER_RT_SECTION(".lprfn$A") NamesStart = '\0';
const char COMPILER_RT_SECTION(".lprfn$Z") NamesEnd = '\0';

uint64_t COMPILER_RT_SECTION(".lprfc$A") CountersStart;
uint64_t COMPILER_RT_SECTION(".lprfc$Z") CountersEnd;
uint32_t COMPILER_RT_SECTION(".lorderfile$A") OrderFileStart;

ValueProfNode COMPILER_RT_SECTION(".lprfnd$A") VNodesStart;
ValueProfNode COMPILER_RT_SECTION(".lprfnd$Z") VNodesEnd;

const __llvm_profile_data *__llvm_profile_begin_data(void) {
return &DataStart + 1;
}
const __llvm_profile_data *__llvm_profile_end_data(void) { return &DataEnd; }

const char *__llvm_profile_begin_names(void) { return &NamesStart + 1; }
const char *__llvm_profile_end_names(void) { return &NamesEnd; }

uint64_t *__llvm_profile_begin_counters(void) { return &CountersStart + 1; }
uint64_t *__llvm_profile_end_counters(void) { return &CountersEnd; }
uint32_t *__llvm_profile_begin_orderfile(void) { return &OrderFileStart; }

ValueProfNode *__llvm_profile_begin_vnodes(void) { return &VNodesStart + 1; }
ValueProfNode *__llvm_profile_end_vnodes(void) { return &VNodesEnd; }

ValueProfNode *CurrentVNode = &VNodesStart + 1;
ValueProfNode *EndVNode = &VNodesEnd;

COMPILER_RT_VISIBILITY int __llvm_write_binary_ids(ProfDataWriter *Writer) {
return 0;
}

#endif
