






























#define _KERNTYPES

#include "filemon.h"

#include <sys/param.h>
#include <sys/types.h>
#include <sys/rbtree.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/wait.h>

#include <sys/ktrace.h>

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if !defined(AT_CWD)
#define AT_CWD -1
#endif

struct filemon;
struct filemon_key;
struct filemon_state;

typedef struct filemon_state *filemon_syscall_t(struct filemon *,
const struct filemon_key *, const struct ktr_syscall *);

static filemon_syscall_t filemon_sys_chdir;
static filemon_syscall_t filemon_sys_execve;
static filemon_syscall_t filemon_sys_exit;
static filemon_syscall_t filemon_sys_fork;
static filemon_syscall_t filemon_sys_link;
static filemon_syscall_t filemon_sys_open;
static filemon_syscall_t filemon_sys_openat;
static filemon_syscall_t filemon_sys_symlink;
static filemon_syscall_t filemon_sys_unlink;
static filemon_syscall_t filemon_sys_rename;

static filemon_syscall_t *const filemon_syscalls[] = {
[SYS_chdir] = &filemon_sys_chdir,
[SYS_execve] = &filemon_sys_execve,
[SYS_exit] = &filemon_sys_exit,
[SYS_fork] = &filemon_sys_fork,
[SYS_link] = &filemon_sys_link,
[SYS_open] = &filemon_sys_open,
[SYS_openat] = &filemon_sys_openat,
[SYS_symlink] = &filemon_sys_symlink,
[SYS_unlink] = &filemon_sys_unlink,
[SYS_rename] = &filemon_sys_rename,
};

struct filemon {
int ktrfd;
FILE *in;
FILE *out;
rb_tree_t active;
pid_t child;


enum {
FILEMON_START = 0,
FILEMON_HEADER,
FILEMON_PAYLOAD,
FILEMON_ERROR,
} state;
unsigned char *p;
size_t resid;


struct ktr_header hdr;
union {
struct ktr_syscall syscall;
struct ktr_sysret sysret;
char namei[PATH_MAX];
unsigned char buf[4096];
} payload;
};

struct filemon_state {
struct filemon_key {
pid_t pid;
lwpid_t lid;
} key;
struct rb_node node;
int syscode;
void (*show)(struct filemon *, const struct filemon_state *,
const struct ktr_sysret *);
unsigned i;
unsigned npath;
char *path[];
};


static int
compare_filemon_states(void *cookie, const void *na, const void *nb)
{
const struct filemon_state *Sa = na;
const struct filemon_state *Sb = nb;

if (Sa->key.pid < Sb->key.pid)
return -1;
if (Sa->key.pid > Sb->key.pid)
return +1;
if (Sa->key.lid < Sb->key.lid)
return -1;
if (Sa->key.lid > Sb->key.lid)
return +1;
return 0;
}


static int
compare_filemon_key(void *cookie, const void *n, const void *k)
{
const struct filemon_state *S = n;
const struct filemon_key *key = k;

if (S->key.pid < key->pid)
return -1;
if (S->key.pid > key->pid)
return +1;
if (S->key.lid < key->lid)
return -1;
if (S->key.lid > key->lid)
return +1;
return 0;
}

static const rb_tree_ops_t filemon_rb_ops = {
.rbto_compare_nodes = &compare_filemon_states,
.rbto_compare_key = &compare_filemon_key,
.rbto_node_offset = offsetof(struct filemon_state, node),
.rbto_context = NULL,
};







const char *
filemon_path(void)
{

return "ktrace";
}







struct filemon *
filemon_open(void)
{
struct filemon *F;
int ktrpipe[2];
int error;


F = calloc(1, sizeof *F);
if (F == NULL)
return NULL;


if (pipe2(ktrpipe, O_CLOEXEC|O_NONBLOCK) == -1) {
error = errno;
goto fail0;
}


if ((F->in = fdopen(ktrpipe[0], "r")) == NULL) {
error = errno;
goto fail1;
}
ktrpipe[0] = -1;





F->ktrfd = ktrpipe[1];
rb_tree_init(&F->active, &filemon_rb_ops);


return F;

fail1: (void)close(ktrpipe[0]);
(void)close(ktrpipe[1]);
fail0: free(F);
errno = error;
return NULL;
}









