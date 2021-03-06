



























#if !defined(__LIBELF_AR_H_)
#define __LIBELF_AR_H_






#include <ar.h>

#define LIBELF_AR_BSD_EXTENDED_NAME_PREFIX "#1/"
#define LIBELF_AR_BSD_SYMTAB_NAME "__.SYMDEF"
#define LIBELF_AR_BSD_EXTENDED_NAME_PREFIX_SIZE (sizeof(LIBELF_AR_BSD_EXTENDED_NAME_PREFIX) - 1)


#define IS_EXTENDED_BSD_NAME(NAME) (strncmp((const char *) (NAME), LIBELF_AR_BSD_EXTENDED_NAME_PREFIX, LIBELF_AR_BSD_EXTENDED_NAME_PREFIX_SIZE) == 0)





unsigned char *_libelf_ar_get_string(const char *_buf, size_t _sz,
unsigned int _rawname, int _svr4names);
char *_libelf_ar_get_raw_name(const struct ar_hdr *_arh);
char *_libelf_ar_get_translated_name(const struct ar_hdr *_arh, Elf *_ar);
int _libelf_ar_get_number(const char *_buf, size_t _sz,
unsigned int _base, size_t *_ret);

#endif
