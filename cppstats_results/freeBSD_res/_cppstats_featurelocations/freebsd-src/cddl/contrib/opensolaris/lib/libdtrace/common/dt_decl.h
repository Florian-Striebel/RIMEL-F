





























#if !defined(_DT_DECL_H)
#define _DT_DECL_H

#include <sys/types.h>
#include <libctf.h>
#include <dtrace.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct dt_node;

typedef struct dt_decl {
ushort_t dd_kind;
ushort_t dd_attr;
ctf_file_t *dd_ctfp;
ctf_id_t dd_type;
char *dd_name;
struct dt_node *dd_node;
struct dt_decl *dd_next;
} dt_decl_t;

#define DT_DA_SIGNED 0x0001
#define DT_DA_UNSIGNED 0x0002
#define DT_DA_SHORT 0x0004
#define DT_DA_LONG 0x0008
#define DT_DA_LONGLONG 0x0010
#define DT_DA_CONST 0x0020
#define DT_DA_RESTRICT 0x0040
#define DT_DA_VOLATILE 0x0080
#define DT_DA_PAREN 0x0100
#define DT_DA_USER 0x0200

typedef enum dt_dclass {
DT_DC_DEFAULT,
DT_DC_AUTO,
DT_DC_REGISTER,
DT_DC_STATIC,
DT_DC_EXTERN,
DT_DC_TYPEDEF,
DT_DC_SELF,
DT_DC_THIS
} dt_dclass_t;

typedef struct dt_scope {
dt_decl_t *ds_decl;
struct dt_scope *ds_next;
char *ds_ident;
ctf_file_t *ds_ctfp;
ctf_id_t ds_type;
dt_dclass_t ds_class;
int ds_enumval;
} dt_scope_t;

extern dt_decl_t *dt_decl_alloc(ushort_t, char *);
extern void dt_decl_free(dt_decl_t *);
extern void dt_decl_reset(void);
extern dt_decl_t *dt_decl_push(dt_decl_t *);
extern dt_decl_t *dt_decl_pop(void);
extern dt_decl_t *dt_decl_pop_param(char **);
extern dt_decl_t *dt_decl_top(void);

extern dt_decl_t *dt_decl_ident(char *);
extern void dt_decl_class(dt_dclass_t);

#define DT_DP_VARARGS 0x1
#define DT_DP_DYNAMIC 0x2
#define DT_DP_VOID 0x4
#define DT_DP_ANON 0x8

extern int dt_decl_prototype(struct dt_node *, struct dt_node *,
const char *, uint_t);

extern dt_decl_t *dt_decl_spec(ushort_t, char *);
extern dt_decl_t *dt_decl_attr(ushort_t);
extern dt_decl_t *dt_decl_array(struct dt_node *);
extern dt_decl_t *dt_decl_func(dt_decl_t *, struct dt_node *);
extern dt_decl_t *dt_decl_ptr(void);

extern dt_decl_t *dt_decl_sou(uint_t, char *);
extern void dt_decl_member(struct dt_node *);

extern dt_decl_t *dt_decl_enum(char *);
extern void dt_decl_enumerator(char *, struct dt_node *);

extern int dt_decl_type(dt_decl_t *, dtrace_typeinfo_t *);

extern void dt_scope_create(dt_scope_t *);
extern void dt_scope_destroy(dt_scope_t *);
extern void dt_scope_push(ctf_file_t *, ctf_id_t);
extern dt_decl_t *dt_scope_pop(void);

#if defined(__cplusplus)
}
#endif

#endif
