








#if !defined(__IMMINTRIN_H)
#error "Never use <amxintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__AMXINTRIN_H)
#define __AMXINTRIN_H
#if defined(__x86_64__)


#define __DEFAULT_FN_ATTRS_TILE __attribute__((__always_inline__, __nodebug__, __target__("amx-tile")))

#define __DEFAULT_FN_ATTRS_INT8 __attribute__((__always_inline__, __nodebug__, __target__("amx-int8")))

#define __DEFAULT_FN_ATTRS_BF16 __attribute__((__always_inline__, __nodebug__, __target__("amx-bf16")))















static __inline__ void __DEFAULT_FN_ATTRS_TILE
_tile_loadconfig(const void *__config) {
__builtin_ia32_tile_loadconfig(__config);
}












static __inline__ void __DEFAULT_FN_ATTRS_TILE
_tile_storeconfig(void *__config) {
__builtin_ia32_tile_storeconfig(__config);
}







static __inline__ void __DEFAULT_FN_ATTRS_TILE _tile_release(void) {
__builtin_ia32_tilerelease();
}















#define _tile_loadd(dst, base, stride) __builtin_ia32_tileloadd64((dst), ((const void *)(base)), (__SIZE_TYPE__)(stride))



















#define _tile_stream_loadd(dst, base, stride) __builtin_ia32_tileloaddt164((dst), ((const void *)(base)), (__SIZE_TYPE__)(stride))

















#define _tile_stored(dst, base, stride) __builtin_ia32_tilestored64((dst), ((void *)(base)), (__SIZE_TYPE__)(stride))










#define _tile_zero(tile) __builtin_ia32_tilezero((tile))

















#define _tile_dpbssd(dst, src0, src1) __builtin_ia32_tdpbssd((dst), (src0), (src1))


















#define _tile_dpbsud(dst, src0, src1) __builtin_ia32_tdpbsud((dst), (src0), (src1))


















#define _tile_dpbusd(dst, src0, src1) __builtin_ia32_tdpbusd((dst), (src0), (src1))


















#define _tile_dpbuud(dst, src0, src1) __builtin_ia32_tdpbuud((dst), (src0), (src1))

















#define _tile_dpbf16ps(dst, src0, src1) __builtin_ia32_tdpbf16ps((dst), (src0), (src1))





typedef int _tile1024i __attribute__((__vector_size__(1024), __aligned__(64)));


static __inline__ _tile1024i __DEFAULT_FN_ATTRS_INT8
_tile_loadd_internal(unsigned short m, unsigned short n, const void *base,
__SIZE_TYPE__ stride) {
return __builtin_ia32_tileloadd64_internal(m, n, base,
(__SIZE_TYPE__)(stride));
}


static __inline__ _tile1024i __DEFAULT_FN_ATTRS_INT8
_tile_loaddt1_internal(unsigned short m, unsigned short n, const void *base,
__SIZE_TYPE__ stride) {
return __builtin_ia32_tileloaddt164_internal(m, n, base,
(__SIZE_TYPE__)(stride));
}


static __inline__ _tile1024i __DEFAULT_FN_ATTRS_INT8
_tile_dpbssd_internal(unsigned short m, unsigned short n, unsigned short k,
_tile1024i dst, _tile1024i src1, _tile1024i src2) {
return __builtin_ia32_tdpbssd_internal(m, n, k, dst, src1, src2);
}


static __inline__ _tile1024i __DEFAULT_FN_ATTRS_INT8
_tile_dpbsud_internal(unsigned short m, unsigned short n, unsigned short k,
_tile1024i dst, _tile1024i src1, _tile1024i src2) {
return __builtin_ia32_tdpbsud_internal(m, n, k, dst, src1, src2);
}


static __inline__ _tile1024i __DEFAULT_FN_ATTRS_INT8
_tile_dpbusd_internal(unsigned short m, unsigned short n, unsigned short k,
_tile1024i dst, _tile1024i src1, _tile1024i src2) {
return __builtin_ia32_tdpbusd_internal(m, n, k, dst, src1, src2);
}


