


















































#if defined(__GNUC__) && defined(__i386__) && ! defined(__MINGW32__)









#define FASTCALL __attribute__((regparm(3)))
#define PTRFASTCALL __attribute__((regparm(3)))
#endif











#if !defined(FASTCALL)
#define FASTCALL
#endif

#if !defined(PTRCALL)
#define PTRCALL
#endif

#if !defined(PTRFASTCALL)
#define PTRFASTCALL
#endif

#if !defined(XML_MIN_SIZE)
#if ! defined(__cplusplus) && ! defined(inline)
#if defined(__GNUC__)
#define inline __inline
#endif
#endif
#endif

#if defined(__cplusplus)
#define inline inline
#else
#if !defined(inline)
#define inline
#endif
#endif

#if !defined(UNUSED_P)
#define UNUSED_P(p) (void)p
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(XML_ENABLE_VISIBILITY)
#if XML_ENABLE_VISIBILITY
__attribute__((visibility("default")))
#endif
#endif
void
_INTERNAL_trim_to_complete_utf8_characters(const char *from,
const char **fromLimRef);

#if defined(__cplusplus)
}
#endif
