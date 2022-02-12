


































#if !defined(__COMPILE_ET_H__)
#define __COMPILE_ET_H__

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

extern long base_id;
extern int number;
extern char *prefix;
extern char name[128];
extern char *id_str;
extern char *filename;
extern int numerror;

struct error_code {
unsigned number;
char *name;
char *string;
struct error_code *next, **tail;
};

extern struct error_code *codes;

#define APPEND(L, V) do { if((L) == NULL) { (L) = (V); (L)->tail = &(V)->next; (L)->next = NULL; }else{ *(L)->tail = (V); (L)->tail = &(V)->next; } }while(0)











#endif
