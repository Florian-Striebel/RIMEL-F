






























#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <libgen.h>
#include <pthread.h>
#include "../lib9p.h"
#include "../lib9p_impl.h"
#include "../fid.h"
#include "../log.h"
#include "../rfuncs.h"
#include "../genacl.h"
#include "backend.h"
#include "fs.h"

#if defined(WITH_CASPER)
#include <libcasper.h>
#include <casper/cap_pwd.h>
#include <casper/cap_grp.h>
#endif

#if defined(__FreeBSD__)
#include <sys/param.h>
#if __FreeBSD_version >= 1000000
#define HAVE_BINDAT
#endif
#endif

#if defined(__FreeBSD__)
#define HAVE_BIRTHTIME
#endif

#if defined(__APPLE__)
#include <sys/syscall.h>
#include "Availability.h"
#define ACL_TYPE_NFS4 ACL_TYPE_EXTENDED
#endif

struct fs_softc {
int fs_rootfd;
bool fs_readonly;
#if defined(WITH_CASPER)
cap_channel_t *fs_cappwd;
cap_channel_t *fs_capgrp;
#endif
};

struct fs_fid {
DIR *ff_dir;
int ff_dirfd;
int ff_fd;
int ff_flags;
char *ff_name;
struct fs_authinfo *ff_ai;
pthread_mutex_t ff_mtx;
struct l9p_acl *ff_acl;
};

#define FF_NO_NFSV4_ACL 0x01































struct fs_authinfo {
pthread_mutex_t ai_mtx;
uint32_t ai_refcnt;
int ai_flags;
uid_t ai_uid;
int ai_ngids;
gid_t ai_gids[];
};






static bool fs_attach_mutex_inited;
static pthread_mutex_t fs_attach_mutex;




static struct passwd *fs_getpwuid(struct fs_softc *, uid_t, struct r_pgdata *);
static struct group *fs_getgrgid(struct fs_softc *, gid_t, struct r_pgdata *);
static int fs_buildname(struct l9p_fid *, char *, char *, size_t);
static int fs_pdir(struct fs_softc *, struct l9p_fid *, char *, size_t,
struct stat *st);
static int fs_dpf(char *, char *, size_t);
static int fs_oflags_dotu(int, int *);
static int fs_oflags_dotl(uint32_t, int *, enum l9p_omode *);
static int fs_nde(struct fs_softc *, struct l9p_fid *, bool, gid_t,
struct stat *, uid_t *, gid_t *);
static struct fs_fid *open_fid(int, const char *, struct fs_authinfo *, bool);
static void dostat(struct fs_softc *, struct l9p_stat *, char *,
struct stat *, bool dotu);
static void dostatfs(struct l9p_statfs *, struct statfs *, long);
static void fillacl(struct fs_fid *ff);
static struct l9p_acl *getacl(struct fs_fid *ff, int fd, const char *path);
static void dropacl(struct fs_fid *ff);
static struct l9p_acl *look_for_nfsv4_acl(struct fs_fid *ff, int fd,
const char *path);
static int check_access(int32_t,
struct l9p_acl *, struct stat *, struct l9p_acl *, struct stat *,
struct fs_authinfo *, gid_t);
static void generate_qid(struct stat *, struct l9p_qid *);

static int fs_icreate(void *, struct l9p_fid *, char *, int,
bool, mode_t, gid_t, struct stat *);
static int fs_iopen(void *, struct l9p_fid *, int, enum l9p_omode,
gid_t, struct stat *);
static int fs_imkdir(void *, struct l9p_fid *, char *,
bool, mode_t, gid_t, struct stat *);
static int fs_imkfifo(void *, struct l9p_fid *, char *,
bool, mode_t, gid_t, struct stat *);
static int fs_imknod(void *, struct l9p_fid *, char *,
bool, mode_t, dev_t, gid_t, struct stat *);
static int fs_imksocket(void *, struct l9p_fid *, char *,
bool, mode_t, gid_t, struct stat *);
static int fs_isymlink(void *, struct l9p_fid *, char *, char *,
gid_t, struct stat *);




static int fs_attach(void *, struct l9p_request *);
static int fs_clunk(void *, struct l9p_fid *);
static int fs_create(void *, struct l9p_request *);
static int fs_open(void *, struct l9p_request *);
static int fs_read(void *, struct l9p_request *);
static int fs_remove(void *, struct l9p_fid *);
static int fs_stat(void *, struct l9p_request *);
static int fs_walk(void *, struct l9p_request *);
static int fs_write(void *, struct l9p_request *);
static int fs_wstat(void *, struct l9p_request *);
static int fs_statfs(void *, struct l9p_request *);
static int fs_lopen(void *, struct l9p_request *);
static int fs_lcreate(void *, struct l9p_request *);
static int fs_symlink(void *, struct l9p_request *);
static int fs_mknod(void *, struct l9p_request *);
static int fs_rename(void *, struct l9p_request *);
static int fs_readlink(void *, struct l9p_request *);
static int fs_getattr(void *, struct l9p_request *);
static int fs_setattr(void *, struct l9p_request *);
static int fs_xattrwalk(void *, struct l9p_request *);
static int fs_xattrcreate(void *, struct l9p_request *);
static int fs_readdir(void *, struct l9p_request *);
static int fs_fsync(void *, struct l9p_request *);
static int fs_lock(void *, struct l9p_request *);
static int fs_getlock(void *, struct l9p_request *);
static int fs_link(void *, struct l9p_request *);
static int fs_renameat(void *, struct l9p_request *);
static int fs_unlinkat(void *, struct l9p_request *);
static void fs_freefid(void *, struct l9p_fid *);
































static int
fs_oflags_dotu(int mode, int *aflags)
{
int flags;
#define CONVERT(theirs, ours) do { if (mode & (theirs)) { mode &= ~(theirs); flags |= ours; } } while (0)







switch (mode & L9P_OACCMODE) {

case L9P_OREAD:
default:
flags = O_RDONLY;
break;

case L9P_OWRITE:
flags = O_WRONLY;
break;

case L9P_ORDWR:
flags = O_RDWR;
break;

case L9P_OEXEC:
if (mode & L9P_OTRUNC)
return (EINVAL);
flags = O_RDONLY;
break;
}

flags |= O_NOCTTY | O_NOFOLLOW;

CONVERT(L9P_OTRUNC, O_TRUNC);











mode &= ~(L9P_OACCMODE | L9P_ORCLOSE | L9P_OCEXEC | L9P_ODIRECT);
if (mode != 0) {
L9P_LOG(L9P_INFO,
"fs_oflags_dotu: untranslated bits: %#x",
(unsigned)mode);
return (EINVAL);
}

*aflags = flags;
return (0);
#undef CONVERT
}










static int
fs_oflags_dotl(uint32_t l_mode, int *aflags, enum l9p_omode *ap9)
{
int flags;
enum l9p_omode p9;
#define CLEAR(theirs) l_mode &= ~(uint32_t)(theirs)
#define CONVERT(theirs, ours) do { if (l_mode & (theirs)) { CLEAR(theirs); flags |= ours; } } while (0)










flags = l_mode & O_ACCMODE;
if (flags == 3)
return (EINVAL);
CLEAR(O_ACCMODE);

if ((l_mode & (L9P_L_O_PATH | L9P_L_O_NOFOLLOW)) ==
(L9P_L_O_PATH | L9P_L_O_NOFOLLOW)) {
CLEAR(L9P_L_O_PATH | L9P_L_O_NOFOLLOW);
p9 = L9P_OEXEC;
} else {




p9 = (enum l9p_omode)flags;
}


if (l_mode & L9P_L_O_TRUNC)
p9 |= L9P_OTRUNC;

flags |= O_NOCTTY | O_NOFOLLOW;











CLEAR(L9P_L_O_CREAT);
CONVERT(L9P_L_O_EXCL, O_EXCL);
CONVERT(L9P_L_O_TRUNC, O_TRUNC);
CONVERT(L9P_L_O_DIRECTORY, O_DIRECTORY);
CONVERT(L9P_L_O_APPEND, O_APPEND);
CONVERT(L9P_L_O_NONBLOCK, O_NONBLOCK);






CLEAR(L9P_L_O_CLOEXEC);
CLEAR(L9P_L_O_DIRECT);
CLEAR(L9P_L_O_DSYNC);
CLEAR(L9P_L_O_FASYNC);
CLEAR(L9P_L_O_LARGEFILE);
CLEAR(L9P_L_O_NOATIME);
CLEAR(L9P_L_O_NOCTTY);
CLEAR(L9P_L_O_NOFOLLOW);
CLEAR(L9P_L_O_SYNC);

if (l_mode != 0) {
L9P_LOG(L9P_INFO,
"fs_oflags_dotl: untranslated bits: %#x",
(unsigned)l_mode);
return (EINVAL);
}

*aflags = flags;
*ap9 = p9;
return (0);
#undef CLEAR
#undef CONVERT
}

