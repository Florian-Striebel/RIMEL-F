






























#if !defined(_DTRACE_H)
#define _DTRACE_H

#include <sys/dtrace.h>
#include <stdarg.h>
#include <stdio.h>
#include <gelf.h>
#include <libproc.h>
#if !defined(illumos)
#include <rtld_db.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif












#define DTRACE_VERSION 3

struct ps_prochandle;
struct dt_node;
typedef struct dtrace_hdl dtrace_hdl_t;
typedef struct dtrace_prog dtrace_prog_t;
typedef struct dtrace_vector dtrace_vector_t;
typedef struct dtrace_aggdata dtrace_aggdata_t;

#define DTRACE_O_NODEV 0x01
#define DTRACE_O_NOSYS 0x02
#define DTRACE_O_LP64 0x04
#define DTRACE_O_ILP32 0x08
#define DTRACE_O_MASK 0x0f

extern dtrace_hdl_t *dtrace_open(int, int, int *);
extern dtrace_hdl_t *dtrace_vopen(int, int, int *,
const dtrace_vector_t *, void *);

extern int dtrace_go(dtrace_hdl_t *);
extern int dtrace_stop(dtrace_hdl_t *);
extern void dtrace_sleep(dtrace_hdl_t *);
extern void dtrace_close(dtrace_hdl_t *);

extern int dtrace_errno(dtrace_hdl_t *);
extern const char *dtrace_errmsg(dtrace_hdl_t *, int);
extern const char *dtrace_faultstr(dtrace_hdl_t *, int);
extern const char *dtrace_subrstr(dtrace_hdl_t *, int);

extern int dtrace_setopt(dtrace_hdl_t *, const char *, const char *);
extern int dtrace_getopt(dtrace_hdl_t *, const char *, dtrace_optval_t *);

extern void dtrace_update(dtrace_hdl_t *);
extern int dtrace_ctlfd(dtrace_hdl_t *);










typedef struct dtrace_proginfo {
dtrace_attribute_t dpi_descattr;
dtrace_attribute_t dpi_stmtattr;
uint_t dpi_aggregates;
uint_t dpi_recgens;
uint_t dpi_matches;
uint_t dpi_speculations;
} dtrace_proginfo_t;

#define DTRACE_C_DIFV 0x0001
#define DTRACE_C_EMPTY 0x0002
#define DTRACE_C_ZDEFS 0x0004
#define DTRACE_C_EATTR 0x0008
#define DTRACE_C_CPP 0x0010
#define DTRACE_C_KNODEF 0x0020
#define DTRACE_C_UNODEF 0x0040
#define DTRACE_C_PSPEC 0x0080
#define DTRACE_C_ETAGS 0x0100
#define DTRACE_C_ARGREF 0x0200
#define DTRACE_C_DEFARG 0x0800
#define DTRACE_C_NOLIBS 0x1000
#define DTRACE_C_CTL 0x2000
#define DTRACE_C_MASK 0x3bff

extern dtrace_prog_t *dtrace_program_strcompile(dtrace_hdl_t *,
const char *, dtrace_probespec_t, uint_t, int, char *const []);

extern dtrace_prog_t *dtrace_program_fcompile(dtrace_hdl_t *,
FILE *, uint_t, int, char *const []);

extern int dtrace_program_exec(dtrace_hdl_t *, dtrace_prog_t *,
dtrace_proginfo_t *);
extern void dtrace_program_info(dtrace_hdl_t *, dtrace_prog_t *,
dtrace_proginfo_t *);

#define DTRACE_D_STRIP 0x01
#define DTRACE_D_PROBES 0x02
#define DTRACE_D_MASK 0x03

extern int dtrace_program_link(dtrace_hdl_t *, dtrace_prog_t *,
uint_t, const char *, int, char *const []);

extern int dtrace_program_header(dtrace_hdl_t *, FILE *, const char *);

extern void *dtrace_dof_create(dtrace_hdl_t *, dtrace_prog_t *, uint_t);
extern void dtrace_dof_destroy(dtrace_hdl_t *, void *);

extern void *dtrace_getopt_dof(dtrace_hdl_t *);
extern void *dtrace_geterr_dof(dtrace_hdl_t *);

