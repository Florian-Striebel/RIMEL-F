









































#if !defined(LDNS_RBTREE_H_)
#define LDNS_RBTREE_H_

#if defined(__cplusplus)
extern "C" {
#endif







typedef struct ldns_rbnode_t ldns_rbnode_t;



struct ldns_rbnode_t {

ldns_rbnode_t *parent;

ldns_rbnode_t *left;

ldns_rbnode_t *right;

const void *key;

const void *data;

uint8_t color;
};


#define LDNS_RBTREE_NULL &ldns_rbtree_null_node

extern ldns_rbnode_t ldns_rbtree_null_node;


typedef struct ldns_rbtree_t ldns_rbtree_t;

struct ldns_rbtree_t {

ldns_rbnode_t *root;


size_t count;





int (*cmp) (const void *, const void *);
};






ldns_rbtree_t *ldns_rbtree_create(int (*cmpf)(const void *, const void *));





void ldns_rbtree_free(ldns_rbtree_t *rbtree);






void ldns_rbtree_init(ldns_rbtree_t *rbtree, int (*cmpf)(const void *, const void *));







ldns_rbnode_t *ldns_rbtree_insert(ldns_rbtree_t *rbtree, ldns_rbnode_t *data);







void ldns_rbtree_insert_vref(ldns_rbnode_t *data, void *rbtree);








ldns_rbnode_t *ldns_rbtree_delete(ldns_rbtree_t *rbtree, const void *key);







ldns_rbnode_t *ldns_rbtree_search(ldns_rbtree_t *rbtree, const void *key);










int ldns_rbtree_find_less_equal(ldns_rbtree_t *rbtree, const void *key,
ldns_rbnode_t **result);






ldns_rbnode_t *ldns_rbtree_first(const ldns_rbtree_t *rbtree);






ldns_rbnode_t *ldns_rbtree_last(const ldns_rbtree_t *rbtree);






ldns_rbnode_t *ldns_rbtree_next(ldns_rbnode_t *rbtree);






ldns_rbnode_t *ldns_rbtree_previous(ldns_rbnode_t *rbtree);






ldns_rbtree_t *ldns_rbtree_split(ldns_rbtree_t *tree, size_t elements);





void ldns_rbtree_join(ldns_rbtree_t *tree1, ldns_rbtree_t *tree2);





#define LDNS_RBTREE_FOR(node, type, rbtree) for(node=(type)ldns_rbtree_first(rbtree); (ldns_rbnode_t*)node != LDNS_RBTREE_NULL; node = (type)ldns_rbtree_next((ldns_rbnode_t*)node))















void ldns_traverse_postorder(ldns_rbtree_t* tree,
void (*func)(ldns_rbnode_t*, void*), void* arg);

#if defined(__cplusplus)
}
#endif

#endif
