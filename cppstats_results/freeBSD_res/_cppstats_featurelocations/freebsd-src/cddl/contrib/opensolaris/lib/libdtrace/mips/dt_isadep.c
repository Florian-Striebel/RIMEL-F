

























#pragma ident "%Z%%M% %I% %E% SMI"

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

#include <dt_impl.h>
#include <dt_pid.h>


int
dt_pid_create_entry_probe(struct ps_prochandle *P, dtrace_hdl_t *dtp,
fasttrap_probe_spec_t *ftp, const GElf_Sym *symp)
{

dt_dprintf("%s: unimplemented\n", __func__);
return (DT_PROC_ERR);
}

int
dt_pid_create_return_probe(struct ps_prochandle *P, dtrace_hdl_t *dtp,
fasttrap_probe_spec_t *ftp, const GElf_Sym *symp, uint64_t *stret)
{

dt_dprintf("%s: unimplemented\n", __func__);
return (DT_PROC_ERR);
}


int
dt_pid_create_offset_probe(struct ps_prochandle *P, dtrace_hdl_t *dtp,
fasttrap_probe_spec_t *ftp, const GElf_Sym *symp, ulong_t off)
{

dt_dprintf("%s: unimplemented\n", __func__);
return (DT_PROC_ERR);
}


int
dt_pid_create_glob_offset_probes(struct ps_prochandle *P, dtrace_hdl_t *dtp,
fasttrap_probe_spec_t *ftp, const GElf_Sym *symp, const char *pattern)
{

dt_dprintf("%s: unimplemented\n", __func__);
return (DT_PROC_ERR);
}
