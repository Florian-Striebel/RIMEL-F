



























#if !defined(_LIBPE_H_)
#define _LIBPE_H_

#include <sys/types.h>

#include "pe.h"


typedef struct _PE PE;
typedef struct _PE_Scn PE_Scn;


typedef struct PE_Buffer {
unsigned int pb_align;
off_t pb_off;
size_t pb_size;
void *pb_buf;
} PE_Buffer;


typedef enum {
PE_O_UNKNOWN = 0,
PE_O_PE32,
PE_O_PE32P,
PE_O_COFF,
} PE_Object;


typedef enum {
PE_C_NULL = 0,
PE_C_CLR,
PE_C_FDDONE,
PE_C_FDREAD,
PE_C_RDWR,
PE_C_READ,
PE_C_SET,
PE_C_WRITE,
PE_C_NUM
} PE_Cmd;


#define PE_F_DIRTY 0x001U
#define PE_F_STRIP_DOS_STUB 0x002U
#define PE_F_STRIP_RICH_HEADER 0x004U
#define PE_F_STRIP_SYMTAB 0x008U
#define PE_F_STRIP_DEBUG 0x010U
#define PE_F_STRIP_SECTION 0x020U

#if defined(__cplusplus)
extern "C" {
#endif

PE_CoffHdr *pe_coff_header(PE *);
int pe_cntl(PE *, PE_Cmd);
PE_DataDir *pe_data_dir(PE *);
void pe_finish(PE *);
int pe_flag(PE *, PE_Cmd, unsigned int);
int pe_flag_buffer(PE_Buffer *, PE_Cmd, unsigned int);
int pe_flag_coff_header(PE *, PE_Cmd, unsigned int);
int pe_flag_data_dir(PE *, PE_Cmd, unsigned int);
int pe_flag_dos_header(PE *, PE_Cmd, unsigned int);
int pe_flag_opt_header(PE *, PE_Cmd, unsigned int);
int pe_flag_section_header(PE_Scn *, PE_Cmd, unsigned int);
int pe_flag_scn(PE_Scn *, PE_Cmd, unsigned int);
PE_Buffer *pe_getbuffer(PE_Scn *, PE_Buffer *);
PE_Scn *pe_getscn(PE *, size_t);
PE *pe_init(int, PE_Cmd, PE_Object);
PE_Scn *pe_insertscn(PE *, size_t);
PE_DosHdr *pe_msdos_header(PE *);
char *pe_msdos_stub(PE *, size_t *);
size_t pe_ndxscn(PE_Scn *);
PE_Buffer *pe_newbuffer(PE_Scn *);
PE_Scn *pe_newscn(PE *);
PE_Scn *pe_nextscn(PE *, PE_Scn *);
PE_Object pe_object(PE *);
PE_OptHdr *pe_opt_header(PE *);
PE_RichHdr *pe_rich_header(PE *);
int pe_rich_header_validate(PE *);
PE_SecHdr *pe_section_header(PE_Scn *);
off_t pe_update(PE *);
int pe_update_coff_header(PE *, PE_CoffHdr *);
int pe_update_opt_header(PE *, PE_OptHdr *);
int pe_update_data_dir(PE *, PE_DataDir *);
int ps_update_msdos_header(PE *, PE_DosHdr *);
int ps_update_msdos_stub(PE *, char *, size_t);
int pe_update_section_header(PE_Scn *, PE_SecHdr *);
int pe_update_symtab(PE *, char *, size_t, unsigned int);

#if defined(__cplusplus)
}
#endif

#endif
