



























#if !defined(_LIBELF_H_)
#define _LIBELF_H_

#include <sys/types.h>
#include <sys/elf32.h>
#include <sys/elf64.h>


typedef struct _Elf Elf;
typedef struct _Elf_Scn Elf_Scn;


typedef enum {
ELF_K_NONE = 0,
ELF_K_AR,
ELF_K_COFF,
ELF_K_ELF,
ELF_K_NUM
} Elf_Kind;

#define ELF_K_FIRST ELF_K_NONE
#define ELF_K_LAST ELF_K_NUM


typedef enum {
ELF_T_ADDR,
ELF_T_BYTE,
ELF_T_CAP,
ELF_T_DYN,
ELF_T_EHDR,
ELF_T_HALF,
ELF_T_LWORD,
ELF_T_MOVE,
ELF_T_MOVEP,
ELF_T_NOTE,
ELF_T_OFF,
ELF_T_PHDR,
ELF_T_REL,
ELF_T_RELA,
ELF_T_SHDR,
ELF_T_SWORD,
ELF_T_SXWORD,
ELF_T_SYMINFO,
ELF_T_SYM,
ELF_T_VDEF,
ELF_T_VNEED,
ELF_T_WORD,
ELF_T_XWORD,
ELF_T_GNUHASH,
ELF_T_NUM
} Elf_Type;

#define ELF_T_FIRST ELF_T_ADDR
#define ELF_T_LAST ELF_T_GNUHASH


typedef enum {
ELF_C_NULL = 0,
ELF_C_CLR,
ELF_C_FDDONE,
ELF_C_FDREAD,
ELF_C_RDWR,
ELF_C_READ,
ELF_C_SET,
ELF_C_WRITE,
ELF_C_NUM
} Elf_Cmd;

#define ELF_C_FIRST ELF_C_NULL
#define ELF_C_LAST ELF_C_NUM





typedef struct _Elf_Data {



uint64_t d_align;
void *d_buf;
uint64_t d_off;
uint64_t d_size;
Elf_Type d_type;
unsigned int d_version;
} Elf_Data;





typedef struct {
time_t ar_date;
char *ar_name;
gid_t ar_gid;
mode_t ar_mode;
char *ar_rawname;
size_t ar_size;
uid_t ar_uid;




unsigned int ar_flags;
} Elf_Arhdr;





typedef struct {
off_t as_off;
unsigned long as_hash;
char *as_name;
} Elf_Arsym;





enum Elf_Error {
ELF_E_NONE,
ELF_E_ARCHIVE,
ELF_E_ARGUMENT,
ELF_E_CLASS,
ELF_E_DATA,
ELF_E_HEADER,
ELF_E_IO,
ELF_E_LAYOUT,
ELF_E_MODE,
ELF_E_RANGE,
ELF_E_RESOURCE,
ELF_E_SECTION,
ELF_E_SEQUENCE,
ELF_E_UNIMPL,
ELF_E_VERSION,
ELF_E_INVALID_SECTION_FLAGS,
ELF_E_INVALID_SECTION_TYPE,
ELF_E_NOT_COMPRESSED,
ELF_E_NUM
};





#define ELF_F_LAYOUT 0x001U
#define ELF_F_DIRTY 0x002U


#define ELF_F_ARCHIVE 0x100U
#define ELF_F_ARCHIVE_SYSV 0x200U

