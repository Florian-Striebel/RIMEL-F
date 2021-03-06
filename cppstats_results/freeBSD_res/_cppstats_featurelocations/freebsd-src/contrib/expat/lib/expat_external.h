































#if !defined(Expat_External_INCLUDED)
#define Expat_External_INCLUDED 1

























#if !defined(XMLCALL)
#if defined(_MSC_VER)
#define XMLCALL __cdecl
#elif defined(__GNUC__) && defined(__i386) && ! defined(__INTEL_COMPILER)
#define XMLCALL __attribute__((cdecl))
#else










#define XMLCALL
#endif
#endif

#if ! defined(XML_STATIC) && ! defined(XMLIMPORT)
#if !defined(XML_BUILDING_EXPAT)


#if defined(_MSC_EXTENSIONS) && ! defined(__BEOS__) && ! defined(__CYGWIN__)
#define XMLIMPORT __declspec(dllimport)
#endif

#endif
#endif

#if !defined(XML_ENABLE_VISIBILITY)
#define XML_ENABLE_VISIBILITY 0
#endif

#if ! defined(XMLIMPORT) && XML_ENABLE_VISIBILITY
#define XMLIMPORT __attribute__((visibility("default")))
#endif


#if !defined(XMLIMPORT)
#define XMLIMPORT
#endif

#if defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96))

#define XML_ATTR_MALLOC __attribute__((__malloc__))
#else
#define XML_ATTR_MALLOC
#endif

#if defined(__GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))

#define XML_ATTR_ALLOC_SIZE(x) __attribute__((__alloc_size__(x)))
#else
#define XML_ATTR_ALLOC_SIZE(x)
#endif

#define XMLPARSEAPI(type) XMLIMPORT type XMLCALL

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(XML_UNICODE_WCHAR_T)
#if !defined(XML_UNICODE)
#define XML_UNICODE
#endif
#if defined(__SIZEOF_WCHAR_T__) && (__SIZEOF_WCHAR_T__ != 2)
#error "sizeof(wchar_t) != 2; Need -fshort-wchar for both Expat and libc"
#endif
#endif

#if defined(XML_UNICODE)
#if defined(XML_UNICODE_WCHAR_T)
typedef wchar_t XML_Char;
typedef wchar_t XML_LChar;
#else
typedef unsigned short XML_Char;
typedef char XML_LChar;
#endif
#else
typedef char XML_Char;
typedef char XML_LChar;
#endif

#if defined(XML_LARGE_SIZE)
typedef long long XML_Index;
typedef unsigned long long XML_Size;
#else
typedef long XML_Index;
typedef unsigned long XML_Size;
#endif

#if defined(__cplusplus)
}
#endif

#endif