typedef struct dtrace_stmtdesc {
dtrace_ecbdesc_t *dtsd_ecbdesc;
dtrace_actdesc_t *dtsd_action;
dtrace_actdesc_t *dtsd_action_last;
void *dtsd_aggdata;
void *dtsd_fmtdata;
void *dtsd_strdata;
void (*dtsd_callback)(void);
void *dtsd_data;
dtrace_attribute_t dtsd_descattr;
dtrace_attribute_t dtsd_stmtattr;
} dtrace_stmtdesc_t;

typedef int dtrace_stmt_f(dtrace_hdl_t *, dtrace_prog_t *,
dtrace_stmtdesc_t *, void *);

extern dtrace_stmtdesc_t *dtrace_stmt_create(dtrace_hdl_t *,
dtrace_ecbdesc_t *);
extern dtrace_actdesc_t *dtrace_stmt_action(dtrace_hdl_t *,
dtrace_stmtdesc_t *);
extern int dtrace_stmt_add(dtrace_hdl_t *, dtrace_prog_t *,
dtrace_stmtdesc_t *);
extern int dtrace_stmt_iter(dtrace_hdl_t *, dtrace_prog_t *,
dtrace_stmt_f *, void *);
extern void dtrace_stmt_destroy(dtrace_hdl_t *, dtrace_stmtdesc_t *);




typedef enum {
DTRACEFLOW_ENTRY,
DTRACEFLOW_RETURN,
DTRACEFLOW_NONE
} dtrace_flowkind_t;

#define DTRACE_CONSUME_ERROR -1
#define DTRACE_CONSUME_THIS 0
#define DTRACE_CONSUME_NEXT 1
#define DTRACE_CONSUME_ABORT 2

typedef struct dtrace_probedata {
dtrace_hdl_t *dtpda_handle;
dtrace_eprobedesc_t *dtpda_edesc;
dtrace_probedesc_t *dtpda_pdesc;
processorid_t dtpda_cpu;
caddr_t dtpda_data;
dtrace_flowkind_t dtpda_flow;
const char *dtpda_prefix;
int dtpda_indent;
} dtrace_probedata_t;

typedef int dtrace_consume_probe_f(const dtrace_probedata_t *, void *);
typedef int dtrace_consume_rec_f(const dtrace_probedata_t *,
const dtrace_recdesc_t *, void *);

extern int dtrace_consume(dtrace_hdl_t *, FILE *,
dtrace_consume_probe_f *, dtrace_consume_rec_f *, void *);

#define DTRACE_STATUS_NONE 0
#define DTRACE_STATUS_OKAY 1
#define DTRACE_STATUS_EXITED 2
#define DTRACE_STATUS_FILLED 3
#define DTRACE_STATUS_STOPPED 4

extern int dtrace_status(dtrace_hdl_t *);













extern void *dtrace_printf_create(dtrace_hdl_t *, const char *);
extern void *dtrace_printa_create(dtrace_hdl_t *, const char *);
extern size_t dtrace_printf_format(dtrace_hdl_t *, void *, char *, size_t);

extern int dtrace_fprintf(dtrace_hdl_t *, FILE *, void *,
const dtrace_probedata_t *, const dtrace_recdesc_t *, uint_t,
const void *, size_t);

extern int dtrace_fprinta(dtrace_hdl_t *, FILE *, void *,
const dtrace_probedata_t *, const dtrace_recdesc_t *, uint_t,
const void *, size_t);

extern int dtrace_system(dtrace_hdl_t *, FILE *, void *,
const dtrace_probedata_t *, const dtrace_recdesc_t *, uint_t,
const void *, size_t);

extern int dtrace_freopen(dtrace_hdl_t *, FILE *, void *,
const dtrace_probedata_t *, const dtrace_recdesc_t *, uint_t,
const void *, size_t);










extern int dtrace_print(dtrace_hdl_t *, FILE *, const char *,
caddr_t, size_t);




typedef enum {
DTRACE_WORKSTATUS_ERROR = -1,
DTRACE_WORKSTATUS_OKAY,
DTRACE_WORKSTATUS_DONE
} dtrace_workstatus_t;

extern dtrace_workstatus_t dtrace_work(dtrace_hdl_t *, FILE *,
dtrace_consume_probe_f *, dtrace_consume_rec_f *, void *);




