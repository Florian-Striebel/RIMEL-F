


































#if !defined(BC_VECTOR_H)
#define BC_VECTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <status.h>


#define BC_VEC_INVALID_IDX (SIZE_MAX)



#define BC_VEC_START_CAP (UINTMAX_C(1)<<5)


typedef unsigned char uchar;






typedef void (*BcVecFree)(void *ptr);


struct BcId;

#if BC_LONG_BIT >= 64


typedef uint32_t BcSize;

#else


typedef uint16_t BcSize;

#endif


typedef enum BcDtorType {


BC_DTOR_NONE,


BC_DTOR_VEC,


BC_DTOR_NUM,

#if !BC_ENABLE_LIBRARY

#if !defined(NDEBUG)


BC_DTOR_FUNC,

#endif


BC_DTOR_SLAB,


BC_DTOR_CONST,


BC_DTOR_RESULT,

#if BC_ENABLE_HISTORY


BC_DTOR_HISTORY_STRING,

#endif
#else


BC_DTOR_BCL_NUM,

#endif

} BcDtorType;


typedef struct BcVec {



char *restrict v;


size_t len;



size_t cap;


BcSize size;


BcSize dtor;

} BcVec;







void bc_vec_init(BcVec *restrict v, size_t esize, BcDtorType dtor);







void bc_vec_expand(BcVec *restrict v, size_t req);






void bc_vec_grow(BcVec *restrict v, size_t n);







void bc_vec_npop(BcVec *restrict v, size_t n);









void bc_vec_npopAt(BcVec *restrict v, size_t n, size_t idx);







void bc_vec_push(BcVec *restrict v, const void *data);







void bc_vec_npush(BcVec *restrict v, size_t n, const void *data);








void* bc_vec_pushEmpty(BcVec *restrict v);







void bc_vec_pushByte(BcVec *restrict v, uchar data);








void bc_vec_pushIndex(BcVec *restrict v, size_t idx);










void bc_vec_pushAt(BcVec *restrict v, const void *data, size_t idx);









void bc_vec_string(BcVec *restrict v, size_t len, const char *restrict str);







void bc_vec_concat(BcVec *restrict v, const char *restrict str);





void bc_vec_empty(BcVec *restrict v);

#if BC_ENABLE_HISTORY








void bc_vec_replaceAt(BcVec *restrict v, size_t idx, const void *data);

#endif








void* bc_vec_item(const BcVec *restrict v, size_t idx);








void* bc_vec_item_rev(const BcVec *restrict v, size_t idx);





void bc_vec_clear(BcVec *restrict v);





void bc_vec_free(void *vec);












bool bc_map_insert(BcVec *restrict v, const char *name,
size_t idx, size_t *restrict i);









size_t bc_map_index(const BcVec *restrict v, const char *name);

#if DC_ENABLED







const char* bc_map_name(const BcVec *restrict v, size_t idx);

#endif





#define bc_vec_pop(v) (bc_vec_npop((v), 1))





#define bc_vec_popAll(v) (bc_vec_npop((v), (v)->len))






#define bc_vec_top(v) (bc_vec_item_rev((v), 0))





#define bc_map_init(v) (bc_vec_init((v), sizeof(BcId), BC_DTOR_NONE))


extern const BcVecFree bc_vec_dtors[];

#if !BC_ENABLE_LIBRARY


#define BC_SLAB_SIZE (4096)


typedef struct BcSlab {


char *s;


size_t len;

} BcSlab;





void bc_slab_free(void *slab);





void bc_slabvec_init(BcVec *restrict v);







char* bc_slabvec_strdup(BcVec *restrict v, const char *str);

#if BC_ENABLED





void bc_slabvec_undo(BcVec *restrict v, size_t len);

#endif






void bc_slabvec_clear(BcVec *restrict v);

#if BC_DEBUG_CODE





void bc_slabvec_print(BcVec *v, const char *func);

#endif


#define bc_slabvec_free bc_vec_free

#if !defined(_WIN32)








#define bc_strcpy(d, l, s) strcpy(d, s)

#else








#define bc_strcpy(d, l, s) strcpy_s(d, l, s)

#endif

#endif

#endif
