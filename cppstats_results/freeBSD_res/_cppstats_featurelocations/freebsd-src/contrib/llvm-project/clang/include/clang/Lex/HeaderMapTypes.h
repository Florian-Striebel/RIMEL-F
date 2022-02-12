







#if !defined(LLVM_CLANG_LEX_HEADERMAPTYPES_H)
#define LLVM_CLANG_LEX_HEADERMAPTYPES_H

#include <cstdint>

namespace clang {

enum {
HMAP_HeaderMagicNumber = ('h' << 24) | ('m' << 16) | ('a' << 8) | 'p',
HMAP_HeaderVersion = 1,
HMAP_EmptyBucketKey = 0
};

struct HMapBucket {
uint32_t Key;
uint32_t Prefix;
uint32_t Suffix;
};

struct HMapHeader {
uint32_t Magic;
uint16_t Version;
uint16_t Reserved;
uint32_t StringsOffset;
uint32_t NumEntries;
uint32_t NumBuckets;
uint32_t MaxValueLength;


};

}

#endif
