






















#if !defined(UTHASH_H)
#define UTHASH_H

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "mum.h"





#if defined(_MSC_VER)
#if _MSC_VER >= 1600 && defined(__cplusplus)
#define DECLTYPE(x) (decltype(x))
#else
#define NO_DECLTYPE
#define DECLTYPE(x)
#endif
#else
#define DECLTYPE(x) (__typeof(x))
#endif

#if defined(NO_DECLTYPE)
#define DECLTYPE_ASSIGN(dst,src) do { char **_da_dst = (char**)(&(dst)); *_da_dst = (char*)(src); } while(0)




#else
#define DECLTYPE_ASSIGN(dst,src) do { (dst) = DECLTYPE(dst)(src); } while(0)



#endif


#if defined(_MSC_VER)
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#else
#include <inttypes.h>
#endif

#define UTHASH_VERSION 1.9.8

#if !defined(uthash_fatal)
#define uthash_fatal(msg) exit(-1)
#endif
#if !defined(uthash_malloc)
#define uthash_malloc(sz) malloc(sz)
#endif
#if !defined(uthash_free)
#define uthash_free(ptr,sz) free(ptr)
#endif

#if !defined(uthash_noexpand_fyi)
#define uthash_noexpand_fyi(tbl)
#endif
#if !defined(uthash_expand_fyi)
#define uthash_expand_fyi(tbl)
#endif


#define HASH_INITIAL_NUM_BUCKETS 32
#define HASH_INITIAL_NUM_BUCKETS_LOG2 5
#define HASH_BKT_CAPACITY_THRESH 10


#define ELMT_FROM_HH(tbl,hhp) ((void*)(((char*)(hhp)) - ((tbl)->hho)))

#define HASH_FIND(hh,head,keyptr,keylen,out) do { unsigned _hf_bkt,_hf_hashv; out=NULL; if (head) { HASH_FCN(keyptr,keylen, (head)->hh.tbl->num_buckets, _hf_hashv, _hf_bkt); if (HASH_BLOOM_TEST((head)->hh.tbl, _hf_hashv)) { HASH_FIND_IN_BKT((head)->hh.tbl, hh, (head)->hh.tbl->buckets[ _hf_bkt ], keyptr,keylen,out); } } } while (0)












#if defined(HASH_BLOOM)
#define HASH_BLOOM_BITLEN (1ULL << HASH_BLOOM)
#define HASH_BLOOM_BYTELEN (HASH_BLOOM_BITLEN/8) + ((HASH_BLOOM_BITLEN%8) ? 1:0)
#define HASH_BLOOM_MAKE(tbl) do { (tbl)->bloom_nbits = HASH_BLOOM; (tbl)->bloom_bv = (uint8_t*)uthash_malloc(HASH_BLOOM_BYTELEN); if (!((tbl)->bloom_bv)) { uthash_fatal( "out of memory"); } memset((tbl)->bloom_bv, 0, HASH_BLOOM_BYTELEN); (tbl)->bloom_sig = HASH_BLOOM_SIGNATURE; } while (0)








#define HASH_BLOOM_FREE(tbl) do { uthash_free((tbl)->bloom_bv, HASH_BLOOM_BYTELEN); } while (0)




#define HASH_BLOOM_BITSET(bv,idx) (bv[(idx)/8] |= (1U << ((idx)%8)))
#define HASH_BLOOM_BITTEST(bv,idx) (bv[(idx)/8] & (1U << ((idx)%8)))

#define HASH_BLOOM_ADD(tbl,hashv) HASH_BLOOM_BITSET((tbl)->bloom_bv, (hashv & (uint32_t)((1ULL << (tbl)->bloom_nbits) - 1)))


#define HASH_BLOOM_TEST(tbl,hashv) HASH_BLOOM_BITTEST((tbl)->bloom_bv, (hashv & (uint32_t)((1ULL << (tbl)->bloom_nbits) - 1)))


#else
#define HASH_BLOOM_MAKE(tbl)
#define HASH_BLOOM_FREE(tbl)
#define HASH_BLOOM_ADD(tbl,hashv)
#define HASH_BLOOM_TEST(tbl,hashv) (1)
#define HASH_BLOOM_BYTELEN 0
#endif