static struct passwd *
fs_getpwuid(struct fs_softc *sc, uid_t uid, struct r_pgdata *pg)
{
#if defined(WITH_CASPER)
return (r_cap_getpwuid(sc->fs_cappwd, uid, pg));
#else
(void)sc;
return (r_getpwuid(uid, pg));
#endif
}

static struct group *
fs_getgrgid(struct fs_softc *sc, gid_t gid, struct r_pgdata *pg)
{
#if defined(WITH_CASPER)
return (r_cap_getgrgid(sc->fs_capgrp, gid, pg));
#else
(void)sc;
return (r_getgrgid(gid, pg));
#endif
}




static int
fs_buildname(struct l9p_fid *dir, char *name, char *buf, size_t size)
{
struct fs_fid *dirf = dir->lo_aux;
size_t dlen, nlen1;

assert(dirf != NULL);
dlen = strlen(dirf->ff_name);
nlen1 = strlen(name) + 1;
if (dlen + 1 + nlen1 > size)
return (ENAMETOOLONG);
memcpy(buf, dirf->ff_name, dlen);
buf[dlen] = '/';
memcpy(buf + dlen + 1, name, nlen1);
return (0);
}






static int
fs_pdir(struct fs_softc *sc __unused, struct l9p_fid *fid, char *buf,
size_t size, struct stat *st)
{
struct fs_fid *ff;
char *path;

ff = fid->lo_aux;
assert(ff != NULL);
path = ff->ff_name;
path = r_dirname(path, buf, size);
if (path == NULL)
return (ENAMETOOLONG);
if (fstatat(ff->ff_dirfd, path, st, AT_SYMLINK_NOFOLLOW) != 0)
return (errno);
if (!S_ISDIR(st->st_mode))
return (ENOTDIR);
return (0);
}










static int
fs_dpf(char *dbuf, char *fname, size_t size)
{
size_t dlen, nlen1;

dlen = strlen(dbuf);
nlen1 = strlen(fname) + 1;
if (dlen + 1 + nlen1 > size)
return (ENAMETOOLONG);
dbuf[dlen] = '/';
memcpy(dbuf + dlen + 1, fname, nlen1);
return (0);
}
















static int
fs_nde(struct fs_softc *sc, struct l9p_fid *dir, bool isdir, gid_t egid,
struct stat *st, uid_t *nuid, gid_t *ngid)
{
struct fs_fid *dirf;
struct fs_authinfo *ai;
int32_t op;
int error;

if (sc->fs_readonly)
return (EROFS);
dirf = dir->lo_aux;
assert(dirf != NULL);
if (fstatat(dirf->ff_dirfd, dirf->ff_name, st,
AT_SYMLINK_NOFOLLOW) != 0)
return (errno);
if (!S_ISDIR(st->st_mode))
return (ENOTDIR);
dirf = dir->lo_aux;
ai = dirf->ff_ai;
fillacl(dirf);
op = isdir ? L9P_ACE_ADD_SUBDIRECTORY : L9P_ACE_ADD_FILE;
error = check_access(op, dirf->ff_acl, st, NULL, NULL, ai, egid);
if (error)
return (EPERM);

*nuid = ai->ai_uid != (uid_t)-1 ? ai->ai_uid : st->st_uid;
*ngid = egid != (gid_t)-1 ? egid :
ai->ai_ngids > 0 ? ai->ai_gids[0] : st->st_gid;
return (0);
}







static struct fs_fid *
open_fid(int dirfd, const char *path, struct fs_authinfo *ai, bool creating)
{
struct fs_fid *ret;
uint32_t newcount;
int error;

ret = l9p_calloc(1, sizeof(*ret));
error = pthread_mutex_init(&ret->ff_mtx, NULL);
if (error) {
free(ret);
return (NULL);
}
ret->ff_fd = -1;
ret->ff_dirfd = dirfd;
ret->ff_name = strdup(path);
if (ret->ff_name == NULL) {
pthread_mutex_destroy(&ret->ff_mtx);
free(ret);
return (NULL);
}
pthread_mutex_lock(&ai->ai_mtx);
newcount = ++ai->ai_refcnt;
pthread_mutex_unlock(&ai->ai_mtx);







assert(newcount > 1 || creating);
L9P_LOG(L9P_DEBUG, "authinfo %p now used by %lu",
(void *)ai, (u_long)newcount);
ret->ff_ai = ai;
return (ret);
}

static void
dostat(struct fs_softc *sc, struct l9p_stat *s, char *name,
struct stat *buf, bool dotu)
{
struct passwd *user;
struct group *group;

memset(s, 0, sizeof(struct l9p_stat));

generate_qid(buf, &s->qid);

s->type = 0;
s->dev = 0;
s->mode = buf->st_mode & 0777;

if (S_ISDIR(buf->st_mode))
s->mode |= L9P_DMDIR;

if (S_ISLNK(buf->st_mode) && dotu)
s->mode |= L9P_DMSYMLINK;

if (S_ISCHR(buf->st_mode) || S_ISBLK(buf->st_mode))
s->mode |= L9P_DMDEVICE;

if (S_ISSOCK(buf->st_mode))
s->mode |= L9P_DMSOCKET;

if (S_ISFIFO(buf->st_mode))
s->mode |= L9P_DMNAMEDPIPE;

s->atime = (uint32_t)buf->st_atime;
s->mtime = (uint32_t)buf->st_mtime;
s->length = (uint64_t)buf->st_size;

s->name = r_basename(name, NULL, 0);

if (!dotu) {
struct r_pgdata udata, gdata;

user = fs_getpwuid(sc, buf->st_uid, &udata);
group = fs_getgrgid(sc, buf->st_gid, &gdata);
s->uid = user != NULL ? strdup(user->pw_name) : NULL;
s->gid = group != NULL ? strdup(group->gr_name) : NULL;
s->muid = user != NULL ? strdup(user->pw_name) : NULL;
r_pgfree(&udata);
r_pgfree(&gdata);
} else {







s->n_uid = buf->st_uid;
s->n_gid = buf->st_gid;
s->n_muid = buf->st_uid;

if (S_ISLNK(buf->st_mode)) {
char target[MAXPATHLEN];
ssize_t ret = readlink(name, target, MAXPATHLEN);

if (ret < 0) {
s->extension = NULL;
return;
}

s->extension = strndup(target, (size_t)ret);
}

if (S_ISBLK(buf->st_mode)) {
asprintf(&s->extension, "b %d %d", major(buf->st_rdev),
minor(buf->st_rdev));
}

if (S_ISCHR(buf->st_mode)) {
asprintf(&s->extension, "c %d %d", major(buf->st_rdev),
minor(buf->st_rdev));
}
}
}

static void dostatfs(struct l9p_statfs *out, struct statfs *in, long namelen)
{

out->type = L9P_FSTYPE;
out->bsize = in->f_bsize;
out->blocks = in->f_blocks;
out->bfree = in->f_bfree;
out->bavail = in->f_bavail;
out->files = in->f_files;
out->ffree = in->f_ffree;
out->namelen = (uint32_t)namelen;
out->fsid = ((uint64_t)in->f_fsid.val[0] << 32) |
(uint64_t)in->f_fsid.val[1];
}

static void
generate_qid(struct stat *buf, struct l9p_qid *qid)
{
qid->path = buf->st_ino;
qid->version = 0;

if (S_ISREG(buf->st_mode))
qid->type |= L9P_QTFILE;

if (S_ISDIR(buf->st_mode))
qid->type |= L9P_QTDIR;

if (S_ISLNK(buf->st_mode))
qid->type |= L9P_QTSYMLINK;
}






static void
fillacl(struct fs_fid *ff)
{

if (ff->ff_acl == NULL && (ff->ff_flags & FF_NO_NFSV4_ACL) == 0) {
ff->ff_acl = look_for_nfsv4_acl(ff, ff->ff_fd, ff->ff_name);
if (ff->ff_acl == NULL)
ff->ff_flags |= FF_NO_NFSV4_ACL;
}
}









static struct l9p_acl *
getacl(struct fs_fid *ff, int fd, const char *path)
{

if (ff->ff_flags & FF_NO_NFSV4_ACL)
return (NULL);
return look_for_nfsv4_acl(ff, fd, path);
}





static void
dropacl(struct fs_fid *ff)
{

l9p_acl_free(ff->ff_acl);
ff->ff_acl = NULL;
ff->ff_flags = ff->ff_ai->ai_flags;
}






static struct l9p_acl *
look_for_nfsv4_acl(struct fs_fid *ff, int fd, const char *path)
{
struct l9p_acl *acl;
acl_t sysacl;
int doclose = 0;

if (fd < 0) {
fd = openat(ff->ff_dirfd, path, 0);
doclose = 1;
}

sysacl = acl_get_fd_np(fd, ACL_TYPE_NFS4);
if (sysacl == NULL) {




if (errno != EINVAL) {
L9P_LOG(L9P_ERROR,
"error retrieving NFSv4 ACL from "
"fdesc %d (%s): %s", fd,
path, strerror(errno));
}

if (doclose)
close(fd);

return (NULL);
}
#if defined(HAVE_FREEBSD_ACLS)
acl = l9p_freebsd_nfsv4acl_to_acl(sysacl);
#else
acl = NULL;
#endif
acl_free(sysacl);

if (doclose)
close(fd);

return (acl);
}













