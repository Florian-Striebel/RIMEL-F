


























#include <sys/param.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include <assert.h>
#include <elf.h>
#include <sys/types.h>
#include <fcntl.h>
#include <gelf.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>

#include <libelf.h>

#include <dt_impl.h>
#include <dt_provider.h>
#include <dt_program.h>
#include <dt_string.h>

#define ESHDR_NULL 0
#define ESHDR_SHSTRTAB 1
#define ESHDR_DOF 2
#define ESHDR_STRTAB 3
#define ESHDR_SYMTAB 4
#define ESHDR_REL 5
#define ESHDR_NUM 6

#define PWRITE_SCN(index, data) (lseek64(fd, (off64_t)elf_file.shdr[(index)].sh_offset, SEEK_SET) != (off64_t)elf_file.shdr[(index)].sh_offset || dt_write(dtp, fd, (data), elf_file.shdr[(index)].sh_size) != elf_file.shdr[(index)].sh_size)





static const char DTRACE_SHSTRTAB32[] = "\0"
".shstrtab\0"
".SUNW_dof\0"
".strtab\0"
".symtab\0"
".rel.SUNW_dof";

static const char DTRACE_SHSTRTAB64[] = "\0"
".shstrtab\0"
".SUNW_dof\0"
".strtab\0"
".symtab\0"
".rela.SUNW_dof";

static const char DOFSTR[] = "__SUNW_dof";
static const char DOFLAZYSTR[] = "___SUNW_dof";

typedef struct dt_link_pair {
struct dt_link_pair *dlp_next;
void *dlp_str;
void *dlp_sym;
} dt_link_pair_t;

typedef struct dof_elf32 {
uint32_t de_nrel;
Elf32_Rel *de_rel;
uint32_t de_nsym;
Elf32_Sym *de_sym;
uint32_t de_strlen;
char *de_strtab;
uint32_t de_global;
} dof_elf32_t;

static int
prepare_elf32(dtrace_hdl_t *dtp, const dof_hdr_t *dof, dof_elf32_t *dep)
{
dof_sec_t *dofs, *s;
dof_relohdr_t *dofrh;
dof_relodesc_t *dofr;
char *strtab;
int i, j, nrel;
size_t strtabsz = 1;
uint32_t count = 0;
size_t base;
Elf32_Sym *sym;
Elf32_Rel *rel;


dofs = (dof_sec_t *)((char *)dof + dof->dofh_secoff);





for (i = 0; i < dof->dofh_secnum; i++) {
if (dofs[i].dofs_type != DOF_SECT_URELHDR)
continue;


dofrh = (dof_relohdr_t *)((char *)dof + dofs[i].dofs_offset);

s = &dofs[dofrh->dofr_strtab];
strtab = (char *)dof + s->dofs_offset;
assert(strtab[0] == '\0');
strtabsz += s->dofs_size - 1;

s = &dofs[dofrh->dofr_relsec];

dofr = (dof_relodesc_t *)((char *)dof + s->dofs_offset);
count += s->dofs_size / s->dofs_entsize;
}

dep->de_strlen = strtabsz;
dep->de_nrel = count;
dep->de_nsym = count + 1;

if (dtp->dt_lazyload) {
dep->de_strlen += sizeof (DOFLAZYSTR);
dep->de_nsym++;
} else {
dep->de_strlen += sizeof (DOFSTR);
dep->de_nsym++;
}

if ((dep->de_rel = calloc(dep->de_nrel,
sizeof (dep->de_rel[0]))) == NULL) {
return (dt_set_errno(dtp, EDT_NOMEM));
}

if ((dep->de_sym = calloc(dep->de_nsym, sizeof (Elf32_Sym))) == NULL) {
free(dep->de_rel);
return (dt_set_errno(dtp, EDT_NOMEM));
}

if ((dep->de_strtab = calloc(dep->de_strlen, 1)) == NULL) {
free(dep->de_rel);
free(dep->de_sym);
return (dt_set_errno(dtp, EDT_NOMEM));
}

count = 0;
strtabsz = 1;
dep->de_strtab[0] = '\0';
rel = dep->de_rel;
sym = dep->de_sym;
dep->de_global = 1;




bzero(sym, sizeof (Elf32_Sym));
sym++;





for (i = 0; i < dof->dofh_secnum; i++) {
if (dofs[i].dofs_type != DOF_SECT_URELHDR)
continue;


dofrh = (dof_relohdr_t *)((char *)dof + dofs[i].dofs_offset);

s = &dofs[dofrh->dofr_strtab];
strtab = (char *)dof + s->dofs_offset;
bcopy(strtab + 1, dep->de_strtab + strtabsz, s->dofs_size);
base = strtabsz;
strtabsz += s->dofs_size - 1;

s = &dofs[dofrh->dofr_relsec];

dofr = (dof_relodesc_t *)((char *)dof + s->dofs_offset);
nrel = s->dofs_size / s->dofs_entsize;

s = &dofs[dofrh->dofr_tgtsec];

for (j = 0; j < nrel; j++) {
#if defined(__aarch64__)
rel->r_offset = s->dofs_offset +
dofr[j].dofr_offset;
rel->r_info = ELF32_R_INFO(count + dep->de_global,
R_ARM_REL32);
#elif defined(__arm__)

printf("%s:%s(%d): arm not implemented\n",
__FUNCTION__, __FILE__, __LINE__);
#elif defined(__i386) || defined(__amd64)
rel->r_offset = s->dofs_offset +
dofr[j].dofr_offset;
rel->r_info = ELF32_R_INFO(count + dep->de_global,
R_386_PC32);
#elif defined(__mips__)

printf("%s:%s(%d): MIPS not implemented\n",
__FUNCTION__, __FILE__, __LINE__);
#elif defined(__powerpc__)




rel->r_offset = s->dofs_offset +
dofr[j].dofr_offset + 4;
rel->r_info = ELF32_R_INFO(count + dep->de_global,
R_PPC_REL32);
#elif defined(__riscv)

printf("%s:%s(%d): RISC-V not implemented\n",
__FUNCTION__, __FILE__, __LINE__);
#else
#error unknown ISA
#endif

sym->st_name = base + dofr[j].dofr_name - 1;
sym->st_value = 0;
sym->st_size = 0;
sym->st_info = ELF32_ST_INFO(STB_GLOBAL, STT_FUNC);
sym->st_other = ELF32_ST_VISIBILITY(STV_HIDDEN);
sym->st_shndx = SHN_UNDEF;

rel++;
sym++;
count++;
}
}





sym->st_name = strtabsz;
sym->st_value = 0;
sym->st_size = dof->dofh_filesz;
sym->st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT);
sym->st_other = ELF32_ST_VISIBILITY(STV_HIDDEN);
sym->st_shndx = ESHDR_DOF;
sym++;

