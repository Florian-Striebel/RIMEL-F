






















#include <stdint.h>





typedef enum
{
_URC_NO_REASON = 0,
_URC_OK = 0,
_URC_FOREIGN_EXCEPTION_CAUGHT = 1,
_URC_END_OF_STACK = 5,
_URC_HANDLER_FOUND = 6,
_URC_INSTALL_CONTEXT = 7,
_URC_CONTINUE_UNWIND = 8,
_URC_FAILURE = 9,
_URC_FATAL_PHASE1_ERROR = _URC_FAILURE
} _Unwind_Reason_Code;

typedef int _Unwind_Action;

typedef uint32_t _Unwind_State;
#if defined(__clang__)
static const _Unwind_State _US_VIRTUAL_UNWIND_FRAME = 0;
static const _Unwind_State _US_UNWIND_FRAME_STARTING = 1;
static const _Unwind_State _US_UNWIND_FRAME_RESUME = 2;
static const _Unwind_State _US_ACTION_MASK = 3;
#else
#define _US_VIRTUAL_UNWIND_FRAME 0
#define _US_UNWIND_FRAME_STARTING 1
#define _US_UNWIND_FRAME_RESUME 2
#define _US_ACTION_MASK 3
#endif

typedef struct _Unwind_Context _Unwind_Context;

typedef uint32_t _Unwind_EHT_Header;

struct _Unwind_Exception
{
uint64_t exception_class;
void (*exception_cleanup)(_Unwind_Reason_Code, struct _Unwind_Exception *);

struct
{
uint32_t reserved1;
uint32_t reserved2;
uint32_t reserved3;
uint32_t reserved4;
uint32_t reserved5;

} unwinder_cache;

struct
{
uint32_t sp;
uint32_t bitpattern[5];
} barrier_cache;

struct
{
uint32_t bitpattern[4];
} cleanup_cache;

struct
{

uint32_t fnstart;

_Unwind_EHT_Header *ehtp;

uint32_t additional;
uint32_t reserved1;
} pr_cache;

long long int :0;
};


_Unwind_Reason_Code _Unwind_RaiseException(struct _Unwind_Exception *ucbp);
void _Unwind_Resume(struct _Unwind_Exception *ucbp);
void _Unwind_Complete(struct _Unwind_Exception *ucbp);
void _Unwind_DeleteException(struct _Unwind_Exception *ucbp);
void *_Unwind_GetLanguageSpecificData(struct _Unwind_Context*);

typedef enum
{
_UVRSR_OK = 0,
_UVRSR_NOT_IMPLEMENTED = 1,
_UVRSR_FAILED = 2
} _Unwind_VRS_Result;
typedef enum
{
_UVRSC_CORE = 0,
_UVRSC_VFP = 1,
_UVRSC_WMMXD = 3,
_UVRSC_WMMXC = 4
} _Unwind_VRS_RegClass;
typedef enum
{
_UVRSD_UINT32 = 0,
_UVRSD_VFPX = 1,
_UVRSD_UINT64 = 3,
_UVRSD_FLOAT = 4,
_UVRSD_DOUBLE = 5
} _Unwind_VRS_DataRepresentation;

_Unwind_VRS_Result _Unwind_VRS_Get(_Unwind_Context *context,
_Unwind_VRS_RegClass regclass,
uint32_t regno,
_Unwind_VRS_DataRepresentation representation,
void *valuep);
_Unwind_VRS_Result _Unwind_VRS_Set(_Unwind_Context *context,
_Unwind_VRS_RegClass regclass,
uint32_t regno,
_Unwind_VRS_DataRepresentation representation,
void *valuep);


extern unsigned long _Unwind_GetDataRelBase(struct _Unwind_Context *);


extern unsigned long _Unwind_GetTextRelBase(struct _Unwind_Context *);
extern unsigned long _Unwind_GetRegionStart(struct _Unwind_Context *);

typedef _Unwind_Reason_Code (*_Unwind_Trace_Fn) (struct _Unwind_Context *,
void *);
extern _Unwind_Reason_Code _Unwind_Backtrace (_Unwind_Trace_Fn, void *);
extern _Unwind_Reason_Code
_Unwind_Resume_or_Rethrow (struct _Unwind_Exception *);






#define _UA_SEARCH_PHASE 1
#define _UA_CLEANUP_PHASE 2
#define _UA_HANDLER_FRAME 4
#define _UA_FORCE_UNWIND 8

static inline unsigned long _Unwind_GetGR(struct _Unwind_Context *context, int reg)
{
unsigned long val;
_Unwind_VRS_Get(context, _UVRSC_CORE, reg, _UVRSD_UINT32, &val);
return val;
}
static inline void _Unwind_SetGR(struct _Unwind_Context *context, int reg, unsigned long val)
{
_Unwind_VRS_Set(context, _UVRSC_CORE, reg, _UVRSD_UINT32, &val);
}
static inline unsigned long _Unwind_GetIP(_Unwind_Context *context)
{

return _Unwind_GetGR(context, 15) & ~1;
}
static inline void _Unwind_SetIP(_Unwind_Context *context, unsigned long val)
{



unsigned long thumbState = _Unwind_GetGR(context, 15) & 1;
_Unwind_SetGR(context, 15, (val | thumbState));
}


_Unwind_Reason_Code __gnu_unwind_frame(struct _Unwind_Exception*, struct _Unwind_Context*);


#define DECLARE_PERSONALITY_FUNCTION(name) _Unwind_Reason_Code name(_Unwind_State state,struct _Unwind_Exception *exceptionObject,struct _Unwind_Context *context);




#define BEGIN_PERSONALITY_FUNCTION(name) _Unwind_Reason_Code name(_Unwind_State state,struct _Unwind_Exception *exceptionObject,struct _Unwind_Context *context){int version = 1;uint64_t exceptionClass = exceptionObject->exception_class;int actions;switch (state){default: return _URC_FAILURE;case _US_VIRTUAL_UNWIND_FRAME:{actions = _UA_SEARCH_PHASE;break;}case _US_UNWIND_FRAME_STARTING:{actions = _UA_CLEANUP_PHASE;if (exceptionObject->barrier_cache.sp == _Unwind_GetGR(context, 13)){actions |= _UA_HANDLER_FRAME;}break;}case _US_UNWIND_FRAME_RESUME:{return continueUnwinding(exceptionObject, context);break;}}_Unwind_SetGR (context, 12, reinterpret_cast<unsigned long>(exceptionObject));
































#define CALL_PERSONALITY_FUNCTION(name) name(state,exceptionObject,context)
