











#if !defined(SANITIZER_TSAN_INTERFACE_H)
#define SANITIZER_TSAN_INTERFACE_H

#include <sanitizer/common_interface_defs.h>

#if defined(__cplusplus)
extern "C" {
#endif



void __tsan_acquire(void *addr);
void __tsan_release(void *addr);















static const unsigned __tsan_mutex_linker_init = 1 << 0;

static const unsigned __tsan_mutex_write_reentrant = 1 << 1;

static const unsigned __tsan_mutex_read_reentrant = 1 << 2;




static const unsigned __tsan_mutex_not_static = 1 << 8;




static const unsigned __tsan_mutex_read_lock = 1 << 3;

static const unsigned __tsan_mutex_try_lock = 1 << 4;

static const unsigned __tsan_mutex_try_lock_failed = 1 << 5;




static const unsigned __tsan_mutex_recursive_lock = 1 << 6;



static const unsigned __tsan_mutex_recursive_unlock = 1 << 7;


static const unsigned __tsan_mutex_try_read_lock =
__tsan_mutex_read_lock | __tsan_mutex_try_lock;
static const unsigned __tsan_mutex_try_read_lock_failed =
__tsan_mutex_try_read_lock | __tsan_mutex_try_lock_failed;



void __tsan_mutex_create(void *addr, unsigned flags);





void __tsan_mutex_destroy(void *addr, unsigned flags);






void __tsan_mutex_pre_lock(void *addr, unsigned flags);








void __tsan_mutex_post_lock(void *addr, unsigned flags, int recursion);





int __tsan_mutex_pre_unlock(void *addr, unsigned flags);




void __tsan_mutex_post_unlock(void *addr, unsigned flags);



void __tsan_mutex_pre_signal(void *addr, unsigned flags);
void __tsan_mutex_post_signal(void *addr, unsigned flags);









void __tsan_mutex_pre_divert(void *addr, unsigned flags);
void __tsan_mutex_post_divert(void *addr, unsigned flags);











void *__tsan_external_register_tag(const char *object_type);
void __tsan_external_register_header(void *tag, const char *header);
void __tsan_external_assign_tag(void *addr, void *tag);
void __tsan_external_read(void *addr, void *caller_pc, void *tag);
void __tsan_external_write(void *addr, void *caller_pc, void *tag);









void *__tsan_get_current_fiber(void);
void *__tsan_create_fiber(unsigned flags);
void __tsan_destroy_fiber(void *fiber);
void __tsan_switch_to_fiber(void *fiber, unsigned flags);
void __tsan_set_fiber_name(void *fiber, const char *name);



static const unsigned __tsan_switch_to_fiber_no_sync = 1 << 0;


void __tsan_on_initialize();





int __tsan_on_finalize(int failed);

#if defined(__cplusplus)
}
#endif

#endif
