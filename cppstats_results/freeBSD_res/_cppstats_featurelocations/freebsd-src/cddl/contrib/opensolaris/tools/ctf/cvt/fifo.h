

























#if !defined(_FIFO_H)
#define _FIFO_H

#pragma ident "%Z%%M% %I% %E% SMI"





#if defined(__cplusplus)
extern "C" {
#endif

typedef struct fifo fifo_t;

extern fifo_t *fifo_new(void);
extern void fifo_add(fifo_t *, void *);
extern void *fifo_remove(fifo_t *);
extern void fifo_free(fifo_t *, void (*)(void *));
extern int fifo_len(fifo_t *);
extern int fifo_empty(fifo_t *);
extern int fifo_iter(fifo_t *, int (*)(void *, void *), void *);

#if defined(__cplusplus)
}
#endif

#endif
