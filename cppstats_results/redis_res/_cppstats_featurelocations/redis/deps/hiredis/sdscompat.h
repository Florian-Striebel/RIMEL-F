








































#if !defined(HIREDIS_SDS_COMPAT)
#define HIREDIS_SDS_COMPAT

#define sds hisds

#define sdslen hi_sdslen
#define sdsavail hi_sdsavail
#define sdssetlen hi_sdssetlen
#define sdsinclen hi_sdsinclen
#define sdsalloc hi_sdsalloc
#define sdssetalloc hi_sdssetalloc

#define sdsAllocPtr hi_sdsAllocPtr
#define sdsAllocSize hi_sdsAllocSize
#define sdscat hi_sdscat
#define sdscatfmt hi_sdscatfmt
#define sdscatlen hi_sdscatlen
#define sdscatprintf hi_sdscatprintf
#define sdscatrepr hi_sdscatrepr
#define sdscatsds hi_sdscatsds
#define sdscatvprintf hi_sdscatvprintf
#define sdsclear hi_sdsclear
#define sdscmp hi_sdscmp
#define sdscpy hi_sdscpy
#define sdscpylen hi_sdscpylen
#define sdsdup hi_sdsdup
#define sdsempty hi_sdsempty
#define sds_free hi_sds_free
#define sdsfree hi_sdsfree
#define sdsfreesplitres hi_sdsfreesplitres
#define sdsfromlonglong hi_sdsfromlonglong
#define sdsgrowzero hi_sdsgrowzero
#define sdsIncrLen hi_sdsIncrLen
#define sdsjoin hi_sdsjoin
#define sdsjoinsds hi_sdsjoinsds
#define sdsll2str hi_sdsll2str
#define sdsMakeRoomFor hi_sdsMakeRoomFor
#define sds_malloc hi_sds_malloc
#define sdsmapchars hi_sdsmapchars
#define sdsnew hi_sdsnew
#define sdsnewlen hi_sdsnewlen
#define sdsrange hi_sdsrange
#define sds_realloc hi_sds_realloc
#define sdsRemoveFreeSpace hi_sdsRemoveFreeSpace
#define sdssplitargs hi_sdssplitargs
#define sdssplitlen hi_sdssplitlen
#define sdstolower hi_sdstolower
#define sdstoupper hi_sdstoupper
#define sdstrim hi_sdstrim
#define sdsull2str hi_sdsull2str
#define sdsupdatelen hi_sdsupdatelen

#endif
