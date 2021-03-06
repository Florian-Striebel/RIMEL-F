




















#if !defined(_BZLIB_PRIVATE_H)
#define _BZLIB_PRIVATE_H

#include <stdlib.h>

#if !defined(BZ_NO_STDIO)
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#endif

#include "bzlib.h"





#define BZ_VERSION "1.0.8, 13-Jul-2019"

typedef char Char;
typedef unsigned char Bool;
typedef unsigned char UChar;
typedef int Int32;
typedef unsigned int UInt32;
typedef short Int16;
typedef unsigned short UInt16;

#define True ((Bool)1)
#define False ((Bool)0)

#if !defined(__GNUC__)
#define __inline__
#endif

#if !defined(BZ_NO_STDIO)

extern void BZ2_bz__AssertH__fail ( int errcode );
#define AssertH(cond,errcode) { if (!(cond)) BZ2_bz__AssertH__fail ( errcode ); }


#if BZ_DEBUG
#define AssertD(cond,msg) { if (!(cond)) { fprintf ( stderr, "\n\nlibbzip2(debug build): internal error\n\t%s\n", msg );exit(1); }}





#else
#define AssertD(cond,msg)
#endif

#define VPrintf0(zf) fprintf(stderr,zf)

#define VPrintf1(zf,za1) fprintf(stderr,zf,za1)

#define VPrintf2(zf,za1,za2) fprintf(stderr,zf,za1,za2)

#define VPrintf3(zf,za1,za2,za3) fprintf(stderr,zf,za1,za2,za3)

#define VPrintf4(zf,za1,za2,za3,za4) fprintf(stderr,zf,za1,za2,za3,za4)

#define VPrintf5(zf,za1,za2,za3,za4,za5) fprintf(stderr,zf,za1,za2,za3,za4,za5)


#else

extern void bz_internal_error ( int errcode );
#define AssertH(cond,errcode) { if (!(cond)) bz_internal_error ( errcode ); }

#define AssertD(cond,msg) do { } while (0)
#define VPrintf0(zf) do { } while (0)
#define VPrintf1(zf,za1) do { } while (0)
#define VPrintf2(zf,za1,za2) do { } while (0)
#define VPrintf3(zf,za1,za2,za3) do { } while (0)
#define VPrintf4(zf,za1,za2,za3,za4) do { } while (0)
#define VPrintf5(zf,za1,za2,za3,za4,za5) do { } while (0)

#endif


#define BZALLOC(nnn) (strm->bzalloc)(strm->opaque,(nnn),1)
#define BZFREE(ppp) (strm->bzfree)(strm->opaque,(ppp))




#define BZ_HDR_B 0x42
#define BZ_HDR_Z 0x5a
#define BZ_HDR_h 0x68
#define BZ_HDR_0 0x30



#define BZ_MAX_ALPHA_SIZE 258
#define BZ_MAX_CODE_LEN 23

#define BZ_RUNA 0
#define BZ_RUNB 1

#define BZ_N_GROUPS 6
#define BZ_G_SIZE 50
#define BZ_N_ITERS 4

#define BZ_MAX_SELECTORS (2 + (900000 / BZ_G_SIZE))





extern Int32 BZ2_rNums[512];

#define BZ_RAND_DECLS Int32 rNToGo; Int32 rTPos



#define BZ_RAND_INIT_MASK s->rNToGo = 0; s->rTPos = 0



#define BZ_RAND_MASK ((s->rNToGo == 1) ? 1 : 0)

#define BZ_RAND_UPD_MASK if (s->rNToGo == 0) { s->rNToGo = BZ2_rNums[s->rTPos]; s->rTPos++; if (s->rTPos == 512) s->rTPos = 0; } s->rNToGo--;











extern UInt32 BZ2_crc32Table[256];

#define BZ_INITIALISE_CRC(crcVar) { crcVar = 0xffffffffL; }




#define BZ_FINALISE_CRC(crcVar) { crcVar = ~(crcVar); }




