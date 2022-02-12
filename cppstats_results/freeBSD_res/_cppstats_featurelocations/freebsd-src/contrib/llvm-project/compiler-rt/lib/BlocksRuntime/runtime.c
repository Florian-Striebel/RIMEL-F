























#include "Block_private.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "config.h"

#if defined(HAVE_AVAILABILITY_MACROS_H)
#include <AvailabilityMacros.h>
#endif

#if defined(HAVE_TARGET_CONDITIONALS_H)
#include <TargetConditionals.h>
#endif

#if defined(HAVE_OSATOMIC_COMPARE_AND_SWAP_INT) && defined(HAVE_OSATOMIC_COMPARE_AND_SWAP_LONG)

#if defined(HAVE_LIBKERN_OSATOMIC_H)
#include <libkern/OSAtomic.h>
#endif

#elif defined(__WIN32__) || defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>

static __inline bool OSAtomicCompareAndSwapLong(long oldl, long newl, long volatile *dst) {

long original = InterlockedCompareExchange(dst, newl, oldl);
return (original == oldl);
}

static __inline bool OSAtomicCompareAndSwapInt(int oldi, int newi, int volatile *dst) {

int original = InterlockedCompareExchange(dst, newi, oldi);
return (original == oldi);
}








#elif defined(HAVE_SYNC_BOOL_COMPARE_AND_SWAP_INT) && defined(HAVE_SYNC_BOOL_COMPARE_AND_SWAP_LONG)

static __inline bool OSAtomicCompareAndSwapLong(long oldl, long newl, long volatile *dst) {
return __sync_bool_compare_and_swap(dst, oldl, newl);
}

static __inline bool OSAtomicCompareAndSwapInt(int oldi, int newi, int volatile *dst) {
return __sync_bool_compare_and_swap(dst, oldi, newi);
}

#else
#error unknown atomic compare-and-swap primitive
#endif






static void *_Block_copy_class = _NSConcreteMallocBlock;
static void *_Block_copy_finalizing_class = _NSConcreteMallocBlock;
static int _Block_copy_flag = BLOCK_NEEDS_FREE;
static int _Byref_flag_initial_value = BLOCK_NEEDS_FREE | 2;

static const int WANTS_ONE = (1 << 16);

static bool isGC = false;





#if 0
static unsigned long int latching_incr_long(unsigned long int *where) {
while (1) {
unsigned long int old_value = *(volatile unsigned long int *)where;
if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
return BLOCK_REFCOUNT_MASK;
}
if (OSAtomicCompareAndSwapLong(old_value, old_value+1, (volatile long int *)where)) {
return old_value+1;
}
}
}
#endif

static int latching_incr_int(int *where) {
while (1) {
int old_value = *(volatile int *)where;
if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
return BLOCK_REFCOUNT_MASK;
}
if (OSAtomicCompareAndSwapInt(old_value, old_value+1, (volatile int *)where)) {
return old_value+1;
}
}
}

#if 0
static int latching_decr_long(unsigned long int *where) {
while (1) {
unsigned long int old_value = *(volatile int *)where;
if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
return BLOCK_REFCOUNT_MASK;
}
if ((old_value & BLOCK_REFCOUNT_MASK) == 0) {
return 0;
}
if (OSAtomicCompareAndSwapLong(old_value, old_value-1, (volatile long int *)where)) {
return old_value-1;
}
}
}
#endif

static int latching_decr_int(int *where) {
while (1) {
int old_value = *(volatile int *)where;
if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
return BLOCK_REFCOUNT_MASK;
}
if ((old_value & BLOCK_REFCOUNT_MASK) == 0) {
return 0;
}
if (OSAtomicCompareAndSwapInt(old_value, old_value-1, (volatile int *)where)) {
return old_value-1;
}
}
}





#if 0
#pragma mark GC Support Routines
#endif


static void *_Block_alloc_default(const unsigned long size, const bool initialCountIsOne, const bool isObject) {
return malloc(size);
}

static void _Block_assign_default(void *value, void **destptr) {
*destptr = value;
}

static void _Block_setHasRefcount_default(const void *ptr, const bool hasRefcount) {
}

static void _Block_do_nothing(const void *aBlock) { }

static void _Block_retain_object_default(const void *ptr) {
if (!ptr) return;
}

static void _Block_release_object_default(const void *ptr) {
if (!ptr) return;
}

static void _Block_assign_weak_default(const void *ptr, void *dest) {
*(void **)dest = (void *)ptr;
}

static void _Block_memmove_default(void *dst, void *src, unsigned long size) {
memmove(dst, src, (size_t)size);
}

