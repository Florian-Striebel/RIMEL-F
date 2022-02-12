

























#include <sys/param.h>
#include <err.h>
#include <gelf.h>
#include <libpe.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "elfcopy.h"

ELFTC_VCSID("$Id: pe.c 3508 2016-12-27 06:19:39Z kaiwang27 $");


void
create_pe(struct elfcopy *ecp, int ifd, int ofd)
{
Elf *e;
Elf_Scn *scn;
Elf_Data *d;
GElf_Ehdr eh;
GElf_Shdr sh;
PE *pe;
PE_Scn *ps;
PE_SecHdr psh;
PE_CoffHdr pch;
PE_OptHdr poh;
PE_Object po;
PE_Buffer *pb;
const char *name;
size_t indx;
time_t timestamp;
int elferr;

if (ecp->otf == ETF_EFI || ecp->oem == EM_X86_64)
po = PE_O_PE32P;
else
po = PE_O_PE32;

if ((e = elf_begin(ifd, ELF_C_READ, NULL)) == NULL)
errx(EXIT_FAILURE, "elf_begin() failed: %s",
elf_errmsg(-1));

if (gelf_getehdr(e, &eh) == NULL)
errx(EXIT_FAILURE, "gelf_getehdr() failed: %s",
elf_errmsg(-1));

if (elf_getshstrndx(e, &indx) == 0)
errx(EXIT_FAILURE, "elf_getshstrndx() failed: %s",
elf_errmsg(-1));

if ((pe = pe_init(ofd, PE_C_WRITE, po)) == NULL)
err(EXIT_FAILURE, "pe_init() failed");


memset(&pch, 0, sizeof(pch));
switch (ecp->oem) {
case EM_386:
pch.ch_machine = IMAGE_FILE_MACHINE_I386;
break;
case EM_X86_64:
pch.ch_machine = IMAGE_FILE_MACHINE_AMD64;
break;
default:
pch.ch_machine = IMAGE_FILE_MACHINE_UNKNOWN;
break;
}
if (elftc_timestamp(&timestamp) != 0)
err(EXIT_FAILURE, "elftc_timestamp");
pch.ch_timestamp = (uint32_t) timestamp;
if (pe_update_coff_header(pe, &pch) < 0)
err(EXIT_FAILURE, "pe_update_coff_header() failed");


memset(&poh, 0, sizeof(poh));
if (ecp->otf == ETF_EFI)
poh.oh_subsystem = IMAGE_SUBSYSTEM_EFI_APPLICATION;
poh.oh_entry = (uint32_t) eh.e_entry;







poh.oh_secalign = 0x1000;
poh.oh_filealign = 0x200;


scn = NULL;
while ((scn = elf_nextscn(e, scn)) != NULL) {





if (gelf_getshdr(scn, &sh) == NULL) {
warnx("gelf_getshdr() failed: %s", elf_errmsg(-1));
(void) elf_errno();
continue;
}
if ((name = elf_strptr(e, indx, sh.sh_name)) ==
NULL) {
warnx("elf_strptr() failed: %s", elf_errmsg(-1));
(void) elf_errno();
continue;
}


if (strcmp(name, ".shstrtab") == 0 ||
strcmp(name, ".symtab") == 0 ||
strcmp(name, ".strtab") == 0)
continue;

if ((d = elf_getdata(scn, NULL)) == NULL) {
warnx("elf_getdata() failed: %s", elf_errmsg(-1));
(void) elf_errno();
continue;
}

if (strcmp(name, ".text") == 0) {
poh.oh_textbase = (uint32_t) sh.sh_addr;
poh.oh_textsize = (uint32_t) roundup(sh.sh_size,
poh.oh_filealign);
} else {
if (po == PE_O_PE32 && strcmp(name, ".data") == 0)
poh.oh_database = sh.sh_addr;
if (sh.sh_type == SHT_NOBITS)
poh.oh_bsssize += (uint32_t)
roundup(sh.sh_size, poh.oh_filealign);
else if (sh.sh_flags & SHF_ALLOC)
poh.oh_datasize += (uint32_t)
roundup(sh.sh_size, poh.oh_filealign);
}





if ((ps = pe_newscn(pe)) == NULL) {
warn("pe_newscn() failed");
continue;
}







memset(&psh, 0, sizeof(psh));
strncpy(psh.sh_name, name, sizeof(psh.sh_name));
psh.sh_addr = sh.sh_addr;
psh.sh_virtsize = sh.sh_size;
if (sh.sh_type != SHT_NOBITS)
psh.sh_rawsize = roundup(sh.sh_size, poh.oh_filealign);
else
psh.sh_char |= IMAGE_SCN_CNT_UNINITIALIZED_DATA;




psh.sh_char |= IMAGE_SCN_MEM_READ;
if (sh.sh_flags & SHF_WRITE)
psh.sh_char |= IMAGE_SCN_MEM_WRITE;
if (sh.sh_flags & SHF_EXECINSTR)
psh.sh_char |= IMAGE_SCN_MEM_EXECUTE |
IMAGE_SCN_CNT_CODE;
if ((sh.sh_flags & SHF_ALLOC) && (psh.sh_char & 0xF0) == 0)
psh.sh_char |= IMAGE_SCN_CNT_INITIALIZED_DATA;


if (strcmp(name, ".reloc") == 0)
psh.sh_char |= IMAGE_SCN_MEM_DISCARDABLE;

if (pe_update_section_header(ps, &psh) < 0) {
warn("pe_update_section_header() failed");
continue;
}


if ((pb = pe_newbuffer(ps)) == NULL) {
warn("pe_newbuffer() failed");
continue;
}
pb->pb_align = 1;
pb->pb_off = 0;
if (sh.sh_type != SHT_NOBITS) {
pb->pb_size = roundup(sh.sh_size, poh.oh_filealign);
if ((pb->pb_buf = calloc(1, pb->pb_size)) == NULL) {
warn("calloc failed");
continue;
}
memcpy(pb->pb_buf, d->d_buf, sh.sh_size);
}
}
elferr = elf_errno();
if (elferr != 0)
warnx("elf_nextscn() failed: %s", elf_errmsg(elferr));


if (pe_update_opt_header(pe, &poh) < 0)
err(EXIT_FAILURE, "pe_update_opt_header() failed");


if (pe_update(pe) < 0)
err(EXIT_FAILURE, "pe_update() failed");

pe_finish(pe);
elf_end(e);
}
