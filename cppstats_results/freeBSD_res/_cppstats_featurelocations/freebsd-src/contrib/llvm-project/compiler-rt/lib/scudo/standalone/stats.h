







#if !defined(SCUDO_STATS_H_)
#define SCUDO_STATS_H_

#include "atomic_helpers.h"
#include "list.h"
#include "mutex.h"

#include <string.h>

namespace scudo {


enum StatType { StatAllocated, StatFree, StatMapped, StatCount };

typedef uptr StatCounters[StatCount];






class LocalStats {
public:
void init() {
for (uptr I = 0; I < StatCount; I++)
DCHECK_EQ(get(static_cast<StatType>(I)), 0U);
}

void add(StatType I, uptr V) {
V += atomic_load_relaxed(&StatsArray[I]);
atomic_store_relaxed(&StatsArray[I], V);
}

void sub(StatType I, uptr V) {
V = atomic_load_relaxed(&StatsArray[I]) - V;
atomic_store_relaxed(&StatsArray[I], V);
}

void set(StatType I, uptr V) { atomic_store_relaxed(&StatsArray[I], V); }

uptr get(StatType I) const { return atomic_load_relaxed(&StatsArray[I]); }

LocalStats *Next = nullptr;
LocalStats *Prev = nullptr;

private:
atomic_uptr StatsArray[StatCount] = {};
};


class GlobalStats : public LocalStats {
public:
void init() { LocalStats::init(); }

void link(LocalStats *S) {
ScopedLock L(Mutex);
StatsList.push_back(S);
}

void unlink(LocalStats *S) {
ScopedLock L(Mutex);
StatsList.remove(S);
for (uptr I = 0; I < StatCount; I++)
add(static_cast<StatType>(I), S->get(static_cast<StatType>(I)));
}

void get(uptr *S) const {
ScopedLock L(Mutex);
for (uptr I = 0; I < StatCount; I++)
S[I] = LocalStats::get(static_cast<StatType>(I));
for (const auto &Stats : StatsList) {
for (uptr I = 0; I < StatCount; I++)
S[I] += Stats.get(static_cast<StatType>(I));
}

for (uptr I = 0; I < StatCount; I++)
S[I] = static_cast<sptr>(S[I]) >= 0 ? S[I] : 0;
}

void lock() { Mutex.lock(); }
void unlock() { Mutex.unlock(); }

void disable() { lock(); }
void enable() { unlock(); }

private:
mutable HybridMutex Mutex;
DoublyLinkedList<LocalStats> StatsList;
};

}

#endif
