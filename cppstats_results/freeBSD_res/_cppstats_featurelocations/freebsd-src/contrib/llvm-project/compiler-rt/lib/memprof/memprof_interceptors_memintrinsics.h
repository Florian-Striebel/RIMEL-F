











#if !defined(MEMPROF_MEMINTRIN_H)
#define MEMPROF_MEMINTRIN_H

#include "interception/interception.h"
#include "memprof_interface_internal.h"
#include "memprof_internal.h"
#include "memprof_mapping.h"

DECLARE_REAL(void *, memcpy, void *to, const void *from, uptr size)
DECLARE_REAL(void *, memset, void *block, int c, uptr size)

namespace __memprof {





#define ACCESS_MEMORY_RANGE(offset, size) do { __memprof_record_access_range(offset, size); } while (0)







#define MEMPROF_MEMCPY_IMPL(to, from, size) do { if (UNLIKELY(!memprof_inited)) return internal_memcpy(to, from, size); if (memprof_init_is_running) { return REAL(memcpy)(to, from, size); } ENSURE_MEMPROF_INITED(); MEMPROF_READ_RANGE(from, size); MEMPROF_WRITE_RANGE(to, size); return REAL(memcpy)(to, from, size); } while (0)













#define MEMPROF_MEMSET_IMPL(block, c, size) do { if (UNLIKELY(!memprof_inited)) return internal_memset(block, c, size); if (memprof_init_is_running) { return REAL(memset)(block, c, size); } ENSURE_MEMPROF_INITED(); MEMPROF_WRITE_RANGE(block, size); return REAL(memset)(block, c, size); } while (0)











#define MEMPROF_MEMMOVE_IMPL(to, from, size) do { if (UNLIKELY(!memprof_inited)) return internal_memmove(to, from, size); ENSURE_MEMPROF_INITED(); MEMPROF_READ_RANGE(from, size); MEMPROF_WRITE_RANGE(to, size); return internal_memmove(to, from, size); } while (0)









#define MEMPROF_READ_RANGE(offset, size) ACCESS_MEMORY_RANGE(offset, size)
#define MEMPROF_WRITE_RANGE(offset, size) ACCESS_MEMORY_RANGE(offset, size)

}

#endif
