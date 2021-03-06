































#if !defined(_ELFTC_H)
#define _ELFTC_H

#if !defined(NULL)
#define NULL ((void *) 0)
#endif

#if !defined(offsetof)
#define offsetof(T, M) ((int) &((T*) 0) -> M)
#endif




































#if !defined(LIST_FOREACH_SAFE)
#define LIST_FOREACH_SAFE(var, head, field, tvar) for ((var) = LIST_FIRST((head)); (var) && ((tvar) = LIST_NEXT((var), field), 1); (var) = (tvar))



#endif

#if !defined(SLIST_FOREACH_SAFE)
#define SLIST_FOREACH_SAFE(var, head, field, tvar) for ((var) = SLIST_FIRST((head)); (var) && ((tvar) = SLIST_NEXT((var), field), 1); (var) = (tvar))



#endif

#if !defined(STAILQ_CONCAT)
#define STAILQ_CONCAT(head1, head2) do { if (!STAILQ_EMPTY((head2))) { *(head1)->stqh_last = (head2)->stqh_first; (head1)->stqh_last = (head2)->stqh_last; STAILQ_INIT((head2)); } } while (0)






#endif

#if !defined(STAILQ_EMPTY)
#define STAILQ_EMPTY(head) ((head)->stqh_first == NULL)
#endif

#if !defined(STAILQ_ENTRY)
#define STAILQ_ENTRY(type) struct { struct type *stqe_next; }



#endif

#if !defined(STAILQ_FIRST)
#define STAILQ_FIRST(head) ((head)->stqh_first)
#endif

#if !defined(STAILQ_HEAD)
#define STAILQ_HEAD(name, type) struct name { struct type *stqh_first; struct type **stqh_last; }




#endif

#if !defined(STAILQ_HEAD_INITIALIZER)
#define STAILQ_HEAD_INITIALIZER(head) { NULL, &(head).stqh_first }

#endif

#if !defined(STAILQ_FOREACH)
#define STAILQ_FOREACH(var, head, field) for ((var) = ((head)->stqh_first); (var); (var) = ((var)->field.stqe_next))



#endif

#if !defined(STAILQ_FOREACH_SAFE)
#define STAILQ_FOREACH_SAFE(var, head, field, tvar) for ((var) = STAILQ_FIRST((head)); (var) && ((tvar) = STAILQ_NEXT((var), field), 1); (var) = (tvar))



#endif

#if !defined(STAILQ_INIT)
#define STAILQ_INIT(head) do { (head)->stqh_first = NULL; (head)->stqh_last = &(head)->stqh_first; } while (0)



#endif

#if !defined(STAILQ_INSERT_HEAD)
#define STAILQ_INSERT_HEAD(head, elm, field) do { if (((elm)->field.stqe_next = (head)->stqh_first) == NULL) (head)->stqh_last = &(elm)->field.stqe_next; (head)->stqh_first = (elm); } while (0)




#endif

#if !defined(STAILQ_INSERT_TAIL)
#define STAILQ_INSERT_TAIL(head, elm, field) do { (elm)->field.stqe_next = NULL; *(head)->stqh_last = (elm); (head)->stqh_last = &(elm)->field.stqe_next; } while (0)




#endif

#if !defined(STAILQ_INSERT_AFTER)
#define STAILQ_INSERT_AFTER(head, listelm, elm, field) do { if (((elm)->field.stqe_next = (listelm)->field.stqe_next) == NULL)(head)->stqh_last = &(elm)->field.stqe_next; (listelm)->field.stqe_next = (elm); } while (0)




#endif

#if !defined(STAILQ_LAST)
#define STAILQ_LAST(head, type, field) (STAILQ_EMPTY((head)) ? NULL : ((struct type *)(void *) ((char *)((head)->stqh_last) - offsetof(struct type, field))))



#endif

#if !defined(STAILQ_NEXT)
#define STAILQ_NEXT(elm, field) ((elm)->field.stqe_next)
#endif

