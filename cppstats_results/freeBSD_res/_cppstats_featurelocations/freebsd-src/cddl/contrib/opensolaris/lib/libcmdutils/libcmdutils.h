


































#if !defined(_LIBCMDUTILS_H)
#define _LIBCMDUTILS_H

#if defined(illumos)
#if !defined(_LP64) && !((_FILE_OFFSET_BITS == 64) || defined(_LARGEFILE64_SOURCE))

#error "libcmdutils.h can only be used in a largefile compilation environment"
#endif
#endif






#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <libintl.h>
#include <string.h>
#include <dirent.h>
#if defined(illumos)
#include <attr.h>
#endif
#include <sys/avl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <libnvpair.h>

#if defined(__cplusplus)
extern "C" {
#endif


#define _NOT_SATTR 0
#define _RO_SATTR 1
#define _RW_SATTR 2

#define MAXMAPSIZE (1024*1024*8)
#define SMALLFILESIZE (32*1024)



#if defined(_LP64) || (_FILE_OFFSET_BITS == 64)
typedef struct tree_node {
dev_t node_dev;
ino_t node_ino;
avl_node_t avl_link;
} tree_node_t;
#else
typedef struct tree_node {
dev_t node_dev;
ino64_t node_ino;
avl_node_t avl_link;
} tree_node_t;
#endif




extern int sysattr_type(char *);


extern int sysattr_support(char *, int);


#if defined(_LP64) || (_FILE_OFFSET_BITS == 64)
extern int writefile(int, int, char *, char *, char *, char *,
struct stat *, struct stat *);
#else
extern int writefile(int, int, char *, char *, char *, char *,
struct stat64 *, struct stat64 *);
#endif


extern int get_attrdirs(int, int, char *, int *, int *);


extern int mv_xattrs(char *, char *, char *, int, int);


extern nvlist_t *sysattr_list(char *, int, char *);











extern int tnode_compare(const void *, const void *);







#if defined(_LP64) || (_FILE_OFFSET_BITS == 64)
extern int add_tnode(avl_tree_t **, dev_t, ino_t);
#else
extern int add_tnode(avl_tree_t **, dev_t, ino64_t);
#endif






extern void destroy_tree(avl_tree_t *);








extern int findnextuid(uid_t, uid_t, uid_t *);




extern int findnextgid(gid_t, gid_t, gid_t *);





typedef struct custr custr_t;





extern int custr_alloc(custr_t **);
extern void custr_free(custr_t *);





extern int custr_alloc_buf(custr_t **, void *, size_t);






extern int custr_appendc(custr_t *, char);
extern int custr_append(custr_t *, const char *);






extern int custr_append_printf(custr_t *, const char *, ...);
extern int custr_append_vprintf(custr_t *, const char *, va_list);





extern size_t custr_len(custr_t *);





extern void custr_reset(custr_t *);






extern const char *custr_cstr(custr_t *str);

#define NN_DIVISOR_1000 (1U << 0)


#define NN_NUMBUF_SZ (6)

void nicenum(uint64_t, char *, size_t);
void nicenum_scale(uint64_t, size_t, char *, size_t, uint32_t);

#if defined(__cplusplus)
}
#endif

#endif
