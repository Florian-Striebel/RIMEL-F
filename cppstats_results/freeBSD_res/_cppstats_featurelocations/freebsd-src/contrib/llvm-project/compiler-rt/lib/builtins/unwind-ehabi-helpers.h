







#if !defined(UNWIND_EHABI_HELPERS_H)
#define UNWIND_EHABI_HELPERS_H

#include <stdint.h>

#include <unwind.h>

#if !defined(__ARM_EABI_UNWINDER__)



















#define _URC_OK 0
#define _URC_FAILURE 9

typedef uint32_t _Unwind_State;

#if !defined(_US_UNWIND_FRAME_STARTING)
#define _US_UNWIND_FRAME_STARTING ((_Unwind_State)1)
#endif

#if !defined(_US_ACTION_MASK)
#define _US_ACTION_MASK ((_Unwind_State)3)
#endif

#endif

#endif