static int
check_access(int32_t opmask,
struct l9p_acl *pacl, struct stat *pst,
struct l9p_acl *cacl, struct stat *cst,
struct fs_authinfo *ai, gid_t egid)
{
struct l9p_acl_check_args args;






args.aca_uid = ai->ai_uid;
args.aca_gid = egid;
args.aca_groups = ai->ai_gids;
args.aca_ngroups = (size_t)ai->ai_ngids;
args.aca_parent = pacl;
args.aca_pstat = pst;
args.aca_child = cacl;
args.aca_cstat = cst;
args.aca_aclmode = pacl == NULL && cacl == NULL
? L9P_ACM_STAT_MODE
: L9P_ACM_NFS_ACL | L9P_ACM_ZFS_ACL;

args.aca_superuser = true;
return (l9p_acl_check_access(opmask, &args));
}

static int
fs_attach(void *softc, struct l9p_request *req)
{
struct fs_authinfo *ai;
struct fs_softc *sc = (struct fs_softc *)softc;
struct fs_fid *file;
struct passwd *pwd;
struct stat st;
struct r_pgdata udata;
uint32_t n_uname;
gid_t *gids;
uid_t uid;
int error;
int ngroups;

assert(req->lr_fid != NULL);






pthread_mutex_lock(&fs_attach_mutex);

n_uname = req->lr_req.tattach.n_uname;
if (n_uname != L9P_NONUNAME) {
uid = (uid_t)n_uname;
pwd = fs_getpwuid(sc, uid, &udata);
if (pwd == NULL)
L9P_LOG(L9P_DEBUG,
"Tattach: uid %ld: no such user", (long)uid);
} else {
uid = (uid_t)-1;
#if defined(WITH_CASPER)
pwd = cap_getpwnam(sc->fs_cappwd, req->lr_req.tattach.uname);
#else
pwd = getpwnam(req->lr_req.tattach.uname);
#endif
if (pwd == NULL)
L9P_LOG(L9P_DEBUG,
"Tattach: %s: no such user",
req->lr_req.tattach.uname);
}









if (uid == (uid_t)-1 && pwd != NULL)
uid = pwd->pw_uid;
if (uid == (uid_t)-1)
error = EPERM;
else {
error = 0;
if (fstat(sc->fs_rootfd, &st) != 0)
error = errno;
else if (!S_ISDIR(st.st_mode))
error = ENOTDIR;
}
if (error) {
pthread_mutex_unlock(&fs_attach_mutex);
L9P_LOG(L9P_DEBUG,
"Tattach: denying uid=%ld access to rootdir: %s",
(long)uid, strerror(error));





return (error == ENOENT || error == ENOTDIR ? error : EPERM);
}

if (pwd != NULL) {





gids = l9p_getgrlist(pwd->pw_name, pwd->pw_gid, &ngroups);
} else {
gids = NULL;
ngroups = 0;
}





pthread_mutex_unlock(&fs_attach_mutex);

ai = malloc(sizeof(*ai) + (size_t)ngroups * sizeof(gid_t));
if (ai == NULL) {
free(gids);
return (ENOMEM);
}
error = pthread_mutex_init(&ai->ai_mtx, NULL);
if (error) {
free(gids);
free(ai);
return (error);
}
ai->ai_refcnt = 0;
ai->ai_uid = uid;
ai->ai_flags = 0;
ai->ai_ngids = ngroups;
memcpy(ai->ai_gids, gids, (size_t)ngroups * sizeof(gid_t));
free(gids);

file = open_fid(sc->fs_rootfd, ".", ai, true);
if (file == NULL) {
pthread_mutex_destroy(&ai->ai_mtx);
free(ai);
return (ENOMEM);
}

req->lr_fid->lo_aux = file;
generate_qid(&st, &req->lr_resp.rattach.qid);
return (0);
}

static int
fs_clunk(void *softc __unused, struct l9p_fid *fid)
{
struct fs_fid *file;

file = fid->lo_aux;
assert(file != NULL);

if (file->ff_dir) {
closedir(file->ff_dir);
file->ff_dir = NULL;
} else if (file->ff_fd != -1) {
close(file->ff_fd);
file->ff_fd = -1;
}

return (0);
}










static int
fs_create(void *softc, struct l9p_request *req)
{
struct l9p_fid *dir;
struct stat st;
uint32_t dmperm;
mode_t perm;
char *name;
int error;

dir = req->lr_fid;
name = req->lr_req.tcreate.name;
dmperm = req->lr_req.tcreate.perm;
perm = (mode_t)(dmperm & 0777);

if (dmperm & L9P_DMDIR)
error = fs_imkdir(softc, dir, name, true,
perm, (gid_t)-1, &st);
else if (dmperm & L9P_DMSYMLINK)
error = fs_isymlink(softc, dir, name,
req->lr_req.tcreate.extension, (gid_t)-1, &st);
else if (dmperm & L9P_DMNAMEDPIPE)
error = fs_imkfifo(softc, dir, name, true,
perm, (gid_t)-1, &st);
else if (dmperm & L9P_DMSOCKET)
error = fs_imksocket(softc, dir, name, true,
perm, (gid_t)-1, &st);
else if (dmperm & L9P_DMDEVICE) {
unsigned int major, minor;
char type;
dev_t dev;





minor = 0;
if (sscanf(req->lr_req.tcreate.extension, "%c %u %u",
&type, &major, &minor) < 2) {
return (EINVAL);
}

switch (type) {
case 'b':
perm |= S_IFBLK;
break;
case 'c':
perm |= S_IFCHR;
break;
default:
return (EINVAL);
}
dev = makedev(major, minor);
error = fs_imknod(softc, dir, name, true, perm, dev,
(gid_t)-1, &st);
} else {
enum l9p_omode p9;
int flags;

p9 = req->lr_req.tcreate.mode;
error = fs_oflags_dotu(p9, &flags);
if (error)
return (error);
error = fs_icreate(softc, dir, name, flags,
true, perm, (gid_t)-1, &st);
req->lr_resp.rcreate.iounit = req->lr_conn->lc_max_io_size;
}

if (error == 0)
generate_qid(&st, &req->lr_resp.rcreate.qid);

return (error);
}














static inline mode_t
fs_p9perm(mode_t perm, mode_t dir_perm, bool isdir)
{

if (isdir)
perm &= ~0777 | (dir_perm & 0777);
else
perm &= ~0666 | (dir_perm & 0666);
return (perm);
}



















static int
fs_icreate(void *softc, struct l9p_fid *dir, char *name, int flags,
bool isp9, mode_t perm, gid_t egid, struct stat *st)
{
struct fs_fid *file;
gid_t gid;
uid_t uid;
char newname[MAXPATHLEN];
int error, fd;

file = dir->lo_aux;









error = fs_buildname(dir, name, newname, sizeof(newname));
if (error)
return (error);


name = strdup(newname);
if (name == NULL)
return (ENOMEM);


error = fs_nde(softc, dir, false, egid, st, &uid, &gid);
if (error) {
free(name);
return (error);
}


if (isp9)
perm = fs_p9perm(perm, st->st_mode, false);


fd = openat(file->ff_dirfd, newname, flags | O_CREAT | O_EXCL, perm);
if (fd < 0) {
error = errno;
free(name);
return (error);
}


if (fchmod(fd, perm) != 0 ||
fchown(fd, uid, gid) != 0 ||
fstat(fd, st) != 0) {
error = errno;
(void) close(fd);

free(name);
return (error);
}


free(file->ff_name);
file->ff_name = name;
file->ff_fd = fd;
return (0);
}


























static int
fs_iopen(void *softc, struct l9p_fid *fid, int flags, enum l9p_omode p9,
gid_t egid __unused, struct stat *st)
{
struct fs_softc *sc = softc;
struct fs_fid *file;
struct stat first;
int32_t op;
char *name;
int error;
int fd;
DIR *dirp;


if (sc->fs_readonly) {
if ((flags & O_TRUNC) != 0)
return (EROFS);
if ((flags & O_ACCMODE) != O_RDONLY)
return (EROFS);
if (p9 & L9P_ORCLOSE)
return (EROFS);
}

file = fid->lo_aux;
assert(file != NULL);
name = file->ff_name;

if (fstatat(file->ff_dirfd, name, &first, AT_SYMLINK_NOFOLLOW) != 0)
return (errno);
if (S_ISLNK(first.st_mode))
return (EPERM);


switch (flags & O_ACCMODE) {
case O_RDONLY:
op = L9P_ACE_READ_DATA;
break;
case O_WRONLY:
op = L9P_ACE_WRITE_DATA;
break;
case O_RDWR:
op = L9P_ACE_READ_DATA | L9P_ACE_WRITE_DATA;
break;
default:
return (EINVAL);
}
fillacl(file);
error = check_access(op, NULL, NULL, file->ff_acl, &first,
file->ff_ai, (gid_t)-1);
if (error)
return (error);

if (S_ISDIR(first.st_mode)) {

if ((flags & O_ACCMODE) != O_RDONLY || (flags & O_TRUNC))
return (EPERM);
fd = openat(file->ff_dirfd, name, O_DIRECTORY);
dirp = fdopendir(fd);
if (dirp == NULL)
return (EPERM);
fd = dirfd(dirp);
} else {
dirp = NULL;
fd = openat(file->ff_dirfd, name, flags);
if (fd < 0)
return (EPERM);
}





if (fstat(fd, st) != 0 ||
first.st_dev != st->st_dev ||
first.st_ino != st->st_ino) {
if (dirp != NULL)
(void) closedir(dirp);
else
(void) close(fd);
return (EPERM);
}
if (dirp != NULL)
file->ff_dir = dirp;
else
file->ff_fd = fd;
return (0);
}












