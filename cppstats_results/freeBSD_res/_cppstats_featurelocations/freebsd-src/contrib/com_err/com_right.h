


































#if !defined(__COM_RIGHT_H__)
#define __COM_RIGHT_H__

#if !defined(KRB5_LIB_FUNCTION)
#if defined(_WIN32)
#define KRB5_LIB_FUNCTION __declspec(dllimport)
#else
#define KRB5_LIB_FUNCTION
#endif
#endif

#if !defined(KRB5_LIB_CALL)
#if defined(_WIN32)
#define KRB5_LIB_CALL __stdcall
#else
#define KRB5_LIB_CALL
#endif
#endif

#if !defined(KRB5_LIB_VARIABLE)
#if defined(_WIN32)
#define KRB5_LIB_VARIABLE __declspec(dllimport)
#else
#define KRB5_LIB_VARIABLE
#endif
#endif

#if defined(_WIN32)
#define KRB5_CALLCONV __stdcall
#else
#define KRB5_CALLCONV
#endif

#include <sys/cdefs.h>

#if defined(__STDC__)
#include <stdarg.h>
#endif

struct error_table {
char const * const * msgs;
long base;
int n_msgs;
};
struct et_list {
struct et_list *next;
struct error_table *table;
};
extern struct et_list *_et_list;

KRB5_LIB_FUNCTION const char * KRB5_LIB_CALL
com_right (struct et_list *list, long code);

KRB5_LIB_FUNCTION const char * KRB5_LIB_CALL
com_right_r (struct et_list *list, long code, char *, size_t);

KRB5_LIB_FUNCTION void KRB5_LIB_CALL
initialize_error_table_r (struct et_list **, const char **, int, long);

KRB5_LIB_FUNCTION void KRB5_LIB_CALL
free_error_table (struct et_list *);

#endif
