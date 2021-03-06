

























#if !defined(_DT_STRTAB_H)
#define _DT_STRTAB_H

#pragma ident "%Z%%M% %I% %E% SMI"

#include <sys/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct dt_strhash {
const char *str_data;
ulong_t str_buf;
size_t str_off;
size_t str_len;
struct dt_strhash *str_next;
} dt_strhash_t;

typedef struct dt_strtab {
dt_strhash_t **str_hash;
ulong_t str_hashsz;
char **str_bufs;
char *str_ptr;
ulong_t str_nbufs;
size_t str_bufsz;
ulong_t str_nstrs;
size_t str_size;
} dt_strtab_t;

typedef ssize_t dt_strtab_write_f(const char *, size_t, size_t, void *);

extern dt_strtab_t *dt_strtab_create(size_t);
extern void dt_strtab_destroy(dt_strtab_t *);
extern boolean_t dt_strtab_empty(dt_strtab_t *);
extern ssize_t dt_strtab_index(dt_strtab_t *, const char *);
extern ssize_t dt_strtab_insert(dt_strtab_t *, const char *);
extern size_t dt_strtab_size(const dt_strtab_t *);
extern ssize_t dt_strtab_write(const dt_strtab_t *,
dt_strtab_write_f *, void *);
extern ulong_t dt_strtab_hash(const char *, size_t *);

#if defined(__cplusplus)
}
#endif

#endif
