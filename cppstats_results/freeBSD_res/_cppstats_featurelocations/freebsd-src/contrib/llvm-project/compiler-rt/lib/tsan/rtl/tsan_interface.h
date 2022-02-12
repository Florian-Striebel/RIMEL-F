













#if !defined(TSAN_INTERFACE_H)
#define TSAN_INTERFACE_H

#include <sanitizer_common/sanitizer_internal_defs.h>
using __sanitizer::uptr;
using __sanitizer::tid_t;




#if defined(__cplusplus)
extern "C" {
#endif

#if !SANITIZER_GO



SANITIZER_INTERFACE_ATTRIBUTE void __tsan_init();

SANITIZER_INTERFACE_ATTRIBUTE void __tsan_flush_memory();

SANITIZER_INTERFACE_ATTRIBUTE void __tsan_read1(void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_read2(void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_read4(void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_read8(void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_read16(void *addr);

SANITIZER_INTERFACE_ATTRIBUTE void __tsan_write1(void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_write2(void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_write4(void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_write8(void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_write16(void *addr);

SANITIZER_INTERFACE_ATTRIBUTE void __tsan_unaligned_read2(const void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_unaligned_read4(const void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_unaligned_read8(const void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_unaligned_read16(const void *addr);

SANITIZER_INTERFACE_ATTRIBUTE void __tsan_unaligned_write2(void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_unaligned_write4(void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_unaligned_write8(void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_unaligned_write16(void *addr);

SANITIZER_INTERFACE_ATTRIBUTE void __tsan_read1_pc(void *addr, void *pc);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_read2_pc(void *addr, void *pc);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_read4_pc(void *addr, void *pc);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_read8_pc(void *addr, void *pc);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_read16_pc(void *addr, void *pc);

SANITIZER_INTERFACE_ATTRIBUTE void __tsan_write1_pc(void *addr, void *pc);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_write2_pc(void *addr, void *pc);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_write4_pc(void *addr, void *pc);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_write8_pc(void *addr, void *pc);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_write16_pc(void *addr, void *pc);

SANITIZER_INTERFACE_ATTRIBUTE void __tsan_vptr_read(void **vptr_p);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_vptr_update(void **vptr_p, void *new_val);

SANITIZER_INTERFACE_ATTRIBUTE void __tsan_func_entry(void *call_pc);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_func_exit();

SANITIZER_INTERFACE_ATTRIBUTE void __tsan_ignore_thread_begin();
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_ignore_thread_end();

SANITIZER_INTERFACE_ATTRIBUTE
void *__tsan_external_register_tag(const char *object_type);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_external_register_header(void *tag, const char *header);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_external_assign_tag(void *addr, void *tag);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_external_read(void *addr, void *caller_pc, void *tag);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_external_write(void *addr, void *caller_pc, void *tag);

SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_read_range(void *addr, unsigned long size);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_write_range(void *addr, unsigned long size);

SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_read_range_pc(void *addr, unsigned long size, void *pc);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_write_range_pc(void *addr, unsigned long size, void *pc);




SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_on_report(void *report);



SANITIZER_INTERFACE_ATTRIBUTE
void *__tsan_get_current_report();





SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_get_report_data(void *report, const char **description, int *count,
int *stack_count, int *mop_count, int *loc_count,
int *mutex_count, int *thread_count,
int *unique_tid_count, void **sleep_trace,
uptr trace_size);











SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_get_report_tag(void *report, uptr *tag);


SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_get_report_stack(void *report, uptr idx, void **trace,
uptr trace_size);


SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_get_report_mop(void *report, uptr idx, int *tid, void **addr,
int *size, int *write, int *atomic, void **trace,
uptr trace_size);


SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_get_report_loc(void *report, uptr idx, const char **type,
void **addr, uptr *start, uptr *size, int *tid,
int *fd, int *suppressable, void **trace,
uptr trace_size);

SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_get_report_loc_object_type(void *report, uptr idx,
const char **object_type);


SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_get_report_mutex(void *report, uptr idx, uptr *mutex_id, void **addr,
int *destroyed, void **trace, uptr trace_size);


SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_get_report_thread(void *report, uptr idx, int *tid, tid_t *os_id,
int *running, const char **name, int *parent_tid,
void **trace, uptr trace_size);


SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_get_report_unique_tid(void *report, uptr idx, int *tid);



SANITIZER_INTERFACE_ATTRIBUTE
const char *__tsan_locate_address(uptr addr, char *name, uptr name_size,
uptr *region_address, uptr *region_size);


SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_get_alloc_stack(uptr addr, uptr *trace, uptr size, int *thread_id,
tid_t *os_id);

#endif

#if defined(__cplusplus)
}
#endif

namespace __tsan {


typedef unsigned char a8;
typedef unsigned short a16;
typedef unsigned int a32;
typedef unsigned long long a64;
#if !SANITIZER_GO && (defined(__SIZEOF_INT128__) || (__clang_major__ * 100 + __clang_minor__ >= 302)) && !defined(__mips64) && !defined(__s390x__)


__extension__ typedef __int128 a128;
#define __TSAN_HAS_INT128 1
#else
#define __TSAN_HAS_INT128 0
#endif



typedef enum {
mo_relaxed,
mo_consume,
mo_acquire,
mo_release,
mo_acq_rel,
mo_seq_cst
} morder;

struct ThreadState;

extern "C" {
SANITIZER_INTERFACE_ATTRIBUTE
a8 __tsan_atomic8_load(const volatile a8 *a, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a16 __tsan_atomic16_load(const volatile a16 *a, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a32 __tsan_atomic32_load(const volatile a32 *a, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a64 __tsan_atomic64_load(const volatile a64 *a, morder mo);
#if __TSAN_HAS_INT128
SANITIZER_INTERFACE_ATTRIBUTE
a128 __tsan_atomic128_load(const volatile a128 *a, morder mo);
#endif

SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_atomic8_store(volatile a8 *a, a8 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_atomic16_store(volatile a16 *a, a16 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_atomic32_store(volatile a32 *a, a32 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_atomic64_store(volatile a64 *a, a64 v, morder mo);
#if __TSAN_HAS_INT128
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_atomic128_store(volatile a128 *a, a128 v, morder mo);
#endif

SANITIZER_INTERFACE_ATTRIBUTE
a8 __tsan_atomic8_exchange(volatile a8 *a, a8 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a16 __tsan_atomic16_exchange(volatile a16 *a, a16 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a32 __tsan_atomic32_exchange(volatile a32 *a, a32 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a64 __tsan_atomic64_exchange(volatile a64 *a, a64 v, morder mo);
#if __TSAN_HAS_INT128
SANITIZER_INTERFACE_ATTRIBUTE
a128 __tsan_atomic128_exchange(volatile a128 *a, a128 v, morder mo);
#endif

SANITIZER_INTERFACE_ATTRIBUTE
a8 __tsan_atomic8_fetch_add(volatile a8 *a, a8 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a16 __tsan_atomic16_fetch_add(volatile a16 *a, a16 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a32 __tsan_atomic32_fetch_add(volatile a32 *a, a32 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a64 __tsan_atomic64_fetch_add(volatile a64 *a, a64 v, morder mo);
#if __TSAN_HAS_INT128
SANITIZER_INTERFACE_ATTRIBUTE
a128 __tsan_atomic128_fetch_add(volatile a128 *a, a128 v, morder mo);
#endif

SANITIZER_INTERFACE_ATTRIBUTE
a8 __tsan_atomic8_fetch_sub(volatile a8 *a, a8 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a16 __tsan_atomic16_fetch_sub(volatile a16 *a, a16 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a32 __tsan_atomic32_fetch_sub(volatile a32 *a, a32 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a64 __tsan_atomic64_fetch_sub(volatile a64 *a, a64 v, morder mo);
#if __TSAN_HAS_INT128
SANITIZER_INTERFACE_ATTRIBUTE
a128 __tsan_atomic128_fetch_sub(volatile a128 *a, a128 v, morder mo);
#endif

SANITIZER_INTERFACE_ATTRIBUTE
a8 __tsan_atomic8_fetch_and(volatile a8 *a, a8 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a16 __tsan_atomic16_fetch_and(volatile a16 *a, a16 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a32 __tsan_atomic32_fetch_and(volatile a32 *a, a32 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a64 __tsan_atomic64_fetch_and(volatile a64 *a, a64 v, morder mo);
#if __TSAN_HAS_INT128
SANITIZER_INTERFACE_ATTRIBUTE
a128 __tsan_atomic128_fetch_and(volatile a128 *a, a128 v, morder mo);
#endif

SANITIZER_INTERFACE_ATTRIBUTE
a8 __tsan_atomic8_fetch_or(volatile a8 *a, a8 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a16 __tsan_atomic16_fetch_or(volatile a16 *a, a16 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a32 __tsan_atomic32_fetch_or(volatile a32 *a, a32 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a64 __tsan_atomic64_fetch_or(volatile a64 *a, a64 v, morder mo);
#if __TSAN_HAS_INT128
SANITIZER_INTERFACE_ATTRIBUTE
a128 __tsan_atomic128_fetch_or(volatile a128 *a, a128 v, morder mo);
#endif

SANITIZER_INTERFACE_ATTRIBUTE
a8 __tsan_atomic8_fetch_xor(volatile a8 *a, a8 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a16 __tsan_atomic16_fetch_xor(volatile a16 *a, a16 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a32 __tsan_atomic32_fetch_xor(volatile a32 *a, a32 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a64 __tsan_atomic64_fetch_xor(volatile a64 *a, a64 v, morder mo);
#if __TSAN_HAS_INT128
SANITIZER_INTERFACE_ATTRIBUTE
a128 __tsan_atomic128_fetch_xor(volatile a128 *a, a128 v, morder mo);
#endif

SANITIZER_INTERFACE_ATTRIBUTE
a8 __tsan_atomic8_fetch_nand(volatile a8 *a, a8 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a16 __tsan_atomic16_fetch_nand(volatile a16 *a, a16 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a32 __tsan_atomic32_fetch_nand(volatile a32 *a, a32 v, morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
a64 __tsan_atomic64_fetch_nand(volatile a64 *a, a64 v, morder mo);
#if __TSAN_HAS_INT128
SANITIZER_INTERFACE_ATTRIBUTE
a128 __tsan_atomic128_fetch_nand(volatile a128 *a, a128 v, morder mo);
#endif

SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_atomic8_compare_exchange_strong(volatile a8 *a, a8 *c, a8 v,
morder mo, morder fmo);
SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_atomic16_compare_exchange_strong(volatile a16 *a, a16 *c, a16 v,
morder mo, morder fmo);
SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_atomic32_compare_exchange_strong(volatile a32 *a, a32 *c, a32 v,
morder mo, morder fmo);
SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_atomic64_compare_exchange_strong(volatile a64 *a, a64 *c, a64 v,
morder mo, morder fmo);
#if __TSAN_HAS_INT128
SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_atomic128_compare_exchange_strong(volatile a128 *a, a128 *c, a128 v,
morder mo, morder fmo);
#endif

SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_atomic8_compare_exchange_weak(volatile a8 *a, a8 *c, a8 v, morder mo,
morder fmo);
SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_atomic16_compare_exchange_weak(volatile a16 *a, a16 *c, a16 v,
morder mo, morder fmo);
SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_atomic32_compare_exchange_weak(volatile a32 *a, a32 *c, a32 v,
morder mo, morder fmo);
SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_atomic64_compare_exchange_weak(volatile a64 *a, a64 *c, a64 v,
morder mo, morder fmo);
#if __TSAN_HAS_INT128
SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_atomic128_compare_exchange_weak(volatile a128 *a, a128 *c, a128 v,
morder mo, morder fmo);
#endif

SANITIZER_INTERFACE_ATTRIBUTE
a8 __tsan_atomic8_compare_exchange_val(volatile a8 *a, a8 c, a8 v, morder mo,
morder fmo);
SANITIZER_INTERFACE_ATTRIBUTE
a16 __tsan_atomic16_compare_exchange_val(volatile a16 *a, a16 c, a16 v,
morder mo, morder fmo);
SANITIZER_INTERFACE_ATTRIBUTE
a32 __tsan_atomic32_compare_exchange_val(volatile a32 *a, a32 c, a32 v,
morder mo, morder fmo);
SANITIZER_INTERFACE_ATTRIBUTE
a64 __tsan_atomic64_compare_exchange_val(volatile a64 *a, a64 c, a64 v,
morder mo, morder fmo);
#if __TSAN_HAS_INT128
SANITIZER_INTERFACE_ATTRIBUTE
a128 __tsan_atomic128_compare_exchange_val(volatile a128 *a, a128 c, a128 v,
morder mo, morder fmo);
#endif

SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_atomic_thread_fence(morder mo);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_atomic_signal_fence(morder mo);

SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_go_atomic32_load(ThreadState *thr, uptr cpc, uptr pc, u8 *a);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_go_atomic64_load(ThreadState *thr, uptr cpc, uptr pc, u8 *a);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_go_atomic32_store(ThreadState *thr, uptr cpc, uptr pc, u8 *a);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_go_atomic64_store(ThreadState *thr, uptr cpc, uptr pc, u8 *a);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_go_atomic32_fetch_add(ThreadState *thr, uptr cpc, uptr pc, u8 *a);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_go_atomic64_fetch_add(ThreadState *thr, uptr cpc, uptr pc, u8 *a);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_go_atomic32_exchange(ThreadState *thr, uptr cpc, uptr pc, u8 *a);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_go_atomic64_exchange(ThreadState *thr, uptr cpc, uptr pc, u8 *a);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_go_atomic32_compare_exchange(ThreadState *thr, uptr cpc, uptr pc,
u8 *a);
SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_go_atomic64_compare_exchange(ThreadState *thr, uptr cpc, uptr pc,
u8 *a);

SANITIZER_INTERFACE_ATTRIBUTE
void __tsan_on_initialize();

SANITIZER_INTERFACE_ATTRIBUTE
int __tsan_on_finalize(int failed);

}

}

#endif