#if !defined(STAILQ_REMOVE)
#define STAILQ_REMOVE(head, elm, type, field) do { if ((head)->stqh_first == (elm)) { STAILQ_REMOVE_HEAD((head), field); } else { struct type *curelm = (head)->stqh_first; while (curelm->field.stqe_next != (elm)) curelm = curelm->field.stqe_next; if ((curelm->field.stqe_next = curelm->field.stqe_next->field.stqe_next) == NULL) (head)->stqh_last = &(curelm)->field.stqe_next; } } while (0)











#endif

#if !defined(STAILQ_REMOVE_HEAD)
#define STAILQ_REMOVE_HEAD(head, field) do { if (((head)->stqh_first = (head)->stqh_first->field.stqe_next) == NULL) (head)->stqh_last = &(head)->stqh_first; } while (0)




#endif





#if !defined(STAILQ_SORT)
#define STAILQ_SORT(head, type, field, cmp) do { STAILQ_HEAD(, type) _la, _lb; struct type *_p, *_q, *_e; int _i, _sz, _nmerges, _psz, _qsz; _sz = 1; do { _nmerges = 0; STAILQ_INIT(&_lb); while (!STAILQ_EMPTY((head))) { _nmerges++; STAILQ_INIT(&_la); _psz = 0; for (_i = 0; _i < _sz && !STAILQ_EMPTY((head)); _i++) { _e = STAILQ_FIRST((head)); if (_e == NULL) break; _psz++; STAILQ_REMOVE_HEAD((head), field); STAILQ_INSERT_TAIL(&_la, _e, field); } _p = STAILQ_FIRST(&_la); _qsz = _sz; _q = STAILQ_FIRST((head)); while (_psz > 0 || (_qsz > 0 && _q != NULL)) { if (_psz == 0) { _e = _q; _q = STAILQ_NEXT(_q, field); STAILQ_REMOVE_HEAD((head), field); _qsz--; } else if (_qsz == 0 || _q == NULL) { _e = _p; _p = STAILQ_NEXT(_p, field); STAILQ_REMOVE_HEAD(&_la, field);_psz--; } else if (cmp(_p, _q) <= 0) { _e = _p; _p = STAILQ_NEXT(_p, field); STAILQ_REMOVE_HEAD(&_la, field);_psz--; } else { _e = _q; _q = STAILQ_NEXT(_q, field); STAILQ_REMOVE_HEAD((head), field); _qsz--; } STAILQ_INSERT_TAIL(&_lb, _e, field); } } (head)->stqh_first = _lb.stqh_first; (head)->stqh_last = _lb.stqh_last; _sz *= 2; } while (_nmerges > 1); } while (0)
























































#endif

#if !defined(TAILQ_FOREACH_SAFE)
#define TAILQ_FOREACH_SAFE(var, head, field, tvar) for ((var) = TAILQ_FIRST((head)); (var) && ((tvar) = TAILQ_NEXT((var), field), 1); (var) = (tvar))



#endif







#if !defined(ELFTC_VCSID)

#if defined(__DragonFly__)
#define ELFTC_VCSID(ID) __RCSID(ID)
#endif

#if defined(__FreeBSD__)
#define ELFTC_VCSID(ID) __FBSDID(ID)
#endif

#if defined(__APPLE__) || defined(__GLIBC__) || defined(__GNU__) || defined(__linux__)

#if defined(__GNUC__)
#define ELFTC_VCSID(ID) __asm__(".ident\t\"" ID "\"")
#else
#define ELFTC_VCSID(ID)
#endif
#endif

#if defined(__minix)
#if defined(__GNUC__)
#define ELFTC_VCSID(ID) __asm__(".ident\t\"" ID "\"")
#else
#define ELFTC_VCSID(ID)
#endif
#endif

#if defined(__NetBSD__)
#define ELFTC_VCSID(ID) __RCSID(ID)
#endif

#if defined(__OpenBSD__)
#if defined(__GNUC__)
#define ELFTC_VCSID(ID) __asm__(".ident\t\"" ID "\"")
#else
#define ELFTC_VCSID(ID)
#endif
#endif

#endif





#if !defined(ELFTC_GETPROGNAME)

#if defined(__APPLE__) || defined(__DragonFly__) || defined(__FreeBSD__) || defined(__minix) || defined(__NetBSD__)


