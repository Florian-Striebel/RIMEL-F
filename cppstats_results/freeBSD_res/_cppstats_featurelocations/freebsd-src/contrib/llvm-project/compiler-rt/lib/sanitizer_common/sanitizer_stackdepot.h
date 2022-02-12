











#if !defined(SANITIZER_STACKDEPOT_H)
#define SANITIZER_STACKDEPOT_H

#include "sanitizer_common.h"
#include "sanitizer_internal_defs.h"
#include "sanitizer_stacktrace.h"

namespace __sanitizer {


struct StackDepotNode;
struct StackDepotHandle {
StackDepotNode *node_;
StackDepotHandle() : node_(nullptr) {}
explicit StackDepotHandle(StackDepotNode *node) : node_(node) {}
bool valid() { return node_; }
u32 id();
int use_count();
void inc_use_count_unsafe();
};

const int kStackDepotMaxUseCount = 1U << (SANITIZER_ANDROID ? 16 : 20);

StackDepotStats *StackDepotGetStats();
u32 StackDepotPut(StackTrace stack);
StackDepotHandle StackDepotPut_WithHandle(StackTrace stack);

StackTrace StackDepotGet(u32 id);

void StackDepotLockAll();
void StackDepotUnlockAll();
void StackDepotPrintAll();





class StackDepotReverseMap {
public:
StackDepotReverseMap();
StackTrace Get(u32 id);

private:
struct IdDescPair {
u32 id;
StackDepotNode *desc;

static bool IdComparator(const IdDescPair &a, const IdDescPair &b);
};

InternalMmapVector<IdDescPair> map_;


StackDepotReverseMap(const StackDepotReverseMap&);
void operator=(const StackDepotReverseMap&);
};

}

#endif