#define HASH_MAKE_TABLE(hh,head) do { (head)->hh.tbl = (UT_hash_table*)uthash_malloc( sizeof(UT_hash_table)); if (!((head)->hh.tbl)) { uthash_fatal( "out of memory"); } memset((head)->hh.tbl, 0, sizeof(UT_hash_table)); (head)->hh.tbl->tail = &((head)->hh); (head)->hh.tbl->num_buckets = HASH_INITIAL_NUM_BUCKETS; (head)->hh.tbl->log2_num_buckets = HASH_INITIAL_NUM_BUCKETS_LOG2; (head)->hh.tbl->hho = (char*)(&(head)->hh) - (char*)(head); (head)->hh.tbl->buckets = (UT_hash_bucket*)uthash_malloc( HASH_INITIAL_NUM_BUCKETS*sizeof(struct UT_hash_bucket)); if (! (head)->hh.tbl->buckets) { uthash_fatal( "out of memory"); } memset((head)->hh.tbl->buckets, 0, HASH_INITIAL_NUM_BUCKETS*sizeof(struct UT_hash_bucket)); HASH_BLOOM_MAKE((head)->hh.tbl); (head)->hh.tbl->signature = HASH_SIGNATURE; } while(0)


















#define HASH_ADD(hh,head,fieldname,keylen_in,add) HASH_ADD_KEYPTR(hh,head,&((add)->fieldname),keylen_in,add)


#define HASH_REPLACE(hh,head,fieldname,keylen_in,add,replaced) do { replaced=NULL; HASH_FIND(hh,head,&((add)->fieldname),keylen_in,replaced); if (replaced!=NULL) { HASH_DELETE(hh,head,replaced); }; HASH_ADD(hh,head,fieldname,keylen_in,add); } while(0)









#define HASH_ADD_KEYPTR(hh,head,keyptr,keylen_in,add) do { unsigned _ha_bkt; (add)->hh.next = NULL; (add)->hh.key = (const char*)keyptr; (add)->hh.keylen = (unsigned)keylen_in; if (!(head)) { head = (add); (head)->hh.prev = NULL; HASH_MAKE_TABLE(hh,head); } else { (head)->hh.tbl->tail->next = (add); (add)->hh.prev = ELMT_FROM_HH((head)->hh.tbl, (head)->hh.tbl->tail); (head)->hh.tbl->tail = &((add)->hh); } (head)->hh.tbl->num_items++; (add)->hh.tbl = (head)->hh.tbl; HASH_FCN(keyptr,keylen_in, (head)->hh.tbl->num_buckets, (add)->hh.hashv, _ha_bkt); HASH_ADD_TO_BKT((head)->hh.tbl->buckets[_ha_bkt],&(add)->hh); HASH_BLOOM_ADD((head)->hh.tbl,(add)->hh.hashv); HASH_EMIT_KEY(hh,head,keyptr,keylen_in); HASH_FSCK(hh,head); } while(0)
























#define HASH_TO_BKT( hashv, num_bkts, bkt ) do { bkt = ((hashv) & ((num_bkts) - 1)); } while(0)
















#define HASH_DELETE(hh,head,delptr) do { unsigned _hd_bkt; struct UT_hash_handle *_hd_hh_del; if ( ((delptr)->hh.prev == NULL) && ((delptr)->hh.next == NULL) ) { uthash_free((head)->hh.tbl->buckets, (head)->hh.tbl->num_buckets*sizeof(struct UT_hash_bucket) ); HASH_BLOOM_FREE((head)->hh.tbl); uthash_free((head)->hh.tbl, sizeof(UT_hash_table)); head = NULL; } else { _hd_hh_del = &((delptr)->hh); if ((delptr) == ELMT_FROM_HH((head)->hh.tbl,(head)->hh.tbl->tail)) { (head)->hh.tbl->tail = (UT_hash_handle*)((ptrdiff_t)((delptr)->hh.prev) + (head)->hh.tbl->hho); } if ((delptr)->hh.prev) { ((UT_hash_handle*)((ptrdiff_t)((delptr)->hh.prev) + (head)->hh.tbl->hho))->next = (delptr)->hh.next; } else { DECLTYPE_ASSIGN(head,(delptr)->hh.next); } if (_hd_hh_del->next) { ((UT_hash_handle*)((ptrdiff_t)_hd_hh_del->next + (head)->hh.tbl->hho))->prev = _hd_hh_del->prev; } HASH_TO_BKT( _hd_hh_del->hashv, (head)->hh.tbl->num_buckets, _hd_bkt); HASH_DEL_IN_BKT(hh,(head)->hh.tbl->buckets[_hd_bkt], _hd_hh_del); (head)->hh.tbl->num_items--; } HASH_FSCK(hh,head); } while (0)




































