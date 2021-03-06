



























#if !defined(_GELF_H_)
#define _GELF_H_

#include <libelf.h>

typedef Elf64_Addr GElf_Addr;
typedef Elf64_Half GElf_Half;
typedef Elf64_Off GElf_Off;
typedef Elf64_Sword GElf_Sword;
typedef Elf64_Sxword GElf_Sxword;
typedef Elf64_Word GElf_Word;
typedef Elf64_Xword GElf_Xword;

typedef Elf64_Chdr GElf_Chdr;
typedef Elf64_Dyn GElf_Dyn;
typedef Elf64_Ehdr GElf_Ehdr;
typedef Elf64_Phdr GElf_Phdr;
typedef Elf64_Shdr GElf_Shdr;
typedef Elf64_Sym GElf_Sym;
typedef Elf64_Rel GElf_Rel;
typedef Elf64_Rela GElf_Rela;

typedef Elf64_Cap GElf_Cap;
typedef Elf64_Move GElf_Move;
typedef Elf64_Syminfo GElf_Syminfo;

#define GELF_M_INFO ELF64_M_INFO
#define GELF_M_SIZE ELF64_M_SIZE
#define GELF_M_SYM ELF64_M_SYM

#define GELF_R_INFO ELF64_R_INFO
#define GELF_R_SYM ELF64_R_SYM
#define GELF_R_TYPE ELF64_R_TYPE
#define GELF_R_TYPE_DATA ELF64_R_TYPE_DATA
#define GELF_R_TYPE_ID ELF64_R_TYPE_ID
#define GELF_R_TYPE_INFO ELF64_R_TYPE_INFO

#define GELF_ST_BIND ELF64_ST_BIND
#define GELF_ST_INFO ELF64_ST_INFO
#define GELF_ST_TYPE ELF64_ST_TYPE
#define GELF_ST_VISIBILITY ELF64_ST_VISIBILITY

#if defined(__cplusplus)
extern "C" {
#endif
long gelf_checksum(Elf *_elf);
size_t gelf_fsize(Elf *_elf, Elf_Type _type, size_t _count,
unsigned int _version);
GElf_Chdr *gelf_getchdr(Elf_Scn *_scn, GElf_Chdr *_dst);
int gelf_getclass(Elf *_elf);
GElf_Dyn *gelf_getdyn(Elf_Data *_data, int _index, GElf_Dyn *_dst);
GElf_Ehdr *gelf_getehdr(Elf *_elf, GElf_Ehdr *_dst);
GElf_Phdr *gelf_getphdr(Elf *_elf, int _index, GElf_Phdr *_dst);
GElf_Rel *gelf_getrel(Elf_Data *_src, int _index, GElf_Rel *_dst);
GElf_Rela *gelf_getrela(Elf_Data *_src, int _index, GElf_Rela *_dst);
GElf_Shdr *gelf_getshdr(Elf_Scn *_scn, GElf_Shdr *_dst);
GElf_Sym *gelf_getsym(Elf_Data *_src, int _index, GElf_Sym *_dst);
GElf_Sym *gelf_getsymshndx(Elf_Data *_src, Elf_Data *_shindexsrc,
int _index, GElf_Sym *_dst, Elf32_Word *_shindexdst);
void * gelf_newehdr(Elf *_elf, int _class);
void * gelf_newphdr(Elf *_elf, size_t _phnum);
int gelf_update_dyn(Elf_Data *_dst, int _index, GElf_Dyn *_src);
int gelf_update_ehdr(Elf *_elf, GElf_Ehdr *_src);
int gelf_update_phdr(Elf *_elf, int _index, GElf_Phdr *_src);
int gelf_update_rel(Elf_Data *_dst, int _index, GElf_Rel *_src);
int gelf_update_rela(Elf_Data *_dst, int _index, GElf_Rela *_src);
int gelf_update_shdr(Elf_Scn *_dst, GElf_Shdr *_src);
int gelf_update_sym(Elf_Data *_dst, int _index, GElf_Sym *_src);
int gelf_update_symshndx(Elf_Data *_symdst, Elf_Data *_shindexdst,
int _index, GElf_Sym *_symsrc, Elf32_Word _shindexsrc);
Elf_Data *gelf_xlatetof(Elf *_elf, Elf_Data *_dst, const Elf_Data *_src, unsigned int _encode);
Elf_Data *gelf_xlatetom(Elf *_elf, Elf_Data *_dst, const Elf_Data *_src, unsigned int _encode);

GElf_Cap *gelf_getcap(Elf_Data *_data, int _index, GElf_Cap *_cap);
GElf_Move *gelf_getmove(Elf_Data *_src, int _index, GElf_Move *_dst);
GElf_Syminfo *gelf_getsyminfo(Elf_Data *_src, int _index, GElf_Syminfo *_dst);
int gelf_update_cap(Elf_Data *_dst, int _index, GElf_Cap *_src);
int gelf_update_move(Elf_Data *_dst, int _index, GElf_Move *_src);
int gelf_update_syminfo(Elf_Data *_dst, int _index, GElf_Syminfo *_src);
#if defined(__cplusplus)
}
#endif

#endif
