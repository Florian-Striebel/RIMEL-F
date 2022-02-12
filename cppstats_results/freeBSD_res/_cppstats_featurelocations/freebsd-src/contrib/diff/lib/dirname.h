

















#if !defined(DIRNAME_H_)
#define DIRNAME_H_ 1

#include <stddef.h>

#if !defined(DIRECTORY_SEPARATOR)
#define DIRECTORY_SEPARATOR '/'
#endif

#if !defined(ISSLASH)
#define ISSLASH(C) ((C) == DIRECTORY_SEPARATOR)
#endif

#if !defined(FILESYSTEM_PREFIX_LEN)
#define FILESYSTEM_PREFIX_LEN(Filename) 0
#endif

char *base_name (char const *path);
char *dir_name (char const *path);
size_t base_len (char const *path);
size_t dir_len (char const *path);

int strip_trailing_slashes (char *path);

#endif
