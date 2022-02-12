










#if !defined(__CET_H)
#define __CET_H

#if defined(__ASSEMBLER__)

#if !defined(__CET__)
#define _CET_ENDBR
#endif

#if defined(__CET__)

#if defined(__LP64__)
#if __CET__ & 0x1
#define _CET_ENDBR endbr64
#else
#define _CET_ENDBR
#endif
#else
#if __CET__ & 0x1
#define _CET_ENDBR endbr32
#else
#define _CET_ENDBR
#endif
#endif


#if defined(__LP64__)
#define __PROPERTY_ALIGN 3
#else
#define __PROPERTY_ALIGN 2
#endif

.pushsection ".note.gnu.property", "a"
.p2align __PROPERTY_ALIGN
.long 1f - 0f
.long 4f - 1f

.long 5
0:
.asciz "GNU"
1:
.p2align __PROPERTY_ALIGN

.long 0xc0000002
.long 3f - 2f
2:

.long __CET__
3:
.p2align __PROPERTY_ALIGN
4:
.popsection
#endif
#endif
#endif