static int
filemon_closefd(struct filemon *F)
{
int error = 0;


if (F->out == NULL)
return 0;





if (fflush(F->out) == EOF && error == 0)
error = errno;
if (fclose(F->out) == EOF && error == 0)
error = errno;
F->out = NULL;


if (error != 0) {
errno = error;
return -1;
}


return 0;
}








int
filemon_setfd(struct filemon *F, int fd)
{





if ((filemon_closefd(F)) == -1)
return -1;
assert(F->out == NULL);


if ((F->out = fdopen(fd, "a")) == NULL)
return -1;






fprintf(F->out, "#filemon version 4\n");
fprintf(F->out, "#Target pid %jd\n", (intmax_t)getpid());
fprintf(F->out, "V 4\n");


return 0;
}






void
filemon_setpid_parent(struct filemon *F, pid_t pid)
{

F->child = pid;
}







int
filemon_setpid_child(const struct filemon *F, pid_t pid)
{
int ops, trpoints;

ops = KTROP_SET|KTRFLAG_DESCEND;
trpoints = KTRFACv2;
trpoints |= KTRFAC_SYSCALL|KTRFAC_NAMEI|KTRFAC_SYSRET;
trpoints |= KTRFAC_INHERIT;
if (fktrace(F->ktrfd, ops, trpoints, pid) == -1)
return -1;

return 0;
}








int
filemon_close(struct filemon *F)
{
struct filemon_state *S;
int error = 0;


if (filemon_closefd(F) == -1 && error == 0)
error = errno;


if (fclose(F->in) == EOF && error == 0)
error = errno;
if (close(F->ktrfd) == -1 && error == 0)
error = errno;


while ((S = RB_TREE_MIN(&F->active)) != NULL) {
rb_tree_remove_node(&F->active, S);
free(S);
}


free(F);


if (error != 0) {
errno = error;
return -1;
}


return 0;
}








int
filemon_readfd(const struct filemon *F)
{

if (F->state == FILEMON_ERROR)
return -1;
return fileno(F->in);
}







static void
filemon_dispatch(struct filemon *F)
{
const struct filemon_key key = {
.pid = F->hdr.ktr_pid,
.lid = F->hdr.ktr_lid,
};
struct filemon_state *S;

switch (F->hdr.ktr_type) {
case KTR_SYSCALL: {
struct ktr_syscall *call = &F->payload.syscall;
struct filemon_state *S1;


if (call->ktr_code < 0 ||
(size_t)call->ktr_code >= __arraycount(filemon_syscalls) ||
filemon_syscalls[call->ktr_code] == NULL)
break;





S = (*filemon_syscalls[call->ktr_code])(F, &key, call);
if (S == NULL)
break;










S1 = rb_tree_insert_node(&F->active, S);
if (S1 != S) {

free(S);
break;
}
break;
}
case KTR_NAMEI:

S = rb_tree_find_node(&F->active, &key);
if (S == NULL)
break;

if (S->i >= S->npath)
break;

S->path[S->i++] = strndup(F->payload.namei,
sizeof F->payload.namei);
break;
case KTR_SYSRET: {
struct ktr_sysret *ret = &F->payload.sysret;
unsigned i;


S = rb_tree_find_node(&F->active, &key);
if (S == NULL)
break;
rb_tree_remove_node(&F->active, S);







if (S->i == S->npath && S->syscode == ret->ktr_code)
S->show(F, S, ret);


for (i = 0; i < S->i; i++)
free(S->path[i]);
free(S);
break;
}
default:

break;
}
}
















