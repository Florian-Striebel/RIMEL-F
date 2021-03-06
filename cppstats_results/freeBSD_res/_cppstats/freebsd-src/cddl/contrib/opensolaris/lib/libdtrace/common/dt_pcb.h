#pragma ident "%Z%%M% %I% %E% SMI"
#include <dtrace.h>
#include <setjmp.h>
#include <stdio.h>
#if defined(__cplusplus)
extern "C" {
#endif
#include <dt_parser.h>
#include <dt_regset.h>
#include <dt_inttab.h>
#include <dt_strtab.h>
#include <dt_decl.h>
#include <dt_as.h>
typedef struct dt_pcb {
dtrace_hdl_t *pcb_hdl;
struct dt_pcb *pcb_prev;
FILE *pcb_fileptr;
char *pcb_filetag;
const char *pcb_string;
const char *pcb_strptr;
size_t pcb_strlen;
int pcb_sargc;
char *const *pcb_sargv;
ushort_t *pcb_sflagv;
dt_scope_t pcb_dstack;
dt_node_t *pcb_list;
dt_node_t *pcb_hold;
dt_node_t *pcb_root;
dt_idstack_t pcb_globals;
dt_idhash_t *pcb_locals;
dt_idhash_t *pcb_idents;
dt_idhash_t *pcb_pragmas;
dt_inttab_t *pcb_inttab;
dt_strtab_t *pcb_strtab;
dt_regset_t *pcb_regs;
dt_irlist_t pcb_ir;
uint_t pcb_asvidx;
ulong_t **pcb_asxrefs;
uint_t pcb_asxreflen;
const dtrace_probedesc_t *pcb_pdesc;
struct dt_probe *pcb_probe;
dtrace_probeinfo_t pcb_pinfo;
dtrace_attribute_t pcb_amin;
dt_node_t *pcb_dret;
dtrace_difo_t *pcb_difo;
dtrace_prog_t *pcb_prog;
dtrace_stmtdesc_t *pcb_stmt;
dtrace_ecbdesc_t *pcb_ecbdesc;
jmp_buf pcb_jmpbuf;
const char *pcb_region;
dtrace_probespec_t pcb_pspec;
uint_t pcb_cflags;
uint_t pcb_idepth;
yystate_t pcb_yystate;
int pcb_context;
int pcb_token;
int pcb_cstate;
int pcb_braces;
int pcb_brackets;
int pcb_parens;
} dt_pcb_t;
extern void dt_pcb_push(dtrace_hdl_t *, dt_pcb_t *);
extern void dt_pcb_pop(dtrace_hdl_t *, int);
#if defined(__cplusplus)
}
#endif
