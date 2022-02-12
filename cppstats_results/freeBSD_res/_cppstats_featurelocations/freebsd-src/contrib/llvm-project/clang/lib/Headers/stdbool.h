








#if !defined(__STDBOOL_H)
#define __STDBOOL_H


#if !defined(__cplusplus)
#define bool _Bool
#define true 1
#define false 0
#elif defined(__GNUC__) && !defined(__STRICT_ANSI__)

#define _Bool bool
#if __cplusplus < 201103L

#define bool bool
#define false false
#define true true
#endif
#endif

#define __bool_true_false_are_defined 1

#endif
