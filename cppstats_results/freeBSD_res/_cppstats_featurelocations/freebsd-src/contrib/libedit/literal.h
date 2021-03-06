

































#if !defined(_h_el_literal)
#define _h_el_literal

#define EL_LITERAL ((wint_t)0x80000000)

typedef struct el_literal_t {
char **l_buf;
size_t l_idx;
size_t l_len;
} el_literal_t;

libedit_private void literal_init(EditLine *);
libedit_private void literal_end(EditLine *);
libedit_private void literal_clear(EditLine *);
libedit_private wint_t literal_add(EditLine *, const wchar_t *,
const wchar_t *, int *);
libedit_private const char *literal_get(EditLine *, wint_t);

#endif
