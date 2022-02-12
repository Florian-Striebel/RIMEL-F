


























#if defined(WIN32)
#include <windows.h>
#include <shlwapi.h>
#endif

#include "file.h"

#if !defined(lint)
FILE_RCSID("@(#)$File: magic.c,v 1.115 2021/09/20 17:45:41 christos Exp $")
#endif

#include "magic.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#if defined(QUICK)
#include <sys/mman.h>
#endif
#include <limits.h>

#if defined(HAVE_UTIMES)
#include <sys/time.h>
#elif defined(HAVE_UTIME)
#if defined(HAVE_SYS_UTIME_H)
#include <sys/utime.h>
#elif defined(HAVE_UTIME_H)
#include <utime.h>
#endif
#endif

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if !defined(PIPE_BUF)

#if defined(_PC_PIPE_BUF)
#define PIPE_BUF pathconf(".", _PC_PIPE_BUF)
#else
#define PIPE_BUF 512
#endif
#endif

private void close_and_restore(const struct magic_set *, const char *, int,
const struct stat *);
private int unreadable_info(struct magic_set *, mode_t, const char *);
private const char* get_default_magic(void);
#if !defined(COMPILE_ONLY)
private const char *file_or_fd(struct magic_set *, const char *, int);
#endif

#if !defined(STDIN_FILENO)
#define STDIN_FILENO 0
#endif

#if defined(WIN32)

static HINSTANCE _w32_dll_instance = NULL;

static void
_w32_append_path(char **hmagicpath, const char *fmt, ...)
{
char *tmppath;
char *newpath;
va_list ap;

va_start(ap, fmt);
if (vasprintf(&tmppath, fmt, ap) < 0) {
va_end(ap);
return;
}
va_end(ap);

if (access(tmppath, R_OK) == -1)
goto out;

if (*hmagicpath == NULL) {
*hmagicpath = tmppath;
return;
}

if (asprintf(&newpath, "%s%c%s", *hmagicpath, PATHSEP, tmppath) < 0)
goto out;

free(*hmagicpath);
free(tmppath);
*hmagicpath = newpath;
return;
out:
free(tmppath);
}

static void
_w32_get_magic_relative_to(char **hmagicpath, HINSTANCE module)
{
static const char *trypaths[] = {
"%s/share/misc/magic.mgc",
"%s/magic.mgc",
};
LPSTR dllpath;
size_t sp;

dllpath = calloc(MAX_PATH + 1, sizeof(*dllpath));

if (!GetModuleFileNameA(module, dllpath, MAX_PATH))
goto out;

PathRemoveFileSpecA(dllpath);

if (module) {
char exepath[MAX_PATH];
GetModuleFileNameA(NULL, exepath, MAX_PATH);
PathRemoveFileSpecA(exepath);
if (stricmp(exepath, dllpath) == 0)
goto out;
}

sp = strlen(dllpath);
if (sp > 3 && stricmp(&dllpath[sp - 3], "bin") == 0) {
_w32_append_path(hmagicpath,
"%s/../share/misc/magic.mgc", dllpath);
goto out;
}

for (sp = 0; sp < __arraycount(trypaths); sp++)
_w32_append_path(hmagicpath, trypaths[sp], dllpath);
out:
free(dllpath);
}

#if !defined(BUILD_AS_WINDOWS_STATIC_LIBARAY)

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID);

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason,
LPVOID lpvReserved __attribute__((__unused__)))
{
if (fdwReason == DLL_PROCESS_ATTACH)
_w32_dll_instance = hinstDLL;
return 1;
}
#endif
#endif

