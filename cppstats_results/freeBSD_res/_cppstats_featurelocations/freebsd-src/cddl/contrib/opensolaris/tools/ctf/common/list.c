

























#pragma ident "%Z%%M% %I% %E% SMI"





#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "list.h"
#include "memory.h"

struct list {
void *l_data;
struct list *l_next;
};


void
list_add(list_t **list, void *data)
{
list_t *le;

le = xmalloc(sizeof (list_t));
le->l_data = data;
le->l_next = *list;
*list = le;
}


void
slist_add(list_t **list, void *data, int (*cmp)(void *, void *))
{
list_t **nextp;

for (nextp = list; *nextp; nextp = &((*nextp)->l_next)) {
if (cmp((*nextp)->l_data, data) > 0)
break;
}

list_add(nextp, data);
}


static int
list_defcmp(void *d1, void *d2, void *private __unused)
{
return (d1 != d2);
}

void *
list_remove(list_t **list, void *data, int (*cmp)(void *, void *, void *),
void *private)
{
list_t *le, **le2;
void *led;

if (!cmp)
cmp = list_defcmp;

for (le = *list, le2 = list; le; le2 = &le->l_next, le = le->l_next) {
if (cmp(le->l_data, data, private) == 0) {
*le2 = le->l_next;
led = le->l_data;
free(le);
return (led);
}
}

return (NULL);
}

void
list_free(list_t *list, void (*datafree)(void *, void *), void *private)
{
list_t *le;

while (list) {
le = list;
list = list->l_next;
if (le->l_data && datafree)
datafree(le->l_data, private);
free(le);
}
}





int
list_iter(list_t *list, int (*func)(void *, void *), void *private)
{
list_t *lnext;
int cumrc = 0;
int cbrc;

while (list) {
lnext = list->l_next;
if ((cbrc = func(list->l_data, private)) < 0)
return (cbrc);
cumrc += cbrc;
list = lnext;
}

return (cumrc);
}


static int
list_count_cb(void *data __unused, void *private __unused)
{
return (1);
}

int
list_count(list_t *list)
{
return (list_iter(list, list_count_cb, NULL));
}

int
list_empty(list_t *list)
{
return (list == NULL);
}

void *
list_find(list_t *list, void *tmpl, int (*cmp)(void *, void *))
{
for (; list; list = list->l_next) {
if (cmp(list->l_data, tmpl) == 0)
return (list->l_data);
}

return (NULL);
}

void *
list_first(list_t *list)
{
return (list ? list->l_data : NULL);
}

void
list_concat(list_t **list1, list_t *list2)
{
list_t *l, *last;

for (l = *list1, last = NULL; l; last = l, l = l->l_next)
continue;

if (last == NULL)
*list1 = list2;
else
last->l_next = list2;
}




void
slist_merge(list_t **list1p, list_t *list2, int (*cmp)(void *, void *))
{
list_t *list1, *next2;
list_t *last1 = NULL;

if (*list1p == NULL) {
*list1p = list2;
return;
}

list1 = *list1p;
while (list2 != NULL) {
if (cmp(list1->l_data, list2->l_data) > 0) {
next2 = list2->l_next;

if (last1 == NULL) {

*list1p = last1 = list2;
list2->l_next = list1;
} else {
list2->l_next = list1;
last1->l_next = list2;
last1 = list2;
}

list2 = next2;
} else {

last1 = list1;
list1 = list1->l_next;

if (list1 == NULL) {

last1->l_next = list2;
list2 = NULL;
}
}
}
}
