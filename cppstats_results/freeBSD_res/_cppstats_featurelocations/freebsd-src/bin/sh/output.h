




































#if !defined(OUTPUT_INCL)

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

struct output {
char *nextc;
char *bufend;
char *buf;
int bufsize;
short fd;
short flags;
};

extern struct output output;
extern struct output errout;
extern struct output memout;
extern struct output *out1;
extern struct output *out2;


void outcslow(int, struct output *);
void out1str(const char *);
void out1qstr(const char *);
void out2str(const char *);
void out2qstr(const char *);
void outstr(const char *, struct output *);
void outqstr(const char *, struct output *);
void outbin(const void *, size_t, struct output *);
void emptyoutbuf(struct output *);
void flushall(void);
void flushout(struct output *);
void freestdout(void);
int outiserror(struct output *);
void outclearerror(struct output *);
void outfmt(struct output *, const char *, ...) __printflike(2, 3);
void out1fmt(const char *, ...) __printflike(1, 2);
void out2fmt_flush(const char *, ...) __printflike(1, 2);
void fmtstr(char *, int, const char *, ...) __printflike(3, 4);
void doformat(struct output *, const char *, va_list) __printflike(2, 0);
FILE *out1fp(void);
int xwrite(int, const char *, int);

#define outc(c, file) ((file)->nextc == (file)->bufend ? (emptyoutbuf(file), *(file)->nextc++ = (c)) : (*(file)->nextc++ = (c)))
#define out1c(c) outc(c, out1);
#define out2c(c) outcslow(c, out2);

#define OUTPUT_INCL
#endif
