





























#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

#include <dt_impl.h>
#include <dt_pid.h>

#include <dis_tables.h>

#if defined(__FreeBSD__)
#include <libproc.h>
#include <libproc_compat.h>
#endif

#define DT_POPL_EBP 0x5d
#define DT_RET 0xc3
#define DT_RET16 0xc2
#define DT_LEAVE 0xc9
#define DT_JMP32 0xe9
#define DT_JMP8 0xeb
#define DT_REP 0xf3

#define DT_MOVL_EBP_ESP 0xe58b

#define DT_ISJ32(op16) (((op16) & 0xfff0) == 0x0f80)
#define DT_ISJ8(op8) (((op8) & 0xf0) == 0x70)

#define DT_MODRM_REG(modrm) (((modrm) >> 3) & 0x7)

static int dt_instr_size(uchar_t *, dtrace_hdl_t *, pid_t, uintptr_t, char);


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

static int
dt_pid_has_jump_table(struct ps_prochandle *P, dtrace_hdl_t *dtp,
uint8_t *text, fasttrap_probe_spec_t *ftp, const GElf_Sym *symp)
{
ulong_t i;
int size;
#if defined(illumos)
pid_t pid = Pstatus(P)->pr_pid;
char dmodel = Pstatus(P)->pr_dmodel;
#else
pid_t pid = proc_getpid(P);
char dmodel = proc_getmodel(P);
#endif






for (i = 0; i < ftp->ftps_size; i += size) {
size = dt_instr_size(&text[i], dtp, pid, symp->st_value + i,
dmodel);




if (size <= 0) {
dt_dprintf("error at %#lx (assuming jump table)\n", i);
return (1);
}

#if defined(notyet)





if ((text[i] == 0xff && DT_MODRM_REG(text[i + 1]) == 4) ||
(dmodel == PR_MODEL_LP64 && (text[i] & 0xf0) == 0x40 &&
text[i + 1] == 0xff && DT_MODRM_REG(text[i + 2]) == 4)) {
dt_dprintf("found a suspected jump table at %s:%lx\n",
ftp->ftps_func, i);
return (1);
}
#endif
}

return (0);
}