int
filemon_process(struct filemon *F)
{
size_t nread;

top:

if (F->child == 0)
return 0;


if (F->resid > 0) {
nread = fread(F->p, 1, F->resid, F->in);
if (nread == 0) {
if (feof(F->in) != 0)
return 0;
assert(ferror(F->in) != 0);




if (errno == EAGAIN || errno == EINTR)
return 1;
F->state = FILEMON_ERROR;
F->p = NULL;
F->resid = 0;
return -1;
}
assert(nread <= F->resid);
F->p += nread;
F->resid -= nread;
if (F->resid > 0)
return 1;
}


switch (F->state) {
case FILEMON_START:
F->state = FILEMON_HEADER;
F->p = (void *)&F->hdr;
F->resid = sizeof F->hdr;
goto top;
case FILEMON_HEADER:

if (F->hdr.ktr_len < 0 ||
(size_t)F->hdr.ktr_len > sizeof F->payload) {
F->state = FILEMON_ERROR;
F->p = NULL;
F->resid = 0;
errno = EIO;
return -1;
}
F->state = FILEMON_PAYLOAD;
F->p = (void *)&F->payload;
F->resid = (size_t)F->hdr.ktr_len;
goto top;
case FILEMON_PAYLOAD:

filemon_dispatch(F);
F->state = FILEMON_HEADER;
F->p = (void *)&F->hdr;
F->resid = sizeof F->hdr;
goto top;
default:
F->state = FILEMON_ERROR;

case FILEMON_ERROR:
F->p = NULL;
F->resid = 0;
errno = EIO;
return -1;
}
}

static struct filemon_state *
syscall_enter(
const struct filemon_key *key, const struct ktr_syscall *call,
unsigned npath,
void (*show)(struct filemon *, const struct filemon_state *,
const struct ktr_sysret *))
{
struct filemon_state *S;
unsigned i;

S = calloc(1, offsetof(struct filemon_state, path[npath]));
if (S == NULL)
return NULL;
S->key = *key;
S->show = show;
S->syscode = call->ktr_code;
S->i = 0;
S->npath = npath;
for (i = 0; i < npath; i++)
S->path[i] = NULL;

return S;
}

static void
show_paths(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret, const char *prefix)
{
unsigned i;


assert(S->i == S->npath);





if (ret->ktr_error != 0 && ret->ktr_error != -2)
return;
if (F->out == NULL)
return;





fprintf(F->out, "%s %jd", prefix, (intmax_t)S->key.pid);
for (i = 0; i < S->npath; i++) {
const char *q = S->npath > 1 ? "'" : "";
fprintf(F->out, " %s%s%s", q, S->path[i], q);
}
fprintf(F->out, "\n");
}

static void
show_retval(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret, const char *prefix)
{





if (ret->ktr_error != 0 && ret->ktr_error != -2)
return;
if (F->out == NULL)
return;

fprintf(F->out, "%s %jd %jd\n", prefix, (intmax_t)S->key.pid,
(intmax_t)ret->ktr_retval);
}

static void
show_chdir(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
show_paths(F, S, ret, "C");
}

static void
show_execve(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
show_paths(F, S, ret, "E");
}

static void
show_fork(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
show_retval(F, S, ret, "F");
}

static void
show_link(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
show_paths(F, S, ret, "L");
}

static void
show_open_read(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
show_paths(F, S, ret, "R");
}

static void
show_open_write(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
show_paths(F, S, ret, "W");
}

static void
show_open_readwrite(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
show_paths(F, S, ret, "R");
show_paths(F, S, ret, "W");
}

static void
show_openat_read(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
if (S->path[0][0] != '/')
show_paths(F, S, ret, "A");
show_paths(F, S, ret, "R");
}

static void
show_openat_write(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
if (S->path[0][0] != '/')
show_paths(F, S, ret, "A");
show_paths(F, S, ret, "W");
}

static void
show_openat_readwrite(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
if (S->path[0][0] != '/')
show_paths(F, S, ret, "A");
show_paths(F, S, ret, "R");
show_paths(F, S, ret, "W");
}

