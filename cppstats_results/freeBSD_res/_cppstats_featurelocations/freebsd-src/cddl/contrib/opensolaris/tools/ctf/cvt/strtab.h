

























#if !defined(_STRTAB_H)
#define _STRTAB_H

#pragma ident "%Z%%M% %I% %E% SMI"

#include <sys/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct strhash {
const char *str_data;
ulong_t str_buf;
size_t str_off;
size_t str_len;
struct strhash *str_next;
} strhash_t;

typedef struct strtab {
strhash_t **str_hash;
ulong_t str_hashsz;
char **str_bufs;
char *str_ptr;
ulong_t str_nbufs;
size_t str_bufsz;
ulong_t str_nstrs;
size_t str_size;
} strtab_t;

extern void strtab_create(strtab_t *);
extern void strtab_destroy(strtab_t *);
extern size_t strtab_insert(strtab_t *, const char *);
extern size_t strtab_size(const strtab_t *);
extern ssize_t strtab_write(const strtab_t *,
ssize_t (*)(void *, size_t, void *), void *);
extern void strtab_print(const strtab_t *);

#if defined(__cplusplus)
}
#endif

#endif
