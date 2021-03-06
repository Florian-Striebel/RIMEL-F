
























#if !defined(ATF_C_DETAIL_LIST_H)
#define ATF_C_DETAIL_LIST_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include <atf-c/error_fwd.h>





struct atf_list_citer {
const struct atf_list *m_list;
const void *m_entry;
};
typedef struct atf_list_citer atf_list_citer_t;


const void *atf_list_citer_data(const atf_list_citer_t);
atf_list_citer_t atf_list_citer_next(const atf_list_citer_t);


bool atf_equal_list_citer_list_citer(const atf_list_citer_t,
const atf_list_citer_t);





struct atf_list_iter {
struct atf_list *m_list;
void *m_entry;
};
typedef struct atf_list_iter atf_list_iter_t;


void *atf_list_iter_data(const atf_list_iter_t);
atf_list_iter_t atf_list_iter_next(const atf_list_iter_t);


bool atf_equal_list_iter_list_iter(const atf_list_iter_t,
const atf_list_iter_t);





struct atf_list {
void *m_begin;
void *m_end;

size_t m_size;
};
typedef struct atf_list atf_list_t;


atf_error_t atf_list_init(atf_list_t *);
void atf_list_fini(atf_list_t *);


atf_list_iter_t atf_list_begin(atf_list_t *);
atf_list_citer_t atf_list_begin_c(const atf_list_t *);
atf_list_iter_t atf_list_end(atf_list_t *);
atf_list_citer_t atf_list_end_c(const atf_list_t *);
void *atf_list_index(atf_list_t *, const size_t);
const void *atf_list_index_c(const atf_list_t *, const size_t);
size_t atf_list_size(const atf_list_t *);
char **atf_list_to_charpp(const atf_list_t *);


atf_error_t atf_list_append(atf_list_t *, void *, bool);
void atf_list_append_list(atf_list_t *, atf_list_t *);


#define atf_list_for_each(iter, list) for (iter = atf_list_begin(list); !atf_equal_list_iter_list_iter((iter), atf_list_end(list)); iter = atf_list_iter_next(iter))



#define atf_list_for_each_c(iter, list) for (iter = atf_list_begin_c(list); !atf_equal_list_citer_list_citer((iter), atf_list_end_c(list)); iter = atf_list_citer_next(iter))




#endif
