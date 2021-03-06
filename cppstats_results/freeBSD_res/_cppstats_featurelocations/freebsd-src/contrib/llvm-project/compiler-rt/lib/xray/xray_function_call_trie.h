












#if !defined(XRAY_FUNCTION_CALL_TRIE_H)
#define XRAY_FUNCTION_CALL_TRIE_H

#include "xray_buffer_queue.h"
#include "xray_defs.h"
#include "xray_profiling_flags.h"
#include "xray_segmented_array.h"
#include <limits>
#include <memory>
#include <utility>

namespace __xray {


































































class FunctionCallTrie {
public:
struct Node;



struct NodeIdPair {
Node *NodePtr;
int32_t FId;
};

using NodeIdPairArray = Array<NodeIdPair>;
using NodeIdPairAllocatorType = NodeIdPairArray::AllocatorType;






struct Node {
Node *Parent;
NodeIdPairArray Callees;
uint64_t CallCount;
uint64_t CumulativeLocalTime;
int32_t FId;


};

private:
struct ShadowStackEntry {
uint64_t EntryTSC;
Node *NodePtr;
uint16_t EntryCPU;
};

using NodeArray = Array<Node>;
using RootArray = Array<Node *>;
using ShadowStackArray = Array<ShadowStackEntry>;

public:


struct Allocators {
using NodeAllocatorType = NodeArray::AllocatorType;
using RootAllocatorType = RootArray::AllocatorType;
using ShadowStackAllocatorType = ShadowStackArray::AllocatorType;



typename std::aligned_storage<sizeof(NodeAllocatorType),
alignof(NodeAllocatorType)>::type
NodeAllocatorStorage;
typename std::aligned_storage<sizeof(RootAllocatorType),
alignof(RootAllocatorType)>::type
RootAllocatorStorage;
typename std::aligned_storage<sizeof(ShadowStackAllocatorType),
alignof(ShadowStackAllocatorType)>::type
ShadowStackAllocatorStorage;
typename std::aligned_storage<sizeof(NodeIdPairAllocatorType),
alignof(NodeIdPairAllocatorType)>::type
NodeIdPairAllocatorStorage;

NodeAllocatorType *NodeAllocator = nullptr;
RootAllocatorType *RootAllocator = nullptr;
ShadowStackAllocatorType *ShadowStackAllocator = nullptr;
NodeIdPairAllocatorType *NodeIdPairAllocator = nullptr;

Allocators() = default;
Allocators(const Allocators &) = delete;
Allocators &operator=(const Allocators &) = delete;

struct Buffers {
BufferQueue::Buffer NodeBuffer;
BufferQueue::Buffer RootsBuffer;
BufferQueue::Buffer ShadowStackBuffer;
BufferQueue::Buffer NodeIdPairBuffer;
};

explicit Allocators(Buffers &B) XRAY_NEVER_INSTRUMENT {
new (&NodeAllocatorStorage)
NodeAllocatorType(B.NodeBuffer.Data, B.NodeBuffer.Size);
NodeAllocator =
reinterpret_cast<NodeAllocatorType *>(&NodeAllocatorStorage);

new (&RootAllocatorStorage)
RootAllocatorType(B.RootsBuffer.Data, B.RootsBuffer.Size);
RootAllocator =
reinterpret_cast<RootAllocatorType *>(&RootAllocatorStorage);

new (&ShadowStackAllocatorStorage) ShadowStackAllocatorType(
B.ShadowStackBuffer.Data, B.ShadowStackBuffer.Size);
ShadowStackAllocator = reinterpret_cast<ShadowStackAllocatorType *>(
&ShadowStackAllocatorStorage);

new (&NodeIdPairAllocatorStorage) NodeIdPairAllocatorType(
B.NodeIdPairBuffer.Data, B.NodeIdPairBuffer.Size);
NodeIdPairAllocator = reinterpret_cast<NodeIdPairAllocatorType *>(
&NodeIdPairAllocatorStorage);
}

explicit Allocators(uptr Max) XRAY_NEVER_INSTRUMENT {
new (&NodeAllocatorStorage) NodeAllocatorType(Max);
NodeAllocator =
reinterpret_cast<NodeAllocatorType *>(&NodeAllocatorStorage);

new (&RootAllocatorStorage) RootAllocatorType(Max);
RootAllocator =
reinterpret_cast<RootAllocatorType *>(&RootAllocatorStorage);

new (&ShadowStackAllocatorStorage) ShadowStackAllocatorType(Max);
ShadowStackAllocator = reinterpret_cast<ShadowStackAllocatorType *>(
&ShadowStackAllocatorStorage);

new (&NodeIdPairAllocatorStorage) NodeIdPairAllocatorType(Max);
NodeIdPairAllocator = reinterpret_cast<NodeIdPairAllocatorType *>(
&NodeIdPairAllocatorStorage);
}

Allocators(Allocators &&O) XRAY_NEVER_INSTRUMENT {


internal_memcpy(&NodeAllocatorStorage, &O.NodeAllocatorStorage,
sizeof(NodeAllocatorType));
internal_memcpy(&RootAllocatorStorage, &O.RootAllocatorStorage,
sizeof(RootAllocatorType));
internal_memcpy(&ShadowStackAllocatorStorage,
&O.ShadowStackAllocatorStorage,
sizeof(ShadowStackAllocatorType));
internal_memcpy(&NodeIdPairAllocatorStorage,
&O.NodeIdPairAllocatorStorage,
sizeof(NodeIdPairAllocatorType));

NodeAllocator =
reinterpret_cast<NodeAllocatorType *>(&NodeAllocatorStorage);
RootAllocator =
reinterpret_cast<RootAllocatorType *>(&RootAllocatorStorage);
ShadowStackAllocator = reinterpret_cast<ShadowStackAllocatorType *>(
&ShadowStackAllocatorStorage);
NodeIdPairAllocator = reinterpret_cast<NodeIdPairAllocatorType *>(
&NodeIdPairAllocatorStorage);

O.NodeAllocator = nullptr;
O.RootAllocator = nullptr;
O.ShadowStackAllocator = nullptr;
O.NodeIdPairAllocator = nullptr;
}

Allocators &operator=(Allocators &&O) XRAY_NEVER_INSTRUMENT {


if (NodeAllocator)
NodeAllocator->~NodeAllocatorType();
if (O.NodeAllocator) {
new (&NodeAllocatorStorage)
NodeAllocatorType(std::move(*O.NodeAllocator));
NodeAllocator =
reinterpret_cast<NodeAllocatorType *>(&NodeAllocatorStorage);
O.NodeAllocator = nullptr;
} else {
NodeAllocator = nullptr;
}

if (RootAllocator)
RootAllocator->~RootAllocatorType();
if (O.RootAllocator) {
new (&RootAllocatorStorage)
RootAllocatorType(std::move(*O.RootAllocator));
RootAllocator =
reinterpret_cast<RootAllocatorType *>(&RootAllocatorStorage);
O.RootAllocator = nullptr;
} else {
RootAllocator = nullptr;
}

if (ShadowStackAllocator)
ShadowStackAllocator->~ShadowStackAllocatorType();
if (O.ShadowStackAllocator) {
new (&ShadowStackAllocatorStorage)
ShadowStackAllocatorType(std::move(*O.ShadowStackAllocator));
ShadowStackAllocator = reinterpret_cast<ShadowStackAllocatorType *>(
&ShadowStackAllocatorStorage);
O.ShadowStackAllocator = nullptr;
} else {
ShadowStackAllocator = nullptr;
}

if (NodeIdPairAllocator)
NodeIdPairAllocator->~NodeIdPairAllocatorType();
if (O.NodeIdPairAllocator) {
new (&NodeIdPairAllocatorStorage)
NodeIdPairAllocatorType(std::move(*O.NodeIdPairAllocator));
NodeIdPairAllocator = reinterpret_cast<NodeIdPairAllocatorType *>(
&NodeIdPairAllocatorStorage);
O.NodeIdPairAllocator = nullptr;
} else {
NodeIdPairAllocator = nullptr;
}

return *this;
}

~Allocators() XRAY_NEVER_INSTRUMENT {
if (NodeAllocator != nullptr)
NodeAllocator->~NodeAllocatorType();
if (RootAllocator != nullptr)
RootAllocator->~RootAllocatorType();
if (ShadowStackAllocator != nullptr)
ShadowStackAllocator->~ShadowStackAllocatorType();
if (NodeIdPairAllocator != nullptr)
NodeIdPairAllocator->~NodeIdPairAllocatorType();
}
};

static Allocators InitAllocators() XRAY_NEVER_INSTRUMENT {
return InitAllocatorsCustom(profilingFlags()->per_thread_allocator_max);
}

static Allocators InitAllocatorsCustom(uptr Max) XRAY_NEVER_INSTRUMENT {
Allocators A(Max);
return A;
}

static Allocators
InitAllocatorsFromBuffers(Allocators::Buffers &Bufs) XRAY_NEVER_INSTRUMENT {
Allocators A(Bufs);
return A;
}

private:
NodeArray Nodes;
RootArray Roots;
ShadowStackArray ShadowStack;
NodeIdPairAllocatorType *NodeIdPairAllocator;
uint32_t OverflowedFunctions;

public:
explicit FunctionCallTrie(const Allocators &A) XRAY_NEVER_INSTRUMENT
: Nodes(*A.NodeAllocator),
Roots(*A.RootAllocator),
ShadowStack(*A.ShadowStackAllocator),
NodeIdPairAllocator(A.NodeIdPairAllocator),
OverflowedFunctions(0) {}

FunctionCallTrie() = delete;
FunctionCallTrie(const FunctionCallTrie &) = delete;
FunctionCallTrie &operator=(const FunctionCallTrie &) = delete;

FunctionCallTrie(FunctionCallTrie &&O) XRAY_NEVER_INSTRUMENT
: Nodes(std::move(O.Nodes)),
Roots(std::move(O.Roots)),
ShadowStack(std::move(O.ShadowStack)),
NodeIdPairAllocator(O.NodeIdPairAllocator),
OverflowedFunctions(O.OverflowedFunctions) {}

FunctionCallTrie &operator=(FunctionCallTrie &&O) XRAY_NEVER_INSTRUMENT {
Nodes = std::move(O.Nodes);
Roots = std::move(O.Roots);
ShadowStack = std::move(O.ShadowStack);
NodeIdPairAllocator = O.NodeIdPairAllocator;
OverflowedFunctions = O.OverflowedFunctions;
return *this;
}

~FunctionCallTrie() XRAY_NEVER_INSTRUMENT {}

void enterFunction(const int32_t FId, uint64_t TSC,
uint16_t CPU) XRAY_NEVER_INSTRUMENT {
DCHECK_NE(FId, 0);



if (UNLIKELY(OverflowedFunctions)) {
++OverflowedFunctions;
return;
}



if (UNLIKELY(ShadowStack.empty())) {
auto *NewRoot = Nodes.AppendEmplace(
nullptr, NodeIdPairArray(*NodeIdPairAllocator), 0u, 0u, FId);
if (UNLIKELY(NewRoot == nullptr))
return;
if (Roots.AppendEmplace(NewRoot) == nullptr) {
Nodes.trim(1);
return;
}
if (ShadowStack.AppendEmplace(TSC, NewRoot, CPU) == nullptr) {
Nodes.trim(1);
Roots.trim(1);
++OverflowedFunctions;
return;
}
return;
}


DCHECK(!ShadowStack.empty());
auto TopNode = ShadowStack.back().NodePtr;
DCHECK_NE(TopNode, nullptr);



auto* Callee = TopNode->Callees.find_element(
[FId](const NodeIdPair &NR) { return NR.FId == FId; });
if (Callee != nullptr) {
CHECK_NE(Callee->NodePtr, nullptr);
if (ShadowStack.AppendEmplace(TSC, Callee->NodePtr, CPU) == nullptr)
++OverflowedFunctions;
return;
}


auto* NewNode = Nodes.AppendEmplace(
TopNode, NodeIdPairArray(*NodeIdPairAllocator), 0u, 0u, FId);
if (UNLIKELY(NewNode == nullptr))
return;
DCHECK_NE(NewNode, nullptr);
TopNode->Callees.AppendEmplace(NewNode, FId);
if (ShadowStack.AppendEmplace(TSC, NewNode, CPU) == nullptr)
++OverflowedFunctions;
return;
}

void exitFunction(int32_t FId, uint64_t TSC,
uint16_t CPU) XRAY_NEVER_INSTRUMENT {


if (OverflowedFunctions) {
--OverflowedFunctions;
return;
}





uint64_t CumulativeTreeTime = 0;

while (!ShadowStack.empty()) {
const auto &Top = ShadowStack.back();
auto TopNode = Top.NodePtr;
DCHECK_NE(TopNode, nullptr);














uint64_t LocalTime =
Top.EntryTSC > TSC
? (std::numeric_limits<uint64_t>::max() - Top.EntryTSC) + TSC
: TSC - Top.EntryTSC;
TopNode->CallCount++;
TopNode->CumulativeLocalTime += LocalTime - CumulativeTreeTime;
CumulativeTreeTime += LocalTime;
ShadowStack.trim(1);


if (TopNode->FId == FId)
break;
}
}

const RootArray &getRoots() const XRAY_NEVER_INSTRUMENT { return Roots; }














void deepCopyInto(FunctionCallTrie &O) const XRAY_NEVER_INSTRUMENT {
DCHECK(O.getRoots().empty());



struct NodeAndParent {
FunctionCallTrie::Node *Node;
FunctionCallTrie::Node *NewNode;
};
using Stack = Array<NodeAndParent>;

typename Stack::AllocatorType StackAllocator(
profilingFlags()->stack_allocator_max);
Stack DFSStack(StackAllocator);

for (const auto Root : getRoots()) {

auto NewRoot = O.Nodes.AppendEmplace(
nullptr, NodeIdPairArray(*O.NodeIdPairAllocator), Root->CallCount,
Root->CumulativeLocalTime, Root->FId);


if (UNLIKELY(NewRoot == nullptr))
return;

if (UNLIKELY(O.Roots.Append(NewRoot) == nullptr))
return;



if (DFSStack.AppendEmplace(Root, NewRoot) == nullptr)
return;
while (!DFSStack.empty()) {
NodeAndParent NP = DFSStack.back();
DCHECK_NE(NP.Node, nullptr);
DCHECK_NE(NP.NewNode, nullptr);
DFSStack.trim(1);
for (const auto Callee : NP.Node->Callees) {
auto NewNode = O.Nodes.AppendEmplace(
NP.NewNode, NodeIdPairArray(*O.NodeIdPairAllocator),
Callee.NodePtr->CallCount, Callee.NodePtr->CumulativeLocalTime,
Callee.FId);
if (UNLIKELY(NewNode == nullptr))
return;
if (UNLIKELY(NP.NewNode->Callees.AppendEmplace(NewNode, Callee.FId) ==
nullptr))
return;
if (UNLIKELY(DFSStack.AppendEmplace(Callee.NodePtr, NewNode) ==
nullptr))
return;
}
}
}
}









void mergeInto(FunctionCallTrie &O) const XRAY_NEVER_INSTRUMENT {
struct NodeAndTarget {
FunctionCallTrie::Node *OrigNode;
FunctionCallTrie::Node *TargetNode;
};
using Stack = Array<NodeAndTarget>;
typename Stack::AllocatorType StackAllocator(
profilingFlags()->stack_allocator_max);
Stack DFSStack(StackAllocator);

for (const auto Root : getRoots()) {
Node *TargetRoot = nullptr;
auto R = O.Roots.find_element(
[&](const Node *Node) { return Node->FId == Root->FId; });
if (R == nullptr) {
TargetRoot = O.Nodes.AppendEmplace(
nullptr, NodeIdPairArray(*O.NodeIdPairAllocator), 0u, 0u,
Root->FId);
if (UNLIKELY(TargetRoot == nullptr))
return;

O.Roots.Append(TargetRoot);
} else {
TargetRoot = *R;
}

DFSStack.AppendEmplace(Root, TargetRoot);
while (!DFSStack.empty()) {
NodeAndTarget NT = DFSStack.back();
DCHECK_NE(NT.OrigNode, nullptr);
DCHECK_NE(NT.TargetNode, nullptr);
DFSStack.trim(1);

NT.TargetNode->CallCount += NT.OrigNode->CallCount;
NT.TargetNode->CumulativeLocalTime += NT.OrigNode->CumulativeLocalTime;
for (const auto Callee : NT.OrigNode->Callees) {
auto TargetCallee = NT.TargetNode->Callees.find_element(
[&](const FunctionCallTrie::NodeIdPair &C) {
return C.FId == Callee.FId;
});
if (TargetCallee == nullptr) {
auto NewTargetNode = O.Nodes.AppendEmplace(
NT.TargetNode, NodeIdPairArray(*O.NodeIdPairAllocator), 0u, 0u,
Callee.FId);

if (UNLIKELY(NewTargetNode == nullptr))
return;

TargetCallee =
NT.TargetNode->Callees.AppendEmplace(NewTargetNode, Callee.FId);
}
DFSStack.AppendEmplace(Callee.NodePtr, TargetCallee->NodePtr);
}
}
}
}
};

}

#endif