static int
fs_imkdir(void *softc, struct l9p_fid *dir, char *name,
bool isp9, mode_t perm, gid_t egid, struct stat *st)
{
struct fs_fid *ff;
gid_t gid;
uid_t uid;
char newname[MAXPATHLEN];
int error, fd;

ff = dir->lo_aux;
error = fs_buildname(dir, name, newname, sizeof(newname));
if (error)
return (error);

error = fs_nde(softc, dir, true, egid, st, &uid, &gid);
if (error)
return (error);

if (isp9)
perm = fs_p9perm(perm, st->st_mode, true);

if (mkdirat(ff->ff_dirfd, newname, perm) != 0)
return (errno);

fd = openat(ff->ff_dirfd, newname,
O_DIRECTORY | O_RDONLY | O_NOFOLLOW);
if (fd < 0 ||
fchown(fd, uid, gid) != 0 ||
fchmod(fd, perm) != 0 ||
fstat(fd, st) != 0) {
error = errno;

}
if (fd >= 0)
(void) close(fd);

return (error);
}

#if defined(__APPLE__)





static int fs_ifchdir_thread_local(int fd)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
return syscall(SYS___pthread_fchdir, fd);
#pragma clang diagnostic pop
}
#endif






static int
fs_imknod(void *softc, struct l9p_fid *dir, char *name,
bool isp9, mode_t mode, dev_t dev, gid_t egid, struct stat *st)
{
struct fs_fid *ff;
mode_t perm;
gid_t gid;
uid_t uid;
char newname[MAXPATHLEN];
int error;

ff = dir->lo_aux;
error = fs_buildname(dir, name, newname, sizeof(newname));
if (error)
return (error);

error = fs_nde(softc, dir, false, egid, st, &uid, &gid);
if (error)
return (error);

if (isp9) {
perm = fs_p9perm(mode & 0777, st->st_mode, false);
mode = (mode & ~0777) | perm;
} else {
perm = mode & 0777;
}

#if defined(__APPLE__)
if (fs_ifchdir_thread_local(ff->ff_dirfd) < 0) {
return -1;
}
error = mknod(newname, mode, dev);
int preserved_errno = errno;

fs_ifchdir_thread_local(-1);
if (error < 0) {
errno = preserved_errno;
return errno;
}
#else
if (mknodat(ff->ff_dirfd, newname, mode, dev) != 0)
return (errno);
#endif


if (fchownat(ff->ff_dirfd, newname, uid, gid, AT_SYMLINK_NOFOLLOW) != 0 ||
fchmodat(ff->ff_dirfd, newname, perm, AT_SYMLINK_NOFOLLOW) != 0 ||
fstatat(ff->ff_dirfd, newname, st, AT_SYMLINK_NOFOLLOW) != 0)
error = errno;
else if ((st->st_mode & S_IFMT) != (mode & S_IFMT))
error = EPERM;



return (error);
}




static int
fs_imkfifo(void *softc, struct l9p_fid *dir, char *name,
bool isp9, mode_t perm, gid_t egid, struct stat *st)
{
struct fs_fid *ff;
gid_t gid;
uid_t uid;
char newname[MAXPATHLEN];
int error;

ff = dir->lo_aux;
error = fs_buildname(dir, name, newname, sizeof(newname));
if (error)
return (error);

error = fs_nde(softc, dir, false, egid, st, &uid, &gid);
if (error)
return (error);

if (isp9)
perm = fs_p9perm(perm, st->st_mode, false);

if (mkfifo(newname, perm) != 0)
return (errno);


if (fchownat(ff->ff_dirfd, newname, uid, gid, AT_SYMLINK_NOFOLLOW) != 0 ||
fchmodat(ff->ff_dirfd, newname, perm, AT_SYMLINK_NOFOLLOW) != 0 ||
fstatat(ff->ff_dirfd, newname, st, AT_SYMLINK_NOFOLLOW) != 0)
error = errno;
else if (!S_ISFIFO(st->st_mode))
error = EPERM;



return (error);
}







static int
fs_imksocket(void *softc, struct l9p_fid *dir, char *name,
bool isp9, mode_t perm, gid_t egid, struct stat *st)
{
struct fs_fid *ff;
struct sockaddr_un sun;
char *path;
char newname[MAXPATHLEN];
gid_t gid;
uid_t uid;
int error = 0, s, fd;

ff = dir->lo_aux;
error = fs_buildname(dir, name, newname, sizeof(newname));
if (error)
return (error);

error = fs_nde(softc, dir, false, egid, st, &uid, &gid);
if (error)
return (error);

if (isp9)
perm = fs_p9perm(perm, st->st_mode, false);

s = socket(AF_UNIX, SOCK_STREAM, 0);
if (s < 0)
return (errno);

path = newname;
fd = -1;
#if defined(HAVE_BINDAT)

if (strlen(path) >= sizeof(sun.sun_path)) {
fd = openat(ff->ff_dirfd, ff->ff_name,
O_RDONLY | O_DIRECTORY | O_NOFOLLOW);
if (fd >= 0)
path = name;
}
#endif











if (strlen(path) >= sizeof(sun.sun_path)) {
error = ENAMETOOLONG;
goto out;
}
sun.sun_family = AF_UNIX;
sun.sun_len = sizeof(struct sockaddr_un);
strncpy(sun.sun_path, path, sizeof(sun.sun_path));

#if defined(HAVE_BINDAT)
if (fd >= 0) {
if (bindat(fd, s, (struct sockaddr *)&sun, sun.sun_len) < 0)
error = errno;
goto out;
}
#endif

if (bind(s, (struct sockaddr *)&sun, sun.sun_len) < 0)
error = errno;
out:

if (error == 0) {






if (fchownat(ff->ff_dirfd, newname, uid, gid, AT_SYMLINK_NOFOLLOW) != 0 ||
fchmodat(ff->ff_dirfd, newname, perm, AT_SYMLINK_NOFOLLOW) != 0 ||
fstatat(ff->ff_dirfd, newname, st, AT_SYMLINK_NOFOLLOW) != 0)
error = errno;
else if (!S_ISSOCK(st->st_mode))
error = EPERM;


}








if (fd >= 0 && close(fd) != 0)
if (error == 0)
error = errno;
if (close(s) != 0)
if (error == 0)
error = errno;

return (error);
}







static int
fs_isymlink(void *softc, struct l9p_fid *dir, char *name,
char *symtgt, gid_t egid, struct stat *st)
{
struct fs_fid *ff;
gid_t gid;
uid_t uid;
char newname[MAXPATHLEN];
int error;

ff = dir->lo_aux;
error = fs_buildname(dir, name, newname, sizeof(newname));
if (error)
return (error);

error = fs_nde(softc, dir, false, egid, st, &uid, &gid);
if (error)
return (error);

if (symlinkat(symtgt, ff->ff_dirfd, newname) != 0)
return (errno);


if (fchownat(ff->ff_dirfd, newname, uid, gid, AT_SYMLINK_NOFOLLOW) != 0 ||
fstatat(ff->ff_dirfd, newname, st, AT_SYMLINK_NOFOLLOW) != 0)
error = errno;
else if (!S_ISLNK(st->st_mode))
error = EPERM;



return (error);
}

static int
fs_open(void *softc, struct l9p_request *req)
{
struct l9p_fid *fid = req->lr_fid;
struct stat st;
enum l9p_omode p9;
int error, flags;

p9 = req->lr_req.topen.mode;
error = fs_oflags_dotu(p9, &flags);
if (error)
return (error);

error = fs_iopen(softc, fid, flags, p9, (gid_t)-1, &st);
if (error)
return (error);

generate_qid(&st, &req->lr_resp.ropen.qid);
req->lr_resp.ropen.iounit = req->lr_conn->lc_max_io_size;
return (0);
}







static inline int
fs_lstatat(struct fs_fid *file, char *name, struct stat *st)
{

return (fstatat(dirfd(file->ff_dir), name, st, AT_SYMLINK_NOFOLLOW));
}

