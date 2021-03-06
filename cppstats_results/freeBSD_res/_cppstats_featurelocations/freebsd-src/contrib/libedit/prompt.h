






































#if !defined(_h_el_prompt)
#define _h_el_prompt

typedef wchar_t *(*el_pfunc_t)(EditLine *);

typedef struct el_prompt_t {
el_pfunc_t p_func;
coord_t p_pos;
wchar_t p_ignore;
int p_wide;
} el_prompt_t;

libedit_private void prompt_print(EditLine *, int);
libedit_private int prompt_set(EditLine *, el_pfunc_t, wchar_t, int, int);
libedit_private int prompt_get(EditLine *, el_pfunc_t *, wchar_t *, int);
libedit_private int prompt_init(EditLine *);
libedit_private void prompt_end(EditLine *);

#endif