static __inline__ _tile1024i __DEFAULT_FN_ATTRS_INT8
_tile_dpbuud_internal(unsigned short m, unsigned short n, unsigned short k,
_tile1024i dst, _tile1024i src1, _tile1024i src2) {
return __builtin_ia32_tdpbuud_internal(m, n, k, dst, src1, src2);
}


static __inline__ void __DEFAULT_FN_ATTRS_INT8
_tile_stored_internal(unsigned short m, unsigned short n, void *base,
__SIZE_TYPE__ stride, _tile1024i tile) {
return __builtin_ia32_tilestored64_internal(m, n, base,
(__SIZE_TYPE__)(stride), tile);
}


static __inline__ _tile1024i __DEFAULT_FN_ATTRS_BF16
_tile_dpbf16ps_internal(unsigned short m, unsigned short n, unsigned short k,
_tile1024i dst, _tile1024i src1, _tile1024i src2) {
return __builtin_ia32_tdpbf16ps_internal(m, n, k, dst, src1, src2);
}





typedef struct __tile1024i_str {
const unsigned short row;
const unsigned short col;
_tile1024i tile;
} __tile1024i;














__DEFAULT_FN_ATTRS_TILE
static void __tile_loadd(__tile1024i *dst, const void *base,
__SIZE_TYPE__ stride) {
dst->tile = _tile_loadd_internal(dst->row, dst->col, base, stride);
}
















__DEFAULT_FN_ATTRS_TILE
static void __tile_stream_loadd(__tile1024i *dst, const void *base,
__SIZE_TYPE__ stride) {
dst->tile = _tile_loaddt1_internal(dst->row, dst->col, base, stride);
}

















__DEFAULT_FN_ATTRS_INT8
static void __tile_dpbssd(__tile1024i *dst, __tile1024i src0,
__tile1024i src1) {
dst->tile = _tile_dpbssd_internal(src0.row, src1.col, src0.col, dst->tile,
src0.tile, src1.tile);
}

















__DEFAULT_FN_ATTRS_INT8
static void __tile_dpbsud(__tile1024i *dst, __tile1024i src0,
__tile1024i src1) {
dst->tile = _tile_dpbsud_internal(src0.row, src1.col, src0.col, dst->tile,
src0.tile, src1.tile);
}

















__DEFAULT_FN_ATTRS_INT8
static void __tile_dpbusd(__tile1024i *dst, __tile1024i src0,
__tile1024i src1) {
dst->tile = _tile_dpbusd_internal(src0.row, src1.col, src0.col, dst->tile,
src0.tile, src1.tile);
}

















__DEFAULT_FN_ATTRS_INT8
static void __tile_dpbuud(__tile1024i *dst, __tile1024i src0,
__tile1024i src1) {
dst->tile = _tile_dpbuud_internal(src0.row, src1.col, src0.col, dst->tile,
src0.tile, src1.tile);
}














__DEFAULT_FN_ATTRS_TILE
static void __tile_stored(void *base, __SIZE_TYPE__ stride, __tile1024i src) {
_tile_stored_internal(src.row, src.col, base, stride, src.tile);
}









__DEFAULT_FN_ATTRS_TILE
static void __tile_zero(__tile1024i *dst) {
dst->tile = __builtin_ia32_tilezero_internal(dst->row, dst->col);
}
















__DEFAULT_FN_ATTRS_BF16
static void __tile_dpbf16ps(__tile1024i *dst, __tile1024i src0,
__tile1024i src1) {
dst->tile = _tile_dpbf16ps_internal(src0.row, src1.col, src0.col, dst->tile,
src0.tile, src1.tile);
}

#undef __DEFAULT_FN_ATTRS_TILE
#undef __DEFAULT_FN_ATTRS_INT8
#undef __DEFAULT_FN_ATTRS_BF16

#endif
#endif