#define HASH_FIND_STR(head,findstr,out) HASH_FIND(hh,head,findstr,strlen(findstr),out)

#define HASH_ADD_STR(head,strfield,add) HASH_ADD(hh,head,strfield,strlen(add->strfield),add)

#define HASH_REPLACE_STR(head,strfield,add,replaced) HASH_REPLACE(hh,head,strfield,strlen(add->strfield),add,replaced)

#define HASH_FIND_INT(head,findint,out) HASH_FIND(hh,head,findint,sizeof(int),out)

#define HASH_ADD_INT(head,intfield,add) HASH_ADD(hh,head,intfield,sizeof(int),add)

#define HASH_REPLACE_INT(head,intfield,add,replaced) HASH_REPLACE(hh,head,intfield,sizeof(int),add,replaced)

#define HASH_FIND_PTR(head,findptr,out) HASH_FIND(hh,head,findptr,sizeof(void *),out)

#define HASH_ADD_PTR(head,ptrfield,add) HASH_ADD(hh,head,ptrfield,sizeof(void *),add)

#define HASH_REPLACE_PTR(head,ptrfield,add) HASH_REPLACE(hh,head,ptrfield,sizeof(void *),add,replaced)

#define HASH_DEL(head,delptr) HASH_DELETE(hh,head,delptr)





#if defined(HASH_DEBUG)
#define HASH_OOPS(...) do { fprintf(stderr,__VA_ARGS__); exit(-1); } while (0)
#define HASH_FSCK(hh,head) do { unsigned _bkt_i; unsigned _count, _bkt_count; char *_prev; struct UT_hash_handle *_thh; if (head) { _count = 0; for( _bkt_i = 0; _bkt_i < (head)->hh.tbl->num_buckets; _bkt_i++) { _bkt_count = 0; _thh = (head)->hh.tbl->buckets[_bkt_i].hh_head; _prev = NULL; while (_thh) { if (_prev != (char*)(_thh->hh_prev)) { HASH_OOPS("invalid hh_prev %p, actual %p\n", _thh->hh_prev, _prev ); } _bkt_count++; _prev = (char*)(_thh); _thh = _thh->hh_next; } _count += _bkt_count; if ((head)->hh.tbl->buckets[_bkt_i].count != _bkt_count) { HASH_OOPS("invalid bucket count %d, actual %d\n", (head)->hh.tbl->buckets[_bkt_i].count, _bkt_count); } } if (_count != (head)->hh.tbl->num_items) { HASH_OOPS("invalid hh item count %d, actual %d\n", (head)->hh.tbl->num_items, _count ); } _count = 0; _prev = NULL; _thh = &(head)->hh; while (_thh) { _count++; if (_prev !=(char*)(_thh->prev)) { HASH_OOPS("invalid prev %p, actual %p\n", _thh->prev, _prev ); } _prev = (char*)ELMT_FROM_HH((head)->hh.tbl, _thh); _thh = ( _thh->next ? (UT_hash_handle*)((char*)(_thh->next) + (head)->hh.tbl->hho) : NULL ); } if (_count != (head)->hh.tbl->num_items) { HASH_OOPS("invalid app item count %d, actual %d\n", (head)->hh.tbl->num_items, _count ); } } } while (0)


















































#else
#define HASH_FSCK(hh,head)
#endif




