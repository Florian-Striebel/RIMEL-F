























#if !defined(SANITIZER_DEADLOCK_DETECTOR_H)
#define SANITIZER_DEADLOCK_DETECTOR_H

#include "sanitizer_bvgraph.h"
#include "sanitizer_common.h"

namespace __sanitizer {



template <class BV>
class DeadlockDetectorTLS {
public:

void clear() {
bv_.clear();
epoch_ = 0;
n_recursive_locks = 0;
n_all_locks_ = 0;
}

bool empty() const { return bv_.empty(); }

void ensureCurrentEpoch(uptr current_epoch) {
if (epoch_ == current_epoch) return;
bv_.clear();
epoch_ = current_epoch;
n_recursive_locks = 0;
n_all_locks_ = 0;
}

uptr getEpoch() const { return epoch_; }


bool addLock(uptr lock_id, uptr current_epoch, u32 stk) {
CHECK_EQ(epoch_, current_epoch);
if (!bv_.setBit(lock_id)) {

CHECK_LT(n_recursive_locks, ARRAY_SIZE(recursive_locks));
recursive_locks[n_recursive_locks++] = lock_id;
return false;
}
CHECK_LT(n_all_locks_, ARRAY_SIZE(all_locks_with_contexts_));

u32 lock_id_short = static_cast<u32>(lock_id);
LockWithContext l = {lock_id_short, stk};
all_locks_with_contexts_[n_all_locks_++] = l;
return true;
}

void removeLock(uptr lock_id) {
if (n_recursive_locks) {
for (sptr i = n_recursive_locks - 1; i >= 0; i--) {
if (recursive_locks[i] == lock_id) {
n_recursive_locks--;
Swap(recursive_locks[i], recursive_locks[n_recursive_locks]);
return;
}
}
}
if (!bv_.clearBit(lock_id))
return;
if (n_all_locks_) {
for (sptr i = n_all_locks_ - 1; i >= 0; i--) {
if (all_locks_with_contexts_[i].lock == static_cast<u32>(lock_id)) {
Swap(all_locks_with_contexts_[i],
all_locks_with_contexts_[n_all_locks_ - 1]);
n_all_locks_--;
break;
}
}
}
}

u32 findLockContext(uptr lock_id) {
for (uptr i = 0; i < n_all_locks_; i++)
if (all_locks_with_contexts_[i].lock == static_cast<u32>(lock_id))
return all_locks_with_contexts_[i].stk;
return 0;
}

const BV &getLocks(uptr current_epoch) const {
CHECK_EQ(epoch_, current_epoch);
return bv_;
}

uptr getNumLocks() const { return n_all_locks_; }
uptr getLock(uptr idx) const { return all_locks_with_contexts_[idx].lock; }

private:
BV bv_;
uptr epoch_;
uptr recursive_locks[64];
uptr n_recursive_locks;
struct LockWithContext {
u32 lock;
u32 stk;
};
LockWithContext all_locks_with_contexts_[64];
uptr n_all_locks_;
};








template <class BV>
class DeadlockDetector {
public:
typedef BV BitVector;

uptr size() const { return g_.size(); }


void clear() {
current_epoch_ = 0;
available_nodes_.clear();
recycled_nodes_.clear();
g_.clear();
n_edges_ = 0;
}





uptr newNode(uptr data) {
if (!available_nodes_.empty())
return getAvailableNode(data);
if (!recycled_nodes_.empty()) {
for (sptr i = n_edges_ - 1; i >= 0; i--) {
if (recycled_nodes_.getBit(edges_[i].from) ||
recycled_nodes_.getBit(edges_[i].to)) {
Swap(edges_[i], edges_[n_edges_ - 1]);
n_edges_--;
}
}
CHECK(available_nodes_.empty());

g_.removeEdgesTo(recycled_nodes_);
available_nodes_.setUnion(recycled_nodes_);
recycled_nodes_.clear();
return getAvailableNode(data);
}

current_epoch_ += size();
recycled_nodes_.clear();
available_nodes_.setAll();
g_.clear();
n_edges_ = 0;
return getAvailableNode(data);
}


uptr getData(uptr node) const { return data_[nodeToIndex(node)]; }

bool nodeBelongsToCurrentEpoch(uptr node) {
return node && (node / size() * size()) == current_epoch_;
}

void removeNode(uptr node) {
uptr idx = nodeToIndex(node);
CHECK(!available_nodes_.getBit(idx));
CHECK(recycled_nodes_.setBit(idx));
g_.removeEdgesFrom(idx);
}

void ensureCurrentEpoch(DeadlockDetectorTLS<BV> *dtls) {
dtls->ensureCurrentEpoch(current_epoch_);
}




bool onLockBefore(DeadlockDetectorTLS<BV> *dtls, uptr cur_node) {
ensureCurrentEpoch(dtls);
uptr cur_idx = nodeToIndex(cur_node);
return g_.isReachable(cur_idx, dtls->getLocks(current_epoch_));
}

u32 findLockContext(DeadlockDetectorTLS<BV> *dtls, uptr node) {
return dtls->findLockContext(nodeToIndex(node));
}


void onLockAfter(DeadlockDetectorTLS<BV> *dtls, uptr cur_node, u32 stk = 0) {
ensureCurrentEpoch(dtls);
uptr cur_idx = nodeToIndex(cur_node);
dtls->addLock(cur_idx, current_epoch_, stk);
}



bool hasAllEdges(DeadlockDetectorTLS<BV> *dtls, uptr cur_node) {
uptr local_epoch = dtls->getEpoch();

if (cur_node && local_epoch == current_epoch_ &&
local_epoch == nodeToEpoch(cur_node)) {
uptr cur_idx = nodeToIndexUnchecked(cur_node);
for (uptr i = 0, n = dtls->getNumLocks(); i < n; i++) {
if (!g_.hasEdge(dtls->getLock(i), cur_idx))
return false;
}
return true;
}
return false;
}





uptr addEdges(DeadlockDetectorTLS<BV> *dtls, uptr cur_node, u32 stk,
int unique_tid) {
ensureCurrentEpoch(dtls);
uptr cur_idx = nodeToIndex(cur_node);
uptr added_edges[40];
uptr n_added_edges = g_.addEdges(dtls->getLocks(current_epoch_), cur_idx,
added_edges, ARRAY_SIZE(added_edges));
for (uptr i = 0; i < n_added_edges; i++) {
if (n_edges_ < ARRAY_SIZE(edges_)) {
Edge e = {(u16)added_edges[i], (u16)cur_idx,
dtls->findLockContext(added_edges[i]), stk,
unique_tid};
edges_[n_edges_++] = e;
}
}
return n_added_edges;
}

bool findEdge(uptr from_node, uptr to_node, u32 *stk_from, u32 *stk_to,
int *unique_tid) {
uptr from_idx = nodeToIndex(from_node);
uptr to_idx = nodeToIndex(to_node);
for (uptr i = 0; i < n_edges_; i++) {
if (edges_[i].from == from_idx && edges_[i].to == to_idx) {
*stk_from = edges_[i].stk_from;
*stk_to = edges_[i].stk_to;
*unique_tid = edges_[i].unique_tid;
return true;
}
}
return false;
}



bool onLock(DeadlockDetectorTLS<BV> *dtls, uptr cur_node, u32 stk = 0) {
ensureCurrentEpoch(dtls);
bool is_reachable = !isHeld(dtls, cur_node) && onLockBefore(dtls, cur_node);
addEdges(dtls, cur_node, stk, 0);
onLockAfter(dtls, cur_node, stk);
return is_reachable;
}






bool onTryLock(DeadlockDetectorTLS<BV> *dtls, uptr cur_node, u32 stk = 0) {
ensureCurrentEpoch(dtls);
uptr cur_idx = nodeToIndex(cur_node);
dtls->addLock(cur_idx, current_epoch_, stk);
return false;
}




bool onFirstLock(DeadlockDetectorTLS<BV> *dtls, uptr node, u32 stk = 0) {
if (!dtls->empty()) return false;
if (dtls->getEpoch() && dtls->getEpoch() == nodeToEpoch(node)) {
dtls->addLock(nodeToIndexUnchecked(node), nodeToEpoch(node), stk);
return true;
}
return false;
}




uptr findPathToLock(DeadlockDetectorTLS<BV> *dtls, uptr cur_node, uptr *path,
uptr path_size) {
tmp_bv_.copyFrom(dtls->getLocks(current_epoch_));
uptr idx = nodeToIndex(cur_node);
CHECK(!tmp_bv_.getBit(idx));
uptr res = g_.findShortestPath(idx, tmp_bv_, path, path_size);
for (uptr i = 0; i < res; i++)
path[i] = indexToNode(path[i]);
if (res)
CHECK_EQ(path[0], cur_node);
return res;
}



void onUnlock(DeadlockDetectorTLS<BV> *dtls, uptr node) {
if (dtls->getEpoch() == nodeToEpoch(node))
dtls->removeLock(nodeToIndexUnchecked(node));
}





bool onLockFast(DeadlockDetectorTLS<BV> *dtls, uptr node, u32 stk = 0) {
if (hasAllEdges(dtls, node)) {
dtls->addLock(nodeToIndexUnchecked(node), nodeToEpoch(node), stk);
return true;
}
return false;
}

bool isHeld(DeadlockDetectorTLS<BV> *dtls, uptr node) const {
return dtls->getLocks(current_epoch_).getBit(nodeToIndex(node));
}

uptr testOnlyGetEpoch() const { return current_epoch_; }
bool testOnlyHasEdge(uptr l1, uptr l2) {
return g_.hasEdge(nodeToIndex(l1), nodeToIndex(l2));
}

bool testOnlyHasEdgeRaw(uptr idx1, uptr idx2) {
return g_.hasEdge(idx1, idx2);
}

void Print() {
for (uptr from = 0; from < size(); from++)
for (uptr to = 0; to < size(); to++)
if (g_.hasEdge(from, to))
Printf(" %zx => %zx\n", from, to);
}

private:
void check_idx(uptr idx) const { CHECK_LT(idx, size()); }

void check_node(uptr node) const {
CHECK_GE(node, size());
CHECK_EQ(current_epoch_, nodeToEpoch(node));
}

uptr indexToNode(uptr idx) const {
check_idx(idx);
return idx + current_epoch_;
}

uptr nodeToIndexUnchecked(uptr node) const { return node % size(); }

uptr nodeToIndex(uptr node) const {
check_node(node);
return nodeToIndexUnchecked(node);
}

uptr nodeToEpoch(uptr node) const { return node / size() * size(); }

uptr getAvailableNode(uptr data) {
uptr idx = available_nodes_.getAndClearFirstOne();
data_[idx] = data;
return indexToNode(idx);
}

struct Edge {
u16 from;
u16 to;
u32 stk_from;
u32 stk_to;
int unique_tid;
};

uptr current_epoch_;
BV available_nodes_;
BV recycled_nodes_;
BV tmp_bv_;
BVGraph<BV> g_;
uptr data_[BV::kSize];
Edge edges_[BV::kSize * 32];
uptr n_edges_;
};

}

#endif
