







#if !defined(LLDB_BREAKPOINT_STOPPOINT_HIT_COUNTER_H)
#define LLDB_BREAKPOINT_STOPPOINT_HIT_COUNTER_H

#include <cassert>
#include <cstdint>
#include <limits>

#include "lldb/Utility/LLDBAssert.h"

namespace lldb_private {

class StoppointHitCounter {
public:
uint32_t GetValue() const { return m_hit_count; }

void Increment(uint32_t difference = 1) {
lldbassert(std::numeric_limits<uint32_t>::max() - m_hit_count >= difference);
m_hit_count += difference;
}

void Decrement(uint32_t difference = 1) {
lldbassert(m_hit_count >= difference);
m_hit_count -= difference;
}

void Reset() { m_hit_count = 0; }

private:

uint32_t m_hit_count = 0;
};

}

#endif
