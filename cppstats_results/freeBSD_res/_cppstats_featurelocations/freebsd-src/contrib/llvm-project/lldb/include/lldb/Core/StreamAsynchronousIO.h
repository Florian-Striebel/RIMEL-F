







#if !defined(LLDB_CORE_STREAMASYNCHRONOUSIO_H)
#define LLDB_CORE_STREAMASYNCHRONOUSIO_H

#include "lldb/Utility/Stream.h"

#include <string>

#include <cstddef>

namespace lldb_private {
class Debugger;

class StreamAsynchronousIO : public Stream {
public:
StreamAsynchronousIO(Debugger &debugger, bool for_stdout);

~StreamAsynchronousIO() override;

void Flush() override;

protected:
size_t WriteImpl(const void *src, size_t src_len) override;

private:
Debugger &m_debugger;
std::string m_data;
bool m_for_stdout;
};

}

#endif
