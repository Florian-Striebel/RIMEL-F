






































#if !defined(_h_el_map)
#define _h_el_map

typedef el_action_t (*el_func_t)(EditLine *, wint_t);

typedef struct el_bindings_t {
const wchar_t *name;
int func;
const wchar_t *description;
} el_bindings_t;

typedef struct el_map_t {
el_action_t *alt;
el_action_t *key;
el_action_t *current;
const el_action_t *emacs;
const el_action_t *vic;
const el_action_t *vii;
int type;
el_bindings_t *help;
el_func_t *func;
size_t nfunc;
} el_map_t;

#define MAP_EMACS 0
#define MAP_VI 1

#define N_KEYS 256

libedit_private int map_bind(EditLine *, int, const wchar_t **);
libedit_private int map_init(EditLine *);
libedit_private void map_end(EditLine *);
libedit_private void map_init_vi(EditLine *);
libedit_private void map_init_emacs(EditLine *);
libedit_private int map_set_editor(EditLine *, wchar_t *);
libedit_private int map_get_editor(EditLine *, const wchar_t **);
libedit_private int map_addfunc(EditLine *, const wchar_t *, const wchar_t *,
el_func_t);

#endif
