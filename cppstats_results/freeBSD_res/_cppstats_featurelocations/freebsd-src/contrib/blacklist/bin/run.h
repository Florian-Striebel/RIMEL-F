





























#if !defined(_RUN_H)
#define _RUN_H

__BEGIN_DECLS
struct conf;
void run_flush(const struct conf *);
struct sockaddr_storage;
int run_change(const char *, const struct conf *, char *, size_t);
__END_DECLS

#endif
