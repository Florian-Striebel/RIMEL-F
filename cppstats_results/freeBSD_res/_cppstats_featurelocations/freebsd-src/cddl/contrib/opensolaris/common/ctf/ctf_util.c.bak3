

























#pragma ident "%Z%%M% %I% %E% SMI"

#include <ctf_impl.h>








void
ctf_list_append(ctf_list_t *lp, void *new)
{
ctf_list_t *p = lp->l_prev;
ctf_list_t *q = new;

lp->l_prev = q;
q->l_prev = p;
q->l_next = NULL;

if (p != NULL)
p->l_next = q;
else
lp->l_next = q;
}





void
ctf_list_prepend(ctf_list_t *lp, void *new)
{
ctf_list_t *p = new;
ctf_list_t *q = lp->l_next;

lp->l_next = p;
p->l_prev = NULL;
p->l_next = q;

if (q != NULL)
q->l_prev = p;
else
lp->l_prev = p;
}





void
ctf_list_delete(ctf_list_t *lp, void *existing)
{
ctf_list_t *p = existing;

if (p->l_prev != NULL)
p->l_prev->l_next = p->l_next;
else
lp->l_next = p->l_next;

if (p->l_next != NULL)
p->l_next->l_prev = p->l_prev;
else
lp->l_prev = p->l_prev;
}





const char *
ctf_strraw(ctf_file_t *fp, uint_t name)
{
ctf_strs_t *ctsp = &fp->ctf_str[CTF_NAME_STID(name)];

if (ctsp->cts_strs != NULL && CTF_NAME_OFFSET(name) < ctsp->cts_len)
return (ctsp->cts_strs + CTF_NAME_OFFSET(name));


return (NULL);
}

const char *
ctf_strptr(ctf_file_t *fp, uint_t name)
{
const char *s = ctf_strraw(fp, name);
return (s != NULL ? s : "(?)");
}




char *
ctf_strdup(const char *s1)
{
char *s2 = ctf_alloc(strlen(s1) + 1);

if (s2 != NULL)
(void) strcpy(s2, s1);

return (s2);
}





ctf_file_t *
ctf_set_open_errno(int *errp, int error)
{
if (errp != NULL)
*errp = error;
return (NULL);
}





long
ctf_set_errno(ctf_file_t *fp, int err)
{
fp->ctf_errno = err;
return (CTF_ERR);
}
