



























#if !defined(_h_chartype_f)
#define _h_chartype_f





#if !defined(__NetBSD__) && !defined(__sun) && !(defined(__APPLE__) && defined(__MACH__)) && !defined(__OpenBSD__) && !defined(__FreeBSD__) && !defined(__DragonFly__)





#if !defined(__STDC_ISO_10646__)



#error wchar_t must store ISO 10646 characters
#endif
#endif




#if WCHAR_MAX < INT32_MAX
#warning Build environment does not support non-BMP characters
#endif




typedef struct ct_buffer_t {
char *cbuff;
size_t csize;
wchar_t *wbuff;
size_t wsize;
} ct_buffer_t;


char *ct_encode_string(const wchar_t *, ct_buffer_t *);


wchar_t *ct_decode_string(const char *, ct_buffer_t *);



libedit_private wchar_t **ct_decode_argv(int, const char *[], ct_buffer_t *);




libedit_private ssize_t ct_encode_char(char *, size_t, wchar_t);
libedit_private size_t ct_enc_width(wchar_t);



#define VISUAL_WIDTH_MAX ((size_t)8)




#define MB_FILL_CHAR ((wint_t)-1)



libedit_private int ct_visual_width(wchar_t);




libedit_private ssize_t ct_visual_char(wchar_t *, size_t, wchar_t);



libedit_private const wchar_t *ct_visual_string(const wchar_t *, ct_buffer_t *);



#define CHTYPE_PRINT ( 0)

#define CHTYPE_ASCIICTL (-1)

#define CHTYPE_TAB (-2)

#define CHTYPE_NL (-3)

#define CHTYPE_NONPRINT (-4)

libedit_private int ct_chr_class(wchar_t c);

#endif
