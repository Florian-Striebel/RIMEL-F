







#include "int_lib.h"
#include <stddef.h>

#include <unwind.h>















#if defined(__arm__) && !defined(__ARM_DWARF_EH__) && !defined(__USING_SJLJ_EXCEPTIONS__) && !defined(__FreeBSD__)







#include "unwind-ehabi-helpers.h"
#endif

#if defined(__SEH__) && !defined(__USING_SJLJ_EXCEPTIONS__)
#include <windows.h>
#include <winnt.h>

EXCEPTION_DISPOSITION _GCC_specific_handler(PEXCEPTION_RECORD, void *, PCONTEXT,
PDISPATCHER_CONTEXT,
_Unwind_Personality_Fn);
#endif




#define DW_EH_PE_omit 0xff

#define DW_EH_PE_absptr 0x00
#define DW_EH_PE_uleb128 0x01
#define DW_EH_PE_udata2 0x02
#define DW_EH_PE_udata4 0x03
#define DW_EH_PE_udata8 0x04
#define DW_EH_PE_sleb128 0x09
#define DW_EH_PE_sdata2 0x0A
#define DW_EH_PE_sdata4 0x0B
#define DW_EH_PE_sdata8 0x0C

#define DW_EH_PE_pcrel 0x10
#define DW_EH_PE_textrel 0x20
#define DW_EH_PE_datarel 0x30
#define DW_EH_PE_funcrel 0x40
#define DW_EH_PE_aligned 0x50
#define DW_EH_PE_indirect 0x80


static size_t readULEB128(const uint8_t **data) {
size_t result = 0;
size_t shift = 0;
unsigned char byte;
const uint8_t *p = *data;
do {
byte = *p++;
result |= (byte & 0x7f) << shift;
shift += 7;
} while (byte & 0x80);
*data = p;
return result;
}


static uintptr_t readEncodedPointer(const uint8_t **data, uint8_t encoding) {
const uint8_t *p = *data;
uintptr_t result = 0;

if (encoding == DW_EH_PE_omit)
return 0;


switch (encoding & 0x0F) {
case DW_EH_PE_absptr:
result = *((const uintptr_t *)p);
p += sizeof(uintptr_t);
break;
case DW_EH_PE_uleb128:
result = readULEB128(&p);
break;
case DW_EH_PE_udata2:
result = *((const uint16_t *)p);
p += sizeof(uint16_t);
break;
case DW_EH_PE_udata4:
result = *((const uint32_t *)p);
p += sizeof(uint32_t);
break;
case DW_EH_PE_udata8:
result = *((const uint64_t *)p);
p += sizeof(uint64_t);
break;
case DW_EH_PE_sdata2:
result = *((const int16_t *)p);
p += sizeof(int16_t);
break;
case DW_EH_PE_sdata4:
result = *((const int32_t *)p);
p += sizeof(int32_t);
break;
case DW_EH_PE_sdata8:
result = *((const int64_t *)p);
p += sizeof(int64_t);
break;
case DW_EH_PE_sleb128:
default:

compilerrt_abort();
break;
}


switch (encoding & 0x70) {
case DW_EH_PE_absptr:

break;
case DW_EH_PE_pcrel:
result += (uintptr_t)(*data);
break;
case DW_EH_PE_textrel:
case DW_EH_PE_datarel:
case DW_EH_PE_funcrel:
case DW_EH_PE_aligned:
default:

compilerrt_abort();
break;
}


if (encoding & DW_EH_PE_indirect) {
result = *((const uintptr_t *)result);
}

*data = p;
return result;
}

#if defined(__arm__) && !defined(__USING_SJLJ_EXCEPTIONS__) && !defined(__ARM_DWARF_EH__)

#define USING_ARM_EHABI 1
_Unwind_Reason_Code __gnu_unwind_frame(struct _Unwind_Exception *,
struct _Unwind_Context *);
#endif

