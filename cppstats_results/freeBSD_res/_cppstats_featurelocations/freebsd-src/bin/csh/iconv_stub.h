



























#if !defined(_ICONV_H_)
#define _ICONV_H_

typedef void *iconv_t;
typedef size_t dl_iconv_t(iconv_t, char **, size_t *, char **, size_t *);
typedef int dl_iconv_close_t(iconv_t);

extern iconv_t dl_iconv_open(const char *, const char *);
extern dl_iconv_t *dl_iconv;
extern dl_iconv_close_t *dl_iconv_close;

#define iconv_open dl_iconv_open
#define iconv dl_iconv
#define iconv_close dl_iconv_close

#endif