#define DTRACE_HANDLE_ABORT -1
#define DTRACE_HANDLE_OK 0

typedef struct dtrace_errdata {
dtrace_hdl_t *dteda_handle;
dtrace_eprobedesc_t *dteda_edesc;
dtrace_probedesc_t *dteda_pdesc;
processorid_t dteda_cpu;
int dteda_action;
int dteda_offset;
int dteda_fault;
uint64_t dteda_addr;
const char *dteda_msg;
} dtrace_errdata_t;

typedef int dtrace_handle_err_f(const dtrace_errdata_t *, void *);
extern int dtrace_handle_err(dtrace_hdl_t *, dtrace_handle_err_f *, void *);

typedef enum {
DTRACEDROP_PRINCIPAL,
DTRACEDROP_AGGREGATION,
DTRACEDROP_DYNAMIC,
DTRACEDROP_DYNRINSE,
DTRACEDROP_DYNDIRTY,
DTRACEDROP_SPEC,
DTRACEDROP_SPECBUSY,
DTRACEDROP_SPECUNAVAIL,
DTRACEDROP_STKSTROVERFLOW,
DTRACEDROP_DBLERROR
} dtrace_dropkind_t;

typedef struct dtrace_dropdata {
dtrace_hdl_t *dtdda_handle;
processorid_t dtdda_cpu;
dtrace_dropkind_t dtdda_kind;
uint64_t dtdda_drops;
uint64_t dtdda_total;
const char *dtdda_msg;
} dtrace_dropdata_t;

typedef int dtrace_handle_drop_f(const dtrace_dropdata_t *, void *);
extern int dtrace_handle_drop(dtrace_hdl_t *, dtrace_handle_drop_f *, void *);

typedef void dtrace_handle_proc_f(struct ps_prochandle *, const char *, void *);
extern int dtrace_handle_proc(dtrace_hdl_t *, dtrace_handle_proc_f *, void *);

#define DTRACE_BUFDATA_AGGKEY 0x0001
#define DTRACE_BUFDATA_AGGVAL 0x0002
#define DTRACE_BUFDATA_AGGFORMAT 0x0004
#define DTRACE_BUFDATA_AGGLAST 0x0008

typedef struct dtrace_bufdata {
dtrace_hdl_t *dtbda_handle;
const char *dtbda_buffered;
dtrace_probedata_t *dtbda_probe;
const dtrace_recdesc_t *dtbda_recdesc;
const dtrace_aggdata_t *dtbda_aggdata;
uint32_t dtbda_flags;
} dtrace_bufdata_t;

typedef int dtrace_handle_buffered_f(const dtrace_bufdata_t *, void *);
extern int dtrace_handle_buffered(dtrace_hdl_t *,
dtrace_handle_buffered_f *, void *);

typedef struct dtrace_setoptdata {
dtrace_hdl_t *dtsda_handle;
const dtrace_probedata_t *dtsda_probe;
const char *dtsda_option;
dtrace_optval_t dtsda_oldval;
dtrace_optval_t dtsda_newval;
} dtrace_setoptdata_t;

typedef int dtrace_handle_setopt_f(const dtrace_setoptdata_t *, void *);
extern int dtrace_handle_setopt(dtrace_hdl_t *,
dtrace_handle_setopt_f *, void *);





#define DTRACE_A_PERCPU 0x0001
#define DTRACE_A_KEEPDELTA 0x0002
#define DTRACE_A_ANONYMOUS 0x0004
#define DTRACE_A_TOTAL 0x0008
#define DTRACE_A_MINMAXBIN 0x0010
#define DTRACE_A_HASNEGATIVES 0x0020
#define DTRACE_A_HASPOSITIVES 0x0040

#define DTRACE_AGGZOOM_MAX 0.95

#define DTRACE_AGGWALK_ERROR -1
#define DTRACE_AGGWALK_NEXT 0
#define DTRACE_AGGWALK_ABORT 1
#define DTRACE_AGGWALK_CLEAR 2
#define DTRACE_AGGWALK_NORMALIZE 3
#define DTRACE_AGGWALK_DENORMALIZE 4
#define DTRACE_AGGWALK_REMOVE 5