#if defined(__cplusplus)
extern "C" {
#endif
Elf *elf_begin(int _fd, Elf_Cmd _cmd, Elf *_elf);
int elf_cntl(Elf *_elf, Elf_Cmd _cmd);
int elf_end(Elf *_elf);
const char *elf_errmsg(int _error);
int elf_errno(void);
void elf_fill(int _fill);
unsigned int elf_flagarhdr(Elf_Arhdr *_arh, Elf_Cmd _cmd,
unsigned int _flags);
unsigned int elf_flagdata(Elf_Data *_data, Elf_Cmd _cmd,
unsigned int _flags);
unsigned int elf_flagehdr(Elf *_elf, Elf_Cmd _cmd, unsigned int _flags);
unsigned int elf_flagelf(Elf *_elf, Elf_Cmd _cmd, unsigned int _flags);
unsigned int elf_flagphdr(Elf *_elf, Elf_Cmd _cmd, unsigned int _flags);
unsigned int elf_flagscn(Elf_Scn *_scn, Elf_Cmd _cmd, unsigned int _flags);
unsigned int elf_flagshdr(Elf_Scn *_scn, Elf_Cmd _cmd, unsigned int _flags);
Elf_Arhdr *elf_getarhdr(Elf *_elf);
Elf_Arsym *elf_getarsym(Elf *_elf, size_t *_ptr);
off_t elf_getbase(Elf *_elf);
Elf_Data *elf_getdata(Elf_Scn *, Elf_Data *);
char *elf_getident(Elf *_elf, size_t *_ptr);
int elf_getphdrnum(Elf *_elf, size_t *_dst);
int elf_getphnum(Elf *_elf, size_t *_dst);
Elf_Scn *elf_getscn(Elf *_elf, size_t _index);
int elf_getshdrnum(Elf *_elf, size_t *_dst);
int elf_getshnum(Elf *_elf, size_t *_dst);
int elf_getshdrstrndx(Elf *_elf, size_t *_dst);
int elf_getshstrndx(Elf *_elf, size_t *_dst);
unsigned long elf_hash(const char *_name);
Elf_Kind elf_kind(Elf *_elf);
Elf *elf_memory(char *_image, size_t _size);
size_t elf_ndxscn(Elf_Scn *_scn);
Elf_Data *elf_newdata(Elf_Scn *_scn);
Elf_Scn *elf_newscn(Elf *_elf);
Elf_Scn *elf_nextscn(Elf *_elf, Elf_Scn *_scn);
Elf_Cmd elf_next(Elf *_elf);
Elf *elf_open(int _fd);
Elf *elf_openmemory(char *_image, size_t _size);
off_t elf_rand(Elf *_elf, off_t _off);
Elf_Data *elf_rawdata(Elf_Scn *_scn, Elf_Data *_data);
char *elf_rawfile(Elf *_elf, size_t *_size);
int elf_setshstrndx(Elf *_elf, size_t _shnum);
char *elf_strptr(Elf *_elf, size_t _section, size_t _offset);
off_t elf_update(Elf *_elf, Elf_Cmd _cmd);
unsigned int elf_version(unsigned int _version);

long elf32_checksum(Elf *_elf);
size_t elf32_fsize(Elf_Type _type, size_t _count,
unsigned int _version);
Elf32_Chdr *elf32_getchdr(Elf_Scn *_scn);
Elf32_Ehdr *elf32_getehdr(Elf *_elf);
Elf32_Phdr *elf32_getphdr(Elf *_elf);
Elf32_Shdr *elf32_getshdr(Elf_Scn *_scn);
Elf32_Ehdr *elf32_newehdr(Elf *_elf);
Elf32_Phdr *elf32_newphdr(Elf *_elf, size_t _count);
Elf_Data *elf32_xlatetof(Elf_Data *_dst, const Elf_Data *_src,
unsigned int _enc);
Elf_Data *elf32_xlatetom(Elf_Data *_dst, const Elf_Data *_src,
unsigned int _enc);

long elf64_checksum(Elf *_elf);
size_t elf64_fsize(Elf_Type _type, size_t _count,
unsigned int _version);
Elf64_Chdr *elf64_getchdr(Elf_Scn *_scn);
Elf64_Ehdr *elf64_getehdr(Elf *_elf);
Elf64_Phdr *elf64_getphdr(Elf *_elf);
Elf64_Shdr *elf64_getshdr(Elf_Scn *_scn);
Elf64_Ehdr *elf64_newehdr(Elf *_elf);
Elf64_Phdr *elf64_newphdr(Elf *_elf, size_t _count);
Elf_Data *elf64_xlatetof(Elf_Data *_dst, const Elf_Data *_src,
unsigned int _enc);
Elf_Data *elf64_xlatetom(Elf_Data *_dst, const Elf_Data *_src,
unsigned int _enc);
#if defined(__cplusplus)
}
#endif

#endif
