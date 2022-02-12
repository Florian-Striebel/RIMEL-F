







#if !defined(SANITIZER_PTRAUTH_H)
#define SANITIZER_PTRAUTH_H

#if __has_feature(ptrauth_calls)
#include <ptrauth.h>
#elif defined(__ARM_FEATURE_PAC_DEFAULT) && !defined(__APPLE__)
inline unsigned long ptrauth_strip(void* __value, unsigned int __key) {




unsigned long ret;
asm volatile(
"mov x30, %1\n\t"
"hint #7\n\t"
"mov %0, x30\n\t"
: "=r"(ret)
: "r"(__value)
: "x30");
return ret;
}
#define ptrauth_auth_data(__value, __old_key, __old_data) __value
#define ptrauth_string_discriminator(__string) ((int)0)
#else

#define ptrauth_strip(__value, __key) __value
#define ptrauth_auth_data(__value, __old_key, __old_data) __value
#define ptrauth_string_discriminator(__string) ((int)0)
#endif

#define STRIP_PAC_PC(pc) ((uptr)ptrauth_strip(pc, 0))

#endif