static int
fs_read(void *softc, struct l9p_request *req)
{
struct l9p_stat l9stat;
struct fs_softc *sc;
struct fs_fid *file;
bool dotu = req->lr_conn->lc_version >= L9P_2000U;
ssize_t ret;

sc = softc;
file = req->lr_fid->lo_aux;
assert(file != NULL);

if (file->ff_dir != NULL) {
struct dirent *d;
struct stat st;
struct l9p_message msg;
long o;

pthread_mutex_lock(&file->ff_mtx);















l9p_init_msg(&msg, req, L9P_PACK);
for (;;) {
o = telldir(file->ff_dir);
d = readdir(file->ff_dir);
if (d == NULL)
break;
if (fs_lstatat(file, d->d_name, &st))
continue;
dostat(sc, &l9stat, d->d_name, &st, dotu);
if (l9p_pack_stat(&msg, req, &l9stat) != 0) {
seekdir(file->ff_dir, o);
break;
}
#if defined(__FreeBSD__)
seekdir(file->ff_dir, o);
(void) readdir(file->ff_dir);
#endif
}

pthread_mutex_unlock(&file->ff_mtx);
} else {
size_t niov = l9p_truncate_iov(req->lr_data_iov,
req->lr_data_niov, req->lr_req.io.count);

#if defined(__FreeBSD__)
ret = preadv(file->ff_fd, req->lr_data_iov, niov,
req->lr_req.io.offset);
#else

if (lseek(file->ff_fd, (off_t)req->lr_req.io.offset, SEEK_SET) < 0)
return (errno);

ret = (uint32_t)readv(file->ff_fd, req->lr_data_iov, (int)niov);
#endif

if (ret < 0)
return (errno);

req->lr_resp.io.count = (uint32_t)ret;
}

return (0);
}

static int
fs_remove(void *softc, struct l9p_fid *fid)
{
struct fs_softc *sc = softc;
struct l9p_acl *parent_acl;
struct fs_fid *file;
struct stat pst, cst;
char dirname[MAXPATHLEN];
int error;

if (sc->fs_readonly)
return (EROFS);

error = fs_pdir(sc, fid, dirname, sizeof(dirname), &pst);
if (error)
return (error);

file = fid->lo_aux;
if (fstatat(file->ff_dirfd, file->ff_name, &cst, AT_SYMLINK_NOFOLLOW) != 0)
return (error);

parent_acl = getacl(file, -1, dirname);
fillacl(file);

error = check_access(L9P_ACOP_UNLINK,
parent_acl, &pst, file->ff_acl, &cst, file->ff_ai, (gid_t)-1);
l9p_acl_free(parent_acl);
if (error)
return (error);

if (unlinkat(file->ff_dirfd, file->ff_name,
S_ISDIR(cst.st_mode) ? AT_REMOVEDIR : 0) != 0)
error = errno;

return (error);
}

static int
fs_stat(void *softc, struct l9p_request *req)
{
struct fs_softc *sc;
struct fs_fid *file;
struct stat st;
bool dotu = req->lr_conn->lc_version >= L9P_2000U;

sc = softc;
file = req->lr_fid->lo_aux;
assert(file);

if (fstatat(file->ff_dirfd, file->ff_name, &st,
AT_SYMLINK_NOFOLLOW) != 0)
return (errno);

dostat(sc, &req->lr_resp.rstat.stat, file->ff_name, &st, dotu);
return (0);
}

static int
fs_walk(void *softc, struct l9p_request *req)
{
struct l9p_acl *acl;
struct fs_authinfo *ai;
struct fs_fid *file = req->lr_fid->lo_aux;
struct fs_fid *newfile;
struct stat st;
size_t clen, namelen, need;
char *comp, *succ, *next, *swtmp;
bool atroot;
bool dotdot;
int i, nwname;
int error = 0;
char namebufs[2][MAXPATHLEN];































succ = namebufs[0];
next = namebufs[1];
namelen = strlcpy(succ, file->ff_name, MAXPATHLEN);
if (namelen >= MAXPATHLEN)
return (ENAMETOOLONG);
if (fstatat(file->ff_dirfd, succ, &st, AT_SYMLINK_NOFOLLOW) < 0)
return (errno);
ai = file->ff_ai;
atroot = strlen(succ) == 0;
fillacl(file);
acl = file->ff_acl;

nwname = (int)req->lr_req.twalk.nwname;

for (i = 0; i < nwname; i++) {







if (!S_ISDIR(st.st_mode)) {
error = ENOTDIR;
goto out;
}
error = check_access(L9P_ACE_EXECUTE,
NULL, NULL, acl, &st, ai, (gid_t)-1);
if (error) {
L9P_LOG(L9P_DEBUG,
"Twalk: denying dir-walk on \"%s\" for uid %u",
succ, (unsigned)ai->ai_uid);
error = EPERM;
goto out;
}
comp = req->lr_req.twalk.wname[i];
if (strchr(comp, '/') != NULL) {
error = EINVAL;
break;
}

clen = strlen(comp);
dotdot = false;










if (comp[0] == '.') {
if (clen == 1) {
error = EINVAL;
break;
}
if (comp[1] == '.' && clen == 2)
dotdot = true;
}
if (dotdot) {









if (atroot) {
i = 0;
break;
}
(void) r_dirname(succ, next, MAXPATHLEN);
namelen = strlen(next);
atroot = strlen(next) == 0;
} else {
need = namelen + 1 + clen + 1;
if (need > MAXPATHLEN) {
error = ENAMETOOLONG;
break;
}
memcpy(next, succ, namelen);
next[namelen++] = '/';
memcpy(&next[namelen], comp, clen + 1);
namelen += clen;




atroot = false;
}

if (fstatat(file->ff_dirfd, next, &st, AT_SYMLINK_NOFOLLOW) < 0) {
error = ENOENT;
break;
}





generate_qid(&st, &req->lr_resp.rwalk.wqid[i]);
swtmp = succ;
succ = next;
next = swtmp;
if (acl != NULL && acl != file->ff_acl)
l9p_acl_free(acl);
acl = getacl(file, -1, next);
}






if (error) {
if (i == 0)
goto out;
error = 0;
}

newfile = open_fid(file->ff_dirfd, succ, ai, false);
if (newfile == NULL) {
error = ENOMEM;
goto out;
}
if (req->lr_newfid == req->lr_fid) {





if (acl == file->ff_acl)
acl = NULL;
fs_freefid(softc, req->lr_fid);
file = NULL;
}
req->lr_newfid->lo_aux = newfile;
if (file != NULL && acl != file->ff_acl) {
newfile->ff_acl = acl;
acl = NULL;
}
req->lr_resp.rwalk.nwqid = (uint16_t)i;
out:
if (file != NULL && acl != file->ff_acl)
l9p_acl_free(acl);
return (error);
}

static int
fs_write(void *softc, struct l9p_request *req)
{
struct fs_softc *sc = softc;
struct fs_fid *file;
ssize_t ret;

file = req->lr_fid->lo_aux;
assert(file != NULL);

if (sc->fs_readonly)
return (EROFS);

size_t niov = l9p_truncate_iov(req->lr_data_iov,
req->lr_data_niov, req->lr_req.io.count);

#if defined(__FreeBSD__)
ret = pwritev(file->ff_fd, req->lr_data_iov, niov,
req->lr_req.io.offset);
#else

if (lseek(file->ff_fd, (off_t)req->lr_req.io.offset, SEEK_SET) < 0)
return (errno);

ret = writev(file->ff_fd, req->lr_data_iov,
(int)niov);
#endif

if (ret < 0)
return (errno);

req->lr_resp.io.count = (uint32_t)ret;
return (0);
}

static int
fs_wstat(void *softc, struct l9p_request *req)
{
struct fs_softc *sc = softc;
struct l9p_stat *l9stat = &req->lr_req.twstat.stat;
struct l9p_fid *fid;
struct fs_fid *file;
int error = 0;

fid = req->lr_fid;
file = fid->lo_aux;
assert(file != NULL);













if (sc->fs_readonly)
return (EROFS);

if (l9stat->atime != (uint32_t)~0) {

}

if (l9stat->mtime != (uint32_t)~0) {

}

if (l9stat->dev != (uint32_t)~0) {
error = EPERM;
goto out;
}

if (l9stat->length != (uint64_t)~0) {
if (file->ff_dir != NULL) {
error = EINVAL;
goto out;
}

if (truncate(file->ff_name, (off_t)l9stat->length) != 0) {
error = errno;
goto out;
}
}

if (req->lr_conn->lc_version >= L9P_2000U) {
if (fchownat(file->ff_dirfd, file->ff_name, l9stat->n_uid,
l9stat->n_gid, AT_SYMLINK_NOFOLLOW) != 0) {
error = errno;
goto out;
}
}

if (l9stat->mode != (uint32_t)~0) {
if (fchmodat(file->ff_dirfd, file->ff_name,
l9stat->mode & 0777, 0) != 0) {
error = errno;
goto out;
}
}

if (strlen(l9stat->name) > 0) {
struct l9p_acl *parent_acl;
struct stat st;
char *tmp;
char newname[MAXPATHLEN];






error = fs_pdir(softc, fid, newname, sizeof(newname), &st);
if (error)
goto out;
parent_acl = getacl(file, -1, newname);
error = check_access(L9P_ACE_ADD_FILE,
parent_acl, &st, NULL, NULL, file->ff_ai, (gid_t)-1);
l9p_acl_free(parent_acl);
if (error)
goto out;
error = fs_dpf(newname, l9stat->name, sizeof(newname));
if (error)
goto out;
tmp = strdup(newname);
if (tmp == NULL) {
error = ENOMEM;
goto out;
}
if (renameat(file->ff_dirfd, file->ff_name, file->ff_dirfd,
tmp) != 0) {
error = errno;
free(tmp);
goto out;
}

free(file->ff_name);
file->ff_name = tmp;
}
out:
return (error);
}

