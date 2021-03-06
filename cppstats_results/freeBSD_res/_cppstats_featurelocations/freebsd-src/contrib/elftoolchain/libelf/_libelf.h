



























#if !defined(__LIBELF_H_)
#define __LIBELF_H_

#include <sys/queue.h>
#include <sys/tree.h>

#include "_libelf_config.h"

#include "_elftc.h"





#define LIBELF_MSG_SIZE 256

struct _libelf_globals {
int libelf_arch;
unsigned int libelf_byteorder;
int libelf_class;
int libelf_error;
int libelf_fillchar;
unsigned int libelf_version;
unsigned char libelf_msg[LIBELF_MSG_SIZE];
};

extern struct _libelf_globals _libelf;

#define LIBELF_PRIVATE(N) (_libelf.libelf_##N)

#define LIBELF_ELF_ERROR_MASK 0xFF
#define LIBELF_OS_ERROR_SHIFT 8

#define LIBELF_ERROR(E, O) (((E) & LIBELF_ELF_ERROR_MASK) | ((O) << LIBELF_OS_ERROR_SHIFT))


#define LIBELF_SET_ERROR(E, O) do { LIBELF_PRIVATE(error) = LIBELF_ERROR(ELF_E_##E, (O)); } while (0)



#define LIBELF_ADJUST_AR_SIZE(S) (((S) + 1U) & ~1U)





#define LIBELF_F_API_MASK 0x00FFFFU
#define LIBELF_F_AR_HEADER 0x010000U
#define LIBELF_F_AR_VARIANT_SVR4 0x020000U
#define LIBELF_F_DATA_MALLOCED 0x040000U
#define LIBELF_F_RAWFILE_MALLOC 0x080000U
#define LIBELF_F_RAWFILE_MMAP 0x100000U
#define LIBELF_F_SHDRS_LOADED 0x200000U
#define LIBELF_F_SPECIAL_FILE 0x400000U

RB_HEAD(scntree, _Elf_Scn);
RB_PROTOTYPE(scntree, _Elf_Scn, e_scn, elfscn_cmp);

struct _Elf {
int e_activations;
unsigned int e_byteorder;
int e_class;
Elf_Cmd e_cmd;
int e_fd;
unsigned int e_flags;
Elf_Kind e_kind;
Elf *e_parent;
unsigned char *e_rawfile;
off_t e_rawsize;
unsigned int e_version;





union {
Elf_Arhdr *e_arhdr;
unsigned char *e_rawhdr;
} e_hdr;

union {
struct {
off_t e_next;
int e_nchildren;
unsigned char *e_rawstrtab;
size_t e_rawstrtabsz;
unsigned char *e_rawsymtab;
size_t e_rawsymtabsz;
Elf_Arsym *e_symtab;
size_t e_symtabsz;
} e_ar;
struct {
union {
Elf32_Ehdr *e_ehdr32;
Elf64_Ehdr *e_ehdr64;
} e_ehdr;
union {
Elf32_Phdr *e_phdr32;
Elf64_Phdr *e_phdr64;
} e_phdr;
struct scntree e_scn;
size_t e_nphdr;
size_t e_nscn;
size_t e_strndx;
} e_elf;
} e_u;
};




struct _Libelf_Data {
Elf_Data d_data;
Elf_Scn *d_scn;
unsigned int d_flags;
STAILQ_ENTRY(_Libelf_Data) d_next;
};

struct _Elf_Scn {
union {
Elf32_Shdr s_shdr32;
Elf64_Shdr s_shdr64;
} s_shdr;
STAILQ_HEAD(, _Libelf_Data) s_data;
STAILQ_HEAD(, _Libelf_Data) s_rawdata;
RB_ENTRY(_Elf_Scn) s_tree;
struct _Elf *s_elf;
unsigned int s_flags;
size_t s_ndx;
uint64_t s_offset;
uint64_t s_rawoff;
uint64_t s_size;
};


