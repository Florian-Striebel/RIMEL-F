






































#if !defined(_h_el_refresh)
#define _h_el_refresh

typedef struct {
coord_t r_cursor;
int r_oldcv;
int r_newcv;
} el_refresh_t;

libedit_private void re_putc(EditLine *, wint_t, int);
libedit_private void re_putliteral(EditLine *, const wchar_t *,
const wchar_t *);
libedit_private void re_clear_lines(EditLine *);
libedit_private void re_clear_display(EditLine *);
libedit_private void re_refresh(EditLine *);
libedit_private void re_refresh_cursor(EditLine *);
libedit_private void re_fastaddc(EditLine *);
libedit_private void re_goto_bottom(EditLine *);

#endif