#if defined(HASH_EMIT_KEYS)
#define HASH_EMIT_KEY(hh,head,keyptr,fieldlen) do { unsigned _klen = fieldlen; write(HASH_EMIT_KEYS, &_klen, sizeof(_klen)); write(HASH_EMIT_KEYS, keyptr, fieldlen); } while (0)





#else
#define HASH_EMIT_KEY(hh,head,keyptr,fieldlen)
#endif


#if defined(HASH_FUNCTION)
#define HASH_FCN HASH_FUNCTION
#else
#define HASH_FCN HASH_XX
#endif

#define XX_HASH_PRIME 2654435761U

#define HASH_XX(key,keylen,num_bkts,hashv,bkt) do { hashv = mum_hash (key, keylen, XX_HASH_PRIME); bkt = (hashv) & (num_bkts-1); } while (0)








#define HASH_KEYCMP(a,b,len) memcmp(a,b,len)


#define HASH_FIND_IN_BKT(tbl,hh,head,keyptr,keylen_in,out) do { if (head.hh_head) DECLTYPE_ASSIGN(out,ELMT_FROM_HH(tbl,head.hh_head)); else out=NULL; while (out) { if ((out)->hh.keylen == keylen_in) { if ((HASH_KEYCMP((out)->hh.key,keyptr,keylen_in)) == 0) break; } if ((out)->hh.hh_next) DECLTYPE_ASSIGN(out,ELMT_FROM_HH(tbl,(out)->hh.hh_next)); else out = NULL; } } while(0)













#define HASH_ADD_TO_BKT(head,addhh) do { head.count++; (addhh)->hh_next = head.hh_head; (addhh)->hh_prev = NULL; if (head.hh_head) { (head).hh_head->hh_prev = (addhh); } (head).hh_head=addhh; if (head.count >= ((head.expand_mult+1) * HASH_BKT_CAPACITY_THRESH) && (addhh)->tbl->noexpand != 1) { HASH_EXPAND_BUCKETS((addhh)->tbl); } } while(0)













#define HASH_DEL_IN_BKT(hh,head,hh_del) (head).count--; if ((head).hh_head == hh_del) { (head).hh_head = hh_del->hh_next; } if (hh_del->hh_prev) { hh_del->hh_prev->hh_next = hh_del->hh_next; } if (hh_del->hh_next) { hh_del->hh_next->hh_prev = hh_del->hh_prev; }








































#define HASH_EXPAND_BUCKETS(tbl) do { unsigned _he_bkt; unsigned _he_bkt_i; struct UT_hash_handle *_he_thh, *_he_hh_nxt; UT_hash_bucket *_he_new_buckets, *_he_newbkt; _he_new_buckets = (UT_hash_bucket*)uthash_malloc( 2 * tbl->num_buckets * sizeof(struct UT_hash_bucket)); if (!_he_new_buckets) { uthash_fatal( "out of memory"); } memset(_he_new_buckets, 0, 2 * tbl->num_buckets * sizeof(struct UT_hash_bucket)); tbl->ideal_chain_maxlen = (tbl->num_items >> (tbl->log2_num_buckets+1)) + ((tbl->num_items & ((tbl->num_buckets*2)-1)) ? 1 : 0); tbl->nonideal_items = 0; for(_he_bkt_i = 0; _he_bkt_i < tbl->num_buckets; _he_bkt_i++) { _he_thh = tbl->buckets[ _he_bkt_i ].hh_head; while (_he_thh) { _he_hh_nxt = _he_thh->hh_next; HASH_TO_BKT( _he_thh->hashv, tbl->num_buckets*2, _he_bkt); _he_newbkt = &(_he_new_buckets[ _he_bkt ]); if (++(_he_newbkt->count) > tbl->ideal_chain_maxlen) { tbl->nonideal_items++; _he_newbkt->expand_mult = _he_newbkt->count / tbl->ideal_chain_maxlen; } _he_thh->hh_prev = NULL; _he_thh->hh_next = _he_newbkt->hh_head; if (_he_newbkt->hh_head) _he_newbkt->hh_head->hh_prev = _he_thh; _he_newbkt->hh_head = _he_thh; _he_thh = _he_hh_nxt; } } uthash_free( tbl->buckets, tbl->num_buckets*sizeof(struct UT_hash_bucket) ); tbl->num_buckets *= 2; tbl->log2_num_buckets++; tbl->buckets = _he_new_buckets; tbl->ineff_expands = (tbl->nonideal_items > (tbl->num_items >> 1)) ? (tbl->ineff_expands+1) : 0; if (tbl->ineff_expands > 1) { tbl->noexpand=1; uthash_noexpand_fyi(tbl); } uthash_expand_fyi(tbl); } while(0)



















