enum {
ELF_TOFILE,
ELF_TOMEMORY
};











#define LIBELF_COPY_U32(DST, SRC, NAME) do { if ((SRC)->NAME > UINT32_MAX) { LIBELF_SET_ERROR(RANGE, 0); return (0); } (DST)->NAME = (SRC)->NAME & 0xFFFFFFFFU; } while (0)







#define LIBELF_COPY_S32(DST, SRC, NAME) do { if ((SRC)->NAME > INT32_MAX || (SRC)->NAME < INT32_MIN) { LIBELF_SET_ERROR(RANGE, 0); return (0); } (DST)->NAME = (int32_t) (SRC)->NAME; } while (0)













typedef int _libelf_translator_function(unsigned char *_dst, size_t dsz,
unsigned char *_src, size_t _cnt, int _byteswap);

#if defined(__cplusplus)
extern "C" {
#endif
struct _Libelf_Data *_libelf_allocate_data(Elf_Scn *_s);
Elf *_libelf_allocate_elf(void);
Elf_Scn *_libelf_allocate_scn(Elf *_e, size_t _ndx);
Elf_Arhdr *_libelf_ar_gethdr(Elf *_e);
Elf *_libelf_ar_open(Elf *_e, int _reporterror);
Elf *_libelf_ar_open_member(int _fd, Elf_Cmd _c, Elf *_ar);
Elf_Arsym *_libelf_ar_process_bsd_symtab(Elf *_ar, size_t *_dst);
Elf_Arsym *_libelf_ar_process_svr4_symtab(Elf *_ar, size_t *_dst);
long _libelf_checksum(Elf *_e, int _elfclass);
void *_libelf_ehdr(Elf *_e, int _elfclass, int _allocate);
int _libelf_elfmachine(Elf *_e);
unsigned int _libelf_falign(Elf_Type _t, int _elfclass);
size_t _libelf_fsize(Elf_Type _t, int _elfclass, unsigned int _version,
size_t count);
_libelf_translator_function *_libelf_get_translator(Elf_Type _t,
int _direction, int _elfclass, int _elfmachine);
void *_libelf_getchdr(Elf_Scn *_e, int _elfclass);
void *_libelf_getphdr(Elf *_e, int _elfclass);
void *_libelf_getshdr(Elf_Scn *_scn, int _elfclass);
void _libelf_init_elf(Elf *_e, Elf_Kind _kind);
int _libelf_is_mips64el(Elf *e);
int _libelf_load_section_headers(Elf *e, void *ehdr);
unsigned int _libelf_malign(Elf_Type _t, int _elfclass);
Elf *_libelf_memory(unsigned char *_image, size_t _sz, int _reporterror);
size_t _libelf_msize(Elf_Type _t, int _elfclass, unsigned int _version);
void *_libelf_newphdr(Elf *_e, int _elfclass, size_t _count);
Elf *_libelf_open_object(int _fd, Elf_Cmd _c, int _reporterror);
Elf64_Xword _libelf_mips64el_r_info_tof(Elf64_Xword r_info);
Elf64_Xword _libelf_mips64el_r_info_tom(Elf64_Xword r_info);
struct _Libelf_Data *_libelf_release_data(struct _Libelf_Data *_d);
void _libelf_release_elf(Elf *_e);
Elf_Scn *_libelf_release_scn(Elf_Scn *_s);
int _libelf_setphnum(Elf *_e, void *_eh, int _elfclass, size_t _phnum);
int _libelf_setshnum(Elf *_e, void *_eh, int _elfclass, size_t _shnum);
int _libelf_setshstrndx(Elf *_e, void *_eh, int _elfclass,
size_t _shstrndx);
Elf_Data *_libelf_xlate(Elf_Data *_d, const Elf_Data *_s,
unsigned int _encoding, int _elfclass, int _elfmachine, int _direction);
int _libelf_xlate_shtype(uint32_t _sht);
#if defined(__cplusplus)
}
#endif

#endif
