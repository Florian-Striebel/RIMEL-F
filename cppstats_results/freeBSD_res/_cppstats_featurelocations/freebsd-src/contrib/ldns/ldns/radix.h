









































#if !defined(LDNS_RADIX_H_)
#define LDNS_RADIX_H_

#include <ldns/error.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef uint16_t radix_strlen_t;
typedef struct ldns_radix_array_t ldns_radix_array_t;
typedef struct ldns_radix_node_t ldns_radix_node_t;
typedef struct ldns_radix_t ldns_radix_t;


struct ldns_radix_array_t {

uint8_t* str;

radix_strlen_t len;

ldns_radix_node_t* edge;
};


struct ldns_radix_node_t {

uint8_t* key;

radix_strlen_t klen;

void* data;

ldns_radix_node_t* parent;

uint8_t parent_index;

uint16_t len;

uint16_t offset;

uint16_t capacity;

ldns_radix_array_t* array;
};


struct ldns_radix_t {

ldns_radix_node_t* root;

size_t count;
};






ldns_radix_t* ldns_radix_create(void);






void ldns_radix_init(ldns_radix_t* tree);






void ldns_radix_free(ldns_radix_t* tree);










ldns_status ldns_radix_insert(ldns_radix_t* tree, uint8_t* key,
radix_strlen_t len, void* data);









void* ldns_radix_delete(ldns_radix_t* tree, const uint8_t* key, radix_strlen_t len);









ldns_radix_node_t* ldns_radix_search(ldns_radix_t* tree, const uint8_t* key,
radix_strlen_t len);












int ldns_radix_find_less_equal(ldns_radix_t* tree, const uint8_t* key,
radix_strlen_t len, ldns_radix_node_t** result);







ldns_radix_node_t* ldns_radix_first(const ldns_radix_t* tree);







ldns_radix_node_t* ldns_radix_last(const ldns_radix_t* tree);







ldns_radix_node_t* ldns_radix_next(ldns_radix_node_t* node);







ldns_radix_node_t* ldns_radix_prev(ldns_radix_node_t* node);









ldns_status ldns_radix_split(ldns_radix_t* tree1, size_t num,
ldns_radix_t** tree2);








ldns_status ldns_radix_join(ldns_radix_t* tree1, ldns_radix_t* tree2);









void ldns_radix_traverse_postorder(ldns_radix_node_t* node,
void (*func)(ldns_radix_node_t*, void*), void* arg);







void ldns_radix_printf(FILE* fd, const ldns_radix_t* tree);

#if defined(__cplusplus)
}
#endif

#endif