static void _Block_memmove_gc_broken(void *dest, void *src, unsigned long size) {
void **destp = (void **)dest;
void **srcp = (void **)src;
while (size) {
_Block_assign_default(*srcp, destp);
destp++;
srcp++;
size -= sizeof(void *);
}
}





static void *(*_Block_allocator)(const unsigned long, const bool isOne, const bool isObject) = _Block_alloc_default;
static void (*_Block_deallocator)(const void *) = (void (*)(const void *))free;
static void (*_Block_assign)(void *value, void **destptr) = _Block_assign_default;
static void (*_Block_setHasRefcount)(const void *ptr, const bool hasRefcount) = _Block_setHasRefcount_default;
static void (*_Block_retain_object)(const void *ptr) = _Block_retain_object_default;
static void (*_Block_release_object)(const void *ptr) = _Block_release_object_default;
static void (*_Block_assign_weak)(const void *dest, void *ptr) = _Block_assign_weak_default;
static void (*_Block_memmove)(void *dest, void *src, unsigned long size) = _Block_memmove_default;










void _Block_use_GC( void *(*alloc)(const unsigned long, const bool isOne, const bool isObject),
void (*setHasRefcount)(const void *, const bool),
void (*gc_assign)(void *, void **),
void (*gc_assign_weak)(const void *, void *),
void (*gc_memmove)(void *, void *, unsigned long)) {

isGC = true;
_Block_allocator = alloc;
_Block_deallocator = _Block_do_nothing;
_Block_assign = gc_assign;
_Block_copy_flag = BLOCK_IS_GC;
_Block_copy_class = _NSConcreteAutoBlock;

_Block_copy_finalizing_class = _NSConcreteFinalizingBlock;
_Block_setHasRefcount = setHasRefcount;
_Byref_flag_initial_value = BLOCK_IS_GC;
_Block_retain_object = _Block_do_nothing;
_Block_release_object = _Block_do_nothing;
_Block_assign_weak = gc_assign_weak;
_Block_memmove = gc_memmove;
}


void _Block_use_GC5( void *(*alloc)(const unsigned long, const bool isOne, const bool isObject),
void (*setHasRefcount)(const void *, const bool),
void (*gc_assign)(void *, void **),
void (*gc_assign_weak)(const void *, void *)) {

_Block_use_GC(alloc, setHasRefcount, gc_assign, gc_assign_weak, _Block_memmove_gc_broken);
}









void _Block_use_RR( void (*retain)(const void *),
void (*release)(const void *)) {
_Block_retain_object = retain;
_Block_release_object = release;
}





#if 0
#pragma mark Copy/Release support
#endif


static void *_Block_copy_internal(const void *arg, const int flags) {
struct Block_layout *aBlock;
const bool wantsOne = (WANTS_ONE & flags) == WANTS_ONE;


if (!arg) return NULL;



aBlock = (struct Block_layout *)arg;
if (aBlock->flags & BLOCK_NEEDS_FREE) {

latching_incr_int(&aBlock->flags);
return aBlock;
}
else if (aBlock->flags & BLOCK_IS_GC) {

if (wantsOne && ((latching_incr_int(&aBlock->flags) & BLOCK_REFCOUNT_MASK) == 1)) {

_Block_setHasRefcount(aBlock, true);
}
return aBlock;
}
else if (aBlock->flags & BLOCK_IS_GLOBAL) {
return aBlock;
}


if (!isGC) {
struct Block_layout *result = malloc(aBlock->descriptor->size);
if (!result) return (void *)0;
memmove(result, aBlock, aBlock->descriptor->size);

result->flags &= ~(BLOCK_REFCOUNT_MASK);
result->flags |= BLOCK_NEEDS_FREE | 1;
result->isa = _NSConcreteMallocBlock;
if (result->flags & BLOCK_HAS_COPY_DISPOSE) {

(*aBlock->descriptor->copy)(result, aBlock);
}
return result;
}
else {


unsigned long int flags = aBlock->flags;
bool hasCTOR = (flags & BLOCK_HAS_CTOR) != 0;
struct Block_layout *result = _Block_allocator(aBlock->descriptor->size, wantsOne, hasCTOR);
if (!result) return (void *)0;
memmove(result, aBlock, aBlock->descriptor->size);


flags &= ~(BLOCK_NEEDS_FREE|BLOCK_REFCOUNT_MASK);
if (wantsOne)
flags |= BLOCK_IS_GC | 1;
else
flags |= BLOCK_IS_GC;
result->flags = flags;
if (flags & BLOCK_HAS_COPY_DISPOSE) {

(*aBlock->descriptor->copy)(result, aBlock);
}
if (hasCTOR) {
result->isa = _NSConcreteFinalizingBlock;
}
else {
result->isa = _NSConcreteAutoBlock;
}
return result;
}
}












