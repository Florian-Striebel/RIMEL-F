
































#if !defined(ARCHIVE_PACK_DEV_H)
#define ARCHIVE_PACK_DEV_H

typedef dev_t pack_t(int, unsigned long [], const char **);

pack_t *pack_find(const char *);
pack_t pack_native;

#define major_netbsd(x) ((int32_t)((((x) & 0x000fff00) >> 8)))
#define minor_netbsd(x) ((int32_t)((((x) & 0xfff00000) >> 12) | (((x) & 0x000000ff) >> 0)))

#define makedev_netbsd(x,y) ((dev_t)((((x) << 8) & 0x000fff00) | (((y) << 12) & 0xfff00000) | (((y) << 0) & 0x000000ff)))



#endif
