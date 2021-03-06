





























#if !defined(_DT_PID_H)
#define _DT_PID_H

#include <libproc.h>
#include <sys/fasttrap.h>
#include <dt_impl.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define DT_PROC_ERR (-1)
#define DT_PROC_ALIGN (-2)

extern int dt_pid_create_probes(dtrace_probedesc_t *, dtrace_hdl_t *,
dt_pcb_t *pcb);
extern int dt_pid_create_probes_module(dtrace_hdl_t *, dt_proc_t *);

extern int dt_pid_create_entry_probe(struct ps_prochandle *, dtrace_hdl_t *,
fasttrap_probe_spec_t *, const GElf_Sym *);

extern int dt_pid_create_return_probe(struct ps_prochandle *, dtrace_hdl_t *,
fasttrap_probe_spec_t *, const GElf_Sym *, uint64_t *);

extern int dt_pid_create_offset_probe(struct ps_prochandle *, dtrace_hdl_t *,
fasttrap_probe_spec_t *, const GElf_Sym *, ulong_t);

extern int dt_pid_create_glob_offset_probes(struct ps_prochandle *,
dtrace_hdl_t *, fasttrap_probe_spec_t *, const GElf_Sym *, const char *);

extern void dt_pid_get_types(dtrace_hdl_t *, const dtrace_probedesc_t *,
dtrace_argdesc_t *, int *);

#if defined(__cplusplus)
}
#endif

#endif
