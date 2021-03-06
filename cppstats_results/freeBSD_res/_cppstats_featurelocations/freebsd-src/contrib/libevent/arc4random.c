










































#if !defined(ARC4RANDOM_EXPORT)
#define ARC4RANDOM_EXPORT
#endif

#if !defined(ARC4RANDOM_UINT32)
#define ARC4RANDOM_UINT32 uint32_t
#endif

#if !defined(ARC4RANDOM_NO_INCLUDES)
#include "evconfig-private.h"
#if defined(_WIN32)
#include <wincrypt.h>
#include <process.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/time.h>
#if defined(EVENT__HAVE_SYS_SYSCTL_H)
#include <sys/sysctl.h>
#endif
#endif
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#endif


#define ADD_ENTROPY 32


#define BYTES_BEFORE_RESEED 1600000

struct arc4_stream {
unsigned char i;
unsigned char j;
unsigned char s[256];
};

#if defined(_WIN32)
#define getpid _getpid
#define pid_t int
#endif

static int rs_initialized;
static struct arc4_stream rs;
static pid_t arc4_stir_pid;
static int arc4_count;
static int arc4_seeded_ok;

static inline unsigned char arc4_getbyte(void);

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
arc4_addrandom(const unsigned char *dat, int datlen)
{
int n;
unsigned char si;

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

#if !defined(_WIN32)
static ssize_t
read_all(int fd, unsigned char *buf, size_t count)
{
size_t numread = 0;
ssize_t result;

while (numread < count) {
result = read(fd, buf+numread, count-numread);
if (result<0)
return -1;
else if (result == 0)
break;
numread += result;
}

return (ssize_t)numread;
}
#endif

#if defined(_WIN32)
#define TRY_SEED_WIN32
static int
arc4_seed_win32(void)
{

static int provider_set = 0;
static HCRYPTPROV provider;
unsigned char buf[ADD_ENTROPY];

if (!provider_set) {
if (!CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL,
CRYPT_VERIFYCONTEXT)) {
if (GetLastError() != (DWORD)NTE_BAD_KEYSET)
return -1;
}
provider_set = 1;
}
if (!CryptGenRandom(provider, sizeof(buf), buf))
return -1;
arc4_addrandom(buf, sizeof(buf));
evutil_memclear_(buf, sizeof(buf));
arc4_seeded_ok = 1;
return 0;
}
#endif

#if defined(EVENT__HAVE_SYS_SYSCTL_H) && defined(EVENT__HAVE_SYSCTL)
#if EVENT__HAVE_DECL_CTL_KERN && EVENT__HAVE_DECL_KERN_RANDOM && EVENT__HAVE_DECL_RANDOM_UUID
#define TRY_SEED_SYSCTL_LINUX
static int
arc4_seed_sysctl_linux(void)
{




int mib[] = { CTL_KERN, KERN_RANDOM, RANDOM_UUID };
unsigned char buf[ADD_ENTROPY];
size_t len, n;
unsigned i;
int any_set;

memset(buf, 0, sizeof(buf));

for (len = 0; len < sizeof(buf); len += n) {
n = sizeof(buf) - len;

if (0 != sysctl(mib, 3, &buf[len], &n, NULL, 0))
return -1;
}

for (i=0,any_set=0; i<sizeof(buf); ++i) {
any_set |= buf[i];
}
if (!any_set)
return -1;

arc4_addrandom(buf, sizeof(buf));
evutil_memclear_(buf, sizeof(buf));
arc4_seeded_ok = 1;
return 0;
}
#endif

#if EVENT__HAVE_DECL_CTL_KERN && EVENT__HAVE_DECL_KERN_ARND
#define TRY_SEED_SYSCTL_BSD
static int
arc4_seed_sysctl_bsd(void)
{




int mib[] = { CTL_KERN, KERN_ARND };
unsigned char buf[ADD_ENTROPY];
size_t len, n;
int i, any_set;

memset(buf, 0, sizeof(buf));

len = sizeof(buf);
if (sysctl(mib, 2, buf, &len, NULL, 0) == -1) {
for (len = 0; len < sizeof(buf); len += sizeof(unsigned)) {
n = sizeof(unsigned);
if (n + len > sizeof(buf))
n = len - sizeof(buf);
if (sysctl(mib, 2, &buf[len], &n, NULL, 0) == -1)
return -1;
}
}

for (i=any_set=0; i<sizeof(buf); ++i) {
any_set |= buf[i];
}
if (!any_set)
return -1;

arc4_addrandom(buf, sizeof(buf));
evutil_memclear_(buf, sizeof(buf));
arc4_seeded_ok = 1;
return 0;
}
#endif
#endif

#if defined(__linux__)
#define TRY_SEED_PROC_SYS_KERNEL_RANDOM_UUID
static int
arc4_seed_proc_sys_kernel_random_uuid(void)
{




int fd;
char buf[128];
unsigned char entropy[64];
int bytes, n, i, nybbles;
for (bytes = 0; bytes<ADD_ENTROPY; ) {
fd = evutil_open_closeonexec_("/proc/sys/kernel/random/uuid", O_RDONLY, 0);
if (fd < 0)
return -1;
n = read(fd, buf, sizeof(buf));
close(fd);
if (n<=0)
return -1;
memset(entropy, 0, sizeof(entropy));
for (i=nybbles=0; i<n; ++i) {
if (EVUTIL_ISXDIGIT_(buf[i])) {
int nyb = evutil_hex_char_to_int_(buf[i]);
if (nybbles & 1) {
entropy[nybbles/2] |= nyb;
} else {
entropy[nybbles/2] |= nyb<<4;
}
++nybbles;
}
}
if (nybbles < 2)
return -1;
arc4_addrandom(entropy, nybbles/2);
bytes += nybbles/2;
}
evutil_memclear_(entropy, sizeof(entropy));
evutil_memclear_(buf, sizeof(buf));
arc4_seeded_ok = 1;
return 0;
}
#endif

