#pragma ident "%Z%%M% %I% %E% SMI"
#include <unistd.h>
#include <assert.h>
#include <dt_list.h>
void
dt_list_append(dt_list_t *dlp, void *new)
{
dt_list_t *p = dlp->dl_prev;
dt_list_t *q = new;
dlp->dl_prev = q;
q->dl_prev = p;
q->dl_next = NULL;
if (p != NULL) {
assert(p->dl_next == NULL);
p->dl_next = q;
} else {
assert(dlp->dl_next == NULL);
dlp->dl_next = q;
}
}
void
dt_list_prepend(dt_list_t *dlp, void *new)
{
dt_list_t *p = new;
dt_list_t *q = dlp->dl_next;
dlp->dl_next = p;
p->dl_prev = NULL;
p->dl_next = q;
if (q != NULL) {
assert(q->dl_prev == NULL);
q->dl_prev = p;
} else {
assert(dlp->dl_prev == NULL);
dlp->dl_prev = p;
}
}
void
dt_list_insert(dt_list_t *dlp, void *after_me, void *new)
{
dt_list_t *p = after_me;
dt_list_t *q = new;
if (p == NULL || p->dl_next == NULL) {
dt_list_append(dlp, new);
return;
}
q->dl_next = p->dl_next;
q->dl_prev = p;
p->dl_next = q;
q->dl_next->dl_prev = q;
}
void
dt_list_delete(dt_list_t *dlp, void *existing)
{
dt_list_t *p = existing;
if (p->dl_prev != NULL)
p->dl_prev->dl_next = p->dl_next;
else
dlp->dl_next = p->dl_next;
if (p->dl_next != NULL)
p->dl_next->dl_prev = p->dl_prev;
else
dlp->dl_prev = p->dl_prev;
}
