#if !defined(JEMALLOC_INTERNAL_PROF_TYPES_H)
#define JEMALLOC_INTERNAL_PROF_TYPES_H

typedef struct prof_bt_s prof_bt_t;
typedef struct prof_accum_s prof_accum_t;
typedef struct prof_cnt_s prof_cnt_t;
typedef struct prof_tctx_s prof_tctx_t;
typedef struct prof_gctx_s prof_gctx_t;
typedef struct prof_tdata_s prof_tdata_t;


#if defined(JEMALLOC_PROF)
#define PROF_PREFIX_DEFAULT "jeprof"
#else
#define PROF_PREFIX_DEFAULT ""
#endif
#define LG_PROF_SAMPLE_DEFAULT 19
#define LG_PROF_INTERVAL_DEFAULT -1






#define PROF_BT_MAX 128


#define PROF_CKH_MINITEMS 64


#define PROF_DUMP_BUFSIZE 65536


#define PROF_PRINTF_BUFSIZE 128





#define PROF_NCTX_LOCKS 1024





#define PROF_NTDATA_LOCKS 256





#define PROF_TDATA_STATE_REINCARNATED ((prof_tdata_t *)(uintptr_t)1)
#define PROF_TDATA_STATE_PURGATORY ((prof_tdata_t *)(uintptr_t)2)
#define PROF_TDATA_STATE_MAX PROF_TDATA_STATE_PURGATORY

#endif
