

































#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <com_right.h>

#if defined(LIBINTL)
#include <libintl.h>
#else
#define dgettext(d,s) (s)
#endif

KRB5_LIB_FUNCTION const char * KRB5_LIB_CALL
com_right(struct et_list *list, long code)
{
struct et_list *p;
for (p = list; p; p = p->next)
if (code >= p->table->base && code < p->table->base + p->table->n_msgs)
return p->table->msgs[code - p->table->base];
return NULL;
}

KRB5_LIB_FUNCTION const char * KRB5_LIB_CALL
com_right_r(struct et_list *list, long code, char *str, size_t len)
{
struct et_list *p;
for (p = list; p; p = p->next) {
if (code >= p->table->base && code < p->table->base + p->table->n_msgs) {
const char *msg = p->table->msgs[code - p->table->base];
#if defined(LIBINTL)
char domain[12 + 20];
snprintf(domain, sizeof(domain), "heim_com_err%d", p->table->base);
#endif
strlcpy(str, dgettext(domain, msg), len);
return str;
}
}
return NULL;
}

struct foobar {
struct et_list etl;
struct error_table et;
};

KRB5_LIB_FUNCTION void KRB5_LIB_CALL
initialize_error_table_r(struct et_list **list,
const char **messages,
int num_errors,
long base)
{
struct et_list *et, **end;
struct foobar *f;
for (end = list, et = *list; et; end = &et->next, et = et->next)
if (et->table->msgs == messages)
return;
f = malloc(sizeof(*f));
if (f == NULL)
return;
et = &f->etl;
et->table = &f->et;
et->table->msgs = messages;
et->table->n_msgs = num_errors;
et->table->base = base;
et->next = NULL;
*end = et;
}


KRB5_LIB_FUNCTION void KRB5_LIB_CALL
free_error_table(struct et_list *et)
{
while(et){
struct et_list *p = et;
et = et->next;
free(p);
}
}
