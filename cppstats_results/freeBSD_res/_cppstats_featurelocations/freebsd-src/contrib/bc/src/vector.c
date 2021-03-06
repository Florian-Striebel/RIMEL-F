


































#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <vector.h>
#include <lang.h>
#include <vm.h>

void bc_vec_grow(BcVec *restrict v, size_t n) {

size_t cap, len;
sig_atomic_t lock;

cap = v->cap;
len = v->len + n;


if (len > SIZE_MAX / 2) cap = len;
else {

while (cap < len) cap += cap;
}

BC_SIG_TRYLOCK(lock);

v->v = bc_vm_realloc(v->v, bc_vm_arraySize(cap, v->size));
v->cap = cap;

BC_SIG_TRYUNLOCK(lock);
}

void bc_vec_init(BcVec *restrict v, size_t esize, BcDtorType dtor) {

BC_SIG_ASSERT_LOCKED;

assert(v != NULL && esize);

v->v = bc_vm_malloc(bc_vm_arraySize(BC_VEC_START_CAP, esize));

v->size = (BcSize) esize;
v->cap = BC_VEC_START_CAP;
v->len = 0;
v->dtor = (BcSize) dtor;
}

void bc_vec_expand(BcVec *restrict v, size_t req) {

assert(v != NULL);


if (v->cap < req) {

sig_atomic_t lock;

BC_SIG_TRYLOCK(lock);

v->v = bc_vm_realloc(v->v, bc_vm_arraySize(req, v->size));
v->cap = req;

BC_SIG_TRYUNLOCK(lock);
}
}

void bc_vec_npop(BcVec *restrict v, size_t n) {

sig_atomic_t lock;

assert(v != NULL && n <= v->len);

BC_SIG_TRYLOCK(lock);

if (!v->dtor) v->len -= n;
else {

const BcVecFree d = bc_vec_dtors[v->dtor];
size_t esize = v->size;
size_t len = v->len - n;


while (v->len > len) d(v->v + (esize * --v->len));
}

BC_SIG_TRYUNLOCK(lock);
}

void bc_vec_npopAt(BcVec *restrict v, size_t n, size_t idx) {

char* ptr, *data;
sig_atomic_t lock;

assert(v != NULL);
assert(idx + n < v->len);


ptr = bc_vec_item(v, idx);
data = bc_vec_item(v, idx + n);

BC_SIG_TRYLOCK(lock);

if (v->dtor) {

size_t i;
const BcVecFree d = bc_vec_dtors[v->dtor];


for (i = 0; i < n; ++i) d(bc_vec_item(v, idx + i));
}

v->len -= n;
memmove(ptr, data, (v->len - idx) * v->size);

BC_SIG_TRYUNLOCK(lock);
}

void bc_vec_npush(BcVec *restrict v, size_t n, const void *data) {

sig_atomic_t lock;
size_t esize;

assert(v != NULL && data != NULL);

BC_SIG_TRYLOCK(lock);


if (v->len + n > v->cap) bc_vec_grow(v, n);

esize = v->size;


memcpy(v->v + (esize * v->len), data, esize * n);
v->len += n;

BC_SIG_TRYUNLOCK(lock);
}

inline void bc_vec_push(BcVec *restrict v, const void *data) {
bc_vec_npush(v, 1, data);
}

void* bc_vec_pushEmpty(BcVec *restrict v) {

sig_atomic_t lock;
void *ptr;

assert(v != NULL);

BC_SIG_TRYLOCK(lock);


if (v->len + 1 > v->cap) bc_vec_grow(v, 1);

ptr = v->v + v->size * v->len;
v->len += 1;

BC_SIG_TRYUNLOCK(lock);

return ptr;
}

inline void bc_vec_pushByte(BcVec *restrict v, uchar data) {
assert(v != NULL && v->size == sizeof(uchar));
bc_vec_npush(v, 1, &data);
}

