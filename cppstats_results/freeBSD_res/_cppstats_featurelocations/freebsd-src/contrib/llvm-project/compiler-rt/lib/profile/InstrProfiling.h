







#if !defined(PROFILE_INSTRPROFILING_H_)
#define PROFILE_INSTRPROFILING_H_

#include "InstrProfilingPort.h"
#include <stdio.h>

#define INSTR_PROF_VISIBILITY COMPILER_RT_VISIBILITY
#include "profile/InstrProfData.inc"

enum ValueKind {
#define VALUE_PROF_KIND(Enumerator, Value, Descr) Enumerator = Value,
#include "profile/InstrProfData.inc"
};

typedef void *IntPtrT;
typedef struct COMPILER_RT_ALIGNAS(INSTR_PROF_DATA_ALIGNMENT)
__llvm_profile_data {
#define INSTR_PROF_DATA(Type, LLVMType, Name, Initializer) Type Name;
#include "profile/InstrProfData.inc"
} __llvm_profile_data;

typedef struct __llvm_profile_header {
#define INSTR_PROF_RAW_HEADER(Type, Name, Initializer) Type Name;
#include "profile/InstrProfData.inc"
} __llvm_profile_header;

typedef struct ValueProfNode * PtrToNodeT;
typedef struct ValueProfNode {
#define INSTR_PROF_VALUE_NODE(Type, LLVMType, Name, Initializer) Type Name;
#include "profile/InstrProfData.inc"
} ValueProfNode;






int __llvm_profile_is_continuous_mode_enabled(void);








void __llvm_profile_enable_continuous_mode(void);








void __llvm_profile_set_page_size(unsigned PageSize);





uint8_t __llvm_profile_get_num_padding_bytes(uint64_t SizeInBytes);




uint64_t __llvm_profile_get_size_for_buffer(void);







int __llvm_profile_write_buffer(char *Buffer);

const __llvm_profile_data *__llvm_profile_begin_data(void);
const __llvm_profile_data *__llvm_profile_end_data(void);
const char *__llvm_profile_begin_names(void);
const char *__llvm_profile_end_names(void);
uint64_t *__llvm_profile_begin_counters(void);
uint64_t *__llvm_profile_end_counters(void);
ValueProfNode *__llvm_profile_begin_vnodes();
ValueProfNode *__llvm_profile_end_vnodes();
uint32_t *__llvm_profile_begin_orderfile();





void __llvm_profile_reset_counters(void);










int __llvm_profile_merge_from_buffer(const char *Profile, uint64_t Size);








int __llvm_profile_check_compatibility(const char *Profile,
uint64_t Size);









void INSTR_PROF_VALUE_PROF_FUNC(
#define VALUE_PROF_FUNC_PARAM(ArgType, ArgName, ArgLLVMType) ArgType ArgName
#include "profile/InstrProfData.inc"
);

void __llvm_profile_instrument_target_value(uint64_t TargetValue, void *Data,
uint32_t CounterIndex,
uint64_t CounterValue);










int __llvm_profile_write_file(void);

int __llvm_orderfile_write_file(void);




















int __llvm_profile_dump(void);

int __llvm_orderfile_dump(void);




















void __llvm_profile_set_filename(const char *Name);



























void __llvm_profile_set_file_object(FILE *File, int EnableMerge);


int __llvm_profile_register_write_file_atexit(void);


void __llvm_profile_initialize_file(void);


void __llvm_profile_initialize(void);









const char *__llvm_profile_get_path_prefix();












const char *__llvm_profile_get_filename();


uint64_t __llvm_profile_get_magic(void);


uint64_t __llvm_profile_get_version(void);


uint64_t __llvm_profile_get_data_size(const __llvm_profile_data *Begin,
const __llvm_profile_data *End);














void __llvm_profile_get_padding_sizes_for_counters(
uint64_t DataSize, uint64_t CountersSize, uint64_t NamesSize,
uint64_t *PaddingBytesBeforeCounters, uint64_t *PaddingBytesAfterCounters,
uint64_t *PaddingBytesAfterNames);







void __llvm_profile_set_dumped();










COMPILER_RT_VISIBILITY extern int INSTR_PROF_PROFILE_RUNTIME_VAR;











extern uint64_t INSTR_PROF_RAW_VERSION_VAR;






extern char INSTR_PROF_PROFILE_NAME_VAR[1];

#endif
