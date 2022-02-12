
























#if !defined(ATF_C_DETAIL_MAP_H)
#define ATF_C_DETAIL_MAP_H

#include <stdarg.h>
#include <stdbool.h>

#include <atf-c/detail/list.h>
#include <atf-c/error_fwd.h>





struct atf_map_citer {
const struct atf_map *m_map;
const void *m_entry;
atf_list_citer_t m_listiter;
};
typedef struct atf_map_citer atf_map_citer_t;


const char *atf_map_citer_key(const atf_map_citer_t);
const void *atf_map_citer_data(const atf_map_citer_t);
atf_map_citer_t atf_map_citer_next(const atf_map_citer_t);


bool atf_equal_map_citer_map_citer(const atf_map_citer_t,
const atf_map_citer_t);





struct atf_map_iter {
struct atf_map *m_map;
void *m_entry;
atf_list_iter_t m_listiter;
};
typedef struct atf_map_iter atf_map_iter_t;


const char *atf_map_iter_key(const atf_map_iter_t);
void *atf_map_iter_data(const atf_map_iter_t);
atf_map_iter_t atf_map_iter_next(const atf_map_iter_t);


bool atf_equal_map_iter_map_iter(const atf_map_iter_t,
const atf_map_iter_t);







struct atf_map {
atf_list_t m_list;
};
typedef struct atf_map atf_map_t;


atf_error_t atf_map_init(atf_map_t *);
atf_error_t atf_map_init_charpp(atf_map_t *, const char *const *);
void atf_map_fini(atf_map_t *);


atf_map_iter_t atf_map_begin(atf_map_t *);
atf_map_citer_t atf_map_begin_c(const atf_map_t *);
atf_map_iter_t atf_map_end(atf_map_t *);
atf_map_citer_t atf_map_end_c(const atf_map_t *);
atf_map_iter_t atf_map_find(atf_map_t *, const char *);
atf_map_citer_t atf_map_find_c(const atf_map_t *, const char *);
size_t atf_map_size(const atf_map_t *);
char **atf_map_to_charpp(const atf_map_t *);


atf_error_t atf_map_insert(atf_map_t *, const char *, void *, bool);


#define atf_map_for_each(iter, map) for (iter = atf_map_begin(map); !atf_equal_map_iter_map_iter((iter), atf_map_end(map)); iter = atf_map_iter_next(iter))



#define atf_map_for_each_c(iter, map) for (iter = atf_map_begin_c(map); !atf_equal_map_citer_map_citer((iter), atf_map_end_c(map)); iter = atf_map_citer_next(iter))




#endif
