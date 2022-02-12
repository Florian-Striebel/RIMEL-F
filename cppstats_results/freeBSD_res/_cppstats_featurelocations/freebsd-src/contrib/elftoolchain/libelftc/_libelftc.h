




























#if !defined(__LIBELFTC_H_)
#define __LIBELFTC_H_

#include <stdbool.h>

#include "_elftc.h"

struct _Elftc_Bfd_Target {
const char *bt_name;
unsigned int bt_type;
unsigned int bt_byteorder;
unsigned int bt_elfclass;
unsigned int bt_machine;
unsigned int bt_osabi;
};

extern struct _Elftc_Bfd_Target _libelftc_targets[];


struct vector_str {

size_t size;

size_t capacity;

char **container;
};

#define BUFFER_GROWFACTOR 1.618
#define BUFFER_GROW(x) (((x)+0.5)*BUFFER_GROWFACTOR)

#define ELFTC_FAILURE 0
#define ELFTC_ISDIGIT(C) (isdigit((C) & 0xFF))
#define ELFTC_SUCCESS 1

#define VECTOR_DEF_CAPACITY 8


#if defined(__cplusplus)
extern "C" {
#endif
char *cpp_demangle_ARM(const char *_org);
char *cpp_demangle_gnu2(const char *_org);
char *cpp_demangle_gnu3(const char *_org);
bool is_cpp_mangled_ARM(const char *_org);
bool is_cpp_mangled_gnu2(const char *_org);
bool is_cpp_mangled_gnu3(const char *_org);
unsigned int libelftc_hash_string(const char *);
void vector_str_dest(struct vector_str *_vec);
int vector_str_find(const struct vector_str *_vs, const char *_str,
size_t _len);
char *vector_str_get_flat(const struct vector_str *_vs, size_t *_len);
bool vector_str_init(struct vector_str *_vs);
bool vector_str_pop(struct vector_str *_vs);
bool vector_str_push(struct vector_str *_vs, const char *_str,
size_t _len);
bool vector_str_push_vector(struct vector_str *_dst,
struct vector_str *_org);
bool vector_str_push_vector_head(struct vector_str *_dst,
struct vector_str *_org);
char *vector_str_substr(const struct vector_str *_vs, size_t _begin,
size_t _end, size_t *_rlen);
#if defined(__cplusplus)
}
#endif

#endif
