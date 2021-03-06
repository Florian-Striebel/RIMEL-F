

















#if !defined(_ERROR_H)
#define _ERROR_H 1

#if !defined(__attribute__)

#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 5)
#define __attribute__(Spec)
#endif


#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7)
#define __format__ format
#define __printf__ printf
#endif
#endif

#if defined(__cplusplus)
extern "C" {
#endif





extern void error (int __status, int __errnum, const char *__format, ...)
__attribute__ ((__format__ (__printf__, 3, 4)));

extern void error_at_line (int __status, int __errnum, const char *__fname,
unsigned int __lineno, const char *__format, ...)
__attribute__ ((__format__ (__printf__, 5, 6)));




extern void (*error_print_progname) (void);


extern unsigned int error_message_count;



extern int error_one_per_line;

#if defined(__cplusplus)
}
#endif

#endif
