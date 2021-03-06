











#if !defined(HWASAN_CHECKS_H)
#define HWASAN_CHECKS_H

#include "hwasan_allocator.h"
#include "hwasan_mapping.h"
#include "sanitizer_common/sanitizer_common.h"

namespace __hwasan {
template <unsigned X>
__attribute__((always_inline)) static void SigTrap(uptr p) {
#if defined(__aarch64__)
(void)p;


register uptr x0 asm("x0") = p;
asm("brk %1\n\t" ::"r"(x0), "n"(0x900 + X));
#elif defined(__x86_64__)





asm volatile(
"int3\n"
"nopl %c0(%%rax)\n" ::"n"(0x40 + X),
"D"(p));
#else

__builtin_trap();
#endif

}


template <unsigned X>
__attribute__((always_inline)) static void SigTrap(uptr p, uptr size) {
#if defined(__aarch64__)
register uptr x0 asm("x0") = p;
register uptr x1 asm("x1") = size;
asm("brk %2\n\t" ::"r"(x0), "r"(x1), "n"(0x900 + X));
#elif defined(__x86_64__)

asm volatile(
"int3\n"
"nopl %c0(%%rax)\n" ::"n"(0x40 + X),
"D"(p), "S"(size));
#else
__builtin_trap();
#endif

}

__attribute__((always_inline, nodebug)) static bool PossiblyShortTagMatches(
tag_t mem_tag, uptr ptr, uptr sz) {
tag_t ptr_tag = GetTagFromPointer(ptr);
if (ptr_tag == mem_tag)
return true;
if (mem_tag >= kShadowAlignment)
return false;
if ((ptr & (kShadowAlignment - 1)) + sz > mem_tag)
return false;
#if !defined(__aarch64__)
ptr = UntagAddr(ptr);
#endif
return *(u8 *)(ptr | (kShadowAlignment - 1)) == ptr_tag;
}

enum class ErrorAction { Abort, Recover };
enum class AccessType { Load, Store };

template <ErrorAction EA, AccessType AT, unsigned LogSize>
__attribute__((always_inline, nodebug)) static void CheckAddress(uptr p) {
if (!InTaggableRegion(p))
return;
uptr ptr_raw = p & ~kAddressTagMask;
tag_t mem_tag = *(tag_t *)MemToShadow(ptr_raw);
if (UNLIKELY(!PossiblyShortTagMatches(mem_tag, p, 1 << LogSize))) {
SigTrap<0x20 * (EA == ErrorAction::Recover) +
0x10 * (AT == AccessType::Store) + LogSize>(p);
if (EA == ErrorAction::Abort)
__builtin_unreachable();
}
}

template <ErrorAction EA, AccessType AT>
__attribute__((always_inline, nodebug)) static void CheckAddressSized(uptr p,
uptr sz) {
if (sz == 0 || !InTaggableRegion(p))
return;
tag_t ptr_tag = GetTagFromPointer(p);
uptr ptr_raw = p & ~kAddressTagMask;
tag_t *shadow_first = (tag_t *)MemToShadow(ptr_raw);
tag_t *shadow_last = (tag_t *)MemToShadow(ptr_raw + sz);
for (tag_t *t = shadow_first; t < shadow_last; ++t)
if (UNLIKELY(ptr_tag != *t)) {
SigTrap<0x20 * (EA == ErrorAction::Recover) +
0x10 * (AT == AccessType::Store) + 0xf>(p, sz);
if (EA == ErrorAction::Abort)
__builtin_unreachable();
}
uptr end = p + sz;
uptr tail_sz = end & 0xf;
if (UNLIKELY(tail_sz != 0 &&
!PossiblyShortTagMatches(
*shadow_last, end & ~(kShadowAlignment - 1), tail_sz))) {
SigTrap<0x20 * (EA == ErrorAction::Recover) +
0x10 * (AT == AccessType::Store) + 0xf>(p, sz);
if (EA == ErrorAction::Abort)
__builtin_unreachable();
}
}

}

#endif