static int
fs_statfs(void *softc __unused, struct l9p_request *req)
{
struct fs_fid *file;
struct stat st;
struct statfs f;
long name_max;
int error;
int fd;

file = req->lr_fid->lo_aux;
assert(file);

if (fstatat(file->ff_dirfd, file->ff_name, &st,
AT_SYMLINK_NOFOLLOW) != 0)
return (errno);





fillacl(file);
error = check_access(L9P_ACE_READ_DATA, NULL, NULL,
file->ff_acl, &st, file->ff_ai, (gid_t)-1);
if (error)
return (error);

fd = openat(file->ff_dirfd, file->ff_name, 0);
if (fd < 0)
return (errno);

if (fstatfs(fd, &f) != 0)
return (errno);

name_max = fpathconf(fd, _PC_NAME_MAX);
error = errno;
close(fd);

if (name_max == -1)
return (error);

dostatfs(&req->lr_resp.rstatfs.statfs, &f, name_max);

return (0);
}

static int
fs_lopen(void *softc, struct l9p_request *req)
{
struct l9p_fid *fid = req->lr_fid;
struct stat st;
enum l9p_omode p9;
gid_t gid;
int error, flags;

error = fs_oflags_dotl(req->lr_req.tlopen.flags, &flags, &p9);
if (error)
return (error);

gid = req->lr_req.tlopen.gid;
error = fs_iopen(softc, fid, flags, p9, gid, &st);
if (error)
return (error);

generate_qid(&st, &req->lr_resp.rlopen.qid);
req->lr_resp.rlopen.iounit = req->lr_conn->lc_max_io_size;
return (0);
}

static int
fs_lcreate(void *softc, struct l9p_request *req)
{
struct l9p_fid *dir;
struct stat st;
enum l9p_omode p9;
char *name;
mode_t perm;
gid_t gid;
int error, flags;

dir = req->lr_fid;
name = req->lr_req.tlcreate.name;

error = fs_oflags_dotl(req->lr_req.tlcreate.flags, &flags, &p9);
if (error)
return (error);

perm = (mode_t)req->lr_req.tlcreate.mode & 0777;
gid = req->lr_req.tlcreate.gid;
error = fs_icreate(softc, dir, name, flags, false, perm, gid, &st);
if (error == 0)
generate_qid(&st, &req->lr_resp.rlcreate.qid);
req->lr_resp.rlcreate.iounit = req->lr_conn->lc_max_io_size;
return (error);
}

static int
fs_symlink(void *softc, struct l9p_request *req)
{
struct l9p_fid *dir;
struct stat st;
gid_t gid;
char *name, *symtgt;
int error;

dir = req->lr_fid;
name = req->lr_req.tsymlink.name;
symtgt = req->lr_req.tsymlink.symtgt;
gid = req->lr_req.tsymlink.gid;
error = fs_isymlink(softc, dir, name, symtgt, gid, &st);
if (error == 0)
generate_qid(&st, &req->lr_resp.rsymlink.qid);
return (error);
}

static int
fs_mknod(void *softc, struct l9p_request *req)
{
struct l9p_fid *dir;
struct stat st;
uint32_t mode, major, minor;
dev_t dev;
gid_t gid;
char *name;
int error;

dir = req->lr_fid;
name = req->lr_req.tmknod.name;
mode = req->lr_req.tmknod.mode;
gid = req->lr_req.tmknod.gid;

switch (mode & S_IFMT) {
case S_IFBLK:
case S_IFCHR:
mode = (mode & S_IFMT) | (mode & 0777);
major = req->lr_req.tmknod.major;
minor = req->lr_req.tmknod.major;
dev = makedev(major, minor);
error = fs_imknod(softc, dir, name, false,
(mode_t)mode, dev, gid, &st);
break;

case S_IFIFO:
error = fs_imkfifo(softc, dir, name, false,
(mode_t)(mode & 0777), gid, &st);
break;

case S_IFSOCK:
error = fs_imksocket(softc, dir, name, false,
(mode_t)(mode & 0777), gid, &st);
break;

default:
error = EINVAL;
break;
}
if (error == 0)
generate_qid(&st, &req->lr_resp.rmknod.qid);
return (error);
}

static int
fs_rename(void *softc, struct l9p_request *req)
{
struct fs_softc *sc = softc;
struct fs_authinfo *ai;
struct l9p_acl *oparent_acl;
struct l9p_fid *fid, *f2;
struct fs_fid *file, *f2ff;
struct stat cst, opst, npst;
int32_t op;
bool reparenting;
char *tmp;
char olddir[MAXPATHLEN], newname[MAXPATHLEN];
int error;

if (sc->fs_readonly)
return (EROFS);








fid = req->lr_fid;
file = fid->lo_aux;
assert(file != NULL);
ai = file->ff_ai;

error = fs_pdir(sc, fid, olddir, sizeof(olddir), &opst);
if (error)
return (error);

f2 = req->lr_fid2;
f2ff = f2->lo_aux;
assert(f2ff != NULL);

reparenting = strcmp(olddir, f2ff->ff_name) != 0;

fillacl(file);
fillacl(f2ff);

if (fstatat(file->ff_dirfd, file->ff_name, &cst,
AT_SYMLINK_NOFOLLOW) != 0)
return (errno);





if (reparenting) {
oparent_acl = getacl(file, -1, olddir);
error = check_access(L9P_ACOP_UNLINK,
oparent_acl, &opst, file->ff_acl, &cst, ai, (gid_t)-1);
l9p_acl_free(oparent_acl);
if (error)
return (error);
}







if (fstatat(f2ff->ff_dirfd, f2ff->ff_name, &npst,
AT_SYMLINK_NOFOLLOW) != 0)
return (errno);

op = S_ISDIR(cst.st_mode) ? L9P_ACE_ADD_SUBDIRECTORY : L9P_ACE_ADD_FILE;
error = check_access(op, f2ff->ff_acl, &npst, NULL, NULL,
ai, (gid_t)-1);
if (error)
return (error);






if (strlcpy(newname, f2ff->ff_name, sizeof(newname)) >= sizeof(newname))
return (ENAMETOOLONG);
error = fs_dpf(newname, req->lr_req.trename.name, sizeof(newname));
if (error)
return (error);
tmp = strdup(newname);
if (tmp == NULL)
return (ENOMEM);

if (renameat(file->ff_dirfd, file->ff_name, file->ff_dirfd, tmp) != 0) {
error = errno;
free(tmp);
return (error);
}


free(file->ff_name);
file->ff_name = tmp;

dropacl(file);
return (0);
}

static int
fs_readlink(void *softc __unused, struct l9p_request *req)
{
struct fs_fid *file;
ssize_t linklen;
char buf[MAXPATHLEN];
int error = 0;

file = req->lr_fid->lo_aux;
assert(file);

linklen = readlinkat(file->ff_dirfd, file->ff_name, buf, sizeof(buf));
if (linklen < 0)
error = errno;
else if ((size_t)linklen >= sizeof(buf))
error = ENOMEM;
else if ((req->lr_resp.rreadlink.target = strndup(buf,
(size_t)linklen)) == NULL)
error = ENOMEM;
return (error);
}

