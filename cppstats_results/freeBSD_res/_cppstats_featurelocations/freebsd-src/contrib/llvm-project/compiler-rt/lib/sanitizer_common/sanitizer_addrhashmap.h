











#if !defined(SANITIZER_ADDRHASHMAP_H)
#define SANITIZER_ADDRHASHMAP_H

#include "sanitizer_common.h"
#include "sanitizer_mutex.h"
#include "sanitizer_atomic.h"
#include "sanitizer_allocator_internal.h"

namespace __sanitizer {




















template<typename T, uptr kSize>
class AddrHashMap {
private:
struct Cell {
atomic_uintptr_t addr;
T val;
};

struct AddBucket {
uptr cap;
uptr size;
Cell cells[1];
};

static const uptr kBucketSize = 3;

struct Bucket {
RWMutex mtx;
atomic_uintptr_t add;
Cell cells[kBucketSize];
};

public:
AddrHashMap();

class Handle {
public:
Handle(AddrHashMap<T, kSize> *map, uptr addr);
Handle(AddrHashMap<T, kSize> *map, uptr addr, bool remove);
Handle(AddrHashMap<T, kSize> *map, uptr addr, bool remove, bool create);

~Handle();
T *operator->();
T &operator*();
const T &operator*() const;
bool created() const;
bool exists() const;

private:
friend AddrHashMap<T, kSize>;
AddrHashMap<T, kSize> *map_;
Bucket *bucket_;
Cell *cell_;
uptr addr_;
uptr addidx_;
bool created_;
bool remove_;
bool create_;
};

private:
friend class Handle;
Bucket *table_;

void acquire(Handle *h);
void release(Handle *h);
uptr calcHash(uptr addr);
};

template<typename T, uptr kSize>
AddrHashMap<T, kSize>::Handle::Handle(AddrHashMap<T, kSize> *map, uptr addr) {
map_ = map;
addr_ = addr;
remove_ = false;
create_ = true;
map_->acquire(this);
}

template<typename T, uptr kSize>
AddrHashMap<T, kSize>::Handle::Handle(AddrHashMap<T, kSize> *map, uptr addr,
bool remove) {
map_ = map;
addr_ = addr;
remove_ = remove;
create_ = true;
map_->acquire(this);
}

template<typename T, uptr kSize>
AddrHashMap<T, kSize>::Handle::Handle(AddrHashMap<T, kSize> *map, uptr addr,
bool remove, bool create) {
map_ = map;
addr_ = addr;
remove_ = remove;
create_ = create;
map_->acquire(this);
}

template<typename T, uptr kSize>
AddrHashMap<T, kSize>::Handle::~Handle() {
map_->release(this);
}

template <typename T, uptr kSize>
T *AddrHashMap<T, kSize>::Handle::operator->() {
return &cell_->val;
}

template <typename T, uptr kSize>
const T &AddrHashMap<T, kSize>::Handle::operator*() const {
return cell_->val;
}

template <typename T, uptr kSize>
T &AddrHashMap<T, kSize>::Handle::operator*() {
return cell_->val;
}

template<typename T, uptr kSize>
bool AddrHashMap<T, kSize>::Handle::created() const {
return created_;
}

template<typename T, uptr kSize>
bool AddrHashMap<T, kSize>::Handle::exists() const {
return cell_ != nullptr;
}

template<typename T, uptr kSize>
AddrHashMap<T, kSize>::AddrHashMap() {
table_ = (Bucket*)MmapOrDie(kSize * sizeof(table_[0]), "AddrHashMap");
}

template <typename T, uptr kSize>
void AddrHashMap<T, kSize>::acquire(Handle *h) NO_THREAD_SAFETY_ANALYSIS {
uptr addr = h->addr_;
uptr hash = calcHash(addr);
Bucket *b = &table_[hash];

h->created_ = false;
h->addidx_ = -1U;
h->bucket_ = b;
h->cell_ = nullptr;



if (h->remove_)
goto locked;

retry:

CHECK(!h->remove_);

for (uptr i = 0; i < kBucketSize; i++) {
Cell *c = &b->cells[i];
uptr addr1 = atomic_load(&c->addr, memory_order_acquire);
if (addr1 == addr) {
h->cell_ = c;
return;
}
}


if (atomic_load(&b->add, memory_order_relaxed)) {
b->mtx.ReadLock();
AddBucket *add = (AddBucket*)atomic_load(&b->add, memory_order_relaxed);
for (uptr i = 0; i < add->size; i++) {
Cell *c = &add->cells[i];
uptr addr1 = atomic_load(&c->addr, memory_order_relaxed);
if (addr1 == addr) {
h->addidx_ = i;
h->cell_ = c;
return;
}
}
b->mtx.ReadUnlock();
}

locked:


b->mtx.Lock();
for (uptr i = 0; i < kBucketSize; i++) {
Cell *c = &b->cells[i];
uptr addr1 = atomic_load(&c->addr, memory_order_relaxed);
if (addr1 == addr) {
if (h->remove_) {
h->cell_ = c;
return;
}
b->mtx.Unlock();
goto retry;
}
}


AddBucket *add = (AddBucket*)atomic_load(&b->add, memory_order_relaxed);
if (add) {
for (uptr i = 0; i < add->size; i++) {
Cell *c = &add->cells[i];
uptr addr1 = atomic_load(&c->addr, memory_order_relaxed);
if (addr1 == addr) {
if (h->remove_) {
h->addidx_ = i;
h->cell_ = c;
return;
}
b->mtx.Unlock();
goto retry;
}
}
}


if (h->remove_ || !h->create_) {
b->mtx.Unlock();
return;
}


h->created_ = true;

for (uptr i = 0; i < kBucketSize; i++) {
Cell *c = &b->cells[i];
uptr addr1 = atomic_load(&c->addr, memory_order_relaxed);
if (addr1 == 0) {
h->cell_ = c;
return;
}
}


if (!add) {

const uptr kInitSize = 64;
add = (AddBucket*)InternalAlloc(kInitSize);
internal_memset(add, 0, kInitSize);
add->cap = (kInitSize - sizeof(*add)) / sizeof(add->cells[0]) + 1;
add->size = 0;
atomic_store(&b->add, (uptr)add, memory_order_relaxed);
}
if (add->size == add->cap) {

uptr oldsize = sizeof(*add) + (add->cap - 1) * sizeof(add->cells[0]);
uptr newsize = oldsize * 2;
AddBucket *add1 = (AddBucket*)InternalAlloc(newsize);
internal_memset(add1, 0, newsize);
add1->cap = (newsize - sizeof(*add)) / sizeof(add->cells[0]) + 1;
add1->size = add->size;
internal_memcpy(add1->cells, add->cells, add->size * sizeof(add->cells[0]));
InternalFree(add);
atomic_store(&b->add, (uptr)add1, memory_order_relaxed);
add = add1;
}

uptr i = add->size++;
Cell *c = &add->cells[i];
CHECK_EQ(atomic_load(&c->addr, memory_order_relaxed), 0);
h->addidx_ = i;
h->cell_ = c;
}

template <typename T, uptr kSize>
void AddrHashMap<T, kSize>::release(Handle *h) NO_THREAD_SAFETY_ANALYSIS {
if (!h->cell_)
return;
Bucket *b = h->bucket_;
Cell *c = h->cell_;
uptr addr1 = atomic_load(&c->addr, memory_order_relaxed);
if (h->created_) {

CHECK_EQ(addr1, 0);


atomic_store(&c->addr, h->addr_, memory_order_release);
b->mtx.Unlock();
} else if (h->remove_) {

CHECK_EQ(addr1, h->addr_);
atomic_store(&c->addr, 0, memory_order_release);

AddBucket *add = (AddBucket *)atomic_load(&b->add, memory_order_relaxed);
if (h->addidx_ == -1U) {

if (add && add->size != 0) {
uptr last = --add->size;
Cell *c1 = &add->cells[last];
c->val = c1->val;
uptr addr1 = atomic_load(&c1->addr, memory_order_relaxed);
atomic_store(&c->addr, addr1, memory_order_release);
atomic_store(&c1->addr, 0, memory_order_release);
}
} else {

uptr last = --add->size;
Cell *c1 = &add->cells[last];
if (c != c1) {
*c = *c1;
atomic_store(&c1->addr, 0, memory_order_relaxed);
}
}
if (add && add->size == 0) {

}
b->mtx.Unlock();
} else {
CHECK_EQ(addr1, h->addr_);
if (h->addidx_ != -1U)
b->mtx.ReadUnlock();
}
}

template<typename T, uptr kSize>
uptr AddrHashMap<T, kSize>::calcHash(uptr addr) {
addr += addr << 10;
addr ^= addr >> 6;
return addr % kSize;
}

}

#endif
