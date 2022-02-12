

















#if !defined(SANITIZER_ERRNO_CODES_H)
#define SANITIZER_ERRNO_CODES_H

namespace __sanitizer {

#define errno_ENOMEM 12
#define errno_EBUSY 16
#define errno_EINVAL 22
#define errno_ENAMETOOLONG 36


extern const int errno_EOWNERDEAD;

}

#endif
