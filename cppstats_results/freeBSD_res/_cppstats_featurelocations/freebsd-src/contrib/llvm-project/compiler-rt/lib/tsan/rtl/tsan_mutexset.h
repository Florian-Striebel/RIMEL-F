











#if !defined(TSAN_MUTEXSET_H)
#define TSAN_MUTEXSET_H

#include "tsan_defs.h"

namespace __tsan {

class MutexSet {
public:


static const uptr kMaxSize = 16;
struct Desc {
u64 id;
u64 epoch;
int count;
bool write;
};

MutexSet();

void Add(u64 id, bool write, u64 epoch);
void Del(u64 id, bool write);
void Remove(u64 id);
uptr Size() const;
Desc Get(uptr i) const;

void operator=(const MutexSet &other) {
internal_memcpy(this, &other, sizeof(*this));
}

private:
#if !SANITIZER_GO
uptr size_;
Desc descs_[kMaxSize];
#endif

void RemovePos(uptr i);
MutexSet(const MutexSet&);
};




#if SANITIZER_GO
MutexSet::MutexSet() {}
void MutexSet::Add(u64 id, bool write, u64 epoch) {}
void MutexSet::Del(u64 id, bool write) {}
void MutexSet::Remove(u64 id) {}
void MutexSet::RemovePos(uptr i) {}
uptr MutexSet::Size() const { return 0; }
MutexSet::Desc MutexSet::Get(uptr i) const { return Desc(); }
#endif

}

#endif