private const char *
get_default_magic(void)
{
static const char hmagic[] = "/.magic/magic.mgc";
static char *default_magic;
char *home, *hmagicpath;

#if !defined(WIN32)
struct stat st;

if (default_magic) {
free(default_magic);
default_magic = NULL;
}
if ((home = getenv("HOME")) == NULL)
return MAGIC;

if (asprintf(&hmagicpath, "%s/.magic.mgc", home) < 0)
return MAGIC;
if (stat(hmagicpath, &st) == -1) {
free(hmagicpath);
if (asprintf(&hmagicpath, "%s/.magic", home) < 0)
return MAGIC;
if (stat(hmagicpath, &st) == -1)
goto out;
if (S_ISDIR(st.st_mode)) {
free(hmagicpath);
if (asprintf(&hmagicpath, "%s/%s", home, hmagic) < 0)
return MAGIC;
if (access(hmagicpath, R_OK) == -1)
goto out;
}
}

if (asprintf(&default_magic, "%s:%s", hmagicpath, MAGIC) < 0)
goto out;
free(hmagicpath);
return default_magic;
out:
default_magic = NULL;
free(hmagicpath);
return MAGIC;
#else
hmagicpath = NULL;

if (default_magic) {
free(default_magic);
default_magic = NULL;
}


if ((home = getenv("LOCALAPPDATA")) != NULL)
_w32_append_path(&hmagicpath, "%s%s", home, hmagic);


if ((home = getenv("USERPROFILE")) != NULL)
_w32_append_path(&hmagicpath,
"%s/Local Settings/Application Data%s", home, hmagic);


if ((home = getenv("COMMONPROGRAMFILES")) != NULL)
_w32_append_path(&hmagicpath, "%s%s", home, hmagic);


_w32_get_magic_relative_to(&hmagicpath, NULL);


_w32_get_magic_relative_to(&hmagicpath, _w32_dll_instance);


default_magic = hmagicpath;
return default_magic;
#endif
}

public const char *
magic_getpath(const char *magicfile, int action)
{
if (magicfile != NULL)
return magicfile;

magicfile = getenv("MAGIC");
if (magicfile != NULL)
return magicfile;

return action == FILE_LOAD ? get_default_magic() : MAGIC;
}

public struct magic_set *
magic_open(int flags)
{
return file_ms_alloc(flags);
}

private int
unreadable_info(struct magic_set *ms, mode_t md, const char *file)
{
if (file) {

if (access(file, W_OK) == 0)
if (file_printf(ms, "writable, ") == -1)
return -1;
if (access(file, X_OK) == 0)
if (file_printf(ms, "executable, ") == -1)
return -1;
}
if (S_ISREG(md))
if (file_printf(ms, "regular file, ") == -1)
return -1;
if (file_printf(ms, "no read permission") == -1)
return -1;
return 0;
}

public void
magic_close(struct magic_set *ms)
{
if (ms == NULL)
return;
file_ms_free(ms);
}




public int
magic_load(struct magic_set *ms, const char *magicfile)
{
if (ms == NULL)
return -1;
return file_apprentice(ms, magicfile, FILE_LOAD);
}

#if !defined(COMPILE_ONLY)



public int
magic_load_buffers(struct magic_set *ms, void **bufs, size_t *sizes,
size_t nbufs)
{
if (ms == NULL)
return -1;
return buffer_apprentice(ms, RCAST(struct magic **, bufs),
sizes, nbufs);
}
#endif

public int
magic_compile(struct magic_set *ms, const char *magicfile)
{
if (ms == NULL)
return -1;
return file_apprentice(ms, magicfile, FILE_COMPILE);
}

public int
magic_check(struct magic_set *ms, const char *magicfile)
{
if (ms == NULL)
return -1;
return file_apprentice(ms, magicfile, FILE_CHECK);
}

public int
magic_list(struct magic_set *ms, const char *magicfile)
{
if (ms == NULL)
return -1;
return file_apprentice(ms, magicfile, FILE_LIST);
}