static int
fs_getattr(void *softc __unused, struct l9p_request *req)
{
uint64_t mask, valid;
struct fs_fid *file;
struct stat st;
int error = 0;

file = req->lr_fid->lo_aux;
assert(file);

valid = 0;
if (fstatat(file->ff_dirfd, file->ff_name, &st, AT_SYMLINK_NOFOLLOW)) {
error = errno;
goto out;
}

mask = req->lr_req.tgetattr.request_mask;
if (mask & L9PL_GETATTR_MODE) {

req->lr_resp.rgetattr.mode = st.st_mode;
valid |= L9PL_GETATTR_MODE;
}
if (mask & L9PL_GETATTR_NLINK) {
req->lr_resp.rgetattr.nlink = st.st_nlink;
valid |= L9PL_GETATTR_NLINK;
}
if (mask & L9PL_GETATTR_UID) {

req->lr_resp.rgetattr.uid = st.st_uid;
valid |= L9PL_GETATTR_UID;
}
if (mask & L9PL_GETATTR_GID) {

req->lr_resp.rgetattr.gid = st.st_gid;
valid |= L9PL_GETATTR_GID;
}
if (mask & L9PL_GETATTR_RDEV) {

req->lr_resp.rgetattr.rdev = (uint64_t)st.st_rdev;
valid |= L9PL_GETATTR_RDEV;
}
if (mask & L9PL_GETATTR_ATIME) {
req->lr_resp.rgetattr.atime_sec =
(uint64_t)st.st_atimespec.tv_sec;
req->lr_resp.rgetattr.atime_nsec =
(uint64_t)st.st_atimespec.tv_nsec;
valid |= L9PL_GETATTR_ATIME;
}
if (mask & L9PL_GETATTR_MTIME) {
req->lr_resp.rgetattr.mtime_sec =
(uint64_t)st.st_mtimespec.tv_sec;
req->lr_resp.rgetattr.mtime_nsec =
(uint64_t)st.st_mtimespec.tv_nsec;
valid |= L9PL_GETATTR_MTIME;
}
if (mask & L9PL_GETATTR_CTIME) {
req->lr_resp.rgetattr.ctime_sec =
(uint64_t)st.st_ctimespec.tv_sec;
req->lr_resp.rgetattr.ctime_nsec =
(uint64_t)st.st_ctimespec.tv_nsec;
valid |= L9PL_GETATTR_CTIME;
}
if (mask & L9PL_GETATTR_BTIME) {
#if defined(HAVE_BIRTHTIME)
req->lr_resp.rgetattr.btime_sec =
(uint64_t)st.st_birthtim.tv_sec;
req->lr_resp.rgetattr.btime_nsec =
(uint64_t)st.st_birthtim.tv_nsec;
#else
req->lr_resp.rgetattr.btime_sec = 0;
req->lr_resp.rgetattr.btime_nsec = 0;
#endif
valid |= L9PL_GETATTR_BTIME;
}
if (mask & L9PL_GETATTR_INO)
valid |= L9PL_GETATTR_INO;
if (mask & L9PL_GETATTR_SIZE) {
req->lr_resp.rgetattr.size = (uint64_t)st.st_size;
valid |= L9PL_GETATTR_SIZE;
}
if (mask & L9PL_GETATTR_BLOCKS) {
req->lr_resp.rgetattr.blksize = (uint64_t)st.st_blksize;
req->lr_resp.rgetattr.blocks = (uint64_t)st.st_blocks;
valid |= L9PL_GETATTR_BLOCKS;
}
if (mask & L9PL_GETATTR_GEN) {
req->lr_resp.rgetattr.gen = st.st_gen;
valid |= L9PL_GETATTR_GEN;
}


generate_qid(&st, &req->lr_resp.rgetattr.qid);
out:
req->lr_resp.rgetattr.valid = valid;
return (error);
}




static int
fs_setattr(void *softc, struct l9p_request *req)
{
uint64_t mask;
struct fs_softc *sc = softc;
struct timespec ts[2];
struct fs_fid *file;
struct stat st;
int error = 0;
uid_t uid, gid;

file = req->lr_fid->lo_aux;
assert(file);

if (sc->fs_readonly)
return (EROFS);




mask = req->lr_req.tsetattr.valid;

if (fstatat(file->ff_dirfd, file->ff_name, &st, AT_SYMLINK_NOFOLLOW)) {
error = errno;
goto out;
}

if ((mask & L9PL_SETATTR_SIZE) && S_ISDIR(st.st_mode)) {
error = EISDIR;
goto out;
}

if (mask & L9PL_SETATTR_MODE) {
if (fchmodat(file->ff_dirfd, file->ff_name,
req->lr_req.tsetattr.mode & 0777,
AT_SYMLINK_NOFOLLOW)) {
error = errno;
goto out;
}
}

if (mask & (L9PL_SETATTR_UID | L9PL_SETATTR_GID)) {
uid = mask & L9PL_SETATTR_UID
? req->lr_req.tsetattr.uid
: (uid_t)-1;

gid = mask & L9PL_SETATTR_GID
? req->lr_req.tsetattr.gid
: (gid_t)-1;

if (fchownat(file->ff_dirfd, file->ff_name, uid, gid,
AT_SYMLINK_NOFOLLOW)) {
error = errno;
goto out;
}
}

if (mask & L9PL_SETATTR_SIZE) {

int fd = openat(file->ff_dirfd, file->ff_name, O_RDWR);
if (ftruncate(fd, (off_t)req->lr_req.tsetattr.size)) {
error = errno;
(void) close(fd);
goto out;
}
(void) close(fd);
}

if (mask & (L9PL_SETATTR_ATIME | L9PL_SETATTR_MTIME)) {
ts[0].tv_sec = st.st_atimespec.tv_sec;
ts[0].tv_nsec = st.st_atimespec.tv_nsec;
ts[1].tv_sec = st.st_mtimespec.tv_sec;
ts[1].tv_nsec = st.st_mtimespec.tv_nsec;

if (mask & L9PL_SETATTR_ATIME) {
if (mask & L9PL_SETATTR_ATIME_SET) {
ts[0].tv_sec = req->lr_req.tsetattr.atime_sec;
ts[0].tv_nsec = req->lr_req.tsetattr.atime_nsec;
} else {
if (clock_gettime(CLOCK_REALTIME, &ts[0]) != 0) {
error = errno;
goto out;
}
}
}

if (mask & L9PL_SETATTR_MTIME) {
if (mask & L9PL_SETATTR_MTIME_SET) {
ts[1].tv_sec = req->lr_req.tsetattr.mtime_sec;
ts[1].tv_nsec = req->lr_req.tsetattr.mtime_nsec;
} else {
if (clock_gettime(CLOCK_REALTIME, &ts[1]) != 0) {
error = errno;
goto out;
}
}
}

if (utimensat(file->ff_dirfd, file->ff_name, ts,
AT_SYMLINK_NOFOLLOW)) {
error = errno;
goto out;
}
}
out:
return (error);
}

static int
fs_xattrwalk(void *softc __unused, struct l9p_request *req __unused)
{
return (EOPNOTSUPP);
}

static int
fs_xattrcreate(void *softc __unused, struct l9p_request *req __unused)
{
return (EOPNOTSUPP);
}

static int
fs_readdir(void *softc __unused, struct l9p_request *req)
{
struct l9p_message msg;
struct l9p_dirent de;
struct fs_fid *file;
struct dirent *dp;
struct stat st;
uint32_t count;
int error = 0;

file = req->lr_fid->lo_aux;
assert(file);

if (file->ff_dir == NULL)
return (ENOTDIR);

pthread_mutex_lock(&file->ff_mtx);












if (req->lr_req.io.offset == 0)
rewinddir(file->ff_dir);
else
seekdir(file->ff_dir, (long)req->lr_req.io.offset);

l9p_init_msg(&msg, req, L9P_PACK);
count = (uint32_t)msg.lm_size;
while ((dp = readdir(file->ff_dir)) != NULL) {













if (fs_lstatat(file, dp->d_name, &st))
continue;

de.qid.type = 0;
generate_qid(&st, &de.qid);
de.offset = (uint64_t)telldir(file->ff_dir);
de.type = dp->d_type;
de.name = dp->d_name;


if (l9p_pudirent(&msg, &de) < 0)
break;
count = (uint32_t)msg.lm_size;
}

pthread_mutex_unlock(&file->ff_mtx);
req->lr_resp.io.count = count;
return (error);
}

static int
fs_fsync(void *softc __unused, struct l9p_request *req)
{
struct fs_fid *file;
int error = 0;

file = req->lr_fid->lo_aux;
assert(file);
if (fsync(file->ff_dir != NULL ? dirfd(file->ff_dir) : file->ff_fd))
error = errno;
return (error);
}

static int
fs_lock(void *softc __unused, struct l9p_request *req)
{

switch (req->lr_req.tlock.type) {
case L9PL_LOCK_TYPE_RDLOCK:
case L9PL_LOCK_TYPE_WRLOCK:
case L9PL_LOCK_TYPE_UNLOCK:
break;
default:
return (EINVAL);
}

req->lr_resp.rlock.status = L9PL_LOCK_SUCCESS;
return (0);
}

static int
fs_getlock(void *softc __unused, struct l9p_request *req)
{







switch (req->lr_req.getlock.type) {
case L9PL_LOCK_TYPE_RDLOCK:
case L9PL_LOCK_TYPE_WRLOCK:
case L9PL_LOCK_TYPE_UNLOCK:
break;
default:
return (EINVAL);
}

req->lr_resp.getlock = req->lr_req.getlock;
req->lr_resp.getlock.type = L9PL_LOCK_TYPE_UNLOCK;
req->lr_resp.getlock.client_id = strdup("");
return (0);
}

