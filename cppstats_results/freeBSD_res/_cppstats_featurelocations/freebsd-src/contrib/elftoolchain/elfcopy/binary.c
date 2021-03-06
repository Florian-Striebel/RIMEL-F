

























#include <sys/param.h>
#include <sys/stat.h>
#include <ctype.h>
#include <err.h>
#include <gelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "elfcopy.h"

ELFTC_VCSID("$Id: binary.c 3757 2019-06-28 01:15:28Z emaste $");






void
create_binary(int ifd, int ofd)
{
Elf *e;
Elf_Scn *scn;
Elf_Data *d;
Elf64_Addr baseaddr;
GElf_Shdr sh;
off_t baseoff, off;
int elferr;

if ((e = elf_begin(ifd, ELF_C_READ, NULL)) == NULL)
errx(EXIT_FAILURE, "elf_begin() failed: %s",
elf_errmsg(-1));

baseaddr = 0;
baseoff = 0;
if (lseek(ofd, baseoff, SEEK_SET) < 0)
err(EXIT_FAILURE, "lseek failed");




baseoff = -1;
scn = NULL;
while ((scn = elf_nextscn(e, scn)) != NULL) {
if (gelf_getshdr(scn, &sh) == NULL) {
warnx("gelf_getshdr failed: %s", elf_errmsg(-1));
(void) elf_errno();
continue;
}
if ((sh.sh_flags & SHF_ALLOC) == 0 ||
sh.sh_type == SHT_NOBITS ||
sh.sh_size == 0)
continue;
if (baseoff == -1 || (off_t) sh.sh_offset < baseoff) {
baseoff = sh.sh_offset;
baseaddr = sh.sh_addr;
}
}
elferr = elf_errno();
if (elferr != 0)
warnx("elf_nextscn failed: %s", elf_errmsg(elferr));

if (baseoff == -1)
return;




scn = NULL;
while ((scn = elf_nextscn(e, scn)) != NULL) {
if (gelf_getshdr(scn, &sh) == NULL) {
warnx("gelf_getshdr failed: %s", elf_errmsg(-1));
(void) elf_errno();
continue;
}
if ((sh.sh_flags & SHF_ALLOC) == 0 ||
sh.sh_type == SHT_NOBITS ||
sh.sh_size == 0)
continue;
(void) elf_errno();
if ((d = elf_rawdata(scn, NULL)) == NULL) {
elferr = elf_errno();
if (elferr != 0)
warnx("elf_rawdata failed: %s", elf_errmsg(-1));
continue;
}
if (d->d_buf == NULL || d->d_size == 0)
continue;


off = sh.sh_addr - baseaddr;
if (lseek(ofd, off, SEEK_SET) < 0)
err(EXIT_FAILURE, "lseek failed");


if (write(ofd, d->d_buf, d->d_size) != (ssize_t) d->d_size)
err(EXIT_FAILURE, "write failed");
}
elferr = elf_errno();
if (elferr != 0)
warnx("elf_nextscn failed: %s", elf_errmsg(elferr));
}

#define _SYMBOL_NAMSZ 1024






