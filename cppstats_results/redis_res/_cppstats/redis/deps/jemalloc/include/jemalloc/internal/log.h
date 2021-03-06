#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/malloc_io.h"
#include "jemalloc/internal/mutex.h"
#if defined(JEMALLOC_LOG)
#define JEMALLOC_LOG_VAR_BUFSIZE 1000
#else
#define JEMALLOC_LOG_VAR_BUFSIZE 1
#endif
#define JEMALLOC_LOG_BUFSIZE 4096
extern char log_var_names[JEMALLOC_LOG_VAR_BUFSIZE];
extern atomic_b_t log_init_done;
typedef struct log_var_s log_var_t;
struct log_var_s {
atomic_u_t state;
const char *name;
};
#define LOG_NOT_INITIALIZED 0U
#define LOG_INITIALIZED_NOT_ENABLED 1U
#define LOG_ENABLED 2U
#define LOG_VAR_INIT(name_str) {ATOMIC_INIT(LOG_NOT_INITIALIZED), name_str}
unsigned log_var_update_state(log_var_t *log_var);
#define log_do_begin(log_var) if (config_log) { unsigned log_state = atomic_load_u(&(log_var).state, ATOMIC_RELAXED); if (unlikely(log_state == LOG_NOT_INITIALIZED)) { log_state = log_var_update_state(&(log_var)); assert(log_state != LOG_NOT_INITIALIZED); } if (log_state == LOG_ENABLED) { {
#define log_do_end(log_var) } } }
static inline void
log_impl_varargs(const char *name, ...) {
char buf[JEMALLOC_LOG_BUFSIZE];
va_list ap;
va_start(ap, name);
const char *format = va_arg(ap, const char *);
size_t dst_offset = 0;
dst_offset += malloc_snprintf(buf, JEMALLOC_LOG_BUFSIZE, "%s: ", name);
dst_offset += malloc_vsnprintf(buf + dst_offset,
JEMALLOC_LOG_BUFSIZE - dst_offset, format, ap);
dst_offset += malloc_snprintf(buf + dst_offset,
JEMALLOC_LOG_BUFSIZE - dst_offset, "\n");
va_end(ap);
malloc_write(buf);
}
#define LOG(log_var_str, ...) do { static log_var_t log_var = LOG_VAR_INIT(log_var_str); log_do_begin(log_var) log_impl_varargs((log_var).name, __VA_ARGS__); log_do_end(log_var) } while (0)