static int
fs_link(void *softc __unused, struct l9p_request *req)
{
struct l9p_fid *dir;
struct fs_fid *file;
struct fs_fid *dirf;
struct stat fst, tdst;
int32_t op;
char *name;
char newname[MAXPATHLEN];
int error;


dir = req->lr_fid2;
dirf = dir->lo_aux;
assert(dirf != NULL);

name = req->lr_req.tlink.name;
error = fs_buildname(dir, name, newname, sizeof(newname));
if (error)
return (error);

file = req->lr_fid->lo_aux;
assert(file != NULL);

if (fstatat(dirf->ff_dirfd, dirf->ff_name, &tdst, AT_SYMLINK_NOFOLLOW) != 0 ||
fstatat(file->ff_dirfd, file->ff_name, &fst, AT_SYMLINK_NOFOLLOW) != 0)
return (errno);
if (S_ISDIR(fst.st_mode))
return (EISDIR);
fillacl(dirf);
op = S_ISDIR(fst.st_mode) ? L9P_ACE_ADD_SUBDIRECTORY : L9P_ACE_ADD_FILE;
error = check_access(op,
dirf->ff_acl, &tdst, NULL, NULL, file->ff_ai, (gid_t)-1);
if (error)
return (error);

if (linkat(file->ff_dirfd, file->ff_name, file->ff_dirfd,
newname, 0) != 0)
error = errno;
else
dropacl(file);

return (error);
}

static int
fs_mkdir(void *softc, struct l9p_request *req)
{
struct l9p_fid *dir;
struct stat st;
mode_t perm;
gid_t gid;
char *name;
int error;

dir = req->lr_fid;
name = req->lr_req.tmkdir.name;
perm = (mode_t)req->lr_req.tmkdir.mode;
gid = req->lr_req.tmkdir.gid;

error = fs_imkdir(softc, dir, name, false, perm, gid, &st);
if (error == 0)
generate_qid(&st, &req->lr_resp.rmkdir.qid);
return (error);
}

static int
fs_renameat(void *softc, struct l9p_request *req)
{
struct fs_softc *sc = softc;
struct l9p_fid *olddir, *newdir;
struct l9p_acl *facl;
struct fs_fid *off, *nff;
struct stat odst, ndst, fst;
int32_t op;
bool reparenting;
char *onp, *nnp;
char onb[MAXPATHLEN], nnb[MAXPATHLEN];
int error;

if (sc->fs_readonly)
return (EROFS);

olddir = req->lr_fid;
newdir = req->lr_fid2;
assert(olddir != NULL && newdir != NULL);
off = olddir->lo_aux;
nff = newdir->lo_aux;
assert(off != NULL && nff != NULL);

onp = req->lr_req.trenameat.oldname;
nnp = req->lr_req.trenameat.newname;
error = fs_buildname(olddir, onp, onb, sizeof(onb));
if (error)
return (error);
error = fs_buildname(newdir, nnp, nnb, sizeof(nnb));
if (error)
return (error);
if (fstatat(off->ff_dirfd, onb, &fst, AT_SYMLINK_NOFOLLOW) != 0)
return (errno);

reparenting = olddir != newdir &&
strcmp(off->ff_name, nff->ff_name) != 0;

if (fstatat(off->ff_dirfd, off->ff_name, &odst, AT_SYMLINK_NOFOLLOW) != 0)
return (errno);
if (!S_ISDIR(odst.st_mode))
return (ENOTDIR);
fillacl(off);

if (reparenting) {
if (fstatat(nff->ff_dirfd, nff->ff_name, &ndst, AT_SYMLINK_NOFOLLOW) != 0)
return (errno);
if (!S_ISDIR(ndst.st_mode))
return (ENOTDIR);
facl = getacl(off, -1, onb);
fillacl(nff);

error = check_access(L9P_ACOP_UNLINK,
off->ff_acl, &odst, facl, &fst, off->ff_ai, (gid_t)-1);
l9p_acl_free(facl);
if (error)
return (error);
op = S_ISDIR(fst.st_mode) ? L9P_ACE_ADD_SUBDIRECTORY :
L9P_ACE_ADD_FILE;
error = check_access(op,
nff->ff_acl, &ndst, NULL, NULL, nff->ff_ai, (gid_t)-1);
if (error)
return (error);
}

if (renameat(off->ff_dirfd, onb, nff->ff_dirfd, nnb))
error = errno;

return (error);
}





static int
fs_unlinkat(void *softc, struct l9p_request *req)
{
struct fs_softc *sc = softc;
struct l9p_acl *facl;
struct l9p_fid *dir;
struct fs_fid *dirff;
struct stat dirst, fst;
char *name;
char newname[MAXPATHLEN];
int error;

if (sc->fs_readonly)
return (EROFS);

dir = req->lr_fid;
dirff = dir->lo_aux;
assert(dirff != NULL);
name = req->lr_req.tunlinkat.name;
error = fs_buildname(dir, name, newname, sizeof(newname));
if (error)
return (error);
if (fstatat(dirff->ff_dirfd, newname, &fst, AT_SYMLINK_NOFOLLOW) != 0 ||
fstatat(dirff->ff_dirfd, dirff->ff_name, &dirst, AT_SYMLINK_NOFOLLOW) != 0)
return (errno);
fillacl(dirff);
facl = getacl(dirff, -1, newname);
error = check_access(L9P_ACOP_UNLINK,
dirff->ff_acl, &dirst, facl, &fst, dirff->ff_ai, (gid_t)-1);
l9p_acl_free(facl);
if (error)
return (error);

if (req->lr_req.tunlinkat.flags & L9PL_AT_REMOVEDIR) {
if (unlinkat(dirff->ff_dirfd, newname, AT_REMOVEDIR) != 0)
error = errno;
} else {
if (unlinkat(dirff->ff_dirfd, newname, 0) != 0)
error = errno;
}
return (error);
}

static void
fs_freefid(void *softc __unused, struct l9p_fid *fid)
{
struct fs_fid *f = fid->lo_aux;
struct fs_authinfo *ai;
uint32_t newcount;

if (f == NULL) {

return;
}

if (f->ff_fd != -1)
close(f->ff_fd);

if (f->ff_dir)
closedir(f->ff_dir);

pthread_mutex_destroy(&f->ff_mtx);
free(f->ff_name);
ai = f->ff_ai;
l9p_acl_free(f->ff_acl);
free(f);
pthread_mutex_lock(&ai->ai_mtx);
newcount = --ai->ai_refcnt;
pthread_mutex_unlock(&ai->ai_mtx);
if (newcount == 0) {



L9P_LOG(L9P_DEBUG, "dropped last ref to authinfo %p",
(void *)ai);
pthread_mutex_destroy(&ai->ai_mtx);
free(ai);
} else {
L9P_LOG(L9P_DEBUG, "authinfo %p now used by %lu",
(void *)ai, (u_long)newcount);
}
}

int
l9p_backend_fs_init(struct l9p_backend **backendp, int rootfd, bool ro)
{
struct l9p_backend *backend;
struct fs_softc *sc;
int error;
#if defined(WITH_CASPER)
cap_channel_t *capcas;
#endif

if (!fs_attach_mutex_inited) {
error = pthread_mutex_init(&fs_attach_mutex, NULL);
if (error) {
errno = error;
return (-1);
}
fs_attach_mutex_inited = true;
}

backend = l9p_malloc(sizeof(*backend));
backend->attach = fs_attach;
backend->clunk = fs_clunk;
backend->create = fs_create;
backend->open = fs_open;
backend->read = fs_read;
backend->remove = fs_remove;
backend->stat = fs_stat;
backend->walk = fs_walk;
backend->write = fs_write;
backend->wstat = fs_wstat;
backend->statfs = fs_statfs;
backend->lopen = fs_lopen;
backend->lcreate = fs_lcreate;
backend->symlink = fs_symlink;
backend->mknod = fs_mknod;
backend->rename = fs_rename;
backend->readlink = fs_readlink;
backend->getattr = fs_getattr;
backend->setattr = fs_setattr;
backend->xattrwalk = fs_xattrwalk;
backend->xattrcreate = fs_xattrcreate;
backend->readdir = fs_readdir;
backend->fsync = fs_fsync;
backend->lock = fs_lock;
backend->getlock = fs_getlock;
backend->link = fs_link;
backend->mkdir = fs_mkdir;
backend->renameat = fs_renameat;
backend->unlinkat = fs_unlinkat;
backend->freefid = fs_freefid;

sc = l9p_malloc(sizeof(*sc));
sc->fs_rootfd = rootfd;
sc->fs_readonly = ro;
backend->softc = sc;

#if defined(WITH_CASPER)
capcas = cap_init();
if (capcas == NULL)
return (-1);

sc->fs_cappwd = cap_service_open(capcas, "system.pwd");
if (sc->fs_cappwd == NULL)
return (-1);

sc->fs_capgrp = cap_service_open(capcas, "system.grp");
if (sc->fs_capgrp == NULL)
return (-1);

cap_setpassent(sc->fs_cappwd, 1);
cap_setgroupent(sc->fs_capgrp, 1);
cap_close(capcas);
#else
setpassent(1);
#endif

*backendp = backend;
return (0);
}
