








#if !defined(_LIBCPP_ERRNO_H)
#define _LIBCPP_ERRNO_H













#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#include_next <errno.h>

#if defined(__cplusplus)

#if !defined(EOWNERDEAD) || !defined(ENOTRECOVERABLE) || !defined(EINTEGRITY)

#if defined(ELAST)

static const int __elast1 = ELAST+1;
static const int __elast2 = ELAST+2;
static const int __elast3 = ELAST+3;

#else

static const int __elast1 = 104;
static const int __elast2 = 105;
static const int __elast3 = 106;

#endif

#if !defined(EOWNERDEAD) && !defined(ENOTRECOVERABLE) && !defined(EINTEGRITY)
#define ENOTRECOVERABLE __elast1
#define EOWNERDEAD __elast2
#define EINTEGRITY __elast3
#if defined(ELAST)
#undef ELAST
#define ELAST EINTEGRITY
#endif

#elif !defined(EOWNERDEAD) && !defined(ENOTRECOVERABLE) && defined(EINTEGRITY)
#define ENOTRECOVERABLE __elast1
#define EOWNERDEAD __elast2
#if defined(ELAST)
#undef ELAST
#define ELAST EOWNERDEAD
#endif

#elif !defined(EOWNERDEAD) && defined(ENOTRECOVERABLE) && !defined(EINTEGRITY)
#define EOWNERDEAD __elast1
#define EINTEGRITY __elast2
#if defined(ELAST)
#undef ELAST
#define ELAST EINTEGRITY
#endif

#elif !defined(EOWNERDEAD) && defined(ENOTRECOVERABLE) && defined(EINTEGRITY)
#define EOWNERDEAD __elast1
#if defined(ELAST)
#undef ELAST
#define ELAST EOWNERDEAD
#endif

#elif defined(EOWNERDEAD) && !defined(ENOTRECOVERABLE) && !defined(EINTEGRITY)
#define ENOTRECOVERABLE __elast1
#define EINTEGRITY __elast2
#if defined(ELAST)
#undef ELAST
#define ELAST EINTEGRITY
#endif

#elif defined(EOWNERDEAD) && !defined(ENOTRECOVERABLE) && defined(EINTEGRITY)
#define ENOTRECOVERABLE __elast1
#if defined(ELAST)
#undef ELAST
#define ELAST ENOTRECOVERABLE
#endif

#elif defined(EOWNERDEAD) && defined(ENOTRECOVERABLE) && !defined(EINTEGRITY)
#define EINTEGRITY __elast1
#if defined(ELAST)
#undef ELAST
#define ELAST EINTEGRITY
#endif

#endif

#endif



#if !defined(EAFNOSUPPORT)
#define EAFNOSUPPORT 9901
#endif

#if !defined(EADDRINUSE)
#define EADDRINUSE 9902
#endif

#if !defined(EADDRNOTAVAIL)
#define EADDRNOTAVAIL 9903
#endif

#if !defined(EISCONN)
#define EISCONN 9904
#endif

#if !defined(EBADMSG)
#define EBADMSG 9905
#endif

#if !defined(ECONNABORTED)
#define ECONNABORTED 9906
#endif

#if !defined(EALREADY)
#define EALREADY 9907
#endif

#if !defined(ECONNREFUSED)
#define ECONNREFUSED 9908
#endif

#if !defined(ECONNRESET)
#define ECONNRESET 9909
#endif

#if !defined(EDESTADDRREQ)
#define EDESTADDRREQ 9910
#endif

#if !defined(EHOSTUNREACH)
#define EHOSTUNREACH 9911
#endif

#if !defined(EIDRM)
#define EIDRM 9912
#endif

#if !defined(EMSGSIZE)
#define EMSGSIZE 9913
#endif

#if !defined(ENETDOWN)
#define ENETDOWN 9914
#endif

#if !defined(ENETRESET)
#define ENETRESET 9915
#endif

#if !defined(ENETUNREACH)
#define ENETUNREACH 9916
#endif

#if !defined(ENOBUFS)
#define ENOBUFS 9917
#endif

#if !defined(ENOLINK)
#define ENOLINK 9918
#endif

#if !defined(ENODATA)
#define ENODATA 9919
#endif

#if !defined(ENOMSG)
#define ENOMSG 9920
#endif

#if !defined(ENOPROTOOPT)
#define ENOPROTOOPT 9921
#endif

#if !defined(ENOSR)
#define ENOSR 9922
#endif