#define HASH_SORT(head,cmpfcn) HASH_SRT(hh,head,cmpfcn)
#define HASH_SRT(hh,head,cmpfcn) do { unsigned _hs_i; unsigned _hs_looping,_hs_nmerges,_hs_insize,_hs_psize,_hs_qsize; struct UT_hash_handle *_hs_p, *_hs_q, *_hs_e, *_hs_list, *_hs_tail; if (head) { _hs_insize = 1; _hs_looping = 1; _hs_list = &((head)->hh); while (_hs_looping) { _hs_p = _hs_list; _hs_list = NULL; _hs_tail = NULL; _hs_nmerges = 0; while (_hs_p) { _hs_nmerges++; _hs_q = _hs_p; _hs_psize = 0; for ( _hs_i = 0; _hs_i < _hs_insize; _hs_i++ ) { _hs_psize++; _hs_q = (UT_hash_handle*)((_hs_q->next) ? ((void*)((char*)(_hs_q->next) + (head)->hh.tbl->hho)) : NULL); if (! (_hs_q) ) break; } _hs_qsize = _hs_insize; while ((_hs_psize > 0) || ((_hs_qsize > 0) && _hs_q )) { if (_hs_psize == 0) { _hs_e = _hs_q; _hs_q = (UT_hash_handle*)((_hs_q->next) ? ((void*)((char*)(_hs_q->next) + (head)->hh.tbl->hho)) : NULL); _hs_qsize--; } else if ( (_hs_qsize == 0) || !(_hs_q) ) { _hs_e = _hs_p; if (_hs_p){ _hs_p = (UT_hash_handle*)((_hs_p->next) ? ((void*)((char*)(_hs_p->next) + (head)->hh.tbl->hho)) : NULL); } _hs_psize--; } else if (( cmpfcn(DECLTYPE(head)(ELMT_FROM_HH((head)->hh.tbl,_hs_p)), DECLTYPE(head)(ELMT_FROM_HH((head)->hh.tbl,_hs_q))) ) <= 0) { _hs_e = _hs_p; if (_hs_p){ _hs_p = (UT_hash_handle*)((_hs_p->next) ? ((void*)((char*)(_hs_p->next) + (head)->hh.tbl->hho)) : NULL); } _hs_psize--; } else { _hs_e = _hs_q; _hs_q = (UT_hash_handle*)((_hs_q->next) ? ((void*)((char*)(_hs_q->next) + (head)->hh.tbl->hho)) : NULL); _hs_qsize--; } if ( _hs_tail ) { _hs_tail->next = ((_hs_e) ? ELMT_FROM_HH((head)->hh.tbl,_hs_e) : NULL); } else { _hs_list = _hs_e; } if (_hs_e) { _hs_e->prev = ((_hs_tail) ? ELMT_FROM_HH((head)->hh.tbl,_hs_tail) : NULL); } _hs_tail = _hs_e; } _hs_p = _hs_q; } if (_hs_tail){ _hs_tail->next = NULL; } if ( _hs_nmerges <= 1 ) { _hs_looping=0; (head)->hh.tbl->tail = _hs_tail; DECLTYPE_ASSIGN(head,ELMT_FROM_HH((head)->hh.tbl, _hs_list)); } _hs_insize *= 2; } HASH_FSCK(hh,head); } } while (0)



























































































