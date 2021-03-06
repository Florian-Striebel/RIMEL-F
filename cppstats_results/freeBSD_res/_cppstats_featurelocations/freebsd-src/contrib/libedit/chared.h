






































#if !defined(_h_el_chared)
#define _h_el_chared











#define VI_MOVE




typedef struct c_undo_t {
ssize_t len;
int cursor;
wchar_t *buf;
} c_undo_t;


typedef struct c_redo_t {
wchar_t *buf;
wchar_t *pos;
wchar_t *lim;
el_action_t cmd;
wchar_t ch;
int count;
int action;
} c_redo_t;




typedef struct c_vcmd_t {
int action;
wchar_t *pos;
} c_vcmd_t;




typedef struct c_kill_t {
wchar_t *buf;
wchar_t *last;
wchar_t *mark;
} c_kill_t;

typedef void (*el_zfunc_t)(EditLine *, void *);
typedef const char *(*el_afunc_t)(void *, const char *);





typedef struct el_chared_t {
c_undo_t c_undo;
c_kill_t c_kill;
c_redo_t c_redo;
c_vcmd_t c_vcmd;
el_zfunc_t c_resizefun;
el_afunc_t c_aliasfun;
void * c_resizearg;
void * c_aliasarg;
} el_chared_t;


#define STRQQ "\"\""

#define isglob(a) (strchr("*[]?", (a)) != NULL)

#define NOP 0x00
#define DELETE 0x01
#define INSERT 0x02
#define YANK 0x04

#define CHAR_FWD (+1)
#define CHAR_BACK (-1)

#define MODE_INSERT 0
#define MODE_REPLACE 1
#define MODE_REPLACE_1 2


libedit_private int cv__isword(wint_t);
libedit_private int cv__isWord(wint_t);
libedit_private void cv_delfini(EditLine *);
libedit_private wchar_t *cv__endword(wchar_t *, wchar_t *, int, int (*)(wint_t));
libedit_private int ce__isword(wint_t);
libedit_private void cv_undo(EditLine *);
libedit_private void cv_yank(EditLine *, const wchar_t *, int);
libedit_private wchar_t *cv_next_word(EditLine*, wchar_t *, wchar_t *, int,
int (*)(wint_t));
libedit_private wchar_t *cv_prev_word(wchar_t *, wchar_t *, int, int (*)(wint_t));
libedit_private wchar_t *c__next_word(wchar_t *, wchar_t *, int, int (*)(wint_t));
libedit_private wchar_t *c__prev_word(wchar_t *, wchar_t *, int, int (*)(wint_t));
libedit_private void c_insert(EditLine *, int);
libedit_private void c_delbefore(EditLine *, int);
libedit_private void c_delbefore1(EditLine *);
libedit_private void c_delafter(EditLine *, int);
libedit_private void c_delafter1(EditLine *);
libedit_private int c_gets(EditLine *, wchar_t *, const wchar_t *);
libedit_private int c_hpos(EditLine *);

libedit_private int ch_init(EditLine *);
libedit_private void ch_reset(EditLine *);
libedit_private int ch_resizefun(EditLine *, el_zfunc_t, void *);
libedit_private int ch_aliasfun(EditLine *, el_afunc_t, void *);
libedit_private int ch_enlargebufs(EditLine *, size_t);
libedit_private void ch_end(EditLine *);

#endif
