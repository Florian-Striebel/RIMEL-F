




























#if !defined(_COMMON_UTIL_CTYPE_H)
#define _COMMON_UTIL_CTYPE_H

#if defined(__cplusplus)
extern "C" {
#endif











#if defined(illumos) && (defined(_KERNEL) || defined(_BOOT))

#define isalnum(ch) (isalpha(ch) || isdigit(ch))
#define isalpha(ch) (isupper(ch) || islower(ch))
#define isdigit(ch) ((ch) >= '0' && (ch) <= '9')
#define islower(ch) ((ch) >= 'a' && (ch) <= 'z')
#define isspace(ch) (((ch) == ' ') || ((ch) == '\r') || ((ch) == '\n') || ((ch) == '\t') || ((ch) == '\f'))

#define isupper(ch) ((ch) >= 'A' && (ch) <= 'Z')
#define isxdigit(ch) (isdigit(ch) || ((ch) >= 'a' && (ch) <= 'f') || ((ch) >= 'A' && (ch) <= 'F'))


#endif

#define DIGIT(x) (isdigit(x) ? (x) - '0' : islower(x) ? (x) + 10 - 'a' : (x) + 10 - 'A')


#define MBASE ('z' - 'a' + 1 + 10)







#define lisalnum(x) (isdigit(x) || ((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z'))


#if defined(__cplusplus)
}
#endif

#endif
