
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif

#if !defined(HAVE_ARC4RANDOM_BUF) && (!defined(_WIN32) || defined(__CYGWIN__))

#if defined(HAVE_FCNTL)
#include <fcntl.h>
#endif
#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(HAVE_PTHREAD_H)
#include <pthread.h>
#endif

static void arc4random_buf(void *, size_t);

#endif

#include "archive.h"
#include "archive_random_private.h"

#if defined(HAVE_WINCRYPT_H) && !defined(__CYGWIN__)
#include <wincrypt.h>
#endif

#if !defined(O_CLOEXEC)
#define O_CLOEXEC 0
#endif






int
archive_random(void *buf, size_t nbytes)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
HCRYPTPROV hProv;
BOOL success;

success = CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL,
CRYPT_VERIFYCONTEXT);
if (!success && GetLastError() == (DWORD)NTE_BAD_KEYSET) {
success = CryptAcquireContext(&hProv, NULL, NULL,
PROV_RSA_FULL, CRYPT_NEWKEYSET);
}
if (success) {
success = CryptGenRandom(hProv, (DWORD)nbytes, (BYTE*)buf);
CryptReleaseContext(hProv, 0);
if (success)
return ARCHIVE_OK;
}

return ARCHIVE_FAILED;
#else
arc4random_buf(buf, nbytes);
return ARCHIVE_OK;
#endif
}

#if !defined(HAVE_ARC4RANDOM_BUF) && (!defined(_WIN32) || defined(__CYGWIN__))































#if defined(__GNUC__)
#define inline __inline
#else
#define inline
#endif

struct arc4_stream {
uint8_t i;
uint8_t j;
uint8_t s[256];
};

#define RANDOMDEV "/dev/urandom"
#define KEYSIZE 128
#if defined(HAVE_PTHREAD_H)
static pthread_mutex_t arc4random_mtx = PTHREAD_MUTEX_INITIALIZER;
#define _ARC4_LOCK() pthread_mutex_lock(&arc4random_mtx);
#define _ARC4_UNLOCK() pthread_mutex_unlock(&arc4random_mtx);
#else
#define _ARC4_LOCK()
#define _ARC4_UNLOCK()
#endif

static int rs_initialized;
static struct arc4_stream rs;
static pid_t arc4_stir_pid;
static int arc4_count;

static inline uint8_t arc4_getbyte(void);
static void arc4_stir(void);

static inline void
arc4_init(void)
{
int n;

for (n = 0; n < 256; n++)
rs.s[n] = n;
rs.i = 0;
rs.j = 0;
}

static inline void
arc4_addrandom(uint8_t *dat, int datlen)
{
int n;
uint8_t si;

rs.i--;
for (n = 0; n < 256; n++) {
rs.i = (rs.i + 1);
si = rs.s[rs.i];
rs.j = (rs.j + si + dat[n % datlen]);
rs.s[rs.i] = rs.s[rs.j];
rs.s[rs.j] = si;
}
rs.j = rs.i;
}

static void
arc4_stir(void)
{
int done, fd, i;
struct {
struct timeval tv;
pid_t pid;
uint8_t rnd[KEYSIZE];
} rdat;

if (!rs_initialized) {
arc4_init();
rs_initialized = 1;
}
done = 0;
fd = open(RANDOMDEV, O_RDONLY | O_CLOEXEC, 0);
if (fd >= 0) {
if (read(fd, &rdat, KEYSIZE) == KEYSIZE)
done = 1;
(void)close(fd);
}
if (!done) {
(void)gettimeofday(&rdat.tv, NULL);
rdat.pid = getpid();

}

arc4_addrandom((uint8_t *)&rdat, KEYSIZE);








for (i = 0; i < 3072; i++)
(void)arc4_getbyte();
arc4_count = 1600000;
}

static void
arc4_stir_if_needed(void)
{
pid_t pid = getpid();

if (arc4_count <= 0 || !rs_initialized || arc4_stir_pid != pid) {
arc4_stir_pid = pid;
arc4_stir();
}
}

static inline uint8_t
arc4_getbyte(void)
{
uint8_t si, sj;

rs.i = (rs.i + 1);
si = rs.s[rs.i];
rs.j = (rs.j + si);
sj = rs.s[rs.j];
rs.s[rs.i] = sj;
rs.s[rs.j] = si;
return (rs.s[(si + sj) & 0xff]);
}

static void
arc4random_buf(void *_buf, size_t n)
{
uint8_t *buf = (uint8_t *)_buf;
_ARC4_LOCK();
arc4_stir_if_needed();
while (n--) {
if (--arc4_count <= 0)
arc4_stir();
buf[n] = arc4_getbyte();
}
_ARC4_UNLOCK();
}

#endif
