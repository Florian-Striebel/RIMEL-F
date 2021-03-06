












#if !defined(SANITIZER_BVGRAPH_H)
#define SANITIZER_BVGRAPH_H

#include "sanitizer_common.h"
#include "sanitizer_bitvector.h"

namespace __sanitizer {



template<class BV>
class BVGraph {
public:
enum SizeEnum : uptr { kSize = BV::kSize };
uptr size() const { return kSize; }

void clear() {
for (uptr i = 0; i < size(); i++)
v[i].clear();
}

bool empty() const {
for (uptr i = 0; i < size(); i++)
if (!v[i].empty())
return false;
return true;
}


bool addEdge(uptr from, uptr to) {
check(from, to);
return v[from].setBit(to);
}


uptr addEdges(const BV &from, uptr to, uptr added_edges[],
uptr max_added_edges) {
uptr res = 0;
t1.copyFrom(from);
while (!t1.empty()) {
uptr node = t1.getAndClearFirstOne();
if (v[node].setBit(to))
if (res < max_added_edges)
added_edges[res++] = node;
}
return res;
}







bool hasEdge(uptr from, uptr to) { return v[from].getBit(to); }


bool removeEdge(uptr from, uptr to) {
return v[from].clearBit(to);
}


bool removeEdgesTo(const BV &to) {
bool res = 0;
for (uptr from = 0; from < size(); from++) {
if (v[from].setDifference(to))
res = true;
}
return res;
}


bool removeEdgesFrom(const BV &from) {
bool res = false;
t1.copyFrom(from);
while (!t1.empty()) {
uptr idx = t1.getAndClearFirstOne();
if (!v[idx].empty()) {
v[idx].clear();
res = true;
}
}
return res;
}

void removeEdgesFrom(uptr from) {
return v[from].clear();
}

bool hasEdge(uptr from, uptr to) const {
check(from, to);
return v[from].getBit(to);
}



bool isReachable(uptr from, const BV &targets) {
BV &to_visit = t1,
&visited = t2;
to_visit.copyFrom(v[from]);
visited.clear();
visited.setBit(from);
while (!to_visit.empty()) {
uptr idx = to_visit.getAndClearFirstOne();
if (visited.setBit(idx))
to_visit.setUnion(v[idx]);
}
return targets.intersectsWith(visited);
}




uptr findPath(uptr from, const BV &targets, uptr *path, uptr path_size) {
if (path_size == 0)
return 0;
path[0] = from;
if (targets.getBit(from))
return 1;


for (typename BV::Iterator it(v[from]); it.hasNext(); ) {
uptr idx = it.next();
if (uptr res = findPath(idx, targets, path + 1, path_size - 1))
return res + 1;
}
return 0;
}


uptr findShortestPath(uptr from, const BV &targets, uptr *path,
uptr path_size) {
for (uptr p = 1; p <= path_size; p++)
if (findPath(from, targets, path, p) == p)
return p;
return 0;
}

private:
void check(uptr idx1, uptr idx2) const {
CHECK_LT(idx1, size());
CHECK_LT(idx2, size());
}
BV v[kSize];

BV t1, t2;
};

}

#endif
