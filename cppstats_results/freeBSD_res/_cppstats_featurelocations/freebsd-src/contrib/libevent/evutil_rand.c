

































#include "event2/event-config.h"
#include "evconfig-private.h"

#include <limits.h>

#include "util-internal.h"
#include "evthread-internal.h"

#if defined(EVENT__HAVE_ARC4RANDOM)
#include <stdlib.h>
#include <string.h>
int
evutil_secure_rng_set_urandom_device_file(char *fname)
{
(void) fname;
return -1;
}
int
evutil_secure_rng_init(void)
{

(void) arc4random();
return 0;
}
#if !defined(EVENT__DISABLE_THREAD_SUPPORT)
int
evutil_secure_rng_global_setup_locks_(const int enable_locks)
{
return 0;
}
#endif
static void
evutil_free_secure_rng_globals_locks(void)
{
}

static void
ev_arc4random_buf(void *buf, size_t n)
{
#if defined(EVENT__HAVE_ARC4RANDOM_BUF) && !defined(__APPLE__)
arc4random_buf(buf, n);
return;
#else
unsigned char *b = buf;

#if defined(EVENT__HAVE_ARC4RANDOM_BUF)






{
void (*tptr)(void *,size_t) =
(void (*)(void*,size_t))arc4random_buf;
if (tptr != NULL) {
arc4random_buf(buf, n);
return;
}
}
#endif


if (n >= 4 && ((ev_uintptr_t)b) & 3) {
ev_uint32_t u = arc4random();
int n_bytes = 4 - (((ev_uintptr_t)b) & 3);
memcpy(b, &u, n_bytes);
b += n_bytes;
n -= n_bytes;
}
while (n >= 4) {
*(ev_uint32_t*)b = arc4random();
b += 4;
n -= 4;
}
if (n) {
ev_uint32_t u = arc4random();
memcpy(b, &u, n);
}
#endif
}

#else

#if defined(EVENT__ssize_t)
#define ssize_t EVENT__ssize_t
#endif
#define ARC4RANDOM_EXPORT static
#define ARC4_LOCK_() EVLOCK_LOCK(arc4rand_lock, 0)
#define ARC4_UNLOCK_() EVLOCK_UNLOCK(arc4rand_lock, 0)
#if !defined(EVENT__DISABLE_THREAD_SUPPORT)
static void *arc4rand_lock;
#endif

#define ARC4RANDOM_UINT32 ev_uint32_t
#define ARC4RANDOM_NOSTIR
#define ARC4RANDOM_NORANDOM
#define ARC4RANDOM_NOUNIFORM

#include "./arc4random.c"

#if !defined(EVENT__DISABLE_THREAD_SUPPORT)
int
evutil_secure_rng_global_setup_locks_(const int enable_locks)
{
EVTHREAD_SETUP_GLOBAL_LOCK(arc4rand_lock, 0);
return 0;
}
#endif

static void
evutil_free_secure_rng_globals_locks(void)
{
#if !defined(EVENT__DISABLE_THREAD_SUPPORT)
if (arc4rand_lock != NULL) {
EVTHREAD_FREE_LOCK(arc4rand_lock, 0);
arc4rand_lock = NULL;
}
#endif
return;
}

int
evutil_secure_rng_set_urandom_device_file(char *fname)
{
#if defined(TRY_SEED_URANDOM)
ARC4_LOCK_();
arc4random_urandom_filename = fname;
ARC4_UNLOCK_();
#endif
return 0;
}

int
evutil_secure_rng_init(void)
{
int val;

ARC4_LOCK_();
if (!arc4_seeded_ok)
arc4_stir();
val = arc4_seeded_ok ? 0 : -1;
ARC4_UNLOCK_();
return val;
}

static void
ev_arc4random_buf(void *buf, size_t n)
{
arc4random_buf(buf, n);
}

#endif

void
evutil_secure_rng_get_bytes(void *buf, size_t n)
{
ev_arc4random_buf(buf, n);
}

void
evutil_secure_rng_add_bytes(const char *buf, size_t n)
{
}

void
evutil_free_secure_rng_globals_(void)
{
evutil_free_secure_rng_globals_locks();
}
