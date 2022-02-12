






































#if !defined(_h_el_keymacro)
#define _h_el_keymacro

typedef union keymacro_value_t {
el_action_t cmd;
wchar_t *str;
} keymacro_value_t;

typedef struct keymacro_node_t keymacro_node_t;

typedef struct el_keymacro_t {
wchar_t *buf;
keymacro_node_t *map;
keymacro_value_t val;
} el_keymacro_t;

#define XK_CMD 0
#define XK_STR 1
#define XK_NOD 2

libedit_private int keymacro_init(EditLine *);
libedit_private void keymacro_end(EditLine *);
libedit_private keymacro_value_t *keymacro_map_cmd(EditLine *, int);
libedit_private keymacro_value_t *keymacro_map_str(EditLine *, wchar_t *);
libedit_private void keymacro_reset(EditLine *);
libedit_private int keymacro_get(EditLine *, wchar_t *, keymacro_value_t *);
libedit_private void keymacro_add(EditLine *, const wchar_t *,
keymacro_value_t *, int);
libedit_private void keymacro_clear(EditLine *, el_action_t *, const wchar_t *);
libedit_private int keymacro_delete(EditLine *, const wchar_t *);
libedit_private void keymacro_print(EditLine *, const wchar_t *);
libedit_private void keymacro_kprint(EditLine *, const wchar_t *,
keymacro_value_t *, int);
libedit_private size_t keymacro__decode_str(const wchar_t *, char *, size_t,
const char *);

#endif
