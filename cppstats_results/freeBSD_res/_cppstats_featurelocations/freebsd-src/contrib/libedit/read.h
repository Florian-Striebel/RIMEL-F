

































#if !defined(_h_el_read)
#define _h_el_read

libedit_private int read_init(EditLine *);
libedit_private void read_end(struct el_read_t *);
libedit_private void read_prepare(EditLine *);
libedit_private void read_finish(EditLine *);
libedit_private int el_read_setfn(struct el_read_t *, el_rfunc_t);
libedit_private el_rfunc_t el_read_getfn(struct el_read_t *);

#endif