private void
close_and_restore(const struct magic_set *ms, const char *name, int fd,
const struct stat *sb)
{
if (fd == STDIN_FILENO || name == NULL)
return;
(void) close(fd);

if ((ms->flags & MAGIC_PRESERVE_ATIME) != 0) {






#if defined(HAVE_UTIMES)
struct timeval utsbuf[2];
(void)memset(utsbuf, 0, sizeof(utsbuf));
utsbuf[0].tv_sec = sb->st_atime;
utsbuf[1].tv_sec = sb->st_mtime;

(void) utimes(name, utsbuf);
#elif defined(HAVE_UTIME_H) || defined(HAVE_SYS_UTIME_H)
struct utimbuf utbuf;

(void)memset(&utbuf, 0, sizeof(utbuf));
utbuf.actime = sb->st_atime;
utbuf.modtime = sb->st_mtime;
(void) utime(name, &utbuf);
#endif
}
}

#if !defined(COMPILE_ONLY)




public const char *
magic_descriptor(struct magic_set *ms, int fd)
{
if (ms == NULL)
return NULL;
return file_or_fd(ms, NULL, fd);
}




public const char *
magic_file(struct magic_set *ms, const char *inname)
{
if (ms == NULL)
return NULL;
return file_or_fd(ms, inname, STDIN_FILENO);
}

private const char *
file_or_fd(struct magic_set *ms, const char *inname, int fd)
{
int rv = -1;
unsigned char *buf;
struct stat sb;
ssize_t nbytes = 0;
int ispipe = 0;
int okstat = 0;
off_t pos = CAST(off_t, -1);

if (file_reset(ms, 1) == -1)
goto out;





#define SLOP (1 + sizeof(union VALUETYPE))
if ((buf = CAST(unsigned char *, malloc(ms->bytes_max + SLOP))) == NULL)
return NULL;

switch (file_fsmagic(ms, inname, &sb)) {
case -1:
goto done;
case 0:
break;
default:
rv = 0;
goto done;
}

#if defined(WIN32)

if (fd == STDIN_FILENO)
_setmode(STDIN_FILENO, O_BINARY);
#endif
if (inname != NULL) {
int flags = O_RDONLY|O_BINARY|O_NONBLOCK|O_CLOEXEC;
errno = 0;
if ((fd = open(inname, flags)) < 0) {
okstat = stat(inname, &sb) == 0;
if (okstat && S_ISFIFO(sb.st_mode))
ispipe = 1;
#if defined(WIN32)






if (!okstat && errno == EACCES) {
sb.st_mode = S_IFBLK;
okstat = 1;
}
#endif
if (okstat &&
unreadable_info(ms, sb.st_mode, inname) == -1)
goto done;
rv = 0;
goto done;
}
#if O_CLOEXEC == 0
(void)fcntl(fd, F_SETFD, FD_CLOEXEC);
#endif
}

if (fd != -1) {
okstat = fstat(fd, &sb) == 0;
if (okstat && S_ISFIFO(sb.st_mode))
ispipe = 1;
if (inname == NULL)
pos = lseek(fd, CAST(off_t, 0), SEEK_CUR);
}




if (ispipe) {
if (fd != -1) {
ssize_t r = 0;

while ((r = sread(fd, RCAST(void *, &buf[nbytes]),
CAST(size_t, ms->bytes_max - nbytes), 1)) > 0) {
nbytes += r;
if (r < PIPE_BUF) break;
}
}

if (nbytes == 0 && inname) {

if (unreadable_info(ms, sb.st_mode, inname) == -1)
goto done;
rv = 0;
goto done;
}

} else if (fd != -1) {

size_t howmany =
#if defined(WIN32)
_isatty(fd) ? 8 * 1024 :
#endif
ms->bytes_max;
if ((nbytes = read(fd, RCAST(void *, buf), howmany)) == -1) {
if (inname == NULL && fd != STDIN_FILENO)
file_error(ms, errno, "cannot read fd %d", fd);
else
file_error(ms, errno, "cannot read `%s'",
inname == NULL ? "/dev/stdin" : inname);
goto done;
}
}

(void)memset(buf + nbytes, 0, SLOP);
if (file_buffer(ms, fd, okstat ? &sb : NULL, inname, buf, CAST(size_t, nbytes)) == -1)
goto done;
rv = 0;
done:
free(buf);
if (fd != -1) {
if (pos != CAST(off_t, -1))
(void)lseek(fd, pos, SEEK_SET);
close_and_restore(ms, inname, fd, &sb);
}
out:
return rv == 0 ? file_getbuffer(ms) : NULL;
}