#if !defined(ENOTSOCK)
#define ENOTSOCK 9923
#endif

#if !defined(ENOSTR)
#define ENOSTR 9924
#endif

#if !defined(ENOTCONN)
#define ENOTCONN 9925
#endif

#if !defined(ENOTSUP)
#define ENOTSUP 9926
#endif

#if !defined(ECANCELED)
#define ECANCELED 9927
#endif

#if !defined(EINPROGRESS)
#define EINPROGRESS 9928
#endif

#if !defined(EOPNOTSUPP)
#define EOPNOTSUPP 9929
#endif

#if !defined(EWOULDBLOCK)
#define EWOULDBLOCK 9930
#endif

#if !defined(EOWNERDEAD)
#define EOWNERDEAD 9931
#endif

#if !defined(EPROTO)
#define EPROTO 9932
#endif

#if !defined(EPROTONOSUPPORT)
#define EPROTONOSUPPORT 9933
#endif

#if !defined(ENOTRECOVERABLE)
#define ENOTRECOVERABLE 9934
#endif

#if !defined(ETIME)
#define ETIME 9935
#endif

#if !defined(ETXTBSY)
#define ETXTBSY 9936
#endif

#if !defined(ETIMEDOUT)
#define ETIMEDOUT 9938
#endif

#if !defined(ELOOP)
#define ELOOP 9939
#endif

#if !defined(EOVERFLOW)
#define EOVERFLOW 9940
#endif

#if !defined(EPROTOTYPE)
#define EPROTOTYPE 9941
#endif

#if !defined(ENOSYS)
#define ENOSYS 9942
#endif

#if !defined(EINVAL)
#define EINVAL 9943
#endif

#if !defined(ERANGE)
#define ERANGE 9944
#endif

#if !defined(EILSEQ)
#define EILSEQ 9945
#endif



#if !defined(E2BIG)
#define E2BIG 9946
#endif

#if !defined(EDOM)
#define EDOM 9947
#endif

#if !defined(EFAULT)
#define EFAULT 9948
#endif

#if !defined(EBADF)
#define EBADF 9949
#endif

#if !defined(EPIPE)
#define EPIPE 9950
#endif

#if !defined(EXDEV)
#define EXDEV 9951
#endif

#if !defined(EBUSY)
#define EBUSY 9952
#endif

#if !defined(ENOTEMPTY)
#define ENOTEMPTY 9953
#endif

#if !defined(ENOEXEC)
#define ENOEXEC 9954
#endif

#if !defined(EEXIST)
#define EEXIST 9955
#endif

#if !defined(EFBIG)
#define EFBIG 9956
#endif

#if !defined(ENAMETOOLONG)
#define ENAMETOOLONG 9957
#endif

#if !defined(ENOTTY)
#define ENOTTY 9958
#endif

#if !defined(EINTR)
#define EINTR 9959
#endif

#if !defined(ESPIPE)
#define ESPIPE 9960
#endif

#if !defined(EIO)
#define EIO 9961
#endif

#if !defined(EISDIR)
#define EISDIR 9962
#endif

#if !defined(ECHILD)
#define ECHILD 9963
#endif

#if !defined(ENOLCK)
#define ENOLCK 9964
#endif

#if !defined(ENOSPC)
#define ENOSPC 9965
#endif

#if !defined(ENXIO)
#define ENXIO 9966
#endif

#if !defined(ENODEV)
#define ENODEV 9967
#endif

#if !defined(ENOENT)
#define ENOENT 9968
#endif

#if !defined(ESRCH)
#define ESRCH 9969
#endif

#if !defined(ENOTDIR)
#define ENOTDIR 9970
#endif

#if !defined(ENOMEM)
#define ENOMEM 9971
#endif

#if !defined(EPERM)
#define EPERM 9972
#endif

#if !defined(EACCES)
#define EACCES 9973
#endif

#if !defined(EROFS)
#define EROFS 9974
#endif

#if !defined(EDEADLK)
#define EDEADLK 9975
#endif

#if !defined(EAGAIN)
#define EAGAIN 9976
#endif

#if !defined(ENFILE)
#define ENFILE 9977
#endif

#if !defined(EMFILE)
#define EMFILE 9978
#endif

#if !defined(EMLINK)
#define EMLINK 9979
#endif

#if !defined(EINTEGRITY)
#define EINTEGRITY 9980
#endif

#endif

#endif
