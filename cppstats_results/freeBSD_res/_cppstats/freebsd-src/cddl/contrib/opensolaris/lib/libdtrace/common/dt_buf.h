#pragma ident "%Z%%M% %I% %E% SMI"
#if defined(__cplusplus)
extern "C" {
#endif
#include <dtrace.h>
typedef struct dt_buf {
const char *dbu_name;
uchar_t *dbu_buf;
uchar_t *dbu_ptr;
size_t dbu_len;
int dbu_err;
int dbu_resizes;
} dt_buf_t;
extern void dt_buf_create(dtrace_hdl_t *, dt_buf_t *, const char *, size_t);
extern void dt_buf_destroy(dtrace_hdl_t *, dt_buf_t *);
extern void dt_buf_reset(dtrace_hdl_t *, dt_buf_t *);
extern void dt_buf_write(dtrace_hdl_t *, dt_buf_t *,
const void *, size_t, size_t);
extern void dt_buf_concat(dtrace_hdl_t *, dt_buf_t *,
const dt_buf_t *, size_t);
extern size_t dt_buf_offset(const dt_buf_t *, size_t);
extern size_t dt_buf_len(const dt_buf_t *);
extern int dt_buf_error(const dt_buf_t *);
extern void *dt_buf_ptr(const dt_buf_t *);
extern void *dt_buf_claim(dtrace_hdl_t *, dt_buf_t *);
#if defined(__cplusplus)
}
#endif
