











#if !defined(__UNWIND_H__)
#define __UNWIND_H__

#include <__libunwind_config.h>

#include <stdint.h>
#include <stddef.h>

#if defined(__SEH__) && !defined(__USING_SJLJ_EXCEPTIONS__) && defined(_WIN32)
#include <windows.h>
#include <ntverp.h>
#endif

#if defined(__APPLE__)
#define LIBUNWIND_UNAVAIL __attribute__ (( unavailable ))
#else
#define LIBUNWIND_UNAVAIL
#endif

typedef enum {
_URC_NO_REASON = 0,
_URC_OK = 0,
_URC_FOREIGN_EXCEPTION_CAUGHT = 1,
_URC_FATAL_PHASE2_ERROR = 2,
_URC_FATAL_PHASE1_ERROR = 3,
_URC_NORMAL_STOP = 4,
_URC_END_OF_STACK = 5,
_URC_HANDLER_FOUND = 6,
_URC_INSTALL_CONTEXT = 7,
_URC_CONTINUE_UNWIND = 8,
#if defined(_LIBUNWIND_ARM_EHABI)
_URC_FAILURE = 9
#endif
} _Unwind_Reason_Code;

typedef enum {
_UA_SEARCH_PHASE = 1,
_UA_CLEANUP_PHASE = 2,
_UA_HANDLER_FRAME = 4,
_UA_FORCE_UNWIND = 8,
_UA_END_OF_STACK = 16
} _Unwind_Action;

typedef struct _Unwind_Context _Unwind_Context;

#if defined(_LIBUNWIND_ARM_EHABI)
typedef uint32_t _Unwind_State;

static const _Unwind_State _US_VIRTUAL_UNWIND_FRAME = 0;
static const _Unwind_State _US_UNWIND_FRAME_STARTING = 1;
static const _Unwind_State _US_UNWIND_FRAME_RESUME = 2;
static const _Unwind_State _US_ACTION_MASK = 3;

static const _Unwind_State _US_FORCE_UNWIND = 8;

typedef uint32_t _Unwind_EHT_Header;





struct _Unwind_Exception;
typedef struct _Unwind_Exception _Unwind_Exception;
typedef struct _Unwind_Exception _Unwind_Control_Block;

struct _Unwind_Exception {
uint64_t exception_class;
void (*exception_cleanup)(_Unwind_Reason_Code, _Unwind_Control_Block*);


struct {
uint32_t reserved1;
uint32_t reserved2;
uint32_t reserved3;
uint32_t reserved4;
uint32_t reserved5;
} unwinder_cache;


struct {
uint32_t sp;
uint32_t bitpattern[5];
} barrier_cache;


struct {
uint32_t bitpattern[4];
} cleanup_cache;


struct {
uint32_t fnstart;
_Unwind_EHT_Header* ehtp;
uint32_t additional;
uint32_t reserved1;
} pr_cache;

long long int :0;
} __attribute__((__aligned__(8)));

typedef _Unwind_Reason_Code (*_Unwind_Stop_Fn)
(_Unwind_State state,
_Unwind_Exception* exceptionObject,
struct _Unwind_Context* context);

typedef _Unwind_Reason_Code (*_Unwind_Personality_Fn)(
_Unwind_State state, _Unwind_Exception *exceptionObject,
struct _Unwind_Context *context);
#else
struct _Unwind_Context;
struct _Unwind_Exception;
typedef struct _Unwind_Exception _Unwind_Exception;

struct _Unwind_Exception {
uint64_t exception_class;
void (*exception_cleanup)(_Unwind_Reason_Code reason,
_Unwind_Exception *exc);
#if defined(__SEH__) && !defined(__USING_SJLJ_EXCEPTIONS__)
uintptr_t private_[6];
#else
uintptr_t private_1;
uintptr_t private_2;
#endif
#if __SIZEOF_POINTER__ == 4




uint32_t reserved[3];
#endif



} __attribute__((__aligned__));

typedef _Unwind_Reason_Code (*_Unwind_Stop_Fn)
(int version,
_Unwind_Action actions,
uint64_t exceptionClass,
_Unwind_Exception* exceptionObject,
struct _Unwind_Context* context,
void* stop_parameter );

