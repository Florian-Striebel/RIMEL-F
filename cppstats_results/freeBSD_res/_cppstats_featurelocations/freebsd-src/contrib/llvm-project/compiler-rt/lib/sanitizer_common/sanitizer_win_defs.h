










#if !defined(SANITIZER_WIN_DEFS_H)
#define SANITIZER_WIN_DEFS_H

#include "sanitizer_platform.h"
#if SANITIZER_WINDOWS

#if !defined(WINAPI)
#if defined(_M_IX86) || defined(__i386__)
#define WINAPI __stdcall
#else
#define WINAPI
#endif
#endif

#if defined(_M_IX86) || defined(__i386__)
#define WIN_SYM_PREFIX "_"
#else
#define WIN_SYM_PREFIX
#endif





#if defined(__MINGW32__)
#define WIN_EXPORT_PREFIX
#else
#define WIN_EXPORT_PREFIX WIN_SYM_PREFIX
#endif


#define STRINGIFY_(A) #A
#define STRINGIFY(A) STRINGIFY_(A)

#if !SANITIZER_GO





















#define WIN_WEAK_ALIAS(Name, Default) __pragma(comment(linker, "/alternatename:" WIN_SYM_PREFIX STRINGIFY(Name) "="WIN_SYM_PREFIX STRINGIFY(Default)))



#define WIN_FORCE_LINK(Name) __pragma(comment(linker, "/include:" WIN_SYM_PREFIX STRINGIFY(Name)))


#define WIN_EXPORT(ExportedName, Name) __pragma(comment(linker, "/export:" WIN_EXPORT_PREFIX STRINGIFY(ExportedName)"=" WIN_EXPORT_PREFIX STRINGIFY(Name)))













#define WEAK_DEFAULT_NAME(Name) Name##__def

#define WEAK_EXPORT_NAME(Name) Name##__dll




#define WIN_WEAK_EXPORT_DEF(ReturnType, Name, ...) WIN_WEAK_ALIAS(Name, WEAK_DEFAULT_NAME(Name)) WIN_EXPORT(WEAK_EXPORT_NAME(Name), Name) extern "C" ReturnType Name(__VA_ARGS__); extern "C" ReturnType WEAK_DEFAULT_NAME(Name)(__VA_ARGS__)








#define WIN_WEAK_IMPORT_DEF(Name) WIN_WEAK_ALIAS(Name, WEAK_EXPORT_NAME(Name))


























































#else



#define WIN_WEAK_EXPORT_DEF(ReturnType, Name, ...) extern "C" ReturnType Name(__VA_ARGS__)


#endif

#endif
#endif
