











#if !defined(MSAN_POISONING_H)
#define MSAN_POISONING_H

#include "msan.h"

namespace __msan {


u32 GetOriginIfPoisoned(uptr addr, uptr size);




void SetOriginIfPoisoned(uptr addr, uptr src_shadow, uptr size, u32 src_origin);




void CopyOrigin(const void *dst, const void *src, uptr size, StackTrace *stack);



void MoveShadowAndOrigin(const void *dst, const void *src, uptr size,
StackTrace *stack);



void CopyShadowAndOrigin(const void *dst, const void *src, uptr size,
StackTrace *stack);



void CopyMemory(void *dst, const void *src, uptr size, StackTrace *stack);


void SetShadow(const void *ptr, uptr size, u8 value);


void SetOrigin(const void *dst, uptr size, u32 origin);


void PoisonMemory(const void *dst, uptr size, StackTrace *stack);

}

#endif