typedef _Unwind_Reason_Code (*_Unwind_Personality_Fn)(
int version, _Unwind_Action actions, uint64_t exceptionClass,
_Unwind_Exception *exceptionObject, struct _Unwind_Context *context);
#endif

#if defined(__cplusplus)
extern "C" {
#endif




#if defined(__USING_SJLJ_EXCEPTIONS__)
extern _Unwind_Reason_Code
_Unwind_SjLj_RaiseException(_Unwind_Exception *exception_object);
extern void _Unwind_SjLj_Resume(_Unwind_Exception *exception_object);
#else
extern _Unwind_Reason_Code
_Unwind_RaiseException(_Unwind_Exception *exception_object);
extern void _Unwind_Resume(_Unwind_Exception *exception_object);
#endif
extern void _Unwind_DeleteException(_Unwind_Exception *exception_object);

#if defined(_LIBUNWIND_ARM_EHABI)
typedef enum {
_UVRSC_CORE = 0,
_UVRSC_VFP = 1,
_UVRSC_WMMXD = 3,
_UVRSC_WMMXC = 4
} _Unwind_VRS_RegClass;

typedef enum {
_UVRSD_UINT32 = 0,
_UVRSD_VFPX = 1,
_UVRSD_UINT64 = 3,
_UVRSD_FLOAT = 4,
_UVRSD_DOUBLE = 5
} _Unwind_VRS_DataRepresentation;

typedef enum {
_UVRSR_OK = 0,
_UVRSR_NOT_IMPLEMENTED = 1,
_UVRSR_FAILED = 2
} _Unwind_VRS_Result;

extern void _Unwind_Complete(_Unwind_Exception* exception_object);

extern _Unwind_VRS_Result
_Unwind_VRS_Get(_Unwind_Context *context, _Unwind_VRS_RegClass regclass,
uint32_t regno, _Unwind_VRS_DataRepresentation representation,
void *valuep);

extern _Unwind_VRS_Result
_Unwind_VRS_Set(_Unwind_Context *context, _Unwind_VRS_RegClass regclass,
uint32_t regno, _Unwind_VRS_DataRepresentation representation,
void *valuep);

extern _Unwind_VRS_Result
_Unwind_VRS_Pop(_Unwind_Context *context, _Unwind_VRS_RegClass regclass,
uint32_t discriminator,
_Unwind_VRS_DataRepresentation representation);
#endif

#if !defined(_LIBUNWIND_ARM_EHABI)

extern uintptr_t _Unwind_GetGR(struct _Unwind_Context *context, int index);
extern void _Unwind_SetGR(struct _Unwind_Context *context, int index,
uintptr_t new_value);
extern uintptr_t _Unwind_GetIP(struct _Unwind_Context *context);
extern void _Unwind_SetIP(struct _Unwind_Context *, uintptr_t new_value);

#else

#if defined(_LIBUNWIND_UNWIND_LEVEL1_EXTERNAL_LINKAGE)
#define _LIBUNWIND_EXPORT_UNWIND_LEVEL1 extern
#else
#define _LIBUNWIND_EXPORT_UNWIND_LEVEL1 static __inline__
#endif








_LIBUNWIND_EXPORT_UNWIND_LEVEL1
uintptr_t _Unwind_GetGR(struct _Unwind_Context *context, int index) {
uintptr_t value = 0;
_Unwind_VRS_Get(context, _UVRSC_CORE, (uint32_t)index, _UVRSD_UINT32, &value);
return value;
}

_LIBUNWIND_EXPORT_UNWIND_LEVEL1
void _Unwind_SetGR(struct _Unwind_Context *context, int index,
uintptr_t value) {
_Unwind_VRS_Set(context, _UVRSC_CORE, (uint32_t)index, _UVRSD_UINT32, &value);
}

_LIBUNWIND_EXPORT_UNWIND_LEVEL1
uintptr_t _Unwind_GetIP(struct _Unwind_Context *context) {

return _Unwind_GetGR(context, 15) & (~(uintptr_t)0x1);
}

_LIBUNWIND_EXPORT_UNWIND_LEVEL1
void _Unwind_SetIP(struct _Unwind_Context *context, uintptr_t value) {
uintptr_t thumb_bit = _Unwind_GetGR(context, 15) & ((uintptr_t)0x1);
_Unwind_SetGR(context, 15, value | thumb_bit);
}
#endif

extern uintptr_t _Unwind_GetRegionStart(struct _Unwind_Context *context);
extern uintptr_t
_Unwind_GetLanguageSpecificData(struct _Unwind_Context *context);
#if defined(__USING_SJLJ_EXCEPTIONS__)
extern _Unwind_Reason_Code
_Unwind_SjLj_ForcedUnwind(_Unwind_Exception *exception_object,
_Unwind_Stop_Fn stop, void *stop_parameter);
#else
extern _Unwind_Reason_Code
_Unwind_ForcedUnwind(_Unwind_Exception *exception_object,
_Unwind_Stop_Fn stop, void *stop_parameter);
#endif

#if defined(__USING_SJLJ_EXCEPTIONS__)
typedef struct _Unwind_FunctionContext *_Unwind_FunctionContext_t;
extern void _Unwind_SjLj_Register(_Unwind_FunctionContext_t fc);
extern void _Unwind_SjLj_Unregister(_Unwind_FunctionContext_t fc);
#endif








#if defined(__USING_SJLJ_EXCEPTIONS__)
extern _Unwind_Reason_Code
_Unwind_SjLj_Resume_or_Rethrow(_Unwind_Exception *exception_object);
#else
extern _Unwind_Reason_Code
_Unwind_Resume_or_Rethrow(_Unwind_Exception *exception_object);
#endif




typedef _Unwind_Reason_Code (*_Unwind_Trace_Fn)(struct _Unwind_Context *,
void *);
extern _Unwind_Reason_Code _Unwind_Backtrace(_Unwind_Trace_Fn, void *);




extern uintptr_t _Unwind_GetCFA(struct _Unwind_Context *);








extern uintptr_t _Unwind_GetIPInfo(struct _Unwind_Context *context,
int *ipBefore);








extern void __register_frame(const void *fde);
extern void __deregister_frame(const void *fde);






struct dwarf_eh_bases {
uintptr_t tbase;
uintptr_t dbase;
uintptr_t func;
};
extern const void *_Unwind_Find_FDE(const void *pc, struct dwarf_eh_bases *);







extern void *_Unwind_FindEnclosingFunction(void *pc);



extern uintptr_t _Unwind_GetDataRelBase(struct _Unwind_Context *context)
LIBUNWIND_UNAVAIL;
extern uintptr_t _Unwind_GetTextRelBase(struct _Unwind_Context *context)
LIBUNWIND_UNAVAIL;




extern void __register_frame_info_bases(const void *fde, void *ob, void *tb,
void *db) LIBUNWIND_UNAVAIL;
extern void __register_frame_info(const void *fde, void *ob)
LIBUNWIND_UNAVAIL;
extern void __register_frame_info_table_bases(const void *fde, void *ob,
void *tb, void *db)
LIBUNWIND_UNAVAIL;
extern void __register_frame_info_table(const void *fde, void *ob)
LIBUNWIND_UNAVAIL;
extern void __register_frame_table(const void *fde)
LIBUNWIND_UNAVAIL;
extern void *__deregister_frame_info(const void *fde)
LIBUNWIND_UNAVAIL;
extern void *__deregister_frame_info_bases(const void *fde)
LIBUNWIND_UNAVAIL;

#if defined(__SEH__) && !defined(__USING_SJLJ_EXCEPTIONS__)
#if !defined(_WIN32)
typedef struct _EXCEPTION_RECORD EXCEPTION_RECORD;
typedef struct _CONTEXT CONTEXT;
typedef struct _DISPATCHER_CONTEXT DISPATCHER_CONTEXT;
#elif !defined(__MINGW32__) && VER_PRODUCTBUILD < 8000
typedef struct _DISPATCHER_CONTEXT DISPATCHER_CONTEXT;
#endif

extern EXCEPTION_DISPOSITION _GCC_specific_handler(EXCEPTION_RECORD *exc,
void *frame, CONTEXT *ctx,
DISPATCHER_CONTEXT *disp,
_Unwind_Personality_Fn pers);
#endif

#if defined(__cplusplus)
}
#endif

#endif
