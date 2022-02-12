#include <umem.h>
#include <stdlib.h>
#include <assert.h>
static umem_nofail_callback_t *nofail_cb = NULL;
struct umem_cache {
umem_constructor_t *constructor;
umem_destructor_t *destructor;
void *callback_data;
size_t bufsize;
};
void *umem_alloc(size_t size, int flags)
{
assert(flags == UMEM_DEFAULT || flags == UMEM_NOFAIL);
if(size == 0)
return NULL;
void *ret = malloc(size);
if(ret == NULL) {
if(!(flags & UMEM_NOFAIL))
return NULL;
if(nofail_cb != NULL)
nofail_cb();
abort();
}
return ret;
}
void *umem_zalloc(size_t size, int flags)
{
assert(flags == UMEM_DEFAULT || flags == UMEM_NOFAIL);
if(size == 0)
return NULL;
void *ret = calloc(1, size);
if(ret == NULL) {
if(!(flags & UMEM_NOFAIL))
return NULL;
if(nofail_cb != NULL)
nofail_cb();
abort();
}
return ret;
}
void umem_free(void *buf, size_t size)
{
free(buf);
}
void umem_nofail_callback(umem_nofail_callback_t *callback)
{
nofail_cb = callback;
}
umem_cache_t *umem_cache_create(char *debug_name, size_t bufsize, size_t align, umem_constructor_t *constructor, umem_destructor_t *destructor, umem_reclaim_t *reclaim, void *callback_data, void *source, int cflags)
{
assert(source == NULL);
umem_cache_t *cache = malloc(sizeof(umem_cache_t));
if(cache == NULL)
return NULL;
cache->constructor = constructor;
cache->destructor = destructor;
cache->callback_data = callback_data;
cache->bufsize = bufsize;
return cache;
}
void *umem_cache_alloc(umem_cache_t *cache, int flags)
{
void *buf = malloc(cache->bufsize);
if(buf == NULL) {
if(!(flags & UMEM_NOFAIL))
return NULL;
if(nofail_cb != NULL)
nofail_cb();
abort();
}
if(cache->constructor != NULL) {
if(cache->constructor(buf, cache->callback_data, flags) != 0) {
free(buf);
if(!(flags & UMEM_NOFAIL))
return NULL;
if(nofail_cb != NULL)
nofail_cb();
abort();
}
}
return buf;
}
void umem_cache_free(umem_cache_t *cache, void *buffer)
{
if(cache->destructor != NULL)
cache->destructor(buffer, cache->callback_data);
free(buffer);
}
void umem_cache_destroy(umem_cache_t *cache)
{
free(cache);
}