void bc_vec_pushIndex(BcVec *restrict v, size_t idx) {

uchar amt, nums[sizeof(size_t) + 1];

assert(v != NULL);
assert(v->size == sizeof(uchar));


for (amt = 0; idx; ++amt) {
nums[amt + 1] = (uchar) idx;
idx &= ((size_t) ~(UCHAR_MAX));
idx >>= sizeof(uchar) * CHAR_BIT;
}

nums[0] = amt;


bc_vec_npush(v, amt + 1, nums);
}

void bc_vec_pushAt(BcVec *restrict v, const void *data, size_t idx) {

assert(v != NULL && data != NULL && idx <= v->len);

BC_SIG_ASSERT_LOCKED;


if (idx == v->len) bc_vec_push(v, data);
else {

char *ptr;
size_t esize;


if (v->len == v->cap) bc_vec_grow(v, 1);

esize = v->size;

ptr = v->v + esize * idx;

memmove(ptr + esize, ptr, esize * (v->len++ - idx));
memcpy(ptr, data, esize);
}
}

void bc_vec_string(BcVec *restrict v, size_t len, const char *restrict str) {

sig_atomic_t lock;

assert(v != NULL && v->size == sizeof(char));
assert(!v->dtor);
assert(!v->len || !v->v[v->len - 1]);
assert(v->v != str);

BC_SIG_TRYLOCK(lock);

bc_vec_popAll(v);
bc_vec_expand(v, bc_vm_growSize(len, 1));
memcpy(v->v, str, len);
v->len = len;

bc_vec_pushByte(v, '\0');

BC_SIG_TRYUNLOCK(lock);
}

void bc_vec_concat(BcVec *restrict v, const char *restrict str) {

sig_atomic_t lock;

assert(v != NULL && v->size == sizeof(char));
assert(!v->dtor);
assert(!v->len || !v->v[v->len - 1]);
assert(v->v != str);

BC_SIG_TRYLOCK(lock);


if (v->len) v->len -= 1;

bc_vec_npush(v, strlen(str) + 1, str);

BC_SIG_TRYUNLOCK(lock);
}

void bc_vec_empty(BcVec *restrict v) {

sig_atomic_t lock;

assert(v != NULL && v->size == sizeof(char));
assert(!v->dtor);

BC_SIG_TRYLOCK(lock);

bc_vec_popAll(v);
bc_vec_pushByte(v, '\0');

BC_SIG_TRYUNLOCK(lock);
}

#if BC_ENABLE_HISTORY
void bc_vec_replaceAt(BcVec *restrict v, size_t idx, const void *data) {

char *ptr;

BC_SIG_ASSERT_LOCKED;

assert(v != NULL);

ptr = bc_vec_item(v, idx);

if (v->dtor) bc_vec_dtors[v->dtor](ptr);

memcpy(ptr, data, v->size);
}
#endif

inline void* bc_vec_item(const BcVec *restrict v, size_t idx) {
assert(v != NULL && v->len && idx < v->len);
return v->v + v->size * idx;
}

inline void* bc_vec_item_rev(const BcVec *restrict v, size_t idx) {
assert(v != NULL && v->len && idx < v->len);
return v->v + v->size * (v->len - idx - 1);
}

inline void bc_vec_clear(BcVec *restrict v) {
BC_SIG_ASSERT_LOCKED;
v->v = NULL;
v->len = 0;
v->dtor = BC_DTOR_NONE;
}

void bc_vec_free(void *vec) {
BcVec *v = (BcVec*) vec;
BC_SIG_ASSERT_LOCKED;
bc_vec_popAll(v);
free(v->v);
}

#if !BC_ENABLE_LIBRARY










static size_t bc_map_find(const BcVec *restrict v, const char *name) {

size_t low = 0, high = v->len;

while (low < high) {

size_t mid = (low + high) / 2;
const BcId *id = bc_vec_item(v, mid);
int result = strcmp(name, id->name);

if (!result) return mid;
else if (result < 0) high = mid;
else low = mid + 1;
}

return low;
}

