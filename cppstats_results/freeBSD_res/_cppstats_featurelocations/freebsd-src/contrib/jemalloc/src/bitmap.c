#define JEMALLOC_BITMAP_C_
#include "jemalloc/internal/jemalloc_preamble.h"
#include "jemalloc/internal/jemalloc_internal_includes.h"

#include "jemalloc/internal/assert.h"



#if defined(BITMAP_USE_TREE)

void
bitmap_info_init(bitmap_info_t *binfo, size_t nbits) {
unsigned i;
size_t group_count;

assert(nbits > 0);
assert(nbits <= (ZU(1) << LG_BITMAP_MAXBITS));






binfo->levels[0].group_offset = 0;
group_count = BITMAP_BITS2GROUPS(nbits);
for (i = 1; group_count > 1; i++) {
assert(i < BITMAP_MAX_LEVELS);
binfo->levels[i].group_offset = binfo->levels[i-1].group_offset
+ group_count;
group_count = BITMAP_BITS2GROUPS(group_count);
}
binfo->levels[i].group_offset = binfo->levels[i-1].group_offset
+ group_count;
assert(binfo->levels[i].group_offset <= BITMAP_GROUPS_MAX);
binfo->nlevels = i;
binfo->nbits = nbits;
}

static size_t
bitmap_info_ngroups(const bitmap_info_t *binfo) {
return binfo->levels[binfo->nlevels].group_offset;
}

void
bitmap_init(bitmap_t *bitmap, const bitmap_info_t *binfo, bool fill) {
size_t extra;
unsigned i;






if (fill) {

memset(bitmap, 0, bitmap_size(binfo));
return;
}







memset(bitmap, 0xffU, bitmap_size(binfo));
extra = (BITMAP_GROUP_NBITS - (binfo->nbits & BITMAP_GROUP_NBITS_MASK))
& BITMAP_GROUP_NBITS_MASK;
if (extra != 0) {
bitmap[binfo->levels[1].group_offset - 1] >>= extra;
}
for (i = 1; i < binfo->nlevels; i++) {
size_t group_count = binfo->levels[i].group_offset -
binfo->levels[i-1].group_offset;
extra = (BITMAP_GROUP_NBITS - (group_count &
BITMAP_GROUP_NBITS_MASK)) & BITMAP_GROUP_NBITS_MASK;
if (extra != 0) {
bitmap[binfo->levels[i+1].group_offset - 1] >>= extra;
}
}
}

#else

void
bitmap_info_init(bitmap_info_t *binfo, size_t nbits) {
assert(nbits > 0);
assert(nbits <= (ZU(1) << LG_BITMAP_MAXBITS));

binfo->ngroups = BITMAP_BITS2GROUPS(nbits);
binfo->nbits = nbits;
}

static size_t
bitmap_info_ngroups(const bitmap_info_t *binfo) {
return binfo->ngroups;
}

void
bitmap_init(bitmap_t *bitmap, const bitmap_info_t *binfo, bool fill) {
size_t extra;

if (fill) {
memset(bitmap, 0, bitmap_size(binfo));
return;
}

memset(bitmap, 0xffU, bitmap_size(binfo));
extra = (BITMAP_GROUP_NBITS - (binfo->nbits & BITMAP_GROUP_NBITS_MASK))
& BITMAP_GROUP_NBITS_MASK;
if (extra != 0) {
bitmap[binfo->ngroups - 1] >>= extra;
}
}

#endif

size_t
bitmap_size(const bitmap_info_t *binfo) {
return (bitmap_info_ngroups(binfo) << LG_SIZEOF_BITMAP);
}
