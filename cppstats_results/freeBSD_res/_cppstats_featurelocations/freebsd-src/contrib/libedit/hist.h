






































#if !defined(_h_el_hist)
#define _h_el_hist

typedef int (*hist_fun_t)(void *, HistEventW *, int, ...);

typedef struct el_history_t {
wchar_t *buf;
size_t sz;
wchar_t *last;
int eventno;
void *ref;
hist_fun_t fun;
HistEventW ev;
} el_history_t;

#define HIST_FUN_INTERNAL(el, fn, arg) ((((*(el)->el_history.fun) ((el)->el_history.ref, &(el)->el_history.ev, fn, arg)) == -1) ? NULL : (el)->el_history.ev.str)


#define HIST_FUN(el, fn, arg) (((el)->el_flags & NARROW_HISTORY) ? hist_convert(el, fn, arg) : HIST_FUN_INTERNAL(el, fn, arg))



#define HIST_NEXT(el) HIST_FUN(el, H_NEXT, NULL)
#define HIST_FIRST(el) HIST_FUN(el, H_FIRST, NULL)
#define HIST_LAST(el) HIST_FUN(el, H_LAST, NULL)
#define HIST_PREV(el) HIST_FUN(el, H_PREV, NULL)
#define HIST_SET(el, num) HIST_FUN(el, H_SET, num)
#define HIST_LOAD(el, fname) HIST_FUN(el, H_LOAD fname)
#define HIST_SAVE(el, fname) HIST_FUN(el, H_SAVE fname)
#define HIST_SAVE_FP(el, fp) HIST_FUN(el, H_SAVE_FP, fp)
#define HIST_NSAVE_FP(el, n, fp) HIST_FUN(el, H_NSAVE_FP, n, fp)

libedit_private int hist_init(EditLine *);
libedit_private void hist_end(EditLine *);
libedit_private el_action_t hist_get(EditLine *);
libedit_private int hist_set(EditLine *, hist_fun_t, void *);
libedit_private int hist_command(EditLine *, int, const wchar_t **);
libedit_private int hist_enlargebuf(EditLine *, size_t, size_t);
libedit_private wchar_t *hist_convert(EditLine *, int, void *);

#endif