#define HASH_SELECT(hh_dst, dst, hh_src, src, cond) do { unsigned _src_bkt, _dst_bkt; void *_last_elt=NULL, *_elt; UT_hash_handle *_src_hh, *_dst_hh, *_last_elt_hh=NULL; ptrdiff_t _dst_hho = ((char*)(&(dst)->hh_dst) - (char*)(dst)); if (src) { for(_src_bkt=0; _src_bkt < (src)->hh_src.tbl->num_buckets; _src_bkt++) { for(_src_hh = (src)->hh_src.tbl->buckets[_src_bkt].hh_head; _src_hh; _src_hh = _src_hh->hh_next) { _elt = ELMT_FROM_HH((src)->hh_src.tbl, _src_hh); if (cond(_elt)) { _dst_hh = (UT_hash_handle*)(((char*)_elt) + _dst_hho); _dst_hh->key = _src_hh->key; _dst_hh->keylen = _src_hh->keylen; _dst_hh->hashv = _src_hh->hashv; _dst_hh->prev = _last_elt; _dst_hh->next = NULL; if (_last_elt_hh) { _last_elt_hh->next = _elt; } if (!dst) { DECLTYPE_ASSIGN(dst,_elt); HASH_MAKE_TABLE(hh_dst,dst); } else { _dst_hh->tbl = (dst)->hh_dst.tbl; } HASH_TO_BKT(_dst_hh->hashv, _dst_hh->tbl->num_buckets, _dst_bkt); HASH_ADD_TO_BKT(_dst_hh->tbl->buckets[_dst_bkt],_dst_hh); (dst)->hh_dst.tbl->num_items++; _last_elt = _elt; _last_elt_hh = _dst_hh; } } } } HASH_FSCK(hh_dst,dst); } while (0)





































#define HASH_CLEAR(hh,head) do { if (head) { uthash_free((head)->hh.tbl->buckets, (head)->hh.tbl->num_buckets*sizeof(struct UT_hash_bucket)); HASH_BLOOM_FREE((head)->hh.tbl); uthash_free((head)->hh.tbl, sizeof(UT_hash_table)); (head)=NULL; } } while(0)










#define HASH_OVERHEAD(hh,head) (size_t)((((head)->hh.tbl->num_items * sizeof(UT_hash_handle)) + ((head)->hh.tbl->num_buckets * sizeof(UT_hash_bucket)) + (sizeof(UT_hash_table)) + (HASH_BLOOM_BYTELEN)))





#if defined(NO_DECLTYPE)
#define HASH_ITER(hh,head,el,tmp) for((el)=(head), (*(char**)(&(tmp)))=(char*)((head)?(head)->hh.next:NULL); el; (el)=(tmp),(*(char**)(&(tmp)))=(char*)((tmp)?(tmp)->hh.next:NULL))


#else
#define HASH_ITER(hh,head,el,tmp) for((el)=(head),(tmp)=DECLTYPE(el)((head)?(head)->hh.next:NULL); el; (el)=(tmp),(tmp)=DECLTYPE(el)((tmp)?(tmp)->hh.next:NULL))


#endif


#define HASH_COUNT(head) HASH_CNT(hh,head)
#define HASH_CNT(hh,head) ((head)?((head)->hh.tbl->num_items):0)

typedef struct UT_hash_bucket {
struct UT_hash_handle *hh_head;
unsigned count;













unsigned expand_mult;

} UT_hash_bucket;


#define HASH_SIGNATURE 0xa0111fe1
#define HASH_BLOOM_SIGNATURE 0xb12220f2

typedef struct UT_hash_table {
UT_hash_bucket *buckets;
unsigned num_buckets, log2_num_buckets;
unsigned num_items;
struct UT_hash_handle *tail;
ptrdiff_t hho;



unsigned ideal_chain_maxlen;




unsigned nonideal_items;







unsigned ineff_expands, noexpand;

uint32_t signature;
#if defined(HASH_BLOOM)
uint32_t bloom_sig;
uint8_t *bloom_bv;
char bloom_nbits;
#endif

} UT_hash_table;

typedef struct UT_hash_handle {
struct UT_hash_table *tbl;
void *prev;
void *next;
struct UT_hash_handle *hh_prev;
struct UT_hash_handle *hh_next;
const void *key;
unsigned keylen;
unsigned hashv;
} UT_hash_handle;

#endif
