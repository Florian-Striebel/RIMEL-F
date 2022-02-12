



















#if defined(__Fuchsia__)

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#include <zircon/process.h>
#include <zircon/sanitizer.h>
#include <zircon/status.h>
#include <zircon/syscalls.h>

#include "InstrProfiling.h"
#include "InstrProfilingInternal.h"
#include "InstrProfilingUtil.h"


COMPILER_RT_VISIBILITY extern intptr_t INSTR_PROF_PROFILE_COUNTER_BIAS_VAR;

COMPILER_RT_VISIBILITY unsigned lprofProfileDumped() {
return 1;
}
COMPILER_RT_VISIBILITY void lprofSetProfileDumped(unsigned Value) {}

static const char ProfileSinkName[] = "llvm-profile";

static inline void lprofWrite(const char *fmt, ...) {
char s[256];

va_list ap;
va_start(ap, fmt);
int ret = vsnprintf(s, sizeof(s), fmt, ap);
va_end(ap);

__sanitizer_log_write(s, ret + 1);
}

struct lprofVMOWriterCtx {

zx_handle_t Vmo;

uint64_t Offset;
};

static uint32_t lprofVMOWriter(ProfDataWriter *This, ProfDataIOVec *IOVecs,
uint32_t NumIOVecs) {
struct lprofVMOWriterCtx *Ctx = (struct lprofVMOWriterCtx *)This->WriterCtx;


size_t Length = 0;
for (uint32_t I = 0; I < NumIOVecs; I++)
Length += IOVecs[I].ElmSize * IOVecs[I].NumElm;


zx_status_t Status = _zx_vmo_set_size(Ctx->Vmo, Ctx->Offset + Length);
if (Status != ZX_OK)
return -1;


for (uint32_t I = 0; I < NumIOVecs; I++) {
size_t Length = IOVecs[I].ElmSize * IOVecs[I].NumElm;
if (IOVecs[I].Data) {
Status = _zx_vmo_write(Ctx->Vmo, IOVecs[I].Data, Ctx->Offset, Length);
if (Status != ZX_OK)
return -1;
} else if (IOVecs[I].UseZeroPadding) {

}
Ctx->Offset += Length;
}


_zx_object_set_property(Ctx->Vmo, ZX_PROP_VMO_CONTENT_SIZE, &Ctx->Offset,
sizeof(Ctx->Offset));

return 0;
}

static void initVMOWriter(ProfDataWriter *This, struct lprofVMOWriterCtx *Ctx) {
This->Write = lprofVMOWriter;
This->WriterCtx = Ctx;
}



COMPILER_RT_VISIBILITY
void __llvm_profile_initialize(void) {

if (GET_VERSION(__llvm_profile_get_version()) != INSTR_PROF_RAW_VERSION) {
lprofWrite("LLVM Profile: runtime and instrumentation version mismatch: "
"expected %d, but got %d\n",
INSTR_PROF_RAW_VERSION,
(int)GET_VERSION(__llvm_profile_get_version()));
return;
}

const __llvm_profile_data *DataBegin = __llvm_profile_begin_data();
const __llvm_profile_data *DataEnd = __llvm_profile_end_data();
const uint64_t *CountersBegin = __llvm_profile_begin_counters();
const uint64_t *CountersEnd = __llvm_profile_end_counters();
const uint64_t DataSize = __llvm_profile_get_data_size(DataBegin, DataEnd);
const uint64_t CountersOffset = sizeof(__llvm_profile_header) +
__llvm_write_binary_ids(NULL) +
(DataSize * sizeof(__llvm_profile_data));
uint64_t CountersSize = CountersEnd - CountersBegin;


if (!CountersSize)
return;

zx_status_t Status;


zx_handle_t Vmo = ZX_HANDLE_INVALID;
Status = _zx_vmo_create(0, ZX_VMO_RESIZABLE, &Vmo);
if (Status != ZX_OK) {
lprofWrite("LLVM Profile: cannot create VMO: %s\n",
_zx_status_get_string(Status));
return;
}


char VmoName[ZX_MAX_NAME_LEN];
snprintf(VmoName, sizeof(VmoName), "%" PRIu64 ".profraw",
lprofGetLoadModuleSignature());
_zx_object_set_property(Vmo, ZX_PROP_NAME, VmoName, strlen(VmoName));


ProfDataWriter VMOWriter;
struct lprofVMOWriterCtx Ctx = {.Vmo = Vmo, .Offset = 0};
initVMOWriter(&VMOWriter, &Ctx);
if (lprofWriteData(&VMOWriter, 0, 0) != 0) {
lprofWrite("LLVM Profile: failed to write data\n");
_zx_handle_close(Vmo);
return;
}

uint64_t Len = 0;
Status = _zx_vmo_get_size(Vmo, &Len);
if (Status != ZX_OK) {
lprofWrite("LLVM Profile: failed to get the VMO size: %s\n",
_zx_status_get_string(Status));
_zx_handle_close(Vmo);
return;
}

uintptr_t Mapping;
Status =
_zx_vmar_map(_zx_vmar_root_self(), ZX_VM_PERM_READ | ZX_VM_PERM_WRITE, 0,
Vmo, 0, Len, &Mapping);
if (Status != ZX_OK) {
lprofWrite("LLVM Profile: failed to map the VMO: %s\n",
_zx_status_get_string(Status));
_zx_handle_close(Vmo);
return;
}



__sanitizer_publish_data(ProfileSinkName, Vmo);


lprofWrite("LLVM Profile: {{{dumpfile:%s:%s}}}\n", ProfileSinkName, VmoName);


INSTR_PROF_PROFILE_COUNTER_BIAS_VAR =
(intptr_t)Mapping - (uintptr_t)CountersBegin + CountersOffset;


lprofReleaseMemoryPagesToOS((uintptr_t)CountersBegin, (uintptr_t)CountersEnd);
}

#endif