if (dtp->dt_lazyload) {
bcopy(DOFLAZYSTR, dep->de_strtab + strtabsz,
sizeof (DOFLAZYSTR));
strtabsz += sizeof (DOFLAZYSTR);
} else {
bcopy(DOFSTR, dep->de_strtab + strtabsz, sizeof (DOFSTR));
strtabsz += sizeof (DOFSTR);
}

assert(count == dep->de_nrel);
assert(strtabsz == dep->de_strlen);

return (0);
}


typedef struct dof_elf64 {
uint32_t de_nrel;
Elf64_Rela *de_rel;
uint32_t de_nsym;
Elf64_Sym *de_sym;

uint32_t de_strlen;
char *de_strtab;

uint32_t de_global;
} dof_elf64_t;

static int
prepare_elf64(dtrace_hdl_t *dtp, const dof_hdr_t *dof, dof_elf64_t *dep)
{
dof_sec_t *dofs, *s;
dof_relohdr_t *dofrh;
dof_relodesc_t *dofr;
char *strtab;
int i, j, nrel;
size_t strtabsz = 1;
uint64_t count = 0;
size_t base;
Elf64_Sym *sym;
Elf64_Rela *rel;


dofs = (dof_sec_t *)((char *)dof + dof->dofh_secoff);





for (i = 0; i < dof->dofh_secnum; i++) {
if (dofs[i].dofs_type != DOF_SECT_URELHDR)
continue;


dofrh = (dof_relohdr_t *)((char *)dof + dofs[i].dofs_offset);

s = &dofs[dofrh->dofr_strtab];
strtab = (char *)dof + s->dofs_offset;
assert(strtab[0] == '\0');
strtabsz += s->dofs_size - 1;

s = &dofs[dofrh->dofr_relsec];

dofr = (dof_relodesc_t *)((char *)dof + s->dofs_offset);
count += s->dofs_size / s->dofs_entsize;
}

dep->de_strlen = strtabsz;
dep->de_nrel = count;
dep->de_nsym = count + 1;

if (dtp->dt_lazyload) {
dep->de_strlen += sizeof (DOFLAZYSTR);
dep->de_nsym++;
} else {
dep->de_strlen += sizeof (DOFSTR);
dep->de_nsym++;
}

if ((dep->de_rel = calloc(dep->de_nrel,
sizeof (dep->de_rel[0]))) == NULL) {
return (dt_set_errno(dtp, EDT_NOMEM));
}

if ((dep->de_sym = calloc(dep->de_nsym, sizeof (Elf64_Sym))) == NULL) {
free(dep->de_rel);
return (dt_set_errno(dtp, EDT_NOMEM));
}

if ((dep->de_strtab = calloc(dep->de_strlen, 1)) == NULL) {
free(dep->de_rel);
free(dep->de_sym);
return (dt_set_errno(dtp, EDT_NOMEM));
}

count = 0;
strtabsz = 1;
dep->de_strtab[0] = '\0';
rel = dep->de_rel;
sym = dep->de_sym;
dep->de_global = 1;




bzero(sym, sizeof (Elf64_Sym));
sym++;





for (i = 0; i < dof->dofh_secnum; i++) {
if (dofs[i].dofs_type != DOF_SECT_URELHDR)
continue;


dofrh = (dof_relohdr_t *)((char *)dof + dofs[i].dofs_offset);

s = &dofs[dofrh->dofr_strtab];
strtab = (char *)dof + s->dofs_offset;
bcopy(strtab + 1, dep->de_strtab + strtabsz, s->dofs_size);
base = strtabsz;
strtabsz += s->dofs_size - 1;

s = &dofs[dofrh->dofr_relsec];

dofr = (dof_relodesc_t *)((char *)dof + s->dofs_offset);
nrel = s->dofs_size / s->dofs_entsize;

s = &dofs[dofrh->dofr_tgtsec];

for (j = 0; j < nrel; j++) {
#if defined(__aarch64__)
rel->r_offset = s->dofs_offset +
dofr[j].dofr_offset;
rel->r_info = ELF64_R_INFO(count + dep->de_global,
R_AARCH64_PREL64);
#elif defined(__arm__)

#elif defined(__mips__)

#elif defined(__powerpc__)
rel->r_offset = s->dofs_offset +
dofr[j].dofr_offset;
rel->r_info = ELF64_R_INFO(count + dep->de_global,
R_PPC64_REL64);
#elif defined(__riscv)

#elif defined(__i386) || defined(__amd64)
rel->r_offset = s->dofs_offset +
dofr[j].dofr_offset;
rel->r_info = ELF64_R_INFO(count + dep->de_global,
R_X86_64_PC64);
#else
#error unknown ISA
#endif

sym->st_name = base + dofr[j].dofr_name - 1;
sym->st_value = 0;
sym->st_size = 0;
sym->st_info = GELF_ST_INFO(STB_GLOBAL, STT_FUNC);
sym->st_other = ELF64_ST_VISIBILITY(STV_HIDDEN);
sym->st_shndx = SHN_UNDEF;

rel++;
sym++;
count++;
}
}





sym->st_name = strtabsz;
sym->st_value = 0;
sym->st_size = dof->dofh_filesz;
sym->st_info = GELF_ST_INFO(STB_GLOBAL, STT_OBJECT);
sym->st_other = ELF64_ST_VISIBILITY(STV_HIDDEN);
sym->st_shndx = ESHDR_DOF;
sym++;

if (dtp->dt_lazyload) {
bcopy(DOFLAZYSTR, dep->de_strtab + strtabsz,
sizeof (DOFLAZYSTR));
strtabsz += sizeof (DOFLAZYSTR);
} else {
bcopy(DOFSTR, dep->de_strtab + strtabsz, sizeof (DOFSTR));
strtabsz += sizeof (DOFSTR);
}

assert(count == dep->de_nrel);
assert(strtabsz == dep->de_strlen);

return (0);
}






