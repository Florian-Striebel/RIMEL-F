







#if !defined(SCUDO_BYTEMAP_H_)
#define SCUDO_BYTEMAP_H_

#include "atomic_helpers.h"
#include "common.h"
#include "mutex.h"

namespace scudo {

template <uptr Size> class FlatByteMap {
public:
void init() { DCHECK(Size == 0 || Map[0] == 0); }

void unmapTestOnly() { memset(Map, 0, Size); }

void set(uptr Index, u8 Value) {
DCHECK_LT(Index, Size);
DCHECK_EQ(0U, Map[Index]);
Map[Index] = Value;
}
u8 operator[](uptr Index) {
DCHECK_LT(Index, Size);
return Map[Index];
}

void disable() {}
void enable() {}

private:
u8 Map[Size] = {};
};

}

#endif
