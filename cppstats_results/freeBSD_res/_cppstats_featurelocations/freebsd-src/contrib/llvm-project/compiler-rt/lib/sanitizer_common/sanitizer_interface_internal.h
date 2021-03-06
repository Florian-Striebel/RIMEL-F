















#if !defined(SANITIZER_INTERFACE_INTERNAL_H)
#define SANITIZER_INTERFACE_INTERNAL_H

#include "sanitizer_internal_defs.h"

extern "C" {


SANITIZER_INTERFACE_ATTRIBUTE
void __sanitizer_set_report_path(const char *path);


SANITIZER_INTERFACE_ATTRIBUTE
void __sanitizer_set_report_fd(void *fd);


SANITIZER_INTERFACE_ATTRIBUTE
const char *__sanitizer_get_report_path();

typedef struct {
int coverage_sandboxed;
__sanitizer::sptr coverage_fd;
unsigned int coverage_max_block_size;
} __sanitizer_sandbox_arguments;


SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE void
__sanitizer_sandbox_on_notify(__sanitizer_sandbox_arguments *args);




SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_report_error_summary(const char *error_summary);

SANITIZER_INTERFACE_ATTRIBUTE void __sanitizer_cov_dump();
SANITIZER_INTERFACE_ATTRIBUTE void __sanitizer_dump_coverage(
const __sanitizer::uptr *pcs, const __sanitizer::uptr len);
SANITIZER_INTERFACE_ATTRIBUTE void __sanitizer_dump_trace_pc_guard_coverage();

SANITIZER_INTERFACE_ATTRIBUTE void __sanitizer_cov(__sanitizer::u32 *guard);




SANITIZER_INTERFACE_ATTRIBUTE int __sanitizer_acquire_crash_state();

SANITIZER_INTERFACE_ATTRIBUTE
void __sanitizer_annotate_contiguous_container(const void *beg,
const void *end,
const void *old_mid,
const void *new_mid);
SANITIZER_INTERFACE_ATTRIBUTE
int __sanitizer_verify_contiguous_container(const void *beg, const void *mid,
const void *end);
SANITIZER_INTERFACE_ATTRIBUTE
const void *__sanitizer_contiguous_container_find_bad_address(
const void *beg, const void *mid, const void *end);

SANITIZER_INTERFACE_ATTRIBUTE
int __sanitizer_get_module_and_offset_for_pc(
__sanitizer::uptr pc, char *module_path,
__sanitizer::uptr module_path_len, __sanitizer::uptr *pc_offset);

SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_cmp();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_cmp1();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_cmp2();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_cmp4();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_cmp8();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_const_cmp1();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_const_cmp2();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_const_cmp4();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_const_cmp8();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_switch();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_div4();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_div8();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_gep();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_pc_indir();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_pc_guard(__sanitizer::u32*);
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_trace_pc_guard_init(__sanitizer::u32*,
__sanitizer::u32*);
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_cov_8bit_counters_init();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE void
__sanitizer_cov_bool_flag_init();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE void
__sanitizer_cov_pcs_init();
}

#endif
