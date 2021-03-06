#include "test/jemalloc_test.h"
static unsigned test_count = 0;
static test_status_t test_counts[test_status_count] = {0, 0, 0};
static test_status_t test_status = test_status_pass;
static const char * test_name = "";
#define NUM_REENTRANT_ALLOCS 20
typedef enum {
non_reentrant = 0,
libc_reentrant = 1,
arena_new_reentrant = 2
} reentrancy_t;
static reentrancy_t reentrancy;
static bool libc_hook_ran = false;
static bool arena_new_hook_ran = false;
static const char *
reentrancy_t_str(reentrancy_t r) {
switch (r) {
case non_reentrant:
return "non-reentrant";
case libc_reentrant:
return "libc-reentrant";
case arena_new_reentrant:
return "arena_new-reentrant";
default:
unreachable();
}
}
static void
do_hook(bool *hook_ran, void (**hook)()) {
*hook_ran = true;
*hook = NULL;
size_t alloc_size = 1;
for (int i = 0; i < NUM_REENTRANT_ALLOCS; i++) {
free(malloc(alloc_size));
alloc_size *= 2;
}
}
static void
libc_reentrancy_hook() {
do_hook(&libc_hook_ran, &test_hooks_libc_hook);
}
static void
arena_new_reentrancy_hook() {
do_hook(&arena_new_hook_ran, &test_hooks_arena_new_hook);
}
bool
test_is_reentrant() {
return reentrancy != non_reentrant;
}
JEMALLOC_FORMAT_PRINTF(1, 2)
void
test_skip(const char *format, ...) {
va_list ap;
va_start(ap, format);
malloc_vcprintf(NULL, NULL, format, ap);
va_end(ap);
malloc_printf("\n");
test_status = test_status_skip;
}
JEMALLOC_FORMAT_PRINTF(1, 2)
void
test_fail(const char *format, ...) {
va_list ap;
va_start(ap, format);
malloc_vcprintf(NULL, NULL, format, ap);
va_end(ap);
malloc_printf("\n");
test_status = test_status_fail;
}
static const char *
test_status_string(test_status_t test_status) {
switch (test_status) {
case test_status_pass: return "pass";
case test_status_skip: return "skip";
case test_status_fail: return "fail";
default: not_reached();
}
}
void
p_test_init(const char *name) {
test_count++;
test_status = test_status_pass;
test_name = name;
}
void
p_test_fini(void) {
test_counts[test_status]++;
malloc_printf("%s (%s): %s\n", test_name, reentrancy_t_str(reentrancy),
test_status_string(test_status));
}
static void
check_global_slow(test_status_t *status) {
#if defined(JEMALLOC_UNIT_TEST)
if (tsd_global_slow()) {
malloc_printf("Testing increased global slow count\n");
*status = test_status_fail;
}
#endif
}
static test_status_t
p_test_impl(bool do_malloc_init, bool do_reentrant, test_t *t, va_list ap) {
test_status_t ret;
if (do_malloc_init) {
if (nallocx(1, 0) == 0) {
malloc_printf("Initialization error");
return test_status_fail;
}
}
ret = test_status_pass;
for (; t != NULL; t = va_arg(ap, test_t *)) {
reentrancy = non_reentrant;
test_hooks_arena_new_hook = test_hooks_libc_hook = NULL;
t();
if (test_status > ret) {
ret = test_status;
}
check_global_slow(&ret);
if (do_reentrant) {
reentrancy = libc_reentrant;
test_hooks_arena_new_hook = NULL;
test_hooks_libc_hook = &libc_reentrancy_hook;
t();
if (test_status > ret) {
ret = test_status;
}
check_global_slow(&ret);
reentrancy = arena_new_reentrant;
test_hooks_libc_hook = NULL;
test_hooks_arena_new_hook = &arena_new_reentrancy_hook;
t();
if (test_status > ret) {
ret = test_status;
}
check_global_slow(&ret);
}
}
malloc_printf("--- %s: %u/%u, %s: %u/%u, %s: %u/%u ---\n",
test_status_string(test_status_pass),
test_counts[test_status_pass], test_count,
test_status_string(test_status_skip),
test_counts[test_status_skip], test_count,
test_status_string(test_status_fail),
test_counts[test_status_fail], test_count);
return ret;
}
test_status_t
p_test(test_t *t, ...) {
test_status_t ret;
va_list ap;
ret = test_status_pass;
va_start(ap, t);
ret = p_test_impl(true, true, t, ap);
va_end(ap);
return ret;
}
test_status_t
p_test_no_reentrancy(test_t *t, ...) {
test_status_t ret;
va_list ap;
ret = test_status_pass;
va_start(ap, t);
ret = p_test_impl(true, false, t, ap);
va_end(ap);
return ret;
}
test_status_t
p_test_no_malloc_init(test_t *t, ...) {
test_status_t ret;
va_list ap;
ret = test_status_pass;
va_start(ap, t);
ret = p_test_impl(false, false, t, ap);
va_end(ap);
return ret;
}
void
p_test_fail(const char *prefix, const char *message) {
malloc_cprintf(NULL, NULL, "%s%s\n", prefix, message);
test_status = test_status_fail;
}
