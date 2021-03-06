























#if !defined(_BLOCK_H_)
#define _BLOCK_H_

#if !defined(BLOCK_EXPORT)
#if defined(__cplusplus)
#define BLOCK_EXPORT extern "C"
#else
#define BLOCK_EXPORT extern
#endif
#endif

#if defined(__cplusplus)
extern "C" {
#endif





BLOCK_EXPORT void *_Block_copy(const void *aBlock);


BLOCK_EXPORT void _Block_release(const void *aBlock);

#if defined(__cplusplus)
}
#endif



#define Block_copy(...) ((__typeof(__VA_ARGS__))_Block_copy((const void *)(__VA_ARGS__)))
#define Block_release(...) _Block_release((const void *)(__VA_ARGS__))


#endif
