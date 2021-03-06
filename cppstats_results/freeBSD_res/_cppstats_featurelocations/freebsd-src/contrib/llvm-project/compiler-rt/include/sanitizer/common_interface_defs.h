










#if !defined(SANITIZER_COMMON_INTERFACE_DEFS_H)
#define SANITIZER_COMMON_INTERFACE_DEFS_H

#include <stddef.h>
#include <stdint.h>


#if !defined(__has_feature)
#define __has_feature(x) 0
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {

int coverage_sandboxed;



intptr_t coverage_fd;




unsigned int coverage_max_block_size;
} __sanitizer_sandbox_arguments;


void __sanitizer_set_report_path(const char *path);


void __sanitizer_set_report_fd(void *fd);


const char *__sanitizer_get_report_path();




void __sanitizer_sandbox_on_notify(__sanitizer_sandbox_arguments *args);




void __sanitizer_report_error_summary(const char *error_summary);










uint16_t __sanitizer_unaligned_load16(const void *p);






uint32_t __sanitizer_unaligned_load32(const void *p);






uint64_t __sanitizer_unaligned_load64(const void *p);





void __sanitizer_unaligned_store16(void *p, uint16_t x);





void __sanitizer_unaligned_store32(void *p, uint32_t x);





void __sanitizer_unaligned_store64(void *p, uint64_t x);




int __sanitizer_acquire_crash_state();


















































void __sanitizer_annotate_contiguous_container(const void *beg,
const void *end,
const void *old_mid,
const void *new_mid);

















int __sanitizer_verify_contiguous_container(const void *beg, const void *mid,
const void *end);











const void *__sanitizer_contiguous_container_find_bad_address(const void *beg,
const void *mid,
const void *end);



void __sanitizer_print_stack_trace(void);











void __sanitizer_symbolize_pc(void *pc, const char *fmt, char *out_buf,
size_t out_buf_size);

void __sanitizer_symbolize_global(void *data_ptr, const char *fmt,
char *out_buf, size_t out_buf_size);






void __sanitizer_set_death_callback(void (*callback)(void));
















void __sanitizer_weak_hook_memcmp(void *called_pc, const void *s1,
const void *s2, size_t n, int result);








void __sanitizer_weak_hook_strncmp(void *called_pc, const char *s1,
const char *s2, size_t n, int result);








void __sanitizer_weak_hook_strncasecmp(void *called_pc, const char *s1,
const char *s2, size_t n, int result);







void __sanitizer_weak_hook_strcmp(void *called_pc, const char *s1,
const char *s2, int result);







void __sanitizer_weak_hook_strcasecmp(void *called_pc, const char *s1,
const char *s2, int result);







void __sanitizer_weak_hook_strstr(void *called_pc, const char *s1,
const char *s2, char *result);

void __sanitizer_weak_hook_strcasestr(void *called_pc, const char *s1,
const char *s2, char *result);

void __sanitizer_weak_hook_memmem(void *called_pc,
const void *s1, size_t len1,
const void *s2, size_t len2, void *result);






void __sanitizer_print_memory_profile(size_t top_percent,
size_t max_number_of_contexts);




























void __sanitizer_start_switch_fiber(void **fake_stack_save,
const void *bottom, size_t size);












void __sanitizer_finish_switch_fiber(void *fake_stack_save,
const void **bottom_old,
size_t *size_old);



int __sanitizer_get_module_and_offset_for_pc(void *pc, char *module_path,
size_t module_path_len,
void **pc_offset);

#if defined(__cplusplus)
}
#endif

#endif