void
create_elf_from_binary(struct elfcopy *ecp, int ifd, const char *ifn)
{
char name[_SYMBOL_NAMSZ];
struct section *sec, *sec_temp, *shtab;
struct stat sb;
GElf_Ehdr oeh;
GElf_Shdr sh;
void *content;
uint64_t off, data_start, data_end, data_size;
char *sym_basename, *p;


if (!TAILQ_EMPTY(&ecp->v_sec))
TAILQ_FOREACH_SAFE(sec, &ecp->v_sec, sec_list, sec_temp) {
TAILQ_REMOVE(&ecp->v_sec, sec, sec_list);
free(sec);
}

if (fstat(ifd, &sb) == -1)
err(EXIT_FAILURE, "fstat failed");


if ((content = malloc(sb.st_size)) == NULL)
err(EXIT_FAILURE, "malloc failed");
if (read(ifd, content, sb.st_size) != sb.st_size)
err(EXIT_FAILURE, "read failed");







if (gelf_newehdr(ecp->eout, ecp->oec) == NULL)
errx(EXIT_FAILURE, "gelf_newehdr failed: %s",
elf_errmsg(-1));
if (gelf_getehdr(ecp->eout, &oeh) == NULL)
errx(EXIT_FAILURE, "gelf_getehdr() failed: %s",
elf_errmsg(-1));


oeh.e_ident[EI_CLASS] = ecp->oec;
oeh.e_ident[EI_DATA] = ecp->oed;




oeh.e_ident[EI_OSABI] = ELFOSABI_NONE;
oeh.e_machine = ecp->oem;
oeh.e_type = ET_REL;
oeh.e_entry = 0;

ecp->flags |= RELOCATABLE;


init_shstrtab(ecp);
ecp->shstrtab->off = 0;





off = gelf_fsize(ecp->eout, ELF_T_EHDR, 1, EV_CURRENT);
if (off == 0)
errx(EXIT_FAILURE, "gelf_fsize() failed: %s", elf_errmsg(-1));
(void) create_external_section(ecp, ".data", NULL, content, sb.st_size,
off, SHT_PROGBITS, ELF_T_BYTE, SHF_ALLOC | SHF_WRITE, 1, 0, 1);


if ((ecp->shstrtab->os = elf_newscn(ecp->eout)) == NULL)
errx(EXIT_FAILURE, "elf_newscn failed: %s",
elf_errmsg(-1));
insert_to_sec_list(ecp, ecp->shstrtab, 1);


shtab = insert_shtab(ecp, 1);


shtab->sz += gelf_fsize(ecp->eout, ELF_T_SHDR, 2, EV_CURRENT);

if ((sym_basename = strdup(ifn)) == NULL)
err(1, "strdup");
for (p = sym_basename; *p != '\0'; p++)
if (!isalnum(*p & 0xFF))
*p = '_';
#define _GEN_SYMNAME(S) do { snprintf(name, sizeof(name), "%s%s%s", "_binary_", sym_basename, S); } while (0)






create_external_symtab(ecp);
data_start = 0;
data_end = data_start + sb.st_size;
data_size = sb.st_size;
_GEN_SYMNAME("_start");
add_to_symtab(ecp, name, data_start, 0, 1,
ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE), 0, 1);
_GEN_SYMNAME("_end");
add_to_symtab(ecp, name, data_end, 0, 1,
ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE), 0, 1);
_GEN_SYMNAME("_size");
add_to_symtab(ecp, name, data_size, 0, SHN_ABS,
ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE), 0, 1);
finalize_external_symtab(ecp);
create_symtab_data(ecp);
#undef _GEN_SYMNAME
free(sym_basename);





if (gelf_update_ehdr(ecp->eout, &oeh) == 0)
errx(EXIT_FAILURE, "gelf_update_ehdr() failed: %s",
elf_errmsg(-1));


ecp->flags |= SYMTAB_EXIST;
update_shdr(ecp, 0);


if (gelf_getshdr(ecp->symtab->os, &sh) == NULL)
errx(EXIT_FAILURE, "692 gelf_getshdr() failed: %s",
elf_errmsg(-1));
sh.sh_link = elf_ndxscn(ecp->strtab->os);
if (!gelf_update_shdr(ecp->symtab->os, &sh))
errx(EXIT_FAILURE, "gelf_update_shdr() failed: %s",
elf_errmsg(-1));


if (gelf_getehdr(ecp->eout, &oeh) == NULL)
errx(EXIT_FAILURE, "gelf_getehdr() failed: %s",
elf_errmsg(-1));


resync_sections(ecp);


oeh.e_shoff = shtab->off;


if (gelf_update_ehdr(ecp->eout, &oeh) == 0)
errx(EXIT_FAILURE, "gelf_update_ehdr() failed: %s",
elf_errmsg(-1));


if (elf_update(ecp->eout, ELF_C_WRITE) < 0)
errx(EXIT_FAILURE, "elf_update() failed: %s",
elf_errmsg(-1));


free(content);
free_elf(ecp);
}
