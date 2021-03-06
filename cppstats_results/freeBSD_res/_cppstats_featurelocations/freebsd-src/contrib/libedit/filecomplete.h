





























#if !defined(_FILECOMPLETE_H_)
#define _FILECOMPLETE_H_

int fn_complete(EditLine *,
char *(*)(const char *, int),
char **(*)(const char *, int, int),
const wchar_t *, const wchar_t *, const char *(*)(const char *), size_t,
int *, int *, int *, int *);
int fn_complete2(EditLine *,
char *(*)(const char *, int),
char **(*)(const char *, int, int),
const wchar_t *, const wchar_t *, const char *(*)(const char *), size_t,
int *, int *, int *, int *, unsigned int);
#define FN_QUOTE_MATCH 1

void fn_display_match_list(EditLine *, char **, size_t, size_t,
const char *(*)(const char *));
char *fn_tilde_expand(const char *);
char *fn_filename_completion_function(const char *, int);

#endif