static int
dump_elf32(dtrace_hdl_t *dtp, const dof_hdr_t *dof, int fd)
{
struct {
Elf32_Ehdr ehdr;
Elf32_Shdr shdr[ESHDR_NUM];
} elf_file;

Elf32_Shdr *shp;
Elf32_Off off;
dof_elf32_t de;
int ret = 0;
uint_t nshdr;

if (prepare_elf32(dtp, dof, &de) != 0)
return (-1);





nshdr = de.de_nrel == 0 ? ESHDR_SYMTAB + 1 : ESHDR_NUM;

bzero(&elf_file, sizeof (elf_file));

elf_file.ehdr.e_ident[EI_MAG0] = ELFMAG0;
elf_file.ehdr.e_ident[EI_MAG1] = ELFMAG1;
elf_file.ehdr.e_ident[EI_MAG2] = ELFMAG2;
elf_file.ehdr.e_ident[EI_MAG3] = ELFMAG3;
elf_file.ehdr.e_ident[EI_VERSION] = EV_CURRENT;
elf_file.ehdr.e_ident[EI_CLASS] = ELFCLASS32;
#if BYTE_ORDER == _BIG_ENDIAN
elf_file.ehdr.e_ident[EI_DATA] = ELFDATA2MSB;
#else
elf_file.ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
#endif
elf_file.ehdr.e_ident[EI_OSABI] = ELFOSABI_FREEBSD;
elf_file.ehdr.e_type = ET_REL;
#if defined(__arm__)
elf_file.ehdr.e_machine = EM_ARM;
#elif defined(__mips__)
elf_file.ehdr.e_machine = EM_MIPS;
#elif defined(__powerpc__)
elf_file.ehdr.e_machine = EM_PPC;
#elif defined(__i386) || defined(__amd64)
elf_file.ehdr.e_machine = EM_386;
#elif defined(__aarch64__)
elf_file.ehdr.e_machine = EM_AARCH64;
#endif
elf_file.ehdr.e_version = EV_CURRENT;
elf_file.ehdr.e_shoff = sizeof (Elf32_Ehdr);
elf_file.ehdr.e_ehsize = sizeof (Elf32_Ehdr);
elf_file.ehdr.e_phentsize = sizeof (Elf32_Phdr);
elf_file.ehdr.e_shentsize = sizeof (Elf32_Shdr);
elf_file.ehdr.e_shnum = nshdr;
elf_file.ehdr.e_shstrndx = ESHDR_SHSTRTAB;
off = sizeof (elf_file) + nshdr * sizeof (Elf32_Shdr);

shp = &elf_file.shdr[ESHDR_SHSTRTAB];
shp->sh_name = 1;
shp->sh_type = SHT_STRTAB;
shp->sh_offset = off;
shp->sh_size = sizeof (DTRACE_SHSTRTAB32);
shp->sh_addralign = sizeof (char);
off = roundup2(shp->sh_offset + shp->sh_size, 8);

shp = &elf_file.shdr[ESHDR_DOF];
shp->sh_name = 11;
shp->sh_flags = SHF_ALLOC;
shp->sh_type = SHT_SUNW_dof;
shp->sh_offset = off;
shp->sh_size = dof->dofh_filesz;
shp->sh_addralign = 8;
off = shp->sh_offset + shp->sh_size;

shp = &elf_file.shdr[ESHDR_STRTAB];
shp->sh_name = 21;
shp->sh_flags = SHF_ALLOC;
shp->sh_type = SHT_STRTAB;
shp->sh_offset = off;
shp->sh_size = de.de_strlen;
shp->sh_addralign = sizeof (char);
off = roundup2(shp->sh_offset + shp->sh_size, 4);

shp = &elf_file.shdr[ESHDR_SYMTAB];
shp->sh_name = 29;
shp->sh_flags = SHF_ALLOC;
shp->sh_type = SHT_SYMTAB;
shp->sh_entsize = sizeof (Elf32_Sym);
shp->sh_link = ESHDR_STRTAB;
shp->sh_offset = off;
shp->sh_info = de.de_global;
shp->sh_size = de.de_nsym * sizeof (Elf32_Sym);
shp->sh_addralign = 4;
off = roundup2(shp->sh_offset + shp->sh_size, 4);

if (de.de_nrel == 0) {
if (dt_write(dtp, fd, &elf_file,
sizeof (elf_file)) != sizeof (elf_file) ||
PWRITE_SCN(ESHDR_SHSTRTAB, DTRACE_SHSTRTAB32) ||
PWRITE_SCN(ESHDR_STRTAB, de.de_strtab) ||
PWRITE_SCN(ESHDR_SYMTAB, de.de_sym) ||
PWRITE_SCN(ESHDR_DOF, dof)) {
ret = dt_set_errno(dtp, errno);
}
} else {
shp = &elf_file.shdr[ESHDR_REL];
shp->sh_name = 37;
shp->sh_flags = SHF_ALLOC;
shp->sh_type = SHT_REL;
shp->sh_entsize = sizeof (de.de_rel[0]);
shp->sh_link = ESHDR_SYMTAB;
shp->sh_info = ESHDR_DOF;
shp->sh_offset = off;
shp->sh_size = de.de_nrel * sizeof (de.de_rel[0]);
shp->sh_addralign = 4;

if (dt_write(dtp, fd, &elf_file,
sizeof (elf_file)) != sizeof (elf_file) ||
PWRITE_SCN(ESHDR_SHSTRTAB, DTRACE_SHSTRTAB32) ||
PWRITE_SCN(ESHDR_STRTAB, de.de_strtab) ||
PWRITE_SCN(ESHDR_SYMTAB, de.de_sym) ||
PWRITE_SCN(ESHDR_REL, de.de_rel) ||
PWRITE_SCN(ESHDR_DOF, dof)) {
ret = dt_set_errno(dtp, errno);
}
}

free(de.de_strtab);
free(de.de_sym);
free(de.de_rel);

return (ret);
}






