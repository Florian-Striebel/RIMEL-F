




























#pragma ident "%Z%%M% %I% %E% SMI"

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

#include <dt_impl.h>
#include <dt_pid.h>

#if !defined(sun)
#include <libproc_compat.h>
#endif


int
dt_pid_create_entry_probe(struct ps_prochandle *P, dtrace_hdl_t *dtp,
fasttrap_probe_spec_t *ftp, const GElf_Sym *symp)
{

ftp->ftps_type = DTFTP_ENTRY;
ftp->ftps_pc = (uintptr_t)symp->st_value;
ftp->ftps_size = (size_t)symp->st_size;
ftp->ftps_noffs = 1;
ftp->ftps_offs[0] = 0;

if (ioctl(dtp->dt_ftfd, FASTTRAPIOC_MAKEPROBE, ftp) != 0) {
dt_dprintf("fasttrap probe creation ioctl failed: %s\n",
strerror(errno));
return (dt_set_errno(dtp, errno));
}

return (1);
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

if (!ALIGNED_POINTER(off, 4))
return (DT_PROC_ALIGN);

ftp->ftps_type = DTFTP_OFFSETS;
ftp->ftps_pc = (uintptr_t)symp->st_value;
ftp->ftps_size = (size_t)symp->st_size;
ftp->ftps_noffs = 1;
ftp->ftps_offs[0] = off;

if (ioctl(dtp->dt_ftfd, FASTTRAPIOC_MAKEPROBE, ftp) != 0) {
dt_dprintf("fasttrap probe creation ioctl failed: %s\n",
strerror(errno));
return (dt_set_errno(dtp, errno));
}

return (1);
}


int
dt_pid_create_glob_offset_probes(struct ps_prochandle *P, dtrace_hdl_t *dtp,
fasttrap_probe_spec_t *ftp, const GElf_Sym *symp, const char *pattern)
{
ulong_t i;

ftp->ftps_type = DTFTP_OFFSETS;
ftp->ftps_pc = (uintptr_t)symp->st_value;
ftp->ftps_size = (size_t)symp->st_size;
ftp->ftps_noffs = 0;







if (strcmp("*", pattern) == 0) {
for (i = 0; i < symp->st_size; i += 4) {
ftp->ftps_offs[ftp->ftps_noffs++] = i;
}
} else {
char name[sizeof (i) * 2 + 1];

for (i = 0; i < symp->st_size; i += 4) {
(void) sprintf(name, "%lx", i);
if (gmatch(name, pattern))
ftp->ftps_offs[ftp->ftps_noffs++] = i;
}
}

if (ioctl(dtp->dt_ftfd, FASTTRAPIOC_MAKEPROBE, ftp) != 0) {
dt_dprintf("fasttrap probe creation ioctl failed: %s\n",
strerror(errno));
return (dt_set_errno(dtp, errno));
}

return (ftp->ftps_noffs);
}