static inline _Unwind_Reason_Code
continueUnwind(struct _Unwind_Exception *exceptionObject,
struct _Unwind_Context *context) {
#if USING_ARM_EHABI


if (__gnu_unwind_frame(exceptionObject, context) != _URC_OK)
return _URC_FAILURE;
#endif
return _URC_CONTINUE_UNWIND;
}







#if __USING_SJLJ_EXCEPTIONS__


COMPILER_RT_ABI _Unwind_Reason_Code __gcc_personality_sj0(
int version, _Unwind_Action actions, uint64_t exceptionClass,
struct _Unwind_Exception *exceptionObject, struct _Unwind_Context *context)
#elif USING_ARM_EHABI

COMPILER_RT_ABI _Unwind_Reason_Code __gcc_personality_v0(
_Unwind_State state, struct _Unwind_Exception *exceptionObject,
struct _Unwind_Context *context)
#elif defined(__SEH__)
static _Unwind_Reason_Code __gcc_personality_imp(
int version, _Unwind_Action actions, uint64_t exceptionClass,
struct _Unwind_Exception *exceptionObject, struct _Unwind_Context *context)
#else
COMPILER_RT_ABI _Unwind_Reason_Code __gcc_personality_v0(
int version, _Unwind_Action actions, uint64_t exceptionClass,
struct _Unwind_Exception *exceptionObject, struct _Unwind_Context *context)
#endif
{


#if USING_ARM_EHABI


if ((state & _US_ACTION_MASK) != _US_UNWIND_FRAME_STARTING)
#else
if (actions & _UA_SEARCH_PHASE)
#endif
return continueUnwind(exceptionObject, context);


const uint8_t *lsda = (uint8_t *)_Unwind_GetLanguageSpecificData(context);
if (lsda == (uint8_t *)0)
return continueUnwind(exceptionObject, context);

uintptr_t pc = (uintptr_t)_Unwind_GetIP(context) - 1;
uintptr_t funcStart = (uintptr_t)_Unwind_GetRegionStart(context);
uintptr_t pcOffset = pc - funcStart;


uint8_t lpStartEncoding = *lsda++;
if (lpStartEncoding != DW_EH_PE_omit) {
readEncodedPointer(&lsda, lpStartEncoding);
}
uint8_t ttypeEncoding = *lsda++;
if (ttypeEncoding != DW_EH_PE_omit) {
readULEB128(&lsda);
}

uint8_t callSiteEncoding = *lsda++;
uint32_t callSiteTableLength = readULEB128(&lsda);
const uint8_t *callSiteTableStart = lsda;
const uint8_t *callSiteTableEnd = callSiteTableStart + callSiteTableLength;
const uint8_t *p = callSiteTableStart;
while (p < callSiteTableEnd) {
uintptr_t start = readEncodedPointer(&p, callSiteEncoding);
size_t length = readEncodedPointer(&p, callSiteEncoding);
size_t landingPad = readEncodedPointer(&p, callSiteEncoding);
readULEB128(&p);
if (landingPad == 0)
continue;
if ((start <= pcOffset) && (pcOffset < (start + length))) {




_Unwind_SetGR(context, __builtin_eh_return_data_regno(0),
(uintptr_t)exceptionObject);
_Unwind_SetGR(context, __builtin_eh_return_data_regno(1), 0);
_Unwind_SetIP(context, (funcStart + landingPad));
return _URC_INSTALL_CONTEXT;
}
}


return continueUnwind(exceptionObject, context);
}

#if defined(__SEH__) && !defined(__USING_SJLJ_EXCEPTIONS__)
COMPILER_RT_ABI EXCEPTION_DISPOSITION
__gcc_personality_seh0(PEXCEPTION_RECORD ms_exc, void *this_frame,
PCONTEXT ms_orig_context, PDISPATCHER_CONTEXT ms_disp) {
return _GCC_specific_handler(ms_exc, this_frame, ms_orig_context, ms_disp,
__gcc_personality_imp);
}
#endif