static int
dump_elf64(dtrace_hdl_t *dtp, const dof_hdr_t *dof, int fd)
{
struct {
Elf64_Ehdr ehdr;
Elf64_Shdr shdr[ESHDR_NUM];
} elf_file;

Elf64_Shdr *shp;
Elf64_Off off;
dof_elf64_t de;
int ret = 0;
uint_t nshdr;

if (prepare_elf64(dtp, dof, &de) != 0)
return (-1);





nshdr = de.de_nrel == 0 ? ESHDR_SYMTAB + 1 : ESHDR_NUM;

bzero(&elf_file, sizeof (elf_file));

elf_file.ehdr.e_ident[EI_MAG0] = ELFMAG0;
elf_file.ehdr.e_ident[EI_MAG1] = ELFMAG1;
elf_file.ehdr.e_ident[EI_MAG2] = ELFMAG2;
elf_file.ehdr.e_ident[EI_MAG3] = ELFMAG3;
elf_file.ehdr.e_ident[EI_VERSION] = EV_CURRENT;
elf_file.ehdr.e_ident[EI_CLASS] = ELFCLASS64;
#if BYTE_ORDER == _BIG_ENDIAN
elf_file.ehdr.e_ident[EI_DATA] = ELFDATA2MSB;
#else
elf_file.ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
#endif
elf_file.ehdr.e_ident[EI_OSABI] = ELFOSABI_FREEBSD;
elf_file.ehdr.e_type = ET_REL;
#if defined(__arm__)
elf_file.ehdr.e_machine = EM_ARM;
#elif defined(__mips__)
elf_file.ehdr.e_machine = EM_MIPS;
#elif defined(__powerpc64__)
#if defined(_CALL_ELF) && _CALL_ELF == 2
elf_file.ehdr.e_flags = 2;
#endif
elf_file.ehdr.e_machine = EM_PPC64;
#elif defined(__i386) || defined(__amd64)
elf_file.ehdr.e_machine = EM_AMD64;
#elif defined(__aarch64__)
elf_file.ehdr.e_machine = EM_AARCH64;
#endif
elf_file.ehdr.e_version = EV_CURRENT;
elf_file.ehdr.e_shoff = sizeof (Elf64_Ehdr);
elf_file.ehdr.e_ehsize = sizeof (Elf64_Ehdr);
elf_file.ehdr.e_phentsize = sizeof (Elf64_Phdr);
elf_file.ehdr.e_shentsize = sizeof (Elf64_Shdr);
elf_file.ehdr.e_shnum = nshdr;
elf_file.ehdr.e_shstrndx = ESHDR_SHSTRTAB;
off = sizeof (elf_file) + nshdr * sizeof (Elf64_Shdr);

shp = &elf_file.shdr[ESHDR_SHSTRTAB];
shp->sh_name = 1;
shp->sh_type = SHT_STRTAB;
shp->sh_offset = off;
shp->sh_size = sizeof (DTRACE_SHSTRTAB64);
shp->sh_addralign = sizeof (char);
off = roundup2(shp->sh_offset + shp->sh_size, 8);

shp = &elf_file.shdr[ESHDR_DOF];
shp->sh_name = 11;
shp->sh_flags = SHF_ALLOC;
shp->sh_type = SHT_SUNW_dof;
shp->sh_offset = off;
shp->sh_size = dof->dofh_filesz;
shp->sh_addralign = 8;
off = shp->sh_offset + shp->sh_size;

shp = &elf_file.shdr[ESHDR_STRTAB];
shp->sh_name = 21;
shp->sh_flags = SHF_ALLOC;
shp->sh_type = SHT_STRTAB;
shp->sh_offset = off;
shp->sh_size = de.de_strlen;
shp->sh_addralign = sizeof (char);
off = roundup2(shp->sh_offset + shp->sh_size, 8);

shp = &elf_file.shdr[ESHDR_SYMTAB];
shp->sh_name = 29;
shp->sh_flags = SHF_ALLOC;
shp->sh_type = SHT_SYMTAB;
shp->sh_entsize = sizeof (Elf64_Sym);
shp->sh_link = ESHDR_STRTAB;
shp->sh_offset = off;
shp->sh_info = de.de_global;
shp->sh_size = de.de_nsym * sizeof (Elf64_Sym);
shp->sh_addralign = 8;
off = roundup2(shp->sh_offset + shp->sh_size, 8);

if (de.de_nrel == 0) {
if (dt_write(dtp, fd, &elf_file,
sizeof (elf_file)) != sizeof (elf_file) ||
PWRITE_SCN(ESHDR_SHSTRTAB, DTRACE_SHSTRTAB64) ||
PWRITE_SCN(ESHDR_STRTAB, de.de_strtab) ||
PWRITE_SCN(ESHDR_SYMTAB, de.de_sym) ||
PWRITE_SCN(ESHDR_DOF, dof)) {
ret = dt_set_errno(dtp, errno);
}
} else {
shp = &elf_file.shdr[ESHDR_REL];
shp->sh_name = 37;
shp->sh_flags = SHF_ALLOC;
shp->sh_type = SHT_RELA;
shp->sh_entsize = sizeof (de.de_rel[0]);
shp->sh_link = ESHDR_SYMTAB;
shp->sh_info = ESHDR_DOF;
shp->sh_offset = off;
shp->sh_size = de.de_nrel * sizeof (de.de_rel[0]);
shp->sh_addralign = 8;

if (dt_write(dtp, fd, &elf_file,
sizeof (elf_file)) != sizeof (elf_file) ||
PWRITE_SCN(ESHDR_SHSTRTAB, DTRACE_SHSTRTAB64) ||
PWRITE_SCN(ESHDR_STRTAB, de.de_strtab) ||
PWRITE_SCN(ESHDR_SYMTAB, de.de_sym) ||
PWRITE_SCN(ESHDR_REL, de.de_rel) ||
PWRITE_SCN(ESHDR_DOF, dof)) {
ret = dt_set_errno(dtp, errno);
}
}

free(de.de_strtab);
free(de.de_sym);
free(de.de_rel);

return (ret);
}

static int
dt_symtab_lookup(Elf_Data *data_sym, int start, int end, uintptr_t addr,
uint_t shn, GElf_Sym *sym, int uses_funcdesc, Elf *elf)
{
Elf64_Addr symval;
Elf_Scn *opd_scn;
Elf_Data *opd_desc;
int i;

for (i = start; i < end && gelf_getsym(data_sym, i, sym) != NULL; i++) {
if (GELF_ST_TYPE(sym->st_info) == STT_FUNC) {
symval = sym->st_value;
if (uses_funcdesc) {
opd_scn = elf_getscn(elf, sym->st_shndx);
opd_desc = elf_rawdata(opd_scn, NULL);
symval =
*(uint64_t*)((char *)opd_desc->d_buf + symval);
}
if ((uses_funcdesc || shn == sym->st_shndx) &&
symval <= addr && addr < symval + sym->st_size)
return (0);
}
}

return (-1);
}

#if defined(__aarch64__)
#define DT_OP_NOP 0xd503201f
#define DT_OP_RET 0xd65f03c0
#define DT_OP_CALL26 0x94000000
#define DT_OP_JUMP26 0x14000000
#define DT_REL_NONE R_AARCH64_NONE

static int
dt_modtext(dtrace_hdl_t *dtp, char *p, int isenabled, GElf_Rela *rela,
uint32_t *off)
{
uint32_t *ip;




if ((rela->r_offset & (sizeof (uint32_t) - 1)) != 0)
return (-1);






if (GELF_R_TYPE(rela->r_info) != R_AARCH64_CALL26 &&
GELF_R_TYPE(rela->r_info) != R_AARCH64_JUMP26 &&
GELF_R_TYPE(rela->r_info) != R_AARCH64_NONE)
return (-1);

ip = (uint32_t *)(p + rela->r_offset);






if (ip[0] == DT_OP_NOP || ip[0] == DT_OP_RET)
return (0);





if (ip[0] != DT_OP_CALL26 && ip[0] != DT_OP_JUMP26) {
dt_dprintf("found %x instead of a call or jmp instruction at "
"%llx\n", ip[0], (u_longlong_t)rela->r_offset);
return (-1);
}








if (ip[0] == DT_OP_CALL26)
ip[0] = DT_OP_NOP;
else
ip[0] = DT_OP_RET;

return (0);
}
#elif defined(__arm__)
#define DT_REL_NONE R_ARM_NONE

static int
dt_modtext(dtrace_hdl_t *dtp, char *p, int isenabled, GElf_Rela *rela,
uint32_t *off)
{
printf("%s:%s(%d): arm not implemented\n", __FUNCTION__, __FILE__,
__LINE__);
return (-1);
}
#elif defined(__mips__)
#define DT_REL_NONE R_MIPS_NONE

