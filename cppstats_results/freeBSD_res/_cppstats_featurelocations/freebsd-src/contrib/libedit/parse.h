






































#if !defined(_h_el_parse)
#define _h_el_parse

libedit_private int parse_line(EditLine *, const wchar_t *);
libedit_private int parse__escape(const wchar_t **);
libedit_private wchar_t *parse__string(wchar_t *, const wchar_t *);
libedit_private int parse_cmd(EditLine *, const wchar_t *);

#endif
