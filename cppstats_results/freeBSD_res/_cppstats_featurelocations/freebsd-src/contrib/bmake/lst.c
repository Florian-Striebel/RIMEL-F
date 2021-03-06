

































#include "make.h"

MAKE_RCSID("$NetBSD: lst.c,v 1.105 2021/03/15 16:45:30 rillig Exp $");

static ListNode *
LstNodeNew(ListNode *prev, ListNode *next, void *datum)
{
ListNode *ln = bmake_malloc(sizeof *ln);

ln->prev = prev;
ln->next = next;
ln->datum = datum;

return ln;
}


List *
Lst_New(void)
{
List *list = bmake_malloc(sizeof *list);
Lst_Init(list);
return list;
}

void
Lst_Done(List *list)
{
ListNode *ln, *next;

for (ln = list->first; ln != NULL; ln = next) {
next = ln->next;
free(ln);
}
}

void
Lst_DoneCall(List *list, LstFreeProc freeProc)
{
ListNode *ln, *next;

for (ln = list->first; ln != NULL; ln = next) {
next = ln->next;
freeProc(ln->datum);
free(ln);
}
}


void
Lst_Free(List *list)
{

Lst_Done(list);
free(list);
}


void
Lst_InsertBefore(List *list, ListNode *ln, void *datum)
{
ListNode *newNode;

assert(datum != NULL);

newNode = LstNodeNew(ln->prev, ln, datum);

if (ln->prev != NULL)
ln->prev->next = newNode;
ln->prev = newNode;

if (ln == list->first)
list->first = newNode;
}


void
Lst_Prepend(List *list, void *datum)
{
ListNode *ln;

assert(datum != NULL);

ln = LstNodeNew(NULL, list->first, datum);

if (list->first == NULL) {
list->first = ln;
list->last = ln;
} else {
list->first->prev = ln;
list->first = ln;
}
}


void
Lst_Append(List *list, void *datum)
{
ListNode *ln;

assert(datum != NULL);

ln = LstNodeNew(list->last, NULL, datum);

if (list->last == NULL) {
list->first = ln;
list->last = ln;
} else {
list->last->next = ln;
list->last = ln;
}
}





void
Lst_Remove(List *list, ListNode *ln)
{

if (ln->next != NULL)
ln->next->prev = ln->prev;
if (ln->prev != NULL)
ln->prev->next = ln->next;


if (list->first == ln)
list->first = ln->next;
if (list->last == ln)
list->last = ln->prev;
}


void
LstNode_Set(ListNode *ln, void *datum)
{
assert(datum != NULL);

ln->datum = datum;
}





void
LstNode_SetNull(ListNode *ln)
{
ln->datum = NULL;
}






ListNode *
Lst_FindDatum(List *list, const void *datum)
{
ListNode *ln;

assert(datum != NULL);

for (ln = list->first; ln != NULL; ln = ln->next)
if (ln->datum == datum)
return ln;

return NULL;
}





void
Lst_MoveAll(List *dst, List *src)
{
if (src->first != NULL) {
src->first->prev = dst->last;
if (dst->last != NULL)
dst->last->next = src->first;
else
dst->first = src->first;

dst->last = src->last;
}
#if defined(CLEANUP)
src->first = NULL;
src->last = NULL;
#endif
}


void
Lst_PrependAll(List *dst, List *src)
{
ListNode *ln;

for (ln = src->last; ln != NULL; ln = ln->prev)
Lst_Prepend(dst, ln->datum);
}


void
Lst_AppendAll(List *dst, List *src)
{
ListNode *ln;

for (ln = src->first; ln != NULL; ln = ln->next)
Lst_Append(dst, ln->datum);
}


void *
Lst_Dequeue(List *list)
{
void *datum = list->first->datum;
Lst_Remove(list, list->first);
assert(datum != NULL);
return datum;
}

void
Vector_Init(Vector *v, size_t itemSize)
{
v->len = 0;
v->cap = 10;
v->itemSize = itemSize;
v->items = bmake_malloc(v->cap * v->itemSize);
}





void *
Vector_Push(Vector *v)
{
if (v->len >= v->cap) {
v->cap *= 2;
v->items = bmake_realloc(v->items, v->cap * v->itemSize);
}
v->len++;
return Vector_Get(v, v->len - 1);
}





void *
Vector_Pop(Vector *v)
{
assert(v->len > 0);
v->len--;
return Vector_Get(v, v->len);
}
