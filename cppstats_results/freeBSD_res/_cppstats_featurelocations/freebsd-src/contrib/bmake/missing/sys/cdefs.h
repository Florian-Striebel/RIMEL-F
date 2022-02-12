







































#if !defined(_SYS_CDEFS_H_)

#if defined(NEED_HOST_CDEFS_H)



#undef NEED_HOST_CDEFS_H




#if defined(__GNUC__)
#include_next <sys/cdefs.h>
#else





#include "/usr/include/sys/cdefs.h"
#endif



#undef __P
#undef _SYS_CDEFS_H_
#endif

#define _SYS_CDEFS_H_

#if defined(NetBSD)
#include <machine/cdefs.h>
#endif

#if defined(__cplusplus)
#if !defined(__BEGIN_DECLS)
#define __BEGIN_DECLS extern "C" {
#endif
#if !defined(__END_DECLS)
#define __END_DECLS };
#endif
#else
#if !defined(__BEGIN_DECLS)
#define __BEGIN_DECLS
#endif
#if !defined(__END_DECLS)
#define __END_DECLS
#endif
#endif








#if defined(__STDC__) || defined(__cplusplus)
#define __P(protos) protos
#if !defined(__CONCAT)
#define __CONCAT(x,y) x ##y
#endif
#define __STRING(x) #x

#define __const const
#define __signed signed
#define __volatile volatile
#if defined(__cplusplus)
#define __inline inline
#else
#if !defined(__GNUC__)
#define __inline
#endif
#endif

#else
#define __P(protos) ()
#define __CONCAT(x,y) xy
#define __STRING(x) "x"

#if !defined(__GNUC__)
#define __const
#define __inline
#define __signed
#define __volatile
#endif







#if !defined(NO_ANSI_KEYWORDS)
#define const __const
#define inline __inline
#define signed __signed
#define volatile __volatile
#endif
#endif









#if !defined(__GNUC__) || __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 5)

#define __attribute__(x)
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define __dead __volatile
#define __pure __const
#endif
#endif

#if defined(sun386)
#define __attribute__(x)
#endif

#if defined(__KPRINTF_ATTRIBUTE__)
#define __kprintf_attribute__(a) __attribute__(a)
#else
#define __kprintf_attribute__(a)
#endif


#if !defined(__dead)
#define __dead
#define __pure
#endif

#define __IDSTRING(name,string) static const char name[] __attribute__((__unused__)) = string


#if !defined(__RCSID)
#define __RCSID(s) __IDSTRING(rcsid,s)
#endif

#if !defined(__COPYRIGHT)
#define __COPYRIGHT(s) __IDSTRING(copyright,s)
#endif

#endif
