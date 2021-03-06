#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <link.h>
#include <sys/dtrace.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libelf.h>
static const char *devnamep = "/dev/dtrace/helper";
#if defined(illumos)
static const char *olddevname = "/devices/pseudo/dtrace@0:helper";
#endif
static const char *modname;
static int gen;
extern dof_hdr_t __SUNW_dof;
static boolean_t dof_init_debug = B_FALSE;
static void
dbg_printf(int debug, const char *fmt, ...)
{
va_list ap;
if (debug && !dof_init_debug)
return;
va_start(ap, fmt);
if (modname == NULL)
(void) fprintf(stderr, "dtrace DOF: ");
else
(void) fprintf(stderr, "dtrace DOF %s: ", modname);
(void) vfprintf(stderr, fmt, ap);
if (fmt[strlen(fmt) - 1] != '\n')
(void) fprintf(stderr, ": %s\n", strerror(errno));
va_end(ap);
}
#if defined(illumos)
#pragma init(dtrace_dof_init)
#else
static void dtrace_dof_init(void) __attribute__ ((constructor));
#endif
static void
dtrace_dof_init(void)
{
dof_hdr_t *dof = &__SUNW_dof;
#if defined(_LP64)
Elf64_Ehdr *elf;
#else
Elf32_Ehdr *elf;
#endif
dof_helper_t dh;
Link_map *lmp = NULL;
#if defined(illumos)
Lmid_t lmid;
#else
u_long lmid = 0;
#endif
int fd;
const char *p;
if (getenv("DTRACE_DOF_INIT_DISABLE") != NULL)
return;
if (getenv("DTRACE_DOF_INIT_DEBUG") != NULL)
dof_init_debug = B_TRUE;
if (dlinfo(RTLD_SELF, RTLD_DI_LINKMAP, &lmp) == -1 || lmp == NULL) {
dbg_printf(1, "couldn't discover module name or address\n");
return;
}
#if defined(illumos)
if (dlinfo(RTLD_SELF, RTLD_DI_LMID, &lmid) == -1) {
dbg_printf(1, "couldn't discover link map ID\n");
return;
}
#endif
if ((modname = strrchr(lmp->l_name, '/')) == NULL)
modname = lmp->l_name;
else
modname++;
if (dof->dofh_ident[DOF_ID_MAG0] != DOF_MAG_MAG0 ||
dof->dofh_ident[DOF_ID_MAG1] != DOF_MAG_MAG1 ||
dof->dofh_ident[DOF_ID_MAG2] != DOF_MAG_MAG2 ||
dof->dofh_ident[DOF_ID_MAG3] != DOF_MAG_MAG3) {
dbg_printf(0, ".SUNW_dof section corrupt\n");
return;
}
#if defined(__FreeBSD__)
elf = (void *)lmp->l_base;
#else
elf = (void *)lmp->l_addr;
#endif
dh.dofhp_dof = (uintptr_t)dof;
#if defined(__FreeBSD__)
dh.dofhp_addr = elf->e_type == ET_DYN ? (uintptr_t) lmp->l_base : 0;
dh.dofhp_pid = getpid();
#else
dh.dofhp_addr = elf->e_type == ET_DYN ? (uintptr_t) lmp->l_addr : 0;
#endif
if (lmid == 0) {
(void) snprintf(dh.dofhp_mod, sizeof (dh.dofhp_mod),
"%s", modname);
} else {
(void) snprintf(dh.dofhp_mod, sizeof (dh.dofhp_mod),
"LM%lu`%s", lmid, modname);
}
if ((p = getenv("DTRACE_DOF_INIT_DEVNAME")) != NULL)
devnamep = p;
if ((fd = open64(devnamep, O_RDWR)) < 0) {
dbg_printf(1, "failed to open helper device %s", devnamep);
#if defined(illumos)
if (p != NULL)
return;
devnamep = olddevname;
if ((fd = open64(devnamep, O_RDWR)) < 0) {
dbg_printf(1, "failed to open helper device %s", devnamep);
return;
}
#else
return;
#endif
}
if ((gen = ioctl(fd, DTRACEHIOC_ADDDOF, &dh)) == -1)
dbg_printf(1, "DTrace ioctl failed for DOF at %p", dof);
else {
dbg_printf(1, "DTrace ioctl succeeded for DOF at %p\n", dof);
#if defined(__FreeBSD__)
gen = dh.dofhp_gen;
#endif
}
(void) close(fd);
}
#if defined(illumos)
#pragma fini(dtrace_dof_fini)
#else
static void dtrace_dof_fini(void) __attribute__ ((destructor));
#endif
static void
dtrace_dof_fini(void)
{
int fd;
if ((fd = open64(devnamep, O_RDWR)) < 0) {
dbg_printf(1, "failed to open helper device %s", devnamep);
return;
}
if ((gen = ioctl(fd, DTRACEHIOC_REMOVE, &gen)) == -1)
dbg_printf(1, "DTrace ioctl failed to remove DOF (%d)\n", gen);
else
dbg_printf(1, "DTrace ioctl removed DOF (%d)\n", gen);
(void) close(fd);
}
