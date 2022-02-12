











#if !defined(DFSAN_INTERFACE_H)
#define DFSAN_INTERFACE_H

#include <stddef.h>
#include <stdint.h>
#include <sanitizer/common_interface_defs.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef uint8_t dfsan_label;
typedef uint32_t dfsan_origin;


typedef void (*dfsan_write_callback_t)(int fd, const void *buf, size_t count);


dfsan_label dfsan_union(dfsan_label l1, dfsan_label l2);


void dfsan_set_label(dfsan_label label, void *addr, size_t size);



void dfsan_add_label(dfsan_label label, void *addr, size_t size);







dfsan_label dfsan_get_label(long data);





dfsan_origin dfsan_get_origin(long data);


dfsan_label dfsan_read_label(const void *addr, size_t size);


int dfsan_has_label(dfsan_label label, dfsan_label elem);







void dfsan_flush(void);




void dfsan_set_write_callback(dfsan_write_callback_t labeled_write_callback);







void dfsan_weak_hook_memcmp(void *caller_pc, const void *s1, const void *s2,
size_t n, dfsan_label s1_label,
dfsan_label s2_label, dfsan_label n_label);
void dfsan_weak_hook_strncmp(void *caller_pc, const char *s1, const char *s2,
size_t n, dfsan_label s1_label,
dfsan_label s2_label, dfsan_label n_label);




void dfsan_print_origin_trace(const void *addr, const char *description);



































size_t dfsan_sprint_origin_trace(const void *addr, const char *description,
char *out_buf, size_t out_buf_size);












size_t dfsan_sprint_stack_trace(char *out_buf, size_t out_buf_size);



dfsan_origin dfsan_get_init_origin(const void *addr);





int dfsan_get_track_origins(void);
#if defined(__cplusplus)
}

template <typename T>
void dfsan_set_label(dfsan_label label, T &data) {
dfsan_set_label(label, (void *)&data, sizeof(T));
}

#endif

#endif
