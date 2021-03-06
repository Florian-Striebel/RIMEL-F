#pragma ident "%Z%%M% %I% %E% SMI"
#include <sys/types.h>
#include <libctf.h>
#include <dtrace.h>
#include <stdio.h>
#if defined(__cplusplus)
extern "C" {
#endif
struct dt_node;
struct dt_ident;
struct dt_pfconv;
struct dt_pfargv;
struct dt_pfargd;
typedef int dt_pfcheck_f(struct dt_pfargv *,
struct dt_pfargd *, struct dt_node *);
typedef int dt_pfprint_f(dtrace_hdl_t *, FILE *, const char *,
const struct dt_pfargd *, const void *, size_t, uint64_t);
typedef struct dt_pfconv {
const char *pfc_name;
const char *pfc_ofmt;
const char *pfc_tstr;
dt_pfcheck_f *pfc_check;
dt_pfprint_f *pfc_print;
ctf_file_t *pfc_cctfp;
ctf_id_t pfc_ctype;
ctf_file_t *pfc_dctfp;
ctf_id_t pfc_dtype;
struct dt_pfconv *pfc_next;
} dt_pfconv_t;
typedef struct dt_pfdict {
dt_pfconv_t **pdi_buckets;
uint_t pdi_nbuckets;
} dt_pfdict_t;
typedef struct dt_pfargd {
const char *pfd_prefix;
size_t pfd_preflen;
char pfd_fmt[8];
uint_t pfd_flags;
int pfd_width;
int pfd_dynwidth;
int pfd_prec;
const dt_pfconv_t *pfd_conv;
const dtrace_recdesc_t *pfd_rec;
struct dt_pfargd *pfd_next;
} dt_pfargd_t;
#define DT_PFCONV_ALT 0x0001
#define DT_PFCONV_ZPAD 0x0002
#define DT_PFCONV_LEFT 0x0004
#define DT_PFCONV_SPOS 0x0008
#define DT_PFCONV_DYNWIDTH 0x0010
#define DT_PFCONV_DYNPREC 0x0020
#define DT_PFCONV_GROUP 0x0040
#define DT_PFCONV_SPACE 0x0080
#define DT_PFCONV_AGG 0x0100
#define DT_PFCONV_SIGNED 0x0200
typedef struct dt_pfargv {
dtrace_hdl_t *pfv_dtp;
char *pfv_format;
dt_pfargd_t *pfv_argv;
uint_t pfv_argc;
uint_t pfv_flags;
} dt_pfargv_t;
typedef struct dt_pfwalk {
const dt_pfargv_t *pfw_argv;
uint_t pfw_aid;
FILE *pfw_fp;
int pfw_err;
} dt_pfwalk_t;
extern int dt_pfdict_create(dtrace_hdl_t *);
extern void dt_pfdict_destroy(dtrace_hdl_t *);
extern dt_pfargv_t *dt_printf_create(dtrace_hdl_t *, const char *);
extern void dt_printf_destroy(dt_pfargv_t *);
#define DT_PRINTF_EXACTLEN 0x1
#define DT_PRINTF_AGGREGATION 0x2
extern void dt_printf_validate(dt_pfargv_t *, uint_t,
struct dt_ident *, int, dtrace_actkind_t, struct dt_node *);
extern void dt_printa_validate(struct dt_node *, struct dt_node *);
extern int dt_print_stack(dtrace_hdl_t *, FILE *,
const char *, caddr_t, int, int);
extern int dt_print_ustack(dtrace_hdl_t *, FILE *,
const char *, caddr_t, uint64_t);
extern int dt_print_mod(dtrace_hdl_t *, FILE *, const char *, caddr_t);
extern int dt_print_umod(dtrace_hdl_t *, FILE *, const char *, caddr_t);
#if defined(__cplusplus)
}
#endif
