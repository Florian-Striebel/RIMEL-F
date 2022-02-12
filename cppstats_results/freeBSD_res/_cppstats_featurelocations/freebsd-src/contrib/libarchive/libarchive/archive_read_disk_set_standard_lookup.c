
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_GRP_H)
#include <grp.h>
#endif
#if defined(HAVE_PWD_H)
#include <pwd.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#include "archive.h"

#if defined(_WIN32) && !defined(__CYGWIN__)
int
archive_read_disk_set_standard_lookup(struct archive *a)
{
archive_set_error(a, -1, "Standard lookups not available on Windows");
return (ARCHIVE_FATAL);
}
#else
#define name_cache_size 127

static const char * const NO_NAME = "(noname)";

struct name_cache {
struct archive *archive;
char *buff;
size_t buff_size;
int probes;
int hits;
size_t size;
struct {
id_t id;
const char *name;
} cache[name_cache_size];
};

static const char * lookup_gname(void *, int64_t);
static const char * lookup_uname(void *, int64_t);
static void cleanup(void *);
static const char * lookup_gname_helper(struct name_cache *, id_t gid);
static const char * lookup_uname_helper(struct name_cache *, id_t uid);














int
archive_read_disk_set_standard_lookup(struct archive *a)
{
struct name_cache *ucache = malloc(sizeof(struct name_cache));
struct name_cache *gcache = malloc(sizeof(struct name_cache));

if (ucache == NULL || gcache == NULL) {
archive_set_error(a, ENOMEM,
"Can't allocate uname/gname lookup cache");
free(ucache);
free(gcache);
return (ARCHIVE_FATAL);
}

memset(ucache, 0, sizeof(*ucache));
ucache->archive = a;
ucache->size = name_cache_size;
memset(gcache, 0, sizeof(*gcache));
gcache->archive = a;
gcache->size = name_cache_size;

archive_read_disk_set_gname_lookup(a, gcache, lookup_gname, cleanup);
archive_read_disk_set_uname_lookup(a, ucache, lookup_uname, cleanup);

return (ARCHIVE_OK);
}

static void
cleanup(void *data)
{
struct name_cache *cache = (struct name_cache *)data;
size_t i;

if (cache != NULL) {
for (i = 0; i < cache->size; i++) {
if (cache->cache[i].name != NULL &&
cache->cache[i].name != NO_NAME)
free((void *)(uintptr_t)cache->cache[i].name);
}
free(cache->buff);
free(cache);
}
}




static const char *
lookup_name(struct name_cache *cache,
const char * (*lookup_fn)(struct name_cache *, id_t), id_t id)
{
const char *name;
int slot;


cache->probes++;

slot = id % cache->size;
if (cache->cache[slot].name != NULL) {
if (cache->cache[slot].id == id) {
cache->hits++;
if (cache->cache[slot].name == NO_NAME)
return (NULL);
return (cache->cache[slot].name);
}
if (cache->cache[slot].name != NO_NAME)
free((void *)(uintptr_t)cache->cache[slot].name);
cache->cache[slot].name = NULL;
}

name = (lookup_fn)(cache, id);
if (name == NULL) {

cache->cache[slot].name = NO_NAME;
cache->cache[slot].id = id;
return (NULL);
}

cache->cache[slot].name = name;
cache->cache[slot].id = id;
return (cache->cache[slot].name);
}

static const char *
lookup_uname(void *data, int64_t uid)
{
struct name_cache *uname_cache = (struct name_cache *)data;
return (lookup_name(uname_cache,
&lookup_uname_helper, (id_t)uid));
}

#if HAVE_GETPWUID_R
static const char *
lookup_uname_helper(struct name_cache *cache, id_t id)
{
struct passwd pwent, *result;
char * nbuff;
size_t nbuff_size;
int r;

if (cache->buff_size == 0) {
cache->buff_size = 256;
cache->buff = malloc(cache->buff_size);
}
if (cache->buff == NULL)
return (NULL);
for (;;) {
result = &pwent;
r = getpwuid_r((uid_t)id, &pwent,
cache->buff, cache->buff_size, &result);
if (r == 0)
break;
if (r != ERANGE)
break;





nbuff_size = cache->buff_size * 2;
nbuff = realloc(cache->buff, nbuff_size);
if (nbuff == NULL)
break;
cache->buff = nbuff;
cache->buff_size = nbuff_size;
}
if (r != 0) {
archive_set_error(cache->archive, errno,
"Can't lookup user for id %d", (int)id);
return (NULL);
}
if (result == NULL)
return (NULL);

return strdup(result->pw_name);
}
#else
static const char *
lookup_uname_helper(struct name_cache *cache, id_t id)
{
struct passwd *result;
(void)cache;

result = getpwuid((uid_t)id);

if (result == NULL)
return (NULL);

return strdup(result->pw_name);
}
#endif

static const char *
lookup_gname(void *data, int64_t gid)
{
struct name_cache *gname_cache = (struct name_cache *)data;
return (lookup_name(gname_cache,
&lookup_gname_helper, (id_t)gid));
}

#if HAVE_GETGRGID_R
static const char *
lookup_gname_helper(struct name_cache *cache, id_t id)
{
struct group grent, *result;
char * nbuff;
size_t nbuff_size;
int r;

if (cache->buff_size == 0) {
cache->buff_size = 256;
cache->buff = malloc(cache->buff_size);
}
if (cache->buff == NULL)
return (NULL);
for (;;) {
result = &grent;
r = getgrgid_r((gid_t)id, &grent,
cache->buff, cache->buff_size, &result);
if (r == 0)
break;
if (r != ERANGE)
break;



nbuff_size = cache->buff_size * 2;
nbuff = realloc(cache->buff, nbuff_size);
if (nbuff == NULL)
break;
cache->buff = nbuff;
cache->buff_size = nbuff_size;
}
if (r != 0) {
archive_set_error(cache->archive, errno,
"Can't lookup group for id %d", (int)id);
return (NULL);
}
if (result == NULL)
return (NULL);

return strdup(result->gr_name);
}
#else
static const char *
lookup_gname_helper(struct name_cache *cache, id_t id)
{
struct group *result;
(void)cache;

result = getgrgid((gid_t)id);

if (result == NULL)
return (NULL);

return strdup(result->gr_name);
}
#endif

#endif
