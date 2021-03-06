

















#if !defined(XO_BUF_H)
#define XO_BUF_H

#define XO_BUFSIZ (8*1024)
#define XO_BUF_HIGH_WATER (XO_BUFSIZ - 512)




typedef struct xo_buffer_s {
char *xb_bufp;
char *xb_curp;
ssize_t xb_size;
} xo_buffer_t;




static inline void
xo_buf_init (xo_buffer_t *xbp)
{
xbp->xb_size = XO_BUFSIZ;
xbp->xb_bufp = xo_realloc(NULL, xbp->xb_size);
xbp->xb_curp = xbp->xb_bufp;
}




static inline void
xo_buf_reset (xo_buffer_t *xbp)
{
xbp->xb_curp = xbp->xb_bufp;
}




static inline int
xo_buf_left (xo_buffer_t *xbp)
{
return xbp->xb_size - (xbp->xb_curp - xbp->xb_bufp);
}




static inline int
xo_buf_is_empty (xo_buffer_t *xbp)
{
return (xbp->xb_curp == xbp->xb_bufp);
}




static inline ssize_t
xo_buf_offset (xo_buffer_t *xbp)
{
return xbp ? (xbp->xb_curp - xbp->xb_bufp) : 0;
}

static inline char *
xo_buf_data (xo_buffer_t *xbp, ssize_t offset)
{
if (xbp == NULL)
return NULL;
return xbp->xb_bufp + offset;
}

static inline char *
xo_buf_cur (xo_buffer_t *xbp)
{
if (xbp == NULL)
return NULL;
return xbp->xb_curp;
}




static inline void
xo_buf_cleanup (xo_buffer_t *xbp)
{
if (xbp->xb_bufp)
xo_free(xbp->xb_bufp);
bzero(xbp, sizeof(*xbp));
}






static inline int
xo_buf_has_room (xo_buffer_t *xbp, ssize_t len)
{
if (xbp->xb_curp + len >= xbp->xb_bufp + xbp->xb_size) {



ssize_t sz = (xbp->xb_curp + len) - xbp->xb_bufp;
sz = (sz + XO_BUFSIZ - 1) & ~(XO_BUFSIZ - 1);

char *bp = xo_realloc(xbp->xb_bufp, sz);
if (bp == NULL)
return 0;

xbp->xb_curp = bp + (xbp->xb_curp - xbp->xb_bufp);
xbp->xb_bufp = bp;
xbp->xb_size = sz;
}

return 1;
}




static inline void
xo_buf_append (xo_buffer_t *xbp, const char *str, ssize_t len)
{
if (str == NULL || len == 0 || !xo_buf_has_room(xbp, len))
return;

memcpy(xbp->xb_curp, str, len);
xbp->xb_curp += len;
}




static inline void
xo_buf_append_str (xo_buffer_t *xbp, const char *str)
{
ssize_t len = strlen(str);

if (!xo_buf_has_room(xbp, len))
return;

memcpy(xbp->xb_curp, str, len);
xbp->xb_curp += len;
}

#endif