static void _Block_byref_assign_copy(void *dest, const void *arg, const int flags) {
struct Block_byref **destp = (struct Block_byref **)dest;
struct Block_byref *src = (struct Block_byref *)arg;



if (src->forwarding->flags & BLOCK_IS_GC) {
;
}
else if ((src->forwarding->flags & BLOCK_REFCOUNT_MASK) == 0) {


bool isWeak = ((flags & (BLOCK_FIELD_IS_BYREF|BLOCK_FIELD_IS_WEAK)) == (BLOCK_FIELD_IS_BYREF|BLOCK_FIELD_IS_WEAK));

struct Block_byref *copy = (struct Block_byref *)_Block_allocator(src->size, false, isWeak);
copy->flags = src->flags | _Byref_flag_initial_value;
copy->forwarding = copy;
src->forwarding = copy;
copy->size = src->size;
if (isWeak) {
copy->isa = &_NSConcreteWeakBlockVariable;
}
if (src->flags & BLOCK_HAS_COPY_DISPOSE) {


copy->byref_keep = src->byref_keep;
copy->byref_destroy = src->byref_destroy;
(*src->byref_keep)(copy, src);
}
else {

_Block_memmove(
(void *)&copy->byref_keep,
(void *)&src->byref_keep,
src->size - sizeof(struct Block_byref_header));
}
}

else if ((src->forwarding->flags & BLOCK_NEEDS_FREE) == BLOCK_NEEDS_FREE) {
latching_incr_int(&src->forwarding->flags);
}

_Block_assign(src->forwarding, (void **)destp);
}


static void _Block_byref_release(const void *arg) {
struct Block_byref *shared_struct = (struct Block_byref *)arg;
int refcount;


shared_struct = shared_struct->forwarding;




if ((shared_struct->flags & BLOCK_NEEDS_FREE) == 0) {
return;
}
refcount = shared_struct->flags & BLOCK_REFCOUNT_MASK;
if (refcount <= 0) {
printf("_Block_byref_release: Block byref data structure at %p underflowed\n", arg);
}
else if ((latching_decr_int(&shared_struct->flags) & BLOCK_REFCOUNT_MASK) == 0) {

if (shared_struct->flags & BLOCK_HAS_COPY_DISPOSE) {

(*shared_struct->byref_destroy)(shared_struct);
}
_Block_deallocator((struct Block_layout *)shared_struct);
}
}









#if 0
#pragma mark SPI/API
#endif

void *_Block_copy(const void *arg) {
return _Block_copy_internal(arg, WANTS_ONE);
}



void _Block_release(void *arg) {
struct Block_layout *aBlock = (struct Block_layout *)arg;
int32_t newCount;
if (!aBlock) return;
newCount = latching_decr_int(&aBlock->flags) & BLOCK_REFCOUNT_MASK;
if (newCount > 0) return;

if (aBlock->flags & BLOCK_IS_GC) {



_Block_setHasRefcount(aBlock, false);
}
else if (aBlock->flags & BLOCK_NEEDS_FREE) {
if (aBlock->flags & BLOCK_HAS_COPY_DISPOSE)(*aBlock->descriptor->dispose)(aBlock);
_Block_deallocator(aBlock);
}
else if (aBlock->flags & BLOCK_IS_GLOBAL) {
;
}
else {
printf("Block_release called upon a stack Block: %p, ignored\n", (void *)aBlock);
}
}




static void _Block_destroy(const void *arg) {
struct Block_layout *aBlock;
if (!arg) return;
aBlock = (struct Block_layout *)arg;
if (aBlock->flags & BLOCK_IS_GC) {

return;
}
_Block_release(aBlock);
}










void *_Block_copy_collectable(const void *aBlock) {
return _Block_copy_internal(aBlock, 0);
}



unsigned long int Block_size(void *arg) {
return ((struct Block_layout *)arg)->descriptor->size;
}


#if 0
#pragma mark Compiler SPI entry points
#endif







































void _Block_object_assign(void *destAddr, const void *object, const int flags) {

if ((flags & BLOCK_BYREF_CALLER) == BLOCK_BYREF_CALLER) {
if ((flags & BLOCK_FIELD_IS_WEAK) == BLOCK_FIELD_IS_WEAK) {
_Block_assign_weak(object, destAddr);
}
else {

_Block_assign((void *)object, destAddr);
}
}
else if ((flags & BLOCK_FIELD_IS_BYREF) == BLOCK_FIELD_IS_BYREF) {


_Block_byref_assign_copy(destAddr, object, flags);
}

else if ((flags & BLOCK_FIELD_IS_BLOCK) == BLOCK_FIELD_IS_BLOCK) {

_Block_assign(_Block_copy_internal(object, flags), destAddr);
}

else if ((flags & BLOCK_FIELD_IS_OBJECT) == BLOCK_FIELD_IS_OBJECT) {

_Block_retain_object(object);

_Block_assign((void *)object, destAddr);
}
}




