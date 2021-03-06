


























#if !defined(SANITIZER_TLS_GET_ADDR_H)
#define SANITIZER_TLS_GET_ADDR_H

#include "sanitizer_atomic.h"
#include "sanitizer_common.h"

namespace __sanitizer {

struct DTLS {


struct DTV {
uptr beg, size;
};
struct DTVBlock {
atomic_uintptr_t next;
DTV dtvs[(4096UL - sizeof(next)) / sizeof(DTLS::DTV)];
};

static_assert(sizeof(DTVBlock) <= 4096UL, "Unexpected block size");

atomic_uintptr_t dtv_block;


uptr last_memalign_size;
uptr last_memalign_ptr;
};

template <typename Fn>
void ForEachDVT(DTLS *dtls, const Fn &fn) {
DTLS::DTVBlock *block =
(DTLS::DTVBlock *)atomic_load(&dtls->dtv_block, memory_order_acquire);
while (block) {
int id = 0;
for (auto &d : block->dtvs) fn(d, id++);
block = (DTLS::DTVBlock *)atomic_load(&block->next, memory_order_acquire);
}
}



DTLS::DTV *DTLS_on_tls_get_addr(void *arg, void *res, uptr static_tls_begin,
uptr static_tls_end);
void DTLS_on_libc_memalign(void *ptr, uptr size);
DTLS *DTLS_Get();
void DTLS_Destroy();

bool DTLSInDestruction(DTLS *dtls);

}

#endif
