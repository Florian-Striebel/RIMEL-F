






































#if !defined(_h_el_terminal)
#define _h_el_terminal

typedef struct {
const wchar_t *name;
int key;
keymacro_value_t fun;
int type;
} funckey_t;

typedef struct {
const char *t_name;
coord_t t_size;
int t_flags;
#define TERM_CAN_INSERT 0x001
#define TERM_CAN_DELETE 0x002
#define TERM_CAN_CEOL 0x004
#define TERM_CAN_TAB 0x008
#define TERM_CAN_ME 0x010
#define TERM_CAN_UP 0x020
#define TERM_HAS_META 0x040
#define TERM_HAS_AUTO_MARGINS 0x080
#define TERM_HAS_MAGIC_MARGINS 0x100
char *t_buf;
size_t t_loc;
char **t_str;
int *t_val;
char *t_cap;
funckey_t *t_fkey;
} el_terminal_t;




#define A_K_DN 0
#define A_K_UP 1
#define A_K_LT 2
#define A_K_RT 3
#define A_K_HO 4
#define A_K_EN 5
#define A_K_DE 6
#define A_K_NKEYS 7

libedit_private void terminal_move_to_line(EditLine *, int);
libedit_private void terminal_move_to_char(EditLine *, int);
libedit_private void terminal_clear_EOL(EditLine *, int);
libedit_private void terminal_overwrite(EditLine *, const wchar_t *, size_t);
libedit_private void terminal_insertwrite(EditLine *, wchar_t *, int);
libedit_private void terminal_deletechars(EditLine *, int);
libedit_private void terminal_clear_screen(EditLine *);
libedit_private void terminal_beep(EditLine *);
libedit_private int terminal_change_size(EditLine *, int, int);
libedit_private int terminal_get_size(EditLine *, int *, int *);
libedit_private int terminal_init(EditLine *);
libedit_private void terminal_bind_arrow(EditLine *);
libedit_private void terminal_print_arrow(EditLine *, const wchar_t *);
libedit_private int terminal_clear_arrow(EditLine *, const wchar_t *);
libedit_private int terminal_set_arrow(EditLine *, const wchar_t *,
keymacro_value_t *, int);
libedit_private void terminal_end(EditLine *);
libedit_private void terminal_get(EditLine *, const char **);
libedit_private int terminal_set(EditLine *, const char *);
libedit_private int terminal_settc(EditLine *, int, const wchar_t **);
libedit_private int terminal_gettc(EditLine *, int, char **);
libedit_private int terminal_telltc(EditLine *, int, const wchar_t **);
libedit_private int terminal_echotc(EditLine *, int, const wchar_t **);
libedit_private void terminal_writec(EditLine *, wint_t);
libedit_private int terminal__putc(EditLine *, wint_t);
libedit_private void terminal__flush(EditLine *);




#define EL_FLAGS (el)->el_terminal.t_flags

#define EL_CAN_INSERT (EL_FLAGS & TERM_CAN_INSERT)
#define EL_CAN_DELETE (EL_FLAGS & TERM_CAN_DELETE)
#define EL_CAN_CEOL (EL_FLAGS & TERM_CAN_CEOL)
#define EL_CAN_TAB (EL_FLAGS & TERM_CAN_TAB)
#define EL_CAN_ME (EL_FLAGS & TERM_CAN_ME)
#define EL_CAN_UP (EL_FLAGS & TERM_CAN_UP)
#define EL_HAS_META (EL_FLAGS & TERM_HAS_META)
#define EL_HAS_AUTO_MARGINS (EL_FLAGS & TERM_HAS_AUTO_MARGINS)
#define EL_HAS_MAGIC_MARGINS (EL_FLAGS & TERM_HAS_MAGIC_MARGINS)

#endif
