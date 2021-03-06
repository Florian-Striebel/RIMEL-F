







#if defined(__linux__) || defined(__FreeBSD__) || defined(__Fuchsia__) || (defined(__sun__) && defined(__svr4__)) || defined(__NetBSD__)


#include <elf.h>
#include <link.h>
#include <stdlib.h>
#include <string.h>

#include "InstrProfiling.h"
#include "InstrProfilingInternal.h"

#if defined(__FreeBSD__) && !defined(ElfW)





#define ElfW(type) __ElfN(type)
#endif

#define PROF_DATA_START INSTR_PROF_SECT_START(INSTR_PROF_DATA_COMMON)
#define PROF_DATA_STOP INSTR_PROF_SECT_STOP(INSTR_PROF_DATA_COMMON)
#define PROF_NAME_START INSTR_PROF_SECT_START(INSTR_PROF_NAME_COMMON)
#define PROF_NAME_STOP INSTR_PROF_SECT_STOP(INSTR_PROF_NAME_COMMON)
#define PROF_CNTS_START INSTR_PROF_SECT_START(INSTR_PROF_CNTS_COMMON)
#define PROF_CNTS_STOP INSTR_PROF_SECT_STOP(INSTR_PROF_CNTS_COMMON)
#define PROF_ORDERFILE_START INSTR_PROF_SECT_START(INSTR_PROF_ORDERFILE_COMMON)
#define PROF_VNODES_START INSTR_PROF_SECT_START(INSTR_PROF_VNODES_COMMON)
#define PROF_VNODES_STOP INSTR_PROF_SECT_STOP(INSTR_PROF_VNODES_COMMON)




extern __llvm_profile_data PROF_DATA_START COMPILER_RT_VISIBILITY
COMPILER_RT_WEAK;
extern __llvm_profile_data PROF_DATA_STOP COMPILER_RT_VISIBILITY
COMPILER_RT_WEAK;
extern uint64_t PROF_CNTS_START COMPILER_RT_VISIBILITY COMPILER_RT_WEAK;
extern uint64_t PROF_CNTS_STOP COMPILER_RT_VISIBILITY COMPILER_RT_WEAK;
extern uint32_t PROF_ORDERFILE_START COMPILER_RT_VISIBILITY COMPILER_RT_WEAK;
extern char PROF_NAME_START COMPILER_RT_VISIBILITY COMPILER_RT_WEAK;
extern char PROF_NAME_STOP COMPILER_RT_VISIBILITY COMPILER_RT_WEAK;
extern ValueProfNode PROF_VNODES_START COMPILER_RT_VISIBILITY COMPILER_RT_WEAK;
extern ValueProfNode PROF_VNODES_STOP COMPILER_RT_VISIBILITY COMPILER_RT_WEAK;

COMPILER_RT_VISIBILITY const __llvm_profile_data *
__llvm_profile_begin_data(void) {
return &PROF_DATA_START;
}
COMPILER_RT_VISIBILITY const __llvm_profile_data *
__llvm_profile_end_data(void) {
return &PROF_DATA_STOP;
}
COMPILER_RT_VISIBILITY const char *__llvm_profile_begin_names(void) {
return &PROF_NAME_START;
}
COMPILER_RT_VISIBILITY const char *__llvm_profile_end_names(void) {
return &PROF_NAME_STOP;
}
COMPILER_RT_VISIBILITY uint64_t *__llvm_profile_begin_counters(void) {
return &PROF_CNTS_START;
}
COMPILER_RT_VISIBILITY uint64_t *__llvm_profile_end_counters(void) {
return &PROF_CNTS_STOP;
}
COMPILER_RT_VISIBILITY uint32_t *__llvm_profile_begin_orderfile(void) {
return &PROF_ORDERFILE_START;
}

COMPILER_RT_VISIBILITY ValueProfNode *
__llvm_profile_begin_vnodes(void) {
return &PROF_VNODES_START;
}
COMPILER_RT_VISIBILITY ValueProfNode *__llvm_profile_end_vnodes(void) {
return &PROF_VNODES_STOP;
}
COMPILER_RT_VISIBILITY ValueProfNode *CurrentVNode = &PROF_VNODES_START;
COMPILER_RT_VISIBILITY ValueProfNode *EndVNode = &PROF_VNODES_STOP;

#if defined(NT_GNU_BUILD_ID)
static size_t RoundUp(size_t size, size_t align) {
return (size + align - 1) & ~(align - 1);
}





static int WriteOneBinaryId(ProfDataWriter *Writer, uint64_t BinaryIdLen,
const uint8_t *BinaryIdData) {
ProfDataIOVec BinaryIdIOVec[] = {
{&BinaryIdLen, sizeof(uint64_t), 1, 0},
{BinaryIdData, sizeof(uint8_t), BinaryIdLen, 0}};
if (Writer->Write(Writer, BinaryIdIOVec,
sizeof(BinaryIdIOVec) / sizeof(*BinaryIdIOVec)))
return -1;


return 0;
}













static int WriteBinaryIdForNote(ProfDataWriter *Writer,
const ElfW(Nhdr) * Note) {
int BinaryIdSize = 0;

const char *NoteName = (const char *)Note + sizeof(ElfW(Nhdr));
if (Note->n_type == NT_GNU_BUILD_ID && Note->n_namesz == 4 &&
memcmp(NoteName, "GNU\0", 4) == 0) {

uint64_t BinaryIdLen = Note->n_descsz;
const uint8_t *BinaryIdData =
(const uint8_t *)(NoteName + RoundUp(Note->n_namesz, 4));
if (Writer != NULL &&
WriteOneBinaryId(Writer, BinaryIdLen, BinaryIdData) == -1)
return -1;

BinaryIdSize = sizeof(BinaryIdLen) + BinaryIdLen;
}

return BinaryIdSize;
}






static int WriteBinaryIds(ProfDataWriter *Writer, const ElfW(Nhdr) * Note,
const ElfW(Nhdr) * NotesEnd) {
int TotalBinaryIdsSize = 0;
while (Note < NotesEnd) {
int Result = WriteBinaryIdForNote(Writer, Note);
if (Result == -1)
return -1;
TotalBinaryIdsSize += Result;


size_t NoteOffset = sizeof(ElfW(Nhdr)) + RoundUp(Note->n_namesz, 4) +
RoundUp(Note->n_descsz, 4);
Note = (const ElfW(Nhdr) *)((const char *)(Note) + NoteOffset);
}

return TotalBinaryIdsSize;
}






COMPILER_RT_VISIBILITY int __llvm_write_binary_ids(ProfDataWriter *Writer) {
extern const ElfW(Ehdr) __ehdr_start __attribute__((visibility("hidden")));
const ElfW(Ehdr) *ElfHeader = &__ehdr_start;
const ElfW(Phdr) *ProgramHeader =
(const ElfW(Phdr) *)((uintptr_t)ElfHeader + ElfHeader->e_phoff);

uint32_t I;

for (I = 0; I < ElfHeader->e_phnum; I++) {

if (ProgramHeader[I].p_type != PT_NOTE)
continue;

const ElfW(Nhdr) *Note =
(const ElfW(Nhdr) *)((uintptr_t)ElfHeader + ProgramHeader[I].p_offset);
const ElfW(Nhdr) *NotesEnd =
(const ElfW(Nhdr) *)((const char *)(Note) + ProgramHeader[I].p_filesz);
return WriteBinaryIds(Writer, Note, NotesEnd);
}

return 0;
}
#else




COMPILER_RT_VISIBILITY int __llvm_write_binary_ids(ProfDataWriter *Writer) {
return 0;
}
#endif

#endif
