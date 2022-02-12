

























#if !defined(ARCHIVE_XXHASH_H_INCLUDED)
#define ARCHIVE_XXHASH_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#error This header is only to be used internally to libarchive.
#endif


typedef enum { XXH_OK=0, XXH_ERROR } XXH_errorcode;

struct archive_xxhash {
unsigned int (*XXH32)(const void* input, unsigned int len,
unsigned int seed);
void* (*XXH32_init)(unsigned int seed);
XXH_errorcode (*XXH32_update)(void* state, const void* input,
unsigned int len);
unsigned int (*XXH32_digest)(void* state);
};

extern const struct archive_xxhash __archive_xxhash;

#endif
