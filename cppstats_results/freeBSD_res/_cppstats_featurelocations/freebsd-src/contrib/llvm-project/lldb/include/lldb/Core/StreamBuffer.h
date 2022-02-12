







#if !defined(LLDB_CORE_STREAMBUFFER_H)
#define LLDB_CORE_STREAMBUFFER_H

#include "lldb/Utility/Stream.h"
#include "llvm/ADT/SmallVector.h"
#include <cstdio>
#include <string>

namespace lldb_private {

template <unsigned N> class StreamBuffer : public Stream {
public:
StreamBuffer() : Stream(0, 4, lldb::eByteOrderBig), m_packet() {}

StreamBuffer(uint32_t flags, uint32_t addr_size, lldb::ByteOrder byte_order)
: Stream(flags, addr_size, byte_order), m_packet() {}

~StreamBuffer() override = default;

void Flush() override {

}

void Clear() { m_packet.clear(); }





const char *GetData() const { return m_packet.data(); }

size_t GetSize() const { return m_packet.size(); }

protected:
llvm::SmallVector<char, N> m_packet;

size_t WriteImpl(const void *s, size_t length) override {
if (s && length)
m_packet.append((const char *)s, ((const char *)s) + length);
return length;
}
};

}

#endif
