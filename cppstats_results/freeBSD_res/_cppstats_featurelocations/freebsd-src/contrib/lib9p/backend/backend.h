



























#if !defined(LIB9P_BACKEND_H)
#define LIB9P_BACKEND_H

struct l9p_backend {
void *softc;
void (*freefid)(void *, struct l9p_fid *);
int (*attach)(void *, struct l9p_request *);
int (*clunk)(void *, struct l9p_fid *);
int (*create)(void *, struct l9p_request *);
int (*open)(void *, struct l9p_request *);
int (*read)(void *, struct l9p_request *);
int (*remove)(void *, struct l9p_fid *);
int (*stat)(void *, struct l9p_request *);
int (*walk)(void *, struct l9p_request *);
int (*write)(void *, struct l9p_request *);
int (*wstat)(void *, struct l9p_request *);
int (*statfs)(void *, struct l9p_request *);
int (*lopen)(void *, struct l9p_request *);
int (*lcreate)(void *, struct l9p_request *);
int (*symlink)(void *, struct l9p_request *);
int (*mknod)(void *, struct l9p_request *);
int (*rename)(void *, struct l9p_request *);
int (*readlink)(void *, struct l9p_request *);
int (*getattr)(void *, struct l9p_request *);
int (*setattr)(void *, struct l9p_request *);
int (*xattrwalk)(void *, struct l9p_request *);
int (*xattrcreate)(void *, struct l9p_request *);
int (*xattrread)(void *, struct l9p_request *);
int (*xattrwrite)(void *, struct l9p_request *);
int (*xattrclunk)(void *, struct l9p_fid *);
int (*readdir)(void *, struct l9p_request *);
int (*fsync)(void *, struct l9p_request *);
int (*lock)(void *, struct l9p_request *);
int (*getlock)(void *, struct l9p_request *);
int (*link)(void *, struct l9p_request *);
int (*mkdir)(void *, struct l9p_request *);
int (*renameat)(void *, struct l9p_request *);
int (*unlinkat)(void *, struct l9p_request *);
};

#endif