#define BZ_UPDATE_CRC(crcVar,cha) { crcVar = (crcVar << 8) ^ BZ2_crc32Table[(crcVar >> 24) ^ ((UChar)cha)]; }










#define BZ_M_IDLE 1
#define BZ_M_RUNNING 2
#define BZ_M_FLUSHING 3
#define BZ_M_FINISHING 4

#define BZ_S_OUTPUT 1
#define BZ_S_INPUT 2

#define BZ_N_RADIX 2
#define BZ_N_QSORT 12
#define BZ_N_SHELL 18
#define BZ_N_OVERSHOOT (BZ_N_RADIX + BZ_N_QSORT + BZ_N_SHELL + 2)






typedef
struct {

bz_stream* strm;



Int32 mode;
Int32 state;


UInt32 avail_in_expect;


UInt32* arr1;
UInt32* arr2;
UInt32* ftab;
Int32 origPtr;


UInt32* ptr;
UChar* block;
UInt16* mtfv;
UChar* zbits;


Int32 workFactor;


UInt32 state_in_ch;
Int32 state_in_len;
BZ_RAND_DECLS;


Int32 nblock;
Int32 nblockMAX;
Int32 numZ;
Int32 state_out_pos;


Int32 nInUse;
Bool inUse[256];
UChar unseqToSeq[256];


UInt32 bsBuff;
Int32 bsLive;


UInt32 blockCRC;
UInt32 combinedCRC;


Int32 verbosity;
Int32 blockNo;
Int32 blockSize100k;


Int32 nMTF;
Int32 mtfFreq [BZ_MAX_ALPHA_SIZE];
UChar selector [BZ_MAX_SELECTORS];
UChar selectorMtf[BZ_MAX_SELECTORS];

UChar len [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
Int32 code [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
Int32 rfreq [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];

UInt32 len_pack[BZ_MAX_ALPHA_SIZE][4];

}
EState;





extern void
BZ2_blockSort ( EState* );

extern void
BZ2_compressBlock ( EState*, Bool );

extern void
BZ2_bsInitWrite ( EState* );

extern void
BZ2_hbAssignCodes ( Int32*, UChar*, Int32, Int32, Int32 );

extern void
BZ2_hbMakeCodeLengths ( UChar*, Int32*, Int32, Int32 );





#define BZ_X_IDLE 1
#define BZ_X_OUTPUT 2

#define BZ_X_MAGIC_1 10
#define BZ_X_MAGIC_2 11
#define BZ_X_MAGIC_3 12
#define BZ_X_MAGIC_4 13
#define BZ_X_BLKHDR_1 14
#define BZ_X_BLKHDR_2 15
#define BZ_X_BLKHDR_3 16
#define BZ_X_BLKHDR_4 17
#define BZ_X_BLKHDR_5 18
#define BZ_X_BLKHDR_6 19
#define BZ_X_BCRC_1 20
#define BZ_X_BCRC_2 21
#define BZ_X_BCRC_3 22
#define BZ_X_BCRC_4 23
#define BZ_X_RANDBIT 24
#define BZ_X_ORIGPTR_1 25
#define BZ_X_ORIGPTR_2 26
#define BZ_X_ORIGPTR_3 27
#define BZ_X_MAPPING_1 28
#define BZ_X_MAPPING_2 29
#define BZ_X_SELECTOR_1 30
#define BZ_X_SELECTOR_2 31
#define BZ_X_SELECTOR_3 32
#define BZ_X_CODING_1 33
#define BZ_X_CODING_2 34
#define BZ_X_CODING_3 35
#define BZ_X_MTF_1 36
#define BZ_X_MTF_2 37
#define BZ_X_MTF_3 38
#define BZ_X_MTF_4 39
#define BZ_X_MTF_5 40
#define BZ_X_MTF_6 41
#define BZ_X_ENDHDR_2 42
#define BZ_X_ENDHDR_3 43
#define BZ_X_ENDHDR_4 44
#define BZ_X_ENDHDR_5 45
#define BZ_X_ENDHDR_6 46
#define BZ_X_CCRC_1 47
#define BZ_X_CCRC_2 48
#define BZ_X_CCRC_3 49
#define BZ_X_CCRC_4 50





#define MTFA_SIZE 4096
#define MTFL_SIZE 16





typedef
struct {

bz_stream* strm;


Int32 state;


UChar state_out_ch;
Int32 state_out_len;
Bool blockRandomised;
BZ_RAND_DECLS;


UInt32 bsBuff;
Int32 bsLive;


Int32 blockSize100k;
Bool smallDecompress;
Int32 currBlockNo;
Int32 verbosity;


Int32 origPtr;
UInt32 tPos;
Int32 k0;
Int32 unzftab[256];
Int32 nblock_used;
Int32 cftab[257];
Int32 cftabCopy[257];


UInt32 *tt;


UInt16 *ll16;
UChar *ll4;


UInt32 storedBlockCRC;
UInt32 storedCombinedCRC;
UInt32 calculatedBlockCRC;
UInt32 calculatedCombinedCRC;


Int32 nInUse;
Bool inUse[256];
Bool inUse16[16];
UChar seqToUnseq[256];


UChar mtfa [MTFA_SIZE];
Int32 mtfbase[256 / MTFL_SIZE];
UChar selector [BZ_MAX_SELECTORS];
UChar selectorMtf[BZ_MAX_SELECTORS];
UChar len [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];

Int32 limit [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
Int32 base [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
Int32 perm [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
Int32 minLens[BZ_N_GROUPS];


Int32 save_i;
Int32 save_j;
Int32 save_t;
Int32 save_alphaSize;
Int32 save_nGroups;
Int32 save_nSelectors;
Int32 save_EOB;
Int32 save_groupNo;
Int32 save_groupPos;
Int32 save_nextSym;
Int32 save_nblockMAX;
Int32 save_nblock;
Int32 save_es;
Int32 save_N;
Int32 save_curr;
Int32 save_zt;
Int32 save_zn;
Int32 save_zvec;
Int32 save_zj;
Int32 save_gSel;
Int32 save_gMinlen;
Int32* save_gLimit;
Int32* save_gBase;
Int32* save_gPerm;

}
DState;





#define BZ_GET_FAST(cccc) if (s->tPos >= (UInt32)100000 * (UInt32)s->blockSize100k) return True; s->tPos = s->tt[s->tPos]; cccc = (UChar)(s->tPos & 0xff); s->tPos >>= 8;






#define BZ_GET_FAST_C(cccc) if (c_tPos >= (UInt32)100000 * (UInt32)ro_blockSize100k) return True; c_tPos = c_tt[c_tPos]; cccc = (UChar)(c_tPos & 0xff); c_tPos >>= 8;






#define SET_LL4(i,n) { if (((i) & 0x1) == 0) s->ll4[(i) >> 1] = (s->ll4[(i) >> 1] & 0xf0) | (n); else s->ll4[(i) >> 1] = (s->ll4[(i) >> 1] & 0x0f) | ((n) << 4); }





#define GET_LL4(i) ((((UInt32)(s->ll4[(i) >> 1])) >> (((i) << 2) & 0x4)) & 0xF)


#define SET_LL(i,n) { s->ll16[i] = (UInt16)(n & 0x0000ffff); SET_LL4(i, n >> 16); }




#define GET_LL(i) (((UInt32)s->ll16[i]) | (GET_LL4(i) << 16))


#define BZ_GET_SMALL(cccc) if (s->tPos >= (UInt32)100000 * (UInt32)s->blockSize100k) return True; cccc = BZ2_indexIntoF ( s->tPos, s->cftab ); s->tPos = GET_LL(s->tPos);








extern Int32
BZ2_indexIntoF ( Int32, Int32* );

extern Int32
BZ2_decompress ( DState* );

extern void
BZ2_hbCreateDecodeTables ( Int32*, Int32*, Int32*, UChar*,
Int32, Int32, Int32 );


#endif




#if defined(BZ_NO_STDIO)
#if !defined(NULL)
#define NULL 0
#endif
#endif





