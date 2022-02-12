























#if !defined(_BLOCK_PRIVATE_H_)
#define _BLOCK_PRIVATE_H_

#if !defined(BLOCK_EXPORT)
#if defined(__cplusplus)
#define BLOCK_EXPORT extern "C"
#else
#define BLOCK_EXPORT extern
#endif
#endif

#if !defined(_MSC_VER)
#include <stdbool.h>
#else

typedef char bool;
#define true (bool)1
#define false (bool)0
#endif

#if defined(__cplusplus)
extern "C" {
#endif


enum {
BLOCK_REFCOUNT_MASK = (0xffff),
BLOCK_NEEDS_FREE = (1 << 24),
BLOCK_HAS_COPY_DISPOSE = (1 << 25),
BLOCK_HAS_CTOR = (1 << 26),
BLOCK_IS_GC = (1 << 27),
BLOCK_IS_GLOBAL = (1 << 28),
BLOCK_HAS_DESCRIPTOR = (1 << 29)
};



struct Block_descriptor {
unsigned long int reserved;
unsigned long int size;
void (*copy)(void *dst, void *src);
void (*dispose)(void *);
};


struct Block_layout {
void *isa;
int flags;
int reserved;
void (*invoke)(void *, ...);
struct Block_descriptor *descriptor;

};


struct Block_byref {
void *isa;
struct Block_byref *forwarding;
int flags;
int size;
void (*byref_keep)(struct Block_byref *dst, struct Block_byref *src);
void (*byref_destroy)(struct Block_byref *);

};


struct Block_byref_header {
void *isa;
struct Block_byref *forwarding;
int flags;
int size;
};




enum {

BLOCK_FIELD_IS_OBJECT = 3,
BLOCK_FIELD_IS_BLOCK = 7,
BLOCK_FIELD_IS_BYREF = 8,
BLOCK_FIELD_IS_WEAK = 16,
BLOCK_BYREF_CALLER = 128
};


BLOCK_EXPORT void _Block_object_assign(void *destAddr, const void *object, const int flags);




BLOCK_EXPORT void _Block_object_dispose(const void *object, const int flags);






BLOCK_EXPORT unsigned long int Block_size(void *block_basic);





BLOCK_EXPORT void * _NSConcreteStackBlock[32];
BLOCK_EXPORT void * _NSConcreteMallocBlock[32];
BLOCK_EXPORT void * _NSConcreteAutoBlock[32];
BLOCK_EXPORT void * _NSConcreteFinalizingBlock[32];
BLOCK_EXPORT void * _NSConcreteGlobalBlock[32];
BLOCK_EXPORT void * _NSConcreteWeakBlockVariable[32];



BLOCK_EXPORT void _Block_use_GC( void *(*alloc)(const unsigned long, const bool isOne, const bool isObject),
void (*setHasRefcount)(const void *, const bool),
void (*gc_assign_strong)(void *, void **),
void (*gc_assign_weak)(const void *, void *),
void (*gc_memmove)(void *, void *, unsigned long));


BLOCK_EXPORT void _Block_use_GC5( void *(*alloc)(const unsigned long, const bool isOne, const bool isObject),
void (*setHasRefcount)(const void *, const bool),
void (*gc_assign_strong)(void *, void **),
void (*gc_assign_weak)(const void *, void *));

BLOCK_EXPORT void _Block_use_RR( void (*retain)(const void *),
void (*release)(const void *));


BLOCK_EXPORT void *_Block_copy_collectable(const void *aBlock);


BLOCK_EXPORT const char *_Block_dump(const void *block);





struct Block_basic {
void *isa;
int Block_flags;
int Block_size;
void (*Block_invoke)(void *);
void (*Block_copy)(void *dst, void *src);
void (*Block_dispose)(void *);

};


#if defined(__cplusplus)
}
#endif


#endif
