
























#if !defined(ATF_C_TP_H)
#define ATF_C_TP_H

#include <stdbool.h>

#include <atf-c/error_fwd.h>

struct atf_tc;





struct atf_tp_impl;
struct atf_tp {
struct atf_tp_impl *pimpl;
};
typedef struct atf_tp atf_tp_t;


atf_error_t atf_tp_init(atf_tp_t *, const char *const *);
void atf_tp_fini(atf_tp_t *);


char **atf_tp_get_config(const atf_tp_t *);
bool atf_tp_has_tc(const atf_tp_t *, const char *);
const struct atf_tc *atf_tp_get_tc(const atf_tp_t *, const char *);
const struct atf_tc *const *atf_tp_get_tcs(const atf_tp_t *);


atf_error_t atf_tp_add_tc(atf_tp_t *, struct atf_tc *);





atf_error_t atf_tp_run(const atf_tp_t *, const char *, const char *);
atf_error_t atf_tp_cleanup(const atf_tp_t *, const char *);

#endif
