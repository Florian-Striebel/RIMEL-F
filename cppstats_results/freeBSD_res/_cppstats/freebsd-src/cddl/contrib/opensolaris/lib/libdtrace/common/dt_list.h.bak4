

























#if !defined(_DT_LIST_H)
#define _DT_LIST_H

#pragma ident "%Z%%M% %I% %E% SMI"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct dt_list {
struct dt_list *dl_prev;
struct dt_list *dl_next;
} dt_list_t;

#define dt_list_prev(elem) ((void *)(((dt_list_t *)(elem))->dl_prev))
#define dt_list_next(elem) ((void *)(((dt_list_t *)(elem))->dl_next))

extern void dt_list_append(dt_list_t *, void *);
extern void dt_list_prepend(dt_list_t *, void *);
extern void dt_list_insert(dt_list_t *, void *, void *);
extern void dt_list_delete(dt_list_t *, void *);

#if defined(__cplusplus)
}
#endif

#endif