int
dt_pid_create_return_probe(struct ps_prochandle *P, dtrace_hdl_t *dtp,
fasttrap_probe_spec_t *ftp, const GElf_Sym *symp, uint64_t *stret)
{
uint8_t *text;
ulong_t i, end;
int size;
#if defined(illumos)
pid_t pid = Pstatus(P)->pr_pid;
char dmodel = Pstatus(P)->pr_dmodel;
#else
pid_t pid = proc_getpid(P);
char dmodel = proc_getmodel(P);
#endif





if ((text = calloc(1, symp->st_size + 4)) == NULL) {
dt_dprintf("mr sparkle: malloc() failed\n");
return (DT_PROC_ERR);
}

if (Pread(P, text, symp->st_size, symp->st_value) != symp->st_size) {
dt_dprintf("mr sparkle: Pread() failed\n");
free(text);
return (DT_PROC_ERR);
}

ftp->ftps_type = DTFTP_RETURN;
ftp->ftps_pc = (uintptr_t)symp->st_value;
ftp->ftps_size = (size_t)symp->st_size;
ftp->ftps_noffs = 0;














if (dt_pid_has_jump_table(P, dtp, text, ftp, symp)) {
for (i = 0, end = ftp->ftps_size; i < end; i += size) {
size = dt_instr_size(&text[i], dtp, pid,
symp->st_value + i, dmodel);


if (size <= 0)
break;

if (text[i] == DT_LEAVE && text[i + 1] == DT_RET) {
dt_dprintf("leave/ret at %lx\n", i + 1);
ftp->ftps_offs[ftp->ftps_noffs++] = i + 1;
size = 2;
} else if (text[i] == DT_LEAVE &&
text[i + 1] == DT_REP && text[i + 2] == DT_RET) {
dt_dprintf("leave/rep ret at %lx\n", i + 1);
ftp->ftps_offs[ftp->ftps_noffs++] = i + 1;
size = 3;
} else if (*(uint16_t *)&text[i] == DT_MOVL_EBP_ESP &&
text[i + 2] == DT_POPL_EBP &&
text[i + 3] == DT_RET) {
dt_dprintf("movl/popl/ret at %lx\n", i + 3);
ftp->ftps_offs[ftp->ftps_noffs++] = i + 3;
size = 4;
} else if (*(uint16_t *)&text[i] == DT_MOVL_EBP_ESP &&
text[i + 2] == DT_POPL_EBP &&
text[i + 3] == DT_REP &&
text[i + 4] == DT_RET) {
dt_dprintf("movl/popl/rep ret at %lx\n", i + 3);
ftp->ftps_offs[ftp->ftps_noffs++] = i + 3;
size = 5;
}
}
} else {
for (i = 0, end = ftp->ftps_size; i < end; i += size) {
size = dt_instr_size(&text[i], dtp, pid,
symp->st_value + i, dmodel);


if (size <= 0)
break;


if (size == 1 && text[i] == DT_RET)
goto is_ret;


if (size == 2 && text[i] == DT_REP &&
text[i + 1] == DT_RET)
goto is_ret;


if (size == 3 && text[i] == DT_RET16)
goto is_ret;


if (size == 4 && text[i] == DT_REP &&
text[i + 1] == DT_RET16)
goto is_ret;


if (size == 5 && text[i] == DT_JMP32 && symp->st_size <=
(uintptr_t)(i + size + *(int32_t *)&text[i + 1]))
goto is_ret;


if (size == 2 && text[i] == DT_JMP8 && symp->st_size <=
(uintptr_t)(i + size + *(int8_t *)&text[i + 1]))
goto is_ret;


if (size == 6 && DT_ISJ32(*(uint16_t *)&text[i]) &&
symp->st_size <=
(uintptr_t)(i + size + *(int32_t *)&text[i + 2]))
goto is_ret;


if (size == 2 && DT_ISJ8(text[i]) && symp->st_size <=
(uintptr_t)(i + size + *(int8_t *)&text[i + 1]))
goto is_ret;

continue;
is_ret:
dt_dprintf("return at offset %lx\n", i);
ftp->ftps_offs[ftp->ftps_noffs++] = i;
}
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
ftp->ftps_type = DTFTP_OFFSETS;
ftp->ftps_pc = (uintptr_t)symp->st_value;
ftp->ftps_size = (size_t)symp->st_size;
ftp->ftps_noffs = 1;

if (strcmp("-", ftp->ftps_func) == 0) {
ftp->ftps_offs[0] = off;
} else {
uint8_t *text;
ulong_t i;
int size;
#if defined(illumos)
pid_t pid = Pstatus(P)->pr_pid;
char dmodel = Pstatus(P)->pr_dmodel;
#else
pid_t pid = proc_getpid(P);
char dmodel = proc_getmodel(P);
#endif

if ((text = malloc(symp->st_size)) == NULL) {
dt_dprintf("mr sparkle: malloc() failed\n");
return (DT_PROC_ERR);
}

if (Pread(P, text, symp->st_size, symp->st_value) !=
symp->st_size) {
dt_dprintf("mr sparkle: Pread() failed\n");
free(text);
return (DT_PROC_ERR);
}






if (dt_pid_has_jump_table(P, dtp, text, ftp, symp)) {
free(text);
return (0);
}

for (i = 0; i < symp->st_size; i += size) {
if (i == off) {
ftp->ftps_offs[0] = i;
break;
}






if (i > off) {
free(text);
return (DT_PROC_ALIGN);
}

size = dt_instr_size(&text[i], dtp, pid,
symp->st_value + i, dmodel);





if (size <= 0) {
free(text);
return (DT_PROC_ALIGN);
}
}

free(text);
}

if (ioctl(dtp->dt_ftfd, FASTTRAPIOC_MAKEPROBE, ftp) != 0) {
dt_dprintf("fasttrap probe creation ioctl failed: %s\n",
strerror(errno));
return (dt_set_errno(dtp, errno));
}

return (ftp->ftps_noffs);
}


int
dt_pid_create_glob_offset_probes(struct ps_prochandle *P, dtrace_hdl_t *dtp,
fasttrap_probe_spec_t *ftp, const GElf_Sym *symp, const char *pattern)
{
uint8_t *text;
int size;
ulong_t i, end = symp->st_size;
#if defined(illumos)
pid_t pid = Pstatus(P)->pr_pid;
char dmodel = Pstatus(P)->pr_dmodel;
#else
pid_t pid = proc_getpid(P);
char dmodel = proc_getmodel(P);
#endif

ftp->ftps_type = DTFTP_OFFSETS;
ftp->ftps_pc = (uintptr_t)symp->st_value;
ftp->ftps_size = (size_t)symp->st_size;
ftp->ftps_noffs = 0;

if ((text = malloc(symp->st_size)) == NULL) {
dt_dprintf("mr sparkle: malloc() failed\n");
return (DT_PROC_ERR);
}

if (Pread(P, text, symp->st_size, symp->st_value) != symp->st_size) {
dt_dprintf("mr sparkle: Pread() failed\n");
free(text);
return (DT_PROC_ERR);
}





if (dt_pid_has_jump_table(P, dtp, text, ftp, symp)) {
free(text);
return (0);
}

if (strcmp("*", pattern) == 0) {
for (i = 0; i < end; i += size) {
ftp->ftps_offs[ftp->ftps_noffs++] = i;

size = dt_instr_size(&text[i], dtp, pid,
symp->st_value + i, dmodel);


if (size <= 0)
break;
}
} else {
char name[sizeof (i) * 2 + 1];

for (i = 0; i < end; i += size) {
(void) snprintf(name, sizeof (name), "%lx", i);
if (gmatch(name, pattern))
ftp->ftps_offs[ftp->ftps_noffs++] = i;

size = dt_instr_size(&text[i], dtp, pid,
symp->st_value + i, dmodel);


if (size <= 0)
break;
}
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

typedef struct dtrace_dis {
uchar_t *instr;
dtrace_hdl_t *dtp;
pid_t pid;
uintptr_t addr;
} dtrace_dis_t;

static int
dt_getbyte(void *data)
{
dtrace_dis_t *dis = data;
int ret = *dis->instr;

if (ret == FASTTRAP_INSTR) {
fasttrap_instr_query_t instr;

instr.ftiq_pid = dis->pid;
instr.ftiq_pc = dis->addr;









if (ioctl(dis->dtp->dt_ftfd, FASTTRAPIOC_GETINSTR, &instr) == 0)
ret = instr.ftiq_instr;
}

dis->addr++;
dis->instr++;

return (ret);
}

static int
dt_instr_size(uchar_t *instr, dtrace_hdl_t *dtp, pid_t pid, uintptr_t addr,
char dmodel)
{
dtrace_dis_t data;
dis86_t x86dis;
uint_t cpu_mode;

data.instr = instr;
data.dtp = dtp;
data.pid = pid;
data.addr = addr;

x86dis.d86_data = &data;
x86dis.d86_get_byte = dt_getbyte;
x86dis.d86_check_func = NULL;

cpu_mode = (dmodel == PR_MODEL_ILP32) ? SIZE32 : SIZE64;

if (dtrace_disx86(&x86dis, cpu_mode) != 0)
return (-1);






if (x86dis.d86_len == 1 &&
(uchar_t)x86dis.d86_bytes[0] == FASTTRAP_INSTR)
return (-1);

return (x86dis.d86_len);
}
