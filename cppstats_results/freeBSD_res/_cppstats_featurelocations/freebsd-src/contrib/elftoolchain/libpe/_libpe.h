



























#if !defined(__LIBPE_H_)
#define __LIBPE_H_

#include <sys/types.h>
#include <sys/queue.h>

#include "libpe.h"

#include "_elftc.h"

typedef struct _PE_SecBuf {
PE_Buffer sb_pb;
PE_Scn *sb_ps;
unsigned int sb_flags;
STAILQ_ENTRY(_PE_SecBuf) sb_next;
} PE_SecBuf;

struct _PE_Scn {
PE *ps_pe;
PE_SecHdr ps_sh;
unsigned int ps_ndx;
unsigned int ps_flags;
unsigned int ps_falign;
STAILQ_HEAD(, _PE_SecBuf) ps_b;
STAILQ_ENTRY(_PE_Scn) ps_next;
};

struct _PE {
int pe_fd;
PE_Cmd pe_cmd;
PE_Object pe_obj;
size_t pe_fsize;
unsigned int pe_flags;
PE_DosHdr *pe_dh;
char *pe_stub;
size_t pe_stub_ex;
char *pe_stub_app;
size_t pe_stub_app_sz;
PE_RichHdr *pe_rh;
char *pe_rh_start;
PE_CoffHdr *pe_ch;
PE_OptHdr *pe_oh;
PE_DataDir *pe_dd;
unsigned int pe_nscn;
char *pe_symtab;
size_t pe_symbtab_sz;
unsigned int pe_nsym;
unsigned int pe_rvamax;
STAILQ_HEAD(, _PE_Scn) pe_scn;
};


#define LIBPE_F_API_MASK 0x000FFFU
#define LIBPE_F_SPECIAL_FILE 0x001000U
#define LIBPE_F_BAD_DOS_HEADER 0x002000U
#define LIBPE_F_BAD_PE_HEADER 0x004000U
#define LIBPE_F_BAD_COFF_HEADER 0x008000U
#define LIBPE_F_BAD_OPT_HEADER 0x010000U
#define LIBPE_F_BAD_SEC_HEADER 0x020000U
#define LIBPE_F_LOAD_DOS_STUB 0x040000U
#define LIBPE_F_FD_DONE 0x080000U
#define LIBPE_F_DIRTY_DOS_HEADER 0x100000U
#define LIBPE_F_DIRTY_COFF_HEADER 0x200000U
#define LIBPE_F_DIRTY_OPT_HEADER 0x400000U
#define LIBPE_F_DIRTY_SEC_HEADER 0x800000U


#define LIBPE_F_LOAD_SECTION 0x1000U
#define LIBPE_F_STRIP_SECTION 0x2000U


#define LIBPE_F_BUFFER_MALLOCED 0x1000U


#define PE_DOS_MAGIC 0x5a4dU
#define PE_RICH_TEXT "Rich"
#define PE_RICH_HIDDEN 0x536e6144U
#define PE_SIGNATURE 0x4550U
#define PE_COFF_OPT_SIZE_32 224
#define PE_COFF_OPT_SIZE_32P 240
#define PE_SYM_ENTRY_SIZE 18


#if defined(ELFTC_NEED_BYTEORDER_EXTENSIONS)
static __inline uint16_t
le16dec(const void *pp)
{
unsigned char const *p = (unsigned char const *)pp;

return ((p[1] << 8) | p[0]);
}

static __inline uint32_t
le32dec(const void *pp)
{
unsigned char const *p = (unsigned char const *)pp;

return ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]);
}

static __inline uint64_t
le64dec(const void *pp)
{
unsigned char const *p = (unsigned char const *)pp;

return (((uint64_t)le32dec(p + 4) << 32) | le32dec(p));
}

static __inline void
le16enc(void *pp, uint16_t u)
{
unsigned char *p = (unsigned char *)pp;

p[0] = u & 0xff;
p[1] = (u >> 8) & 0xff;
}

static __inline void
le32enc(void *pp, uint32_t u)
{
unsigned char *p = (unsigned char *)pp;

p[0] = u & 0xff;
p[1] = (u >> 8) & 0xff;
p[2] = (u >> 16) & 0xff;
p[3] = (u >> 24) & 0xff;
}

static __inline void
le64enc(void *pp, uint64_t u)
{
unsigned char *p = (unsigned char *)pp;

le32enc(p, (uint32_t)(u & 0xffffffffU));
le32enc(p + 4, (uint32_t)(u >> 32));
}
#endif

#define PE_READ16(p,v) do { (v) = le16dec((p)); (p) += 2; } while(0)




#define PE_READ32(p,v) do { (v) = le32dec((p)); (p) += 4; } while(0)




#define PE_WRITE16(p,v) do { le16enc((p), (v)); (p) += 2; } while(0)




#define PE_WRITE32(p,v) do { le32enc((p), (v)); (p) += 4; } while(0)






off_t libpe_align(PE *, off_t, size_t);
PE_SecBuf *libpe_alloc_buffer(PE_Scn *, size_t);
PE_Scn *libpe_alloc_scn(PE *);
int libpe_load_all_sections(PE *);
int libpe_load_section(PE *, PE_Scn *);
int libpe_open_object(PE *);
int libpe_pad(PE *, size_t);
int libpe_parse_msdos_header(PE *, char *);
int libpe_parse_coff_header(PE *, char *);
int libpe_parse_rich_header(PE *);
int libpe_parse_section_headers(PE *);
int libpe_read_msdos_stub(PE *);
void libpe_release_buffer(PE_SecBuf *);
void libpe_release_object(PE *);
void libpe_release_scn(PE_Scn *);
size_t libpe_resync_buffers(PE_Scn *);
int libpe_resync_sections(PE *, off_t);
int libpe_write_buffers(PE_Scn *);
off_t libpe_write_coff_header(PE *, off_t);
off_t libpe_write_msdos_stub(PE *, off_t);
off_t libpe_write_pe_header(PE *, off_t);
off_t libpe_write_sections(PE *, off_t);
off_t libpe_write_section_headers(PE *, off_t);

#endif
