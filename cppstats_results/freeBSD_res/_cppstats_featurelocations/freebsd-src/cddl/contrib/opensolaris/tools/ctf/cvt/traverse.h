

























#if !defined(_TRAVERSE_H)
#define _TRAVERSE_H

#pragma ident "%Z%%M% %I% %E% SMI"






#if defined(__cplusplus)
extern "C" {
#endif

#include "ctftools.h"

typedef int (*tdtrav_cb_f)(tdesc_t *, tdesc_t **, void *);

typedef struct tdtrav_data {
int vgen;

tdtrav_cb_f *firstops;
tdtrav_cb_f *preops;
tdtrav_cb_f *postops;

void *private;
} tdtrav_data_t;

void tdtrav_init(tdtrav_data_t *, int *, tdtrav_cb_f *, tdtrav_cb_f *,
tdtrav_cb_f *, void *);
int tdtraverse(tdesc_t *, tdesc_t **, tdtrav_data_t *);

int iitraverse(iidesc_t *, int *, tdtrav_cb_f *, tdtrav_cb_f *, tdtrav_cb_f *,
void *);
int iitraverse_hash(hash_t *, int *, tdtrav_cb_f *, tdtrav_cb_f *,
tdtrav_cb_f *, void *);
int iitraverse_td(void *, void *);

int tdtrav_assert(tdesc_t *, tdesc_t **, void *);

#if defined(__cplusplus)
}
#endif

#endif