#include <stdlib.h>

#define ELFTC_GETPROGNAME() getprogname()

#endif


#if defined(__GLIBC__) || defined(__linux__)
#if !defined(_GNU_SOURCE)




extern const char *program_invocation_short_name;
#endif

#define ELFTC_GETPROGNAME() program_invocation_short_name

#endif


#if defined(__OpenBSD__)

extern const char *__progname;

#define ELFTC_GETPROGNAME() __progname

#endif

#endif






#if defined(__APPLE__)

#include <libkern/OSByteOrder.h>
#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#if !defined(roundup2)
#define roundup2 roundup
#endif

#define ELFTC_BYTE_ORDER __DARWIN_BYTE_ORDER
#define ELFTC_BYTE_ORDER_LITTLE_ENDIAN __DARWIN_LITTLE_ENDIAN
#define ELFTC_BYTE_ORDER_BIG_ENDIAN __DARWIN_BIG_ENDIAN

#define ELFTC_HAVE_MMAP 1
#define ELFTC_HAVE_STRMODE 1

#define ELFTC_NEED_BYTEORDER_EXTENSIONS 1
#endif


#if defined(__DragonFly__)

#include <osreldate.h>
#include <sys/endian.h>

#define ELFTC_BYTE_ORDER _BYTE_ORDER
#define ELFTC_BYTE_ORDER_LITTLE_ENDIAN _LITTLE_ENDIAN
#define ELFTC_BYTE_ORDER_BIG_ENDIAN _BIG_ENDIAN

#define ELFTC_HAVE_MMAP 1

#endif

#if defined(__GLIBC__) || defined(__linux__)

#include <endian.h>

#define ELFTC_BYTE_ORDER __BYTE_ORDER
#define ELFTC_BYTE_ORDER_LITTLE_ENDIAN __LITTLE_ENDIAN
#define ELFTC_BYTE_ORDER_BIG_ENDIAN __BIG_ENDIAN

#define ELFTC_HAVE_MMAP 1




#define ELFTC_HAVE_STRMODE 0


#define ELFTC_NEED_BYTEORDER_EXTENSIONS 1

#if !defined(roundup2)
#define roundup2 roundup
#endif

#endif


#if defined(__FreeBSD__)

#include <osreldate.h>
#include <sys/endian.h>

#define ELFTC_BYTE_ORDER _BYTE_ORDER
#define ELFTC_BYTE_ORDER_LITTLE_ENDIAN _LITTLE_ENDIAN
#define ELFTC_BYTE_ORDER_BIG_ENDIAN _BIG_ENDIAN

#define ELFTC_HAVE_MMAP 1
#define ELFTC_HAVE_STRMODE 1
#if __FreeBSD_version <= 900000
#define ELFTC_BROKEN_YY_NO_INPUT 1
#endif
#endif


#if defined(__minix)
#define ELFTC_HAVE_MMAP 0
#endif


#if defined(__NetBSD__)

#include <sys/param.h>
#include <sys/endian.h>

#define ELFTC_BYTE_ORDER _BYTE_ORDER
#define ELFTC_BYTE_ORDER_LITTLE_ENDIAN _LITTLE_ENDIAN
#define ELFTC_BYTE_ORDER_BIG_ENDIAN _BIG_ENDIAN

#define ELFTC_HAVE_MMAP 1
#define ELFTC_HAVE_STRMODE 1
#if __NetBSD_Version__ <= 599002100


#define ELFTC_BROKEN_YY_NO_INPUT 1
#endif
#endif


#if defined(__OpenBSD__)

#include <sys/param.h>
#include <sys/endian.h>

#define ELFTC_BYTE_ORDER _BYTE_ORDER
#define ELFTC_BYTE_ORDER_LITTLE_ENDIAN _LITTLE_ENDIAN
#define ELFTC_BYTE_ORDER_BIG_ENDIAN _BIG_ENDIAN

#define ELFTC_HAVE_MMAP 1
#define ELFTC_HAVE_STRMODE 1

#define ELFTC_NEED_BYTEORDER_EXTENSIONS 1
#define roundup2 roundup

#endif

#endif