public const char *
magic_buffer(struct magic_set *ms, const void *buf, size_t nb)
{
if (ms == NULL)
return NULL;
if (file_reset(ms, 1) == -1)
return NULL;




if (file_buffer(ms, -1, NULL, NULL, buf, nb) == -1) {
return NULL;
}
return file_getbuffer(ms);
}
#endif

public const char *
magic_error(struct magic_set *ms)
{
if (ms == NULL)
return "Magic database is not open";
return (ms->event_flags & EVENT_HAD_ERR) ? ms->o.buf : NULL;
}

public int
magic_errno(struct magic_set *ms)
{
if (ms == NULL)
return EINVAL;
return (ms->event_flags & EVENT_HAD_ERR) ? ms->error : 0;
}

public int
magic_getflags(struct magic_set *ms)
{
if (ms == NULL)
return -1;

return ms->flags;
}

public int
magic_setflags(struct magic_set *ms, int flags)
{
if (ms == NULL)
return -1;
#if !defined(HAVE_UTIME) && !defined(HAVE_UTIMES)
if (flags & MAGIC_PRESERVE_ATIME)
return -1;
#endif
ms->flags = flags;
return 0;
}

public int
magic_version(void)
{
return MAGIC_VERSION;
}

public int
magic_setparam(struct magic_set *ms, int param, const void *val)
{
if (ms == NULL)
return -1;
switch (param) {
case MAGIC_PARAM_INDIR_MAX:
ms->indir_max = CAST(uint16_t, *CAST(const size_t *, val));
return 0;
case MAGIC_PARAM_NAME_MAX:
ms->name_max = CAST(uint16_t, *CAST(const size_t *, val));
return 0;
case MAGIC_PARAM_ELF_PHNUM_MAX:
ms->elf_phnum_max = CAST(uint16_t, *CAST(const size_t *, val));
return 0;
case MAGIC_PARAM_ELF_SHNUM_MAX:
ms->elf_shnum_max = CAST(uint16_t, *CAST(const size_t *, val));
return 0;
case MAGIC_PARAM_ELF_NOTES_MAX:
ms->elf_notes_max = CAST(uint16_t, *CAST(const size_t *, val));
return 0;
case MAGIC_PARAM_REGEX_MAX:
ms->regex_max = CAST(uint16_t, *CAST(const size_t *, val));
return 0;
case MAGIC_PARAM_BYTES_MAX:
ms->bytes_max = *CAST(const size_t *, val);
return 0;
case MAGIC_PARAM_ENCODING_MAX:
ms->encoding_max = *CAST(const size_t *, val);
return 0;
default:
errno = EINVAL;
return -1;
}
}

public int
magic_getparam(struct magic_set *ms, int param, void *val)
{
if (ms == NULL)
return -1;
switch (param) {
case MAGIC_PARAM_INDIR_MAX:
*CAST(size_t *, val) = ms->indir_max;
return 0;
case MAGIC_PARAM_NAME_MAX:
*CAST(size_t *, val) = ms->name_max;
return 0;
case MAGIC_PARAM_ELF_PHNUM_MAX:
*CAST(size_t *, val) = ms->elf_phnum_max;
return 0;
case MAGIC_PARAM_ELF_SHNUM_MAX:
*CAST(size_t *, val) = ms->elf_shnum_max;
return 0;
case MAGIC_PARAM_ELF_NOTES_MAX:
*CAST(size_t *, val) = ms->elf_notes_max;
return 0;
case MAGIC_PARAM_REGEX_MAX:
*CAST(size_t *, val) = ms->regex_max;
return 0;
case MAGIC_PARAM_BYTES_MAX:
*CAST(size_t *, val) = ms->bytes_max;
return 0;
case MAGIC_PARAM_ENCODING_MAX:
*CAST(size_t *, val) = ms->encoding_max;
return 0;
default:
errno = EINVAL;
return -1;
}
}