static int
dt_modtext(dtrace_hdl_t *dtp, char *p, int isenabled, GElf_Rela *rela,
uint32_t *off)
{
printf("%s:%s(%d): MIPS not implemented\n", __FUNCTION__, __FILE__,
__LINE__);
return (-1);
}
#elif defined(__powerpc__)

#define DT_OP_XOR_R3 0x7c631a78

#define DT_OP_NOP 0x60000000
#define DT_OP_BLR 0x4e800020


#define DT_IS_BRANCH(inst) ((inst & 0xfc000000) == 0x48000000)
#define DT_IS_BL(inst) (DT_IS_BRANCH(inst) && (inst & 0x01))

#define DT_REL_NONE R_PPC_NONE

static int
dt_modtext(dtrace_hdl_t *dtp, char *p, int isenabled, GElf_Rela *rela,
uint32_t *off)
{
uint32_t *ip;

if ((rela->r_offset & (sizeof (uint32_t) - 1)) != 0)
return (-1);


ip = (uint32_t *)(p + rela->r_offset);




if (GELF_R_TYPE(rela->r_info) != R_PPC_REL24 &&
GELF_R_TYPE(rela->r_info) != R_PPC_PLTREL24 &&
GELF_R_TYPE(rela->r_info) != R_PPC_NONE)
return (-1);






if (isenabled) {
if (ip[0] == DT_OP_XOR_R3) {
(*off) += sizeof (ip[0]);
return (0);
}
} else {
if (ip[0] == DT_OP_NOP) {
(*off) += sizeof (ip[0]);
return (0);
}
}




if (!DT_IS_BRANCH(ip[0])) {
dt_dprintf("found %x instead of a branch instruction at %llx\n",
ip[0], (u_longlong_t)rela->r_offset);
return (-1);
}

if (isenabled) {






if (!DT_IS_BL(ip[0])) {
dt_dprintf("tail call to is-enabled probe at %llx\n",
(u_longlong_t)rela->r_offset);
return (-1);
}

ip[0] = DT_OP_XOR_R3;
(*off) += sizeof (ip[0]);
} else {
if (DT_IS_BL(ip[0]))
ip[0] = DT_OP_NOP;
else
ip[0] = DT_OP_BLR;
}

return (0);
}
#elif defined(__riscv)
#define DT_REL_NONE R_RISCV_NONE
static int
dt_modtext(dtrace_hdl_t *dtp, char *p, int isenabled, GElf_Rela *rela,
uint32_t *off)
{
printf("%s:%s(%d): RISC-V implementation required\n", __FUNCTION__,
__FILE__, __LINE__);
return (-1);
}

#elif defined(__i386) || defined(__amd64)

#define DT_OP_NOP 0x90
#define DT_OP_RET 0xc3
#define DT_OP_CALL 0xe8
#define DT_OP_JMP32 0xe9
#define DT_OP_REX_RAX 0x48
#define DT_OP_XOR_EAX_0 0x33
#define DT_OP_XOR_EAX_1 0xc0

#define DT_REL_NONE R_386_NONE

static int
dt_modtext(dtrace_hdl_t *dtp, char *p, int isenabled, GElf_Rela *rela,
uint32_t *off)
{
uint8_t *ip = (uint8_t *)(p + rela->r_offset - 1);
uint8_t ret;








(*off) -= 1;






if (GELF_R_TYPE(rela->r_info) != R_386_PC32 &&
GELF_R_TYPE(rela->r_info) != R_386_PLT32 &&
GELF_R_TYPE(rela->r_info) != R_386_NONE)
return (-1);








if (!isenabled) {
if ((ip[0] == DT_OP_NOP || ip[0] == DT_OP_RET) &&
ip[1] == DT_OP_NOP && ip[2] == DT_OP_NOP &&
ip[3] == DT_OP_NOP && ip[4] == DT_OP_NOP)
return (0);
} else if (dtp->dt_oflags & DTRACE_O_LP64) {
if (ip[0] == DT_OP_REX_RAX &&
ip[1] == DT_OP_XOR_EAX_0 && ip[2] == DT_OP_XOR_EAX_1 &&
(ip[3] == DT_OP_NOP || ip[3] == DT_OP_RET) &&
ip[4] == DT_OP_NOP) {
(*off) += 3;
return (0);
}
} else {
if (ip[0] == DT_OP_XOR_EAX_0 && ip[1] == DT_OP_XOR_EAX_1 &&
(ip[2] == DT_OP_NOP || ip[2] == DT_OP_RET) &&
ip[3] == DT_OP_NOP && ip[4] == DT_OP_NOP) {
(*off) += 2;
return (0);
}
}





if (ip[0] != DT_OP_CALL && ip[0] != DT_OP_JMP32) {
dt_dprintf("found %x instead of a call or jmp instruction at "
"%llx\n", ip[0], (u_longlong_t)rela->r_offset);
return (-1);
}

ret = (ip[0] == DT_OP_JMP32) ? DT_OP_RET : DT_OP_NOP;








if (!isenabled) {
ip[0] = ret;
ip[1] = DT_OP_NOP;
ip[2] = DT_OP_NOP;
ip[3] = DT_OP_NOP;
ip[4] = DT_OP_NOP;
} else if (dtp->dt_oflags & DTRACE_O_LP64) {
ip[0] = DT_OP_REX_RAX;
ip[1] = DT_OP_XOR_EAX_0;
ip[2] = DT_OP_XOR_EAX_1;
ip[3] = ret;
ip[4] = DT_OP_NOP;
(*off) += 3;
} else {
ip[0] = DT_OP_XOR_EAX_0;
ip[1] = DT_OP_XOR_EAX_1;
ip[2] = ret;
ip[3] = DT_OP_NOP;
ip[4] = DT_OP_NOP;
(*off) += 2;
}

return (0);
}

#else
#error unknown ISA
#endif


static int
dt_link_error(dtrace_hdl_t *dtp, Elf *elf, int fd, dt_link_pair_t *bufs,
const char *format, ...)
{
va_list ap;
dt_link_pair_t *pair;

va_start(ap, format);
dt_set_errmsg(dtp, NULL, NULL, NULL, 0, format, ap);
va_end(ap);

if (elf != NULL)
(void) elf_end(elf);

if (fd >= 0)
(void) close(fd);

while ((pair = bufs) != NULL) {
bufs = pair->dlp_next;
dt_free(dtp, pair->dlp_str);
dt_free(dtp, pair->dlp_sym);
dt_free(dtp, pair);
}

return (dt_set_errno(dtp, EDT_COMPILER));
}





static unsigned int
hash_obj(const char *obj, int fd)
{
char path[PATH_MAX];
unsigned int h;

if (realpath(obj, path) == NULL)
return (-1);

for (h = 2166136261u, obj = &path[0]; *obj != '\0'; obj++)
h = (h ^ *obj) * 16777619;
h &= 0x7fffffff;
return (h);
}