struct dtrace_aggdata {
dtrace_hdl_t *dtada_handle;
dtrace_aggdesc_t *dtada_desc;
dtrace_eprobedesc_t *dtada_edesc;
dtrace_probedesc_t *dtada_pdesc;
caddr_t dtada_data;
uint64_t dtada_normal;
size_t dtada_size;
caddr_t dtada_delta;
caddr_t *dtada_percpu;
caddr_t *dtada_percpu_delta;
int64_t dtada_total;
uint16_t dtada_minbin;
uint16_t dtada_maxbin;
uint32_t dtada_flags;
};

typedef int dtrace_aggregate_f(const dtrace_aggdata_t *, void *);
typedef int dtrace_aggregate_walk_f(dtrace_hdl_t *,
dtrace_aggregate_f *, void *);
typedef int dtrace_aggregate_walk_joined_f(const dtrace_aggdata_t **,
const int, void *);

extern void dtrace_aggregate_clear(dtrace_hdl_t *);
extern int dtrace_aggregate_snap(dtrace_hdl_t *);
extern int dtrace_aggregate_print(dtrace_hdl_t *, FILE *,
dtrace_aggregate_walk_f *);

extern int dtrace_aggregate_walk(dtrace_hdl_t *, dtrace_aggregate_f *, void *);

extern int dtrace_aggregate_walk_joined(dtrace_hdl_t *,
dtrace_aggvarid_t *, int, dtrace_aggregate_walk_joined_f *, void *);

extern int dtrace_aggregate_walk_sorted(dtrace_hdl_t *,
dtrace_aggregate_f *, void *);

extern int dtrace_aggregate_walk_keysorted(dtrace_hdl_t *,
dtrace_aggregate_f *, void *);

extern int dtrace_aggregate_walk_valsorted(dtrace_hdl_t *,
dtrace_aggregate_f *, void *);

extern int dtrace_aggregate_walk_keyvarsorted(dtrace_hdl_t *,
dtrace_aggregate_f *, void *);

extern int dtrace_aggregate_walk_valvarsorted(dtrace_hdl_t *,
dtrace_aggregate_f *, void *);

extern int dtrace_aggregate_walk_keyrevsorted(dtrace_hdl_t *,
dtrace_aggregate_f *, void *);

extern int dtrace_aggregate_walk_valrevsorted(dtrace_hdl_t *,
dtrace_aggregate_f *, void *);

extern int dtrace_aggregate_walk_keyvarrevsorted(dtrace_hdl_t *,
dtrace_aggregate_f *, void *);

extern int dtrace_aggregate_walk_valvarrevsorted(dtrace_hdl_t *,
dtrace_aggregate_f *, void *);

#define DTRACE_AGD_PRINTED 0x1









extern struct ps_prochandle *dtrace_proc_create(dtrace_hdl_t *,
const char *, char *const *, proc_child_func *, void *);

extern struct ps_prochandle *dtrace_proc_grab(dtrace_hdl_t *, pid_t, int);
extern void dtrace_proc_release(dtrace_hdl_t *, struct ps_prochandle *);
extern void dtrace_proc_continue(dtrace_hdl_t *, struct ps_prochandle *);













#define DTRACE_OBJ_EXEC ((const char *)0L)
#define DTRACE_OBJ_RTLD ((const char *)1L)
#define DTRACE_OBJ_CDEFS ((const char *)2L)
#define DTRACE_OBJ_DDEFS ((const char *)3L)
#define DTRACE_OBJ_EVERY ((const char *)-1L)
#define DTRACE_OBJ_KMODS ((const char *)-2L)
#define DTRACE_OBJ_UMODS ((const char *)-3L)

typedef struct dtrace_objinfo {
const char *dto_name;
const char *dto_file;
int dto_id;
uint_t dto_flags;
GElf_Addr dto_text_va;
GElf_Xword dto_text_size;
GElf_Addr dto_data_va;
GElf_Xword dto_data_size;
GElf_Addr dto_bss_va;
GElf_Xword dto_bss_size;
} dtrace_objinfo_t;

#define DTRACE_OBJ_F_KERNEL 0x1
#define DTRACE_OBJ_F_PRIMARY 0x2

