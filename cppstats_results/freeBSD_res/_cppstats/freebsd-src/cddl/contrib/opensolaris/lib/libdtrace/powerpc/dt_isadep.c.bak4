

























#pragma ident "%Z%%M% %I% %E% SMI"

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

#include <dt_impl.h>
#include <dt_pid.h>

#include <libproc_compat.h>


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

uintptr_t temp;
uint32_t *text;
int i;
int srdepth = 0;

if ((text = malloc(symp->st_size + 4)) == NULL) {
dt_dprintf("mr sparkle: malloc() failed\n");
return (DT_PROC_ERR);
}

if (Pread(P, text, symp->st_size, symp->st_value) != symp->st_size) {
dt_dprintf("mr sparkle: Pread() failed\n");
free(text);
return (DT_PROC_ERR);
}





text[symp->st_size / 4] = 0;

ftp->ftps_type = DTFTP_RETURN;
ftp->ftps_pc = symp->st_value;
ftp->ftps_size = symp->st_size;
ftp->ftps_noffs = 0;

for (i = 0; i < symp->st_size / 4; i++) {

if ((text[i] & 0xfc000001) != 0x48000000 &&
text[i] != 0x4e800020)
continue;





if ((text[i] & 0xfc000000) == 0x48000000) {
temp = (text[i] & 0x03fffffc);

if (!(text[i] & 0x02)) {
temp += symp->st_value + i * 4;
}
else {

if (temp & 0x02000000) {
temp |= (UINTPTR_MAX - 0x03ffffff);
}
}
if (temp >= symp->st_value &&
temp <= (symp->st_value + symp->st_size))
continue;
}
dt_dprintf("return at offset %x\n", i * 4);
ftp->ftps_offs[ftp->ftps_noffs++] = i * 4;
}

free(text);
if (ftp->ftps_noffs > 0) {
if (ioctl(dtp->dt_ftfd, FASTTRAPIOC_MAKEPROBE, ftp) != 0) {
dt_dprintf("fasttrap probe creation ioctl failed: %s\n",
strerror(errno));
return (dt_set_errno(dtp, errno));
}
}


return (ftp->ftps_noffs);
}


int
dt_pid_create_offset_probe(struct ps_prochandle *P, dtrace_hdl_t *dtp,
fasttrap_probe_spec_t *ftp, const GElf_Sym *symp, ulong_t off)
{
if (off & 0x3)
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