static int
process_obj(dtrace_hdl_t *dtp, const char *obj, int *eprobesp)
{
static const char dt_prefix[] = "__dtrace";
static const char dt_enabled[] = "enabled";
static const char dt_symprefix[] = "$dtrace";
static const char dt_symfmt[] = "%s%u.%s";
static const char dt_weaksymfmt[] = "%s.%s";
char probename[DTRACE_NAMELEN];
int fd, i, ndx, eprobe, uses_funcdesc = 0, mod = 0;
Elf *elf = NULL;
GElf_Ehdr ehdr;
Elf_Scn *scn_rel, *scn_sym, *scn_str, *scn_tgt;
Elf_Data *data_rel, *data_sym, *data_str, *data_tgt;
GElf_Shdr shdr_rel, shdr_sym, shdr_str, shdr_tgt;
GElf_Sym rsym, fsym, dsym;
GElf_Rela rela;
char *s, *p, *r;
char pname[DTRACE_PROVNAMELEN];
dt_provider_t *pvp;
dt_probe_t *prp;
uint32_t off, eclass, emachine1, emachine2;
size_t symsize, osym, nsym, isym, istr, len;
unsigned int objkey;
dt_link_pair_t *pair, *bufs = NULL;
dt_strtab_t *strtab;
void *tmp;

if ((fd = open64(obj, O_RDWR)) == -1) {
return (dt_link_error(dtp, elf, fd, bufs,
"failed to open %s: %s", obj, strerror(errno)));
}

if ((elf = elf_begin(fd, ELF_C_RDWR, NULL)) == NULL) {
return (dt_link_error(dtp, elf, fd, bufs,
"failed to process %s: %s", obj, elf_errmsg(elf_errno())));
}

switch (elf_kind(elf)) {
case ELF_K_ELF:
break;
case ELF_K_AR:
return (dt_link_error(dtp, elf, fd, bufs, "archives are not "
"permitted; use the contents of the archive instead: %s",
obj));
default:
return (dt_link_error(dtp, elf, fd, bufs,
"invalid file type: %s", obj));
}

if (gelf_getehdr(elf, &ehdr) == NULL) {
return (dt_link_error(dtp, elf, fd, bufs, "corrupt file: %s",
obj));
}

if (dtp->dt_oflags & DTRACE_O_LP64) {
eclass = ELFCLASS64;
#if defined(__mips__)
emachine1 = emachine2 = EM_MIPS;
#elif defined(__powerpc__)
emachine1 = emachine2 = EM_PPC64;
#if !defined(_CALL_ELF) || _CALL_ELF == 1
uses_funcdesc = 1;
#endif
#elif defined(__i386) || defined(__amd64)
emachine1 = emachine2 = EM_AMD64;
#elif defined(__aarch64__)
emachine1 = emachine2 = EM_AARCH64;
#endif
symsize = sizeof (Elf64_Sym);
} else {
eclass = ELFCLASS32;
#if defined(__arm__)
emachine1 = emachine2 = EM_ARM;
#elif defined(__mips__)
emachine1 = emachine2 = EM_MIPS;
#elif defined(__powerpc__)
emachine1 = emachine2 = EM_PPC;
#elif defined(__i386) || defined(__amd64)
emachine1 = emachine2 = EM_386;
#endif
symsize = sizeof (Elf32_Sym);
}

if (ehdr.e_ident[EI_CLASS] != eclass) {
return (dt_link_error(dtp, elf, fd, bufs,
"incorrect ELF class for object file: %s", obj));
}

if (ehdr.e_machine != emachine1 && ehdr.e_machine != emachine2) {
return (dt_link_error(dtp, elf, fd, bufs,
"incorrect ELF machine type for object file: %s", obj));
}






if ((objkey = hash_obj(obj, fd)) == (unsigned int)-1)
return (dt_link_error(dtp, elf, fd, bufs,
"failed to generate unique key for object file: %s", obj));

scn_rel = NULL;
while ((scn_rel = elf_nextscn(elf, scn_rel)) != NULL) {
if (gelf_getshdr(scn_rel, &shdr_rel) == NULL)
goto err;




if (shdr_rel.sh_type != SHT_RELA && shdr_rel.sh_type != SHT_REL)
continue;

if ((data_rel = elf_getdata(scn_rel, NULL)) == NULL)
goto err;





if ((scn_sym = elf_getscn(elf, shdr_rel.sh_link)) == NULL ||
gelf_getshdr(scn_sym, &shdr_sym) == NULL ||
(data_sym = elf_getdata(scn_sym, NULL)) == NULL)
goto err;




if ((scn_str = elf_getscn(elf, shdr_sym.sh_link)) == NULL ||
gelf_getshdr(scn_str, &shdr_str) == NULL ||
(data_str = elf_getdata(scn_str, NULL)) == NULL)
goto err;







if ((scn_tgt = elf_getscn(elf, shdr_rel.sh_info)) == NULL ||
gelf_getshdr(scn_tgt, &shdr_tgt) == NULL ||
(data_tgt = elf_getdata(scn_tgt, NULL)) == NULL)
goto err;


































strtab = dt_strtab_create(1);
nsym = 0;
isym = data_sym->d_size / symsize;
istr = data_str->d_size;

for (i = 0; i < shdr_rel.sh_size / shdr_rel.sh_entsize; i++) {
if (shdr_rel.sh_type == SHT_RELA) {
if (gelf_getrela(data_rel, i, &rela) == NULL)
continue;
} else {
GElf_Rel rel;
if (gelf_getrel(data_rel, i, &rel) == NULL)
continue;
rela.r_offset = rel.r_offset;
rela.r_info = rel.r_info;
rela.r_addend = 0;
}

if (gelf_getsym(data_sym, GELF_R_SYM(rela.r_info),
&rsym) == NULL) {
dt_strtab_destroy(strtab);
goto err;
}

s = (char *)data_str->d_buf + rsym.st_name;

if (strncmp(s, dt_prefix, sizeof (dt_prefix) - 1) != 0)
continue;

if (dt_symtab_lookup(data_sym, 0, isym, rela.r_offset,
shdr_rel.sh_info, &fsym, uses_funcdesc,
elf) != 0) {
dt_strtab_destroy(strtab);
goto err;
}

if (fsym.st_name > data_str->d_size) {
dt_strtab_destroy(strtab);
goto err;
}

s = (char *)data_str->d_buf + fsym.st_name;





if (GELF_ST_TYPE(fsym.st_info) != STT_FUNC) {
dt_strtab_destroy(strtab);
return (dt_link_error(dtp, elf, fd, bufs,
"expected %s to be of type function", s));
}




if (GELF_ST_BIND(fsym.st_info) == STB_WEAK) {
len = snprintf(NULL, 0, dt_weaksymfmt,
dt_symprefix, s) + 1;
} else {
len = snprintf(NULL, 0, dt_symfmt, dt_symprefix,
objkey, s) + 1;
}
if ((p = dt_alloc(dtp, len)) == NULL) {
dt_strtab_destroy(strtab);
goto err;
}
if (GELF_ST_BIND(fsym.st_info) == STB_WEAK) {
(void) snprintf(p, len, dt_weaksymfmt,
dt_symprefix, s);
} else {
(void) snprintf(p, len, dt_symfmt, dt_symprefix,
objkey, s);
}

if (dt_strtab_index(strtab, p) == -1) {




if (GELF_R_TYPE(rela.r_info) != DT_REL_NONE)
nsym++;
(void) dt_strtab_insert(strtab, p);
}

dt_free(dtp, p);
}









osym = isym;
if (nsym > 0) {




len = dt_strtab_size(strtab) - 1;

assert(len > 0);
assert(dt_strtab_index(strtab, "") == 0);

dt_strtab_destroy(strtab);

if ((pair = dt_alloc(dtp, sizeof (*pair))) == NULL)
goto err;

if ((pair->dlp_str = dt_alloc(dtp, data_str->d_size +
len)) == NULL) {
dt_free(dtp, pair);
goto err;
}

if ((pair->dlp_sym = dt_alloc(dtp, data_sym->d_size +
nsym * symsize)) == NULL) {
dt_free(dtp, pair->dlp_str);
dt_free(dtp, pair);
goto err;
}

pair->dlp_next = bufs;
bufs = pair;

bcopy(data_str->d_buf, pair->dlp_str, data_str->d_size);
tmp = data_str->d_buf;
data_str->d_buf = pair->dlp_str;
pair->dlp_str = tmp;
data_str->d_size += len;
(void) elf_flagdata(data_str, ELF_C_SET, ELF_F_DIRTY);

shdr_str.sh_size += len;
(void) gelf_update_shdr(scn_str, &shdr_str);

bcopy(data_sym->d_buf, pair->dlp_sym, data_sym->d_size);
tmp = data_sym->d_buf;
data_sym->d_buf = pair->dlp_sym;
pair->dlp_sym = tmp;
data_sym->d_size += nsym * symsize;
(void) elf_flagdata(data_sym, ELF_C_SET, ELF_F_DIRTY);

shdr_sym.sh_size += nsym * symsize;
(void) gelf_update_shdr(scn_sym, &shdr_sym);

nsym += isym;
} else if (dt_strtab_empty(strtab)) {
dt_strtab_destroy(strtab);
continue;
}





for (i = 0; i < shdr_rel.sh_size / shdr_rel.sh_entsize; i++) {
if (shdr_rel.sh_type == SHT_RELA) {
if (gelf_getrela(data_rel, i, &rela) == NULL)
continue;
} else {
GElf_Rel rel;
if (gelf_getrel(data_rel, i, &rel) == NULL)
continue;
rela.r_offset = rel.r_offset;
rela.r_info = rel.r_info;
rela.r_addend = 0;
}

ndx = GELF_R_SYM(rela.r_info);

if (gelf_getsym(data_sym, ndx, &rsym) == NULL ||
rsym.st_name > data_str->d_size)
goto err;

s = (char *)data_str->d_buf + rsym.st_name;

if (strncmp(s, dt_prefix, sizeof (dt_prefix) - 1) != 0)
continue;

s += sizeof (dt_prefix) - 1;





if (strncmp(s, dt_enabled,
sizeof (dt_enabled) - 1) == 0) {
s += sizeof (dt_enabled) - 1;
eprobe = 1;
*eprobesp = 1;
dt_dprintf("is-enabled probe\n");
} else {
eprobe = 0;
dt_dprintf("normal probe\n");
}

if (*s++ != '_')
goto err;

if ((p = strstr(s, "___")) == NULL ||
p - s >= sizeof (pname))
goto err;

bcopy(s, pname, p - s);
pname[p - s] = '\0';

if (dt_symtab_lookup(data_sym, osym, isym,
rela.r_offset, shdr_rel.sh_info, &fsym,
uses_funcdesc, elf) == 0) {
if (fsym.st_name > data_str->d_size)
goto err;

r = s = (char *) data_str->d_buf + fsym.st_name;
assert(strstr(s, dt_symprefix) == s);
s = strchr(s, '.') + 1;
} else if (dt_symtab_lookup(data_sym, 0, osym,
rela.r_offset, shdr_rel.sh_info, &fsym,
uses_funcdesc, elf) == 0) {
u_int bind;

bind = GELF_ST_BIND(fsym.st_info) == STB_WEAK ?
STB_WEAK : STB_GLOBAL;
s = (char *) data_str->d_buf + fsym.st_name;
if (GELF_R_TYPE(rela.r_info) != DT_REL_NONE) {









dsym = fsym;
dsym.st_name = istr;
dsym.st_info = GELF_ST_INFO(bind,
STT_FUNC);
dsym.st_other =
GELF_ST_VISIBILITY(STV_HIDDEN);
(void) gelf_update_sym(data_sym, isym,
&dsym);
isym++;
assert(isym <= nsym);

r = (char *) data_str->d_buf + istr;
if (bind == STB_WEAK) {
istr += sprintf(r,
dt_weaksymfmt, dt_symprefix,
s);
} else {
istr += sprintf(r, dt_symfmt,
dt_symprefix, objkey, s);
}
istr++;
} else {
if (bind == STB_WEAK) {
(void) asprintf(&r,
dt_weaksymfmt, dt_symprefix,
s);
} else {
(void) asprintf(&r, dt_symfmt,
dt_symprefix, objkey, s);
}
}
} else {
goto err;
}

if ((pvp = dt_provider_lookup(dtp, pname)) == NULL) {
return (dt_link_error(dtp, elf, fd, bufs,
"no such provider %s", pname));
}

if (strlcpy(probename, p + 3, sizeof (probename)) >=
sizeof (probename))
return (dt_link_error(dtp, elf, fd, bufs,
"invalid probe name %s", probename));
(void) strhyphenate(probename);
if ((prp = dt_probe_lookup(pvp, probename)) == NULL)
return (dt_link_error(dtp, elf, fd, bufs,
"no such probe %s", probename));

assert(fsym.st_value <= rela.r_offset);

off = rela.r_offset - fsym.st_value;
if (dt_modtext(dtp, data_tgt->d_buf, eprobe,
&rela, &off) != 0)
goto err;

if (dt_probe_define(pvp, prp, s, r, off, eprobe) != 0) {
return (dt_link_error(dtp, elf, fd, bufs,
"failed to allocate space for probe"));
}





if (shdr_rel.sh_type == SHT_RELA) {
rela.r_info = GELF_R_INFO(
GELF_R_SYM(rela.r_info), DT_REL_NONE);
(void) gelf_update_rela(data_rel, i, &rela);
} else {
GElf_Rel rel;
rel.r_offset = rela.r_offset;
rel.r_info = GELF_R_INFO(
GELF_R_SYM(rela.r_info), DT_REL_NONE);
(void) gelf_update_rel(data_rel, i, &rel);
}

mod = 1;
(void) elf_flagdata(data_tgt, ELF_C_SET, ELF_F_DIRTY);








if (rsym.st_shndx != SHN_ABS) {
rsym.st_shndx = SHN_ABS;
(void) gelf_update_sym(data_sym, ndx, &rsym);
}
}
}

if (mod && elf_update(elf, ELF_C_WRITE) == -1)
goto err;

(void) elf_end(elf);
(void) close(fd);

while ((pair = bufs) != NULL) {
bufs = pair->dlp_next;
dt_free(dtp, pair->dlp_str);
dt_free(dtp, pair->dlp_sym);
dt_free(dtp, pair);
}

return (0);

err:
return (dt_link_error(dtp, elf, fd, bufs,
"an error was encountered while processing %s", obj));
}