typedef int dtrace_obj_f(dtrace_hdl_t *, const dtrace_objinfo_t *, void *);

extern int dtrace_object_iter(dtrace_hdl_t *, dtrace_obj_f *, void *);
extern int dtrace_object_info(dtrace_hdl_t *, const char *, dtrace_objinfo_t *);

typedef struct dtrace_syminfo {
const char *dts_object;
const char *dts_name;
ulong_t dts_id;
} dtrace_syminfo_t;

extern int dtrace_lookup_by_name(dtrace_hdl_t *, const char *, const char *,
GElf_Sym *, dtrace_syminfo_t *);

extern int dtrace_lookup_by_addr(dtrace_hdl_t *, GElf_Addr addr,
GElf_Sym *, dtrace_syminfo_t *);

typedef struct dtrace_typeinfo {
const char *dtt_object;
ctf_file_t *dtt_ctfp;
ctf_id_t dtt_type;
uint_t dtt_flags;
} dtrace_typeinfo_t;

#define DTT_FL_USER 0x1

extern int dtrace_lookup_by_type(dtrace_hdl_t *, const char *, const char *,
dtrace_typeinfo_t *);

extern int dtrace_symbol_type(dtrace_hdl_t *, const GElf_Sym *,
const dtrace_syminfo_t *, dtrace_typeinfo_t *);

extern int dtrace_type_strcompile(dtrace_hdl_t *,
const char *, dtrace_typeinfo_t *);

extern int dtrace_type_fcompile(dtrace_hdl_t *,
FILE *, dtrace_typeinfo_t *);

extern struct dt_node *dt_compile_sugar(dtrace_hdl_t *,
struct dt_node *);









typedef struct dtrace_probeinfo {
dtrace_attribute_t dtp_attr;
dtrace_attribute_t dtp_arga;
const dtrace_typeinfo_t *dtp_argv;
int dtp_argc;
} dtrace_probeinfo_t;

typedef int dtrace_probe_f(dtrace_hdl_t *, const dtrace_probedesc_t *, void *);

extern int dtrace_probe_iter(dtrace_hdl_t *,
const dtrace_probedesc_t *pdp, dtrace_probe_f *, void *);

extern int dtrace_probe_info(dtrace_hdl_t *,
const dtrace_probedesc_t *, dtrace_probeinfo_t *);









struct dtrace_vector {
#if defined(illumos)
int (*dtv_ioctl)(void *, int, void *);
#else
int (*dtv_ioctl)(void *, u_long, void *);
#endif
int (*dtv_lookup_by_addr)(void *, GElf_Addr, GElf_Sym *,
dtrace_syminfo_t *);
int (*dtv_status)(void *, processorid_t);
long (*dtv_sysconf)(void *, int);
};









extern int dtrace_addr2str(dtrace_hdl_t *, uint64_t, char *, int);
extern int dtrace_uaddr2str(dtrace_hdl_t *, pid_t, uint64_t, char *, int);

extern int dtrace_xstr2desc(dtrace_hdl_t *, dtrace_probespec_t,
const char *, int, char *const [], dtrace_probedesc_t *);

extern int dtrace_str2desc(dtrace_hdl_t *, dtrace_probespec_t,
const char *, dtrace_probedesc_t *);

extern int dtrace_id2desc(dtrace_hdl_t *, dtrace_id_t, dtrace_probedesc_t *);

#define DTRACE_DESC2STR_MAX 1024

extern char *dtrace_desc2str(const dtrace_probedesc_t *, char *, size_t);

#define DTRACE_ATTR2STR_MAX 64

extern char *dtrace_attr2str(dtrace_attribute_t, char *, size_t);
extern int dtrace_str2attr(const char *, dtrace_attribute_t *);

extern const char *dtrace_stability_name(dtrace_stability_t);
extern const char *dtrace_class_name(dtrace_class_t);

extern int dtrace_provider_modules(dtrace_hdl_t *, const char **, int);

extern const char *const _dtrace_version;
extern int _dtrace_debug;

#if defined(__cplusplus)
}
#endif

#if !defined(illumos)
#define _SC_CPUID_MAX _SC_NPROCESSORS_CONF
#define _SC_NPROCESSORS_MAX _SC_NPROCESSORS_CONF
#endif

#endif
