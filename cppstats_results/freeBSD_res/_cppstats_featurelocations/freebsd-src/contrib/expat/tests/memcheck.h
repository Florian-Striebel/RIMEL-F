
































#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(XML_MEMCHECK_H)
#define XML_MEMCHECK_H 1



void *tracking_malloc(size_t size);
void tracking_free(void *ptr);
void *tracking_realloc(void *ptr, size_t size);





int tracking_report(void);

#endif

#if defined(__cplusplus)
}
#endif