#if !defined(_WIN32)
#define TRY_SEED_URANDOM
static char *arc4random_urandom_filename = NULL;

static int arc4_seed_urandom_helper_(const char *fname)
{
unsigned char buf[ADD_ENTROPY];
int fd;
size_t n;

fd = evutil_open_closeonexec_(fname, O_RDONLY, 0);
if (fd<0)
return -1;
n = read_all(fd, buf, sizeof(buf));
close(fd);
if (n != sizeof(buf))
return -1;
arc4_addrandom(buf, sizeof(buf));
evutil_memclear_(buf, sizeof(buf));
arc4_seeded_ok = 1;
return 0;
}

static int
arc4_seed_urandom(void)
{

static const char *filenames[] = {
"/dev/srandom", "/dev/urandom", "/dev/random", NULL
};
int i;
if (arc4random_urandom_filename)
return arc4_seed_urandom_helper_(arc4random_urandom_filename);

for (i = 0; filenames[i]; ++i) {
if (arc4_seed_urandom_helper_(filenames[i]) == 0) {
return 0;
}
}

return -1;
}
#endif

static int
arc4_seed(void)
{
int ok = 0;



#if defined(TRY_SEED_WIN32)
if (0 == arc4_seed_win32())
ok = 1;
#endif
#if defined(TRY_SEED_URANDOM)
if (0 == arc4_seed_urandom())
ok = 1;
#endif
#if defined(TRY_SEED_PROC_SYS_KERNEL_RANDOM_UUID)
if (arc4random_urandom_filename == NULL &&
0 == arc4_seed_proc_sys_kernel_random_uuid())
ok = 1;
#endif
#if defined(TRY_SEED_SYSCTL_LINUX)


if (!ok && 0 == arc4_seed_sysctl_linux())
ok = 1;
#endif
#if defined(TRY_SEED_SYSCTL_BSD)
if (0 == arc4_seed_sysctl_bsd())
ok = 1;
#endif
return ok ? 0 : -1;
}

static int
arc4_stir(void)
{
int i;

if (!rs_initialized) {
arc4_init();
rs_initialized = 1;
}

arc4_seed();
if (!arc4_seeded_ok)
return -1;



















for (i = 0; i < 12*256; i++)
(void)arc4_getbyte();

arc4_count = BYTES_BEFORE_RESEED;

return 0;
}


static void
arc4_stir_if_needed(void)
{
pid_t pid = getpid();

if (arc4_count <= 0 || !rs_initialized || arc4_stir_pid != pid)
{
arc4_stir_pid = pid;
arc4_stir();
}
}

static inline unsigned char
arc4_getbyte(void)
{
unsigned char si, sj;

rs.i = (rs.i + 1);
si = rs.s[rs.i];
rs.j = (rs.j + si);
sj = rs.s[rs.j];
rs.s[rs.i] = sj;
rs.s[rs.j] = si;
return (rs.s[(si + sj) & 0xff]);
}

static inline unsigned int
arc4_getword(void)
{
unsigned int val;

val = arc4_getbyte() << 24;
val |= arc4_getbyte() << 16;
val |= arc4_getbyte() << 8;
val |= arc4_getbyte();

return val;
}

#if !defined(ARC4RANDOM_NOSTIR)
ARC4RANDOM_EXPORT int
arc4random_stir(void)
{
int val;
ARC4_LOCK_();
val = arc4_stir();
ARC4_UNLOCK_();
return val;
}
#endif

#if !defined(ARC4RANDOM_NOADDRANDOM)
ARC4RANDOM_EXPORT void
arc4random_addrandom(const unsigned char *dat, int datlen)
{
int j;
ARC4_LOCK_();
if (!rs_initialized)
arc4_stir();
for (j = 0; j < datlen; j += 256) {




arc4_addrandom(dat + j, datlen - j);
}
ARC4_UNLOCK_();
}
#endif

#if !defined(ARC4RANDOM_NORANDOM)
ARC4RANDOM_EXPORT ARC4RANDOM_UINT32
arc4random(void)
{
ARC4RANDOM_UINT32 val;
ARC4_LOCK_();
arc4_count -= 4;
arc4_stir_if_needed();
val = arc4_getword();
ARC4_UNLOCK_();
return val;
}
#endif

ARC4RANDOM_EXPORT void
arc4random_buf(void *buf_, size_t n)
{
unsigned char *buf = buf_;
ARC4_LOCK_();
arc4_stir_if_needed();
while (n--) {
if (--arc4_count <= 0)
arc4_stir();
buf[n] = arc4_getbyte();
}
ARC4_UNLOCK_();
}

#if !defined(ARC4RANDOM_NOUNIFORM)










ARC4RANDOM_EXPORT unsigned int
arc4random_uniform(unsigned int upper_bound)
{
ARC4RANDOM_UINT32 r, min;

if (upper_bound < 2)
return 0;

#if (UINT_MAX > 0xffffffffUL)
min = 0x100000000UL % upper_bound;
#else

if (upper_bound > 0x80000000)
min = 1 + ~upper_bound;
else {

min = ((0xffffffff - (upper_bound * 2)) + 1) % upper_bound;
}
#endif







for (;;) {
r = arc4random();
if (r >= min)
break;
}

return r % upper_bound;
}
#endif
