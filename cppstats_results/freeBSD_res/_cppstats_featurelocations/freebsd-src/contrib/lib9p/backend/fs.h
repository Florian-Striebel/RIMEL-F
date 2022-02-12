



























#if !defined(LIB9P_BACKEND_FS_H)
#define LIB9P_BACKEND_FS_H

#include <stdbool.h>
#include "backend.h"

int l9p_backend_fs_init(struct l9p_backend **backendp, int rootfd, bool ro);

#endif
