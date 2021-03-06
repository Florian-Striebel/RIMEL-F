






























#if !defined(LIB9P_SBUF_H)
#define LIB9P_SBUF_H

#include <stdarg.h>

struct sbuf
{
char *s_buf;
int s_size;
int s_capacity;
int s_position;
};

struct sbuf *sbuf_new_auto(void);
int sbuf_cat(struct sbuf *s, const char *str);
int sbuf_printf(struct sbuf *s, const char *fmt, ...);
int sbuf_vprintf(struct sbuf *s, const char *fmt, va_list args);
int sbuf_done(struct sbuf *s);
void sbuf_delete(struct sbuf *s);
int sbuf_finish(struct sbuf *s);
char *sbuf_data(struct sbuf *s);

#endif

