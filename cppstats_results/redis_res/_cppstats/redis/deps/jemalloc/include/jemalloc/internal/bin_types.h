#include "jemalloc/internal/sc.h"
#define BIN_SHARDS_MAX (1 << EXTENT_BITS_BINSHARD_WIDTH)
#define N_BIN_SHARDS_DEFAULT 1
#define TSD_BINSHARDS_ZERO_INITIALIZER {{UINT8_MAX}}
typedef struct tsd_binshards_s tsd_binshards_t;
struct tsd_binshards_s {
uint8_t binshard[SC_NBINS];
};
