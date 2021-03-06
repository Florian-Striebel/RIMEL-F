






















#if !defined(__UCL_HASH_H)
#define __UCL_HASH_H

#include "ucl.h"



struct ucl_hash_node_s;
typedef struct ucl_hash_node_s ucl_hash_node_t;

typedef int (*ucl_hash_cmp_func) (const void* void_a, const void* void_b);
typedef void (*ucl_hash_free_func) (void *ptr);
typedef void* ucl_hash_iter_t;





struct ucl_hash_struct;
typedef struct ucl_hash_struct ucl_hash_t;





ucl_hash_t* ucl_hash_create (bool ignore_case);




void ucl_hash_destroy (ucl_hash_t* hashlin, ucl_hash_free_func func);





bool ucl_hash_insert (ucl_hash_t* hashlin, const ucl_object_t *obj, const char *key,
unsigned keylen);




void ucl_hash_replace (ucl_hash_t* hashlin, const ucl_object_t *old,
const ucl_object_t *new);




void ucl_hash_delete (ucl_hash_t* hashlin, const ucl_object_t *obj);




const ucl_object_t* ucl_hash_search (ucl_hash_t* hashlin, const char *key,
unsigned keylen);









const void* ucl_hash_iterate2 (ucl_hash_t *hashlin, ucl_hash_iter_t *iter, int *ep);




#define ucl_hash_iterate(hl, ip) ucl_hash_iterate2((hl), (ip), NULL)




bool ucl_hash_iter_has_next (ucl_hash_t *hashlin, ucl_hash_iter_t iter);






bool ucl_hash_reserve (ucl_hash_t *hashlin, size_t sz);

void ucl_hash_sort (ucl_hash_t *hashlin, enum ucl_object_keys_sort_flags fl);

#endif