void _Block_object_dispose(const void *object, const int flags) {

if (flags & BLOCK_FIELD_IS_BYREF) {

_Block_byref_release(object);
}
else if ((flags & (BLOCK_FIELD_IS_BLOCK|BLOCK_BYREF_CALLER)) == BLOCK_FIELD_IS_BLOCK) {


_Block_destroy(object);
}
else if ((flags & (BLOCK_FIELD_IS_WEAK|BLOCK_FIELD_IS_BLOCK|BLOCK_BYREF_CALLER)) == BLOCK_FIELD_IS_OBJECT) {


_Block_release_object(object);
}
}





#if 0
#pragma mark Debugging
#endif


const char *_Block_dump(const void *block) {
struct Block_layout *closure = (struct Block_layout *)block;
static char buffer[512];
char *cp = buffer;
if (closure == NULL) {
sprintf(cp, "NULL passed to _Block_dump\n");
return buffer;
}
if (! (closure->flags & BLOCK_HAS_DESCRIPTOR)) {
printf("Block compiled by obsolete compiler, please recompile source for this Block\n");
exit(1);
}
cp += sprintf(cp, "^%p (new layout) =\n", (void *)closure);
if (closure->isa == NULL) {
cp += sprintf(cp, "isa: NULL\n");
}
else if (closure->isa == _NSConcreteStackBlock) {
cp += sprintf(cp, "isa: stack Block\n");
}
else if (closure->isa == _NSConcreteMallocBlock) {
cp += sprintf(cp, "isa: malloc heap Block\n");
}
else if (closure->isa == _NSConcreteAutoBlock) {
cp += sprintf(cp, "isa: GC heap Block\n");
}
else if (closure->isa == _NSConcreteGlobalBlock) {
cp += sprintf(cp, "isa: global Block\n");
}
else if (closure->isa == _NSConcreteFinalizingBlock) {
cp += sprintf(cp, "isa: finalizing Block\n");
}
else {
cp += sprintf(cp, "isa?: %p\n", (void *)closure->isa);
}
cp += sprintf(cp, "flags:");
if (closure->flags & BLOCK_HAS_DESCRIPTOR) {
cp += sprintf(cp, " HASDESCRIPTOR");
}
if (closure->flags & BLOCK_NEEDS_FREE) {
cp += sprintf(cp, " FREEME");
}
if (closure->flags & BLOCK_IS_GC) {
cp += sprintf(cp, " ISGC");
}
if (closure->flags & BLOCK_HAS_COPY_DISPOSE) {
cp += sprintf(cp, " HASHELP");
}
if (closure->flags & BLOCK_HAS_CTOR) {
cp += sprintf(cp, " HASCTOR");
}
cp += sprintf(cp, "\nrefcount: %u\n", closure->flags & BLOCK_REFCOUNT_MASK);
cp += sprintf(cp, "invoke: %p\n", (void *)(uintptr_t)closure->invoke);
{
struct Block_descriptor *dp = closure->descriptor;
cp += sprintf(cp, "descriptor: %p\n", (void *)dp);
cp += sprintf(cp, "descriptor->reserved: %lu\n", dp->reserved);
cp += sprintf(cp, "descriptor->size: %lu\n", dp->size);

if (closure->flags & BLOCK_HAS_COPY_DISPOSE) {
cp += sprintf(cp, "descriptor->copy helper: %p\n", (void *)(uintptr_t)dp->copy);
cp += sprintf(cp, "descriptor->dispose helper: %p\n", (void *)(uintptr_t)dp->dispose);
}
}
return buffer;
}


const char *_Block_byref_dump(struct Block_byref *src) {
static char buffer[256];
char *cp = buffer;
cp += sprintf(cp, "byref data block %p contents:\n", (void *)src);
cp += sprintf(cp, " forwarding: %p\n", (void *)src->forwarding);
cp += sprintf(cp, " flags: 0x%x\n", src->flags);
cp += sprintf(cp, " size: %d\n", src->size);
if (src->flags & BLOCK_HAS_COPY_DISPOSE) {
cp += sprintf(cp, " copy helper: %p\n", (void *)(uintptr_t)src->byref_keep);
cp += sprintf(cp, " dispose helper: %p\n", (void *)(uintptr_t)src->byref_destroy);
}
return buffer;
}

