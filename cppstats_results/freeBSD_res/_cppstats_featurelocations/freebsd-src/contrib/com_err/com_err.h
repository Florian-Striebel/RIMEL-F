




































#if !defined(__COM_ERR_H__)
#define __COM_ERR_H__

#include <com_right.h>
#include <stdarg.h>

#if !defined(__GNUC__) && !defined(__attribute__)
#define __attribute__(X)
#endif

typedef void (KRB5_CALLCONV *errf) (const char *, long, const char *, va_list);

KRB5_LIB_FUNCTION const char * KRB5_LIB_CALL
error_message (long);

KRB5_LIB_FUNCTION int KRB5_LIB_CALL
init_error_table (const char**, long, int);

KRB5_LIB_FUNCTION void KRB5_LIB_CALL
com_err_va (const char *, long, const char *, va_list)
__attribute__((format(printf, 3, 0)));

KRB5_LIB_FUNCTION void KRB5_LIB_CALL
com_err (const char *, long, const char *, ...)
__attribute__((format(printf, 3, 4)));

KRB5_LIB_FUNCTION errf KRB5_LIB_CALL
set_com_err_hook (errf);

KRB5_LIB_FUNCTION errf KRB5_LIB_CALL
reset_com_err_hook (void);

KRB5_LIB_FUNCTION const char * KRB5_LIB_CALL
error_table_name (int num);

KRB5_LIB_FUNCTION void KRB5_LIB_CALL
add_to_error_table (struct et_list *new_table);

#endif