static void
show_symlink(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
show_paths(F, S, ret, "L");
}

static void
show_unlink(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
show_paths(F, S, ret, "D");
}

static void
show_rename(struct filemon *F, const struct filemon_state *S,
const struct ktr_sysret *ret)
{
show_paths(F, S, ret, "M");
}


static struct filemon_state *
filemon_sys_chdir(struct filemon *F, const struct filemon_key *key,
const struct ktr_syscall *call)
{
return syscall_enter(key, call, 1, &show_chdir);
}




static struct filemon_state *
filemon_sys_execve(struct filemon *F, const struct filemon_key *key,
const struct ktr_syscall *call)
{
return syscall_enter(key, call, 1, &show_execve);
}

static struct filemon_state *
filemon_sys_exit(struct filemon *F, const struct filemon_key *key,
const struct ktr_syscall *call)
{
const register_t *args = (const void *)&call[1];
int status = (int)args[0];

if (F->out != NULL) {
fprintf(F->out, "X %jd %d\n", (intmax_t)key->pid, status);
if (key->pid == F->child) {
fprintf(F->out, "#Bye bye\n");
F->child = 0;
}
}
return NULL;
}


static struct filemon_state *
filemon_sys_fork(struct filemon *F, const struct filemon_key *key,
const struct ktr_syscall *call)
{
return syscall_enter(key, call, 0, &show_fork);
}


static struct filemon_state *
filemon_sys_link(struct filemon *F, const struct filemon_key *key,
const struct ktr_syscall *call)
{
return syscall_enter(key, call, 2, &show_link);
}


static struct filemon_state *
filemon_sys_open(struct filemon *F, const struct filemon_key *key,
const struct ktr_syscall *call)
{
const register_t *args = (const void *)&call[1];
int flags;

if (call->ktr_argsize < 2)
return NULL;
flags = (int)args[1];

if ((flags & O_RDWR) == O_RDWR)
return syscall_enter(key, call, 1, &show_open_readwrite);
else if ((flags & O_WRONLY) == O_WRONLY)
return syscall_enter(key, call, 1, &show_open_write);
else if ((flags & O_RDONLY) == O_RDONLY)
return syscall_enter(key, call, 1, &show_open_read);
else
return NULL;
}


static struct filemon_state *
filemon_sys_openat(struct filemon *F, const struct filemon_key *key,
const struct ktr_syscall *call)
{
const register_t *args = (const void *)&call[1];
int flags, fd;






if (call->ktr_argsize < 3)
return NULL;
fd = (int)args[0];
flags = (int)args[2];

if (fd == AT_CWD) {
if ((flags & O_RDWR) == O_RDWR)
return syscall_enter(key, call, 1,
&show_open_readwrite);
else if ((flags & O_WRONLY) == O_WRONLY)
return syscall_enter(key, call, 1, &show_open_write);
else if ((flags & O_RDONLY) == O_RDONLY)
return syscall_enter(key, call, 1, &show_open_read);
else
return NULL;
} else {
if ((flags & O_RDWR) == O_RDWR)
return syscall_enter(key, call, 1,
&show_openat_readwrite);
else if ((flags & O_WRONLY) == O_WRONLY)
return syscall_enter(key, call, 1, &show_openat_write);
else if ((flags & O_RDONLY) == O_RDONLY)
return syscall_enter(key, call, 1, &show_openat_read);
else
return NULL;
}
}




static struct filemon_state *
filemon_sys_symlink(struct filemon *F, const struct filemon_key *key,
const struct ktr_syscall *call)
{
return syscall_enter(key, call, 2, &show_symlink);
}


static struct filemon_state *
filemon_sys_unlink(struct filemon *F, const struct filemon_key *key,
const struct ktr_syscall *call)
{
return syscall_enter(key, call, 1, &show_unlink);
}


static struct filemon_state *
filemon_sys_rename(struct filemon *F, const struct filemon_key *key,
const struct ktr_syscall *call)
{
return syscall_enter(key, call, 2, &show_rename);
}
