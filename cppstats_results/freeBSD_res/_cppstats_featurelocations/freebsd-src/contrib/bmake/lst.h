












































































#if !defined(MAKE_LST_H)
#define MAKE_LST_H

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif


typedef struct List List;

typedef struct ListNode ListNode;

struct ListNode {
ListNode *prev;
ListNode *next;
void *datum;
};

struct List {
ListNode *first;
ListNode *last;
};


typedef void LstFreeProc(void *);




List *Lst_New(void);

void Lst_Done(List *);

void Lst_DoneCall(List *, LstFreeProc);

void Lst_Free(List *);

#define LST_INIT { NULL, NULL }


MAKE_INLINE void
Lst_Init(List *list)
{
list->first = NULL;
list->last = NULL;
}



MAKE_INLINE bool
Lst_IsEmpty(List *list)
{
return list->first == NULL;
}


ListNode *Lst_FindDatum(List *, const void *);




void Lst_InsertBefore(List *, ListNode *, void *);

void Lst_Prepend(List *, void *);

void Lst_Append(List *, void *);

void Lst_Remove(List *, ListNode *);
void Lst_PrependAll(List *, List *);
void Lst_AppendAll(List *, List *);
void Lst_MoveAll(List *, List *);




void LstNode_Set(ListNode *, void *);

void LstNode_SetNull(ListNode *);




MAKE_INLINE void
Lst_Enqueue(List *list, void *datum) {
Lst_Append(list, datum);
}


void *Lst_Dequeue(List *);





typedef struct Vector {
void *items;
size_t itemSize;
size_t len;
size_t cap;
} Vector;

void Vector_Init(Vector *, size_t);





MAKE_INLINE void *
Vector_Get(Vector *v, size_t i)
{
unsigned char *items = v->items;
return items + i * v->itemSize;
}

void *Vector_Push(Vector *);
void *Vector_Pop(Vector *);

MAKE_INLINE void
Vector_Done(Vector *v) {
free(v->items);
}

#endif
