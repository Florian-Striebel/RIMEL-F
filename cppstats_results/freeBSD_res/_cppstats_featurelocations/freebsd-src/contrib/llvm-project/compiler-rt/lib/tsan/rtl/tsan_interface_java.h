





















#if !defined(TSAN_INTERFACE_JAVA_H)
#define TSAN_INTERFACE_JAVA_H

#if !defined(INTERFACE_ATTRIBUTE)
#define INTERFACE_ATTRIBUTE __attribute__((visibility("default")))
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef unsigned long jptr;


void __tsan_java_init(jptr heap_begin, jptr heap_size) INTERFACE_ATTRIBUTE;



int __tsan_java_fini() INTERFACE_ATTRIBUTE;




void __tsan_java_alloc(jptr ptr, jptr size) INTERFACE_ATTRIBUTE;


void __tsan_java_free(jptr ptr, jptr size) INTERFACE_ATTRIBUTE;



void __tsan_java_move(jptr src, jptr dst, jptr size) INTERFACE_ATTRIBUTE;




void __tsan_java_finalize() INTERFACE_ATTRIBUTE;



jptr __tsan_java_find(jptr *from_ptr, jptr to) INTERFACE_ATTRIBUTE;




void __tsan_java_mutex_lock(jptr addr) INTERFACE_ATTRIBUTE;

void __tsan_java_mutex_unlock(jptr addr) INTERFACE_ATTRIBUTE;

void __tsan_java_mutex_read_lock(jptr addr) INTERFACE_ATTRIBUTE;

void __tsan_java_mutex_read_unlock(jptr addr) INTERFACE_ATTRIBUTE;



void __tsan_java_mutex_lock_rec(jptr addr, int rec) INTERFACE_ATTRIBUTE;





int __tsan_java_mutex_unlock_rec(jptr addr) INTERFACE_ATTRIBUTE;





void __tsan_java_acquire(jptr addr) INTERFACE_ATTRIBUTE;
void __tsan_java_release(jptr addr) INTERFACE_ATTRIBUTE;
void __tsan_java_release_store(jptr addr) INTERFACE_ATTRIBUTE;

#if defined(__cplusplus)
}
#endif

#undef INTERFACE_ATTRIBUTE

#endif
