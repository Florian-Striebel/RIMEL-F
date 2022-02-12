






























#if !defined(MAKE_FILEMON_H)
#define MAKE_FILEMON_H

#include <sys/types.h>

struct filemon;

const char *
filemon_path(void);

struct filemon *
filemon_open(void);
int filemon_close(struct filemon *);

int filemon_setfd(struct filemon *, int);
void filemon_setpid_parent(struct filemon *, pid_t);
int filemon_setpid_child(const struct filemon *, pid_t);

int filemon_readfd(const struct filemon *);
int filemon_process(struct filemon *);

#endif
