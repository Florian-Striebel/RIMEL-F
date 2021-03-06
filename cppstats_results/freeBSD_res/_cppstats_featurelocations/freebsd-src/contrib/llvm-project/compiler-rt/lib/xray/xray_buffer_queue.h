












#if !defined(XRAY_BUFFER_QUEUE_H)
#define XRAY_BUFFER_QUEUE_H

#include "sanitizer_common/sanitizer_atomic.h"
#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_mutex.h"
#include "xray_defs.h"
#include <cstddef>
#include <cstdint>

namespace __xray {






class BufferQueue {
public:









struct ControlBlock {
union {
atomic_uint64_t RefCount;
char Buffer[kCacheLineSize];
};




char Data[1];
};

struct Buffer {
atomic_uint64_t *Extents = nullptr;
uint64_t Generation{0};
void *Data = nullptr;
size_t Size = 0;

private:
friend class BufferQueue;
ControlBlock *BackingStore = nullptr;
ControlBlock *ExtentsBackingStore = nullptr;
size_t Count = 0;
};

struct BufferRep {

Buffer Buff;



bool Used = false;
};

private:



template <class T> class Iterator {
public:
BufferRep *Buffers = nullptr;
size_t Offset = 0;
size_t Max = 0;

Iterator &operator++() {
DCHECK_NE(Offset, Max);
do {
++Offset;
} while (!Buffers[Offset].Used && Offset != Max);
return *this;
}

Iterator operator++(int) {
Iterator C = *this;
++(*this);
return C;
}

T &operator*() const { return Buffers[Offset].Buff; }

T *operator->() const { return &(Buffers[Offset].Buff); }

Iterator(BufferRep *Root, size_t O, size_t M) XRAY_NEVER_INSTRUMENT
: Buffers(Root),
Offset(O),
Max(M) {


while (!Buffers[Offset].Used && Offset != Max) {
++Offset;
}
}

Iterator() = default;
Iterator(const Iterator &) = default;
Iterator(Iterator &&) = default;
Iterator &operator=(const Iterator &) = default;
Iterator &operator=(Iterator &&) = default;
~Iterator() = default;

template <class V>
friend bool operator==(const Iterator &L, const Iterator<V> &R) {
DCHECK_EQ(L.Max, R.Max);
return L.Buffers == R.Buffers && L.Offset == R.Offset;
}

template <class V>
friend bool operator!=(const Iterator &L, const Iterator<V> &R) {
return !(L == R);
}
};


size_t BufferSize;


size_t BufferCount;

SpinMutex Mutex;
atomic_uint8_t Finalizing;


ControlBlock *BackingStore;


ControlBlock *ExtentsBackingStore;


BufferRep *Buffers;


BufferRep *Next;



BufferRep *First;


size_t LiveBuffers;



atomic_uint64_t Generation;


void cleanupBuffers();

public:
enum class ErrorCode : unsigned {
Ok,
NotEnoughMemory,
QueueFinalizing,
UnrecognizedBuffer,
AlreadyFinalized,
AlreadyInitialized,
};

static const char *getErrorString(ErrorCode E) {
switch (E) {
case ErrorCode::Ok:
return "(none)";
case ErrorCode::NotEnoughMemory:
return "no available buffers in the queue";
case ErrorCode::QueueFinalizing:
return "queue already finalizing";
case ErrorCode::UnrecognizedBuffer:
return "buffer being returned not owned by buffer queue";
case ErrorCode::AlreadyFinalized:
return "queue already finalized";
case ErrorCode::AlreadyInitialized:
return "queue already initialized";
}
return "unknown error";
}



BufferQueue(size_t B, size_t N, bool &Success);













ErrorCode getBuffer(Buffer &Buf);







ErrorCode releaseBuffer(Buffer &Buf);








ErrorCode init(size_t BS, size_t BC);

bool finalizing() const {
return atomic_load(&Finalizing, memory_order_acquire);
}

uint64_t generation() const {
return atomic_load(&Generation, memory_order_acquire);
}


size_t ConfiguredBufferSize() const { return BufferSize; }








ErrorCode finalize();




template <class F> void apply(F Fn) XRAY_NEVER_INSTRUMENT {
SpinMutexLock G(&Mutex);
for (auto I = begin(), E = end(); I != E; ++I)
Fn(*I);
}

using const_iterator = Iterator<const Buffer>;
using iterator = Iterator<Buffer>;


iterator begin() const { return iterator(Buffers, 0, BufferCount); }
const_iterator cbegin() const {
return const_iterator(Buffers, 0, BufferCount);
}
iterator end() const { return iterator(Buffers, BufferCount, BufferCount); }
const_iterator cend() const {
return const_iterator(Buffers, BufferCount, BufferCount);
}


~BufferQueue();
};

}

#endif
