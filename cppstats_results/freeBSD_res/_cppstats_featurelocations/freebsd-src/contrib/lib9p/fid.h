


























#if !defined(LIB9P_FID_H)
#define LIB9P_FID_H

#include <stdbool.h>










































struct l9p_fid {
void *lo_aux;
uint32_t lo_fid;
uint32_t lo_flags;
};

enum l9p_lo_flags {
L9P_LO_ISAUTH = 0x01,
L9P_LO_ISDIR = 0x02,
L9P_LO_ISOPEN = 0x04,
L9P_LO_ISVALID = 0x08,
L9P_LO_ISXATTR = 0x10,
};

static inline bool
l9p_fid_isauth(struct l9p_fid *fid)
{
return ((fid->lo_flags & L9P_LO_ISAUTH) != 0);
}

static inline void
l9p_fid_setauth(struct l9p_fid *fid)
{
fid->lo_flags |= L9P_LO_ISAUTH;
}

static inline bool
l9p_fid_isdir(struct l9p_fid *fid)
{
return ((fid->lo_flags & L9P_LO_ISDIR) != 0);
}

static inline void
l9p_fid_setdir(struct l9p_fid *fid)
{
fid->lo_flags |= L9P_LO_ISDIR;
}

static inline void
l9p_fid_unsetdir(struct l9p_fid *fid)
{
fid->lo_flags &= ~(uint32_t)L9P_LO_ISDIR;
}

static inline bool
l9p_fid_isopen(struct l9p_fid *fid)
{
return ((fid->lo_flags & L9P_LO_ISOPEN) != 0);
}

static inline void
l9p_fid_setopen(struct l9p_fid *fid)
{
fid->lo_flags |= L9P_LO_ISOPEN;
}

static inline bool
l9p_fid_isvalid(struct l9p_fid *fid)
{
return ((fid->lo_flags & L9P_LO_ISVALID) != 0);
}

static inline void
l9p_fid_setvalid(struct l9p_fid *fid)
{
fid->lo_flags |= L9P_LO_ISVALID;
}

static inline void
l9p_fid_unsetvalid(struct l9p_fid *fid)
{
fid->lo_flags &= ~(uint32_t)L9P_LO_ISVALID;
}

static inline bool
l9p_fid_isxattr(struct l9p_fid *fid)
{
return ((fid->lo_flags & L9P_LO_ISXATTR) != 0);
}

static inline void
l9p_fid_setxattr(struct l9p_fid *fid)
{
fid->lo_flags |= L9P_LO_ISXATTR;
}

#endif
