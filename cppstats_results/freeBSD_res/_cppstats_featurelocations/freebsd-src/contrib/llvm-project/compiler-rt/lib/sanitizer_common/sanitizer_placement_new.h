













#if !defined(SANITIZER_PLACEMENT_NEW_H)
#define SANITIZER_PLACEMENT_NEW_H

#include "sanitizer_internal_defs.h"

inline void *operator new(__sanitizer::operator_new_size_type sz, void *p) {
return p;
}

#endif
