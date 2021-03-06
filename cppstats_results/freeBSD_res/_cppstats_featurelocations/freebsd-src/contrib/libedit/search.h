






































#if !defined(_h_el_search)
#define _h_el_search

typedef struct el_search_t {
wchar_t *patbuf;
size_t patlen;
int patdir;
int chadir;
wchar_t chacha;
char chatflg;
} el_search_t;


libedit_private int el_match(const wchar_t *, const wchar_t *);
libedit_private int search_init(EditLine *);
libedit_private void search_end(EditLine *);
libedit_private int c_hmatch(EditLine *, const wchar_t *);
libedit_private void c_setpat(EditLine *);
libedit_private el_action_t ce_inc_search(EditLine *, int);
libedit_private el_action_t cv_search(EditLine *, int);
libedit_private el_action_t ce_search_line(EditLine *, int);
libedit_private el_action_t cv_repeat_srch(EditLine *, wint_t);
libedit_private el_action_t cv_csearch(EditLine *, int, wint_t, int, int);

#endif
