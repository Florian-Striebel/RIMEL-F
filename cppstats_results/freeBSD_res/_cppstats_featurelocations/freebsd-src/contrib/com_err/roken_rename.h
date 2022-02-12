


































#if !defined(__roken_rename_h__)
#define __roken_rename_h__

#if !defined(HAVE_SNPRINTF)
#define rk_snprintf _com_err_snprintf
#endif
#if !defined(HAVE_VSNPRINTF)
#define rk_vsnprintf _com_err_vsnprintf
#endif
#if !defined(HAVE_ASPRINTF)
#define rk_asprintf _com_err_asprintf
#endif
#if !defined(HAVE_ASNPRINTF)
#define rk_asnprintf _com_err_asnprintf
#endif
#if !defined(HAVE_VASPRINTF)
#define rk_vasprintf _com_err_vasprintf
#endif
#if !defined(HAVE_VASNPRINTF)
#define rk_vasnprintf _com_err_vasnprintf
#endif
#if !defined(HAVE_STRLCPY)
#define rk_strlcpy _com_err_strlcpy
#endif


#endif
