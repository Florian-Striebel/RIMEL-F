




























#if !defined(_DT_MODULE_H)
#define _DT_MODULE_H

#include <dt_impl.h>

#if defined(__cplusplus)
extern "C" {
#endif

extern dt_module_t *dt_module_create(dtrace_hdl_t *, const char *);
extern int dt_module_load(dtrace_hdl_t *, dt_module_t *);
extern void dt_module_unload(dtrace_hdl_t *, dt_module_t *);
extern void dt_module_destroy(dtrace_hdl_t *, dt_module_t *);

extern dt_module_t *dt_module_lookup_by_name(dtrace_hdl_t *, const char *);
extern dt_module_t *dt_module_lookup_by_ctf(dtrace_hdl_t *, ctf_file_t *);

#if defined(__FreeBSD__)
extern dt_kmodule_t *dt_kmodule_lookup(dtrace_hdl_t *, const char *);
#endif

extern int dt_module_hasctf(dtrace_hdl_t *, dt_module_t *);
extern ctf_file_t *dt_module_getctf(dtrace_hdl_t *, dt_module_t *);
extern dt_ident_t *dt_module_extern(dtrace_hdl_t *, dt_module_t *,
const char *, const dtrace_typeinfo_t *);

extern const char *dt_module_modelname(dt_module_t *);
extern int dt_module_getlibid(dtrace_hdl_t *, dt_module_t *,
const ctf_file_t *);
extern ctf_file_t *dt_module_getctflib(dtrace_hdl_t *, dt_module_t *,
const char *);

#if defined(__cplusplus)
}
#endif

#endif
