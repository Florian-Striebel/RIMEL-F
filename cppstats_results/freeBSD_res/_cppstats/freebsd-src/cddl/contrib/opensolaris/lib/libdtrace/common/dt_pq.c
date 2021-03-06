#include <dtrace.h>
#include <dt_impl.h>
#include <dt_pq.h>
#include <assert.h>
dt_pq_t *
dt_pq_init(dtrace_hdl_t *dtp, uint_t size, dt_pq_value_f value_cb, void *cb_arg)
{
dt_pq_t *p;
assert(size > 1);
if ((p = dt_zalloc(dtp, sizeof (dt_pq_t))) == NULL)
return (NULL);
p->dtpq_items = dt_zalloc(dtp, size * sizeof (p->dtpq_items[0]));
if (p->dtpq_items == NULL) {
dt_free(dtp, p);
return (NULL);
}
p->dtpq_hdl = dtp;
p->dtpq_size = size;
p->dtpq_last = 1;
p->dtpq_value = value_cb;
p->dtpq_arg = cb_arg;
return (p);
}
void
dt_pq_fini(dt_pq_t *p)
{
dtrace_hdl_t *dtp = p->dtpq_hdl;
dt_free(dtp, p->dtpq_items);
dt_free(dtp, p);
}
static uint64_t
dt_pq_getvalue(dt_pq_t *p, uint_t index)
{
void *item = p->dtpq_items[index];
return (p->dtpq_value(item, p->dtpq_arg));
}
void
dt_pq_insert(dt_pq_t *p, void *item)
{
uint_t i;
assert(p->dtpq_last < p->dtpq_size);
i = p->dtpq_last++;
p->dtpq_items[i] = item;
while (i > 1 && dt_pq_getvalue(p, i) < dt_pq_getvalue(p, i / 2)) {
void *tmp = p->dtpq_items[i];
p->dtpq_items[i] = p->dtpq_items[i / 2];
p->dtpq_items[i / 2] = tmp;
i /= 2;
}
}
void *
dt_pq_walk(dt_pq_t *p, uint_t *cookie)
{
(*cookie)++;
if (*cookie >= p->dtpq_last)
return (NULL);
return (p->dtpq_items[*cookie]);
}
void *
dt_pq_pop(dt_pq_t *p)
{
uint_t i = 1;
void *ret;
assert(p->dtpq_last > 0);
if (p->dtpq_last == 1)
return (NULL);
ret = p->dtpq_items[1];
p->dtpq_last--;
p->dtpq_items[1] = p->dtpq_items[p->dtpq_last];
p->dtpq_items[p->dtpq_last] = NULL;
for (;;) {
uint_t lc = i * 2;
uint_t rc = i * 2 + 1;
uint_t c;
uint64_t v;
void *tmp;
if (lc >= p->dtpq_last)
break;
if (rc >= p->dtpq_last) {
c = lc;
v = dt_pq_getvalue(p, lc);
} else {
uint64_t lv = dt_pq_getvalue(p, lc);
uint64_t rv = dt_pq_getvalue(p, rc);
if (lv < rv) {
c = lc;
v = lv;
} else {
c = rc;
v = rv;
}
}
if (v >= dt_pq_getvalue(p, i))
break;
tmp = p->dtpq_items[i];
p->dtpq_items[i] = p->dtpq_items[c];
p->dtpq_items[c] = tmp;
i = c;
}
return (ret);
}
