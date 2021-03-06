#pragma ident "%Z%%M% %I% %E% SMI"
#include <stdlib.h>
#include "fifo.h"
#include "memory.h"
typedef struct fifonode {
void *fn_data;
struct fifonode *fn_next;
} fifonode_t;
struct fifo {
fifonode_t *f_head;
fifonode_t *f_tail;
};
fifo_t *
fifo_new(void)
{
fifo_t *f;
f = xcalloc(sizeof (fifo_t));
return (f);
}
void
fifo_add(fifo_t *f, void *data)
{
fifonode_t *fn = xmalloc(sizeof (fifonode_t));
fn->fn_data = data;
fn->fn_next = NULL;
if (f->f_tail == NULL)
f->f_head = f->f_tail = fn;
else {
f->f_tail->fn_next = fn;
f->f_tail = fn;
}
}
void *
fifo_remove(fifo_t *f)
{
fifonode_t *fn;
void *data;
if ((fn = f->f_head) == NULL)
return (NULL);
data = fn->fn_data;
if ((f->f_head = fn->fn_next) == NULL)
f->f_tail = NULL;
free(fn);
return (data);
}
static void
fifo_nullfree(void *arg)
{
}
void
fifo_free(fifo_t *f, void (*freefn)(void *))
{
fifonode_t *fn = f->f_head;
fifonode_t *tmp;
if (freefn == NULL)
freefn = fifo_nullfree;
while (fn) {
(*freefn)(fn->fn_data);
tmp = fn;
fn = fn->fn_next;
free(tmp);
}
free(f);
}
int
fifo_len(fifo_t *f)
{
fifonode_t *fn;
int i;
for (i = 0, fn = f->f_head; fn; fn = fn->fn_next, i++);
return (i);
}
int
fifo_empty(fifo_t *f)
{
return (f->f_head == NULL);
}
int
fifo_iter(fifo_t *f, int (*iter)(void *data, void *arg), void *arg)
{
fifonode_t *fn;
int rc;
int ret = 0;
for (fn = f->f_head; fn; fn = fn->fn_next) {
if ((rc = iter(fn->fn_data, arg)) < 0)
return (-1);
ret += rc;
}
return (ret);
}