bool bc_map_insert(BcVec *restrict v, const char *name,
size_t idx, size_t *restrict i)
{
BcId id;
BcVec *slabs;

BC_SIG_ASSERT_LOCKED;

assert(v != NULL && name != NULL && i != NULL);

*i = bc_map_find(v, name);

assert(*i <= v->len);

if (*i != v->len && !strcmp(name, ((BcId*) bc_vec_item(v, *i))->name))
return false;

#if BC_ENABLED
slabs = BC_IS_DC ? &vm.main_slabs : &vm.other_slabs;
#else
slabs = &vm.main_slabs;
#endif

id.name = bc_slabvec_strdup(slabs, name);
id.idx = idx;

bc_vec_pushAt(v, &id, *i);

return true;
}

size_t bc_map_index(const BcVec *restrict v, const char *name) {

size_t i;

assert(v != NULL && name != NULL);

i = bc_map_find(v, name);


if (i >= v->len) return BC_VEC_INVALID_IDX;


return strcmp(name, ((BcId*) bc_vec_item(v, i))->name) ?
BC_VEC_INVALID_IDX : i;
}

#if DC_ENABLED
const char* bc_map_name(const BcVec *restrict v, size_t idx) {

size_t i, len = v->len;

for (i = 0; i < len; ++i) {
BcId* id = (BcId*) bc_vec_item(v, i);
if (id->idx == idx) return id->name;
}

BC_UNREACHABLE

return "";
}
#endif





static void bc_slab_init(BcSlab *s) {
s->s = bc_vm_malloc(BC_SLAB_SIZE);
s->len = 0;
}










static char* bc_slab_add(BcSlab *s, const char *str, size_t len) {

char *ptr;

assert(s != NULL);
assert(str != NULL);
assert(len == strlen(str) + 1);

if (s->len + len > BC_SLAB_SIZE) return NULL;

ptr = (char*) (s->s + s->len);

bc_strcpy(ptr, len, str);

s->len += len;

return ptr;
}

void bc_slab_free(void *slab) {
free(((BcSlab*) slab)->s);
}

void bc_slabvec_init(BcVec* v) {

BcSlab *slab;

assert(v != NULL);

bc_vec_init(v, sizeof(BcSlab), BC_DTOR_SLAB);


slab = bc_vec_pushEmpty(v);
bc_slab_init(slab);
}

char* bc_slabvec_strdup(BcVec *v, const char *str) {

char *s;
size_t len;
BcSlab slab;
BcSlab *slab_ptr;

BC_SIG_ASSERT_LOCKED;

assert(v != NULL && v->len);

assert(str != NULL);

len = strlen(str) + 1;


if (BC_UNLIKELY(len >= BC_SLAB_SIZE)) {


slab.len = SIZE_MAX;
slab.s = bc_vm_strdup(str);


bc_vec_pushAt(v, &slab, v->len - 1);

return slab.s;
}


slab_ptr = bc_vec_top(v);
s = bc_slab_add(slab_ptr, str, len);


if (BC_UNLIKELY(s == NULL)) {

slab_ptr = bc_vec_pushEmpty(v);
bc_slab_init(slab_ptr);

s = bc_slab_add(slab_ptr, str, len);

assert(s != NULL);
}

return s;
}

void bc_slabvec_clear(BcVec *v) {

BcSlab *s;
bool again;



do {


s = bc_vec_item(v, 0);



assert(s->len != SIZE_MAX || v->len > 1);


again = (s->len == SIZE_MAX);


if (again) bc_vec_npopAt(v, 1, 0);

} while(again);



if (v->len > 1) bc_vec_npop(v, v->len - 1);


s->len = 0;
}
#endif

#if BC_DEBUG_CODE

void bc_slabvec_print(BcVec *v, const char *func) {

size_t i;
BcSlab *s;

bc_file_printf(&vm.ferr, "%s\n", func);

for (i = 0; i < v->len; ++i) {
s = bc_vec_item(v, i);
bc_file_printf(&vm.ferr, "%zu { s = %zu, len = %zu }\n",
i, (uintptr_t) s->s, s->len);
}

bc_file_puts(&vm.ferr, bc_flush_none, "\n");
bc_file_flush(&vm.ferr, bc_flush_none);
}

#endif