int
dtrace_program_link(dtrace_hdl_t *dtp, dtrace_prog_t *pgp, uint_t dflags,
const char *file, int objc, char *const objv[])
{
char tfile[PATH_MAX];
char drti[PATH_MAX];
dof_hdr_t *dof;
int fd, status, i, cur;
char *cmd, tmp;
size_t len;
int eprobes = 0, ret = 0;






if (pgp == NULL) {
const char *fmt = "%s -o %s -r";

len = snprintf(&tmp, 1, fmt, dtp->dt_ld_path, file) + 1;

for (i = 0; i < objc; i++)
len += strlen(objv[i]) + 1;

cmd = alloca(len);

cur = snprintf(cmd, len, fmt, dtp->dt_ld_path, file);

for (i = 0; i < objc; i++)
cur += snprintf(cmd + cur, len - cur, " %s", objv[i]);

if ((status = system(cmd)) == -1) {
return (dt_link_error(dtp, NULL, -1, NULL,
"failed to run %s: %s", dtp->dt_ld_path,
strerror(errno)));
}

if (WIFSIGNALED(status)) {
return (dt_link_error(dtp, NULL, -1, NULL,
"failed to link %s: %s failed due to signal %d",
file, dtp->dt_ld_path, WTERMSIG(status)));
}

if (WEXITSTATUS(status) != 0) {
return (dt_link_error(dtp, NULL, -1, NULL,
"failed to link %s: %s exited with status %d\n",
file, dtp->dt_ld_path, WEXITSTATUS(status)));
}

for (i = 0; i < objc; i++) {
if (strcmp(objv[i], file) != 0)
(void) unlink(objv[i]);
}

return (0);
}

for (i = 0; i < objc; i++) {
if (process_obj(dtp, objv[i], &eprobes) != 0)
return (-1);
}





if (eprobes && pgp->dp_dofversion < DOF_VERSION_2)
pgp->dp_dofversion = DOF_VERSION_2;

if ((dof = dtrace_dof_create(dtp, pgp, dflags)) == NULL)
return (-1);

snprintf(tfile, sizeof(tfile), "%s.XXXXXX", file);
if ((fd = mkostemp(tfile, O_CLOEXEC)) == -1)
return (dt_link_error(dtp, NULL, -1, NULL,
"failed to create temporary file %s: %s",
tfile, strerror(errno)));





switch (dtp->dt_linktype) {
case DT_LTYP_DOF:
if (dt_write(dtp, fd, dof, dof->dofh_filesz) < dof->dofh_filesz)
ret = errno;

if (close(fd) != 0 && ret == 0)
ret = errno;

if (ret != 0) {
return (dt_link_error(dtp, NULL, -1, NULL,
"failed to write %s: %s", file, strerror(ret)));
}

return (0);

case DT_LTYP_ELF:
break;

default:
return (dt_link_error(dtp, NULL, -1, NULL,
"invalid link type %u\n", dtp->dt_linktype));
}


if (dtp->dt_oflags & DTRACE_O_LP64)
status = dump_elf64(dtp, dof, fd);
else
status = dump_elf32(dtp, dof, fd);

if (status != 0)
return (dt_link_error(dtp, NULL, -1, NULL,
"failed to write %s: %s", tfile,
strerror(dtrace_errno(dtp))));

if (!dtp->dt_lazyload) {
const char *fmt = "%s -o %s -r %s %s";
dt_dirpath_t *dp = dt_list_next(&dtp->dt_lib_path);

(void) snprintf(drti, sizeof (drti), "%s/drti.o", dp->dir_path);

len = snprintf(&tmp, 1, fmt, dtp->dt_ld_path, file, tfile,
drti) + 1;

cmd = alloca(len);

(void) snprintf(cmd, len, fmt, dtp->dt_ld_path, file, tfile,
drti);
if ((status = system(cmd)) == -1) {
ret = dt_link_error(dtp, NULL, fd, NULL,
"failed to run %s: %s", dtp->dt_ld_path,
strerror(errno));
goto done;
}

if (WIFSIGNALED(status)) {
ret = dt_link_error(dtp, NULL, fd, NULL,
"failed to link %s: %s failed due to signal %d",
file, dtp->dt_ld_path, WTERMSIG(status));
goto done;
}

if (WEXITSTATUS(status) != 0) {
ret = dt_link_error(dtp, NULL, fd, NULL,
"failed to link %s: %s exited with status %d\n",
file, dtp->dt_ld_path, WEXITSTATUS(status));
goto done;
}
(void) close(fd);









asprintf(&cmd, "%s --localize-hidden %s", dtp->dt_objcopy_path,
file);
if ((status = system(cmd)) == -1) {
ret = dt_link_error(dtp, NULL, -1, NULL,
"failed to run %s: %s", dtp->dt_objcopy_path,
strerror(errno));
free(cmd);
goto done;
}
free(cmd);

if (WIFSIGNALED(status)) {
ret = dt_link_error(dtp, NULL, -1, NULL,
"failed to link %s: %s failed due to signal %d",
file, dtp->dt_objcopy_path, WTERMSIG(status));
goto done;
}

if (WEXITSTATUS(status) != 0) {
ret = dt_link_error(dtp, NULL, -1, NULL,
"failed to link %s: %s exited with status %d\n",
file, dtp->dt_objcopy_path, WEXITSTATUS(status));
goto done;
}
} else {
if (rename(tfile, file) != 0) {
ret = dt_link_error(dtp, NULL, fd, NULL,
"failed to rename %s to %s: %s", tfile, file,
strerror(errno));
goto done;
}
(void) close(fd);
}

done:
dtrace_dof_destroy(dtp, dof);

if (!dtp->dt_lazyload)
(void) unlink(tfile);
return (ret);
}
