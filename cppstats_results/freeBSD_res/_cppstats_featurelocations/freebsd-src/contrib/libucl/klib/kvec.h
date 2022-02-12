
















































#if !defined(AC_KVEC_H)
#define AC_KVEC_H

#include <stdlib.h>

#define kv_roundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))

#define kvec_t(type) struct { size_t n, m; type *a; }
#define kv_init(v) ((v).n = (v).m = 0, (v).a = 0)
#define kv_destroy(v) free((v).a)
#define kv_A(v, i) ((v).a[(i)])
#define kv_pop(v) ((v).a[--(v).n])
#define kv_size(v) ((v).n)
#define kv_max(v) ((v).m)

#define kv_resize_safe(type, v, s, el) do { type *_tp = (type*)realloc((v).a, sizeof(type) * (s)); if (_tp == NULL) { goto el; } else { (v).a = _tp; (v).m = (s); } } while (0)









#define kv_grow_factor 1.5
#define kv_grow_safe(type, v, el) do { size_t _ts = ((v).m > 1 ? (v).m * kv_grow_factor : 2); type *_tp = (type*)realloc((v).a, sizeof(type) * _ts); if (_tp == NULL) { goto el; } else { (v).a = _tp; (v).m = _ts; } } while (0)










#define kv_copy_safe(type, v1, v0, el) do { if ((v1).m < (v0).n) kv_resize_safe(type, v1, (v0).n, el); (v1).n = (v0).n; memcpy((v1).a, (v0).a, sizeof(type) * (v0).n); } while (0)





#define kv_push_safe(type, v, x, el) do { if ((v).n == (v).m) { kv_grow_safe(type, v, el); } (v).a[(v).n++] = (x); } while (0)






#define kv_prepend_safe(type, v, x, el) do { if ((v).n == (v).m) { kv_grow_safe(type, v, el); } memmove((v).a + 1, (v).a, sizeof(type) * (v).n); (v).a[0] = (x); (v).n ++; } while (0)








#define kv_concat_safe(type, v1, v0, el) do { if ((v1).m < (v0).n + (v1).n) kv_resize_safe(type, v1, (v0).n + (v1).n, el); memcpy((v1).a + (v1).n, (v0).a, sizeof(type) * (v0).n); (v1).n = (v0).n + (v1).n; } while (0)






#define kv_del(type, v, i) do { if ((i) < (v).n) { memmove((v).a + (i), (v).a + ((i) + 1), sizeof(type) * ((v).n - (i) - 1)); (v).n --; } } while (0)











#define kv_resize(type, v, s) ((v).m = (s), (v).a = (type*)realloc((v).a, sizeof(type) * (v).m))

#define kv_grow(type, v) ((v).m = ((v).m > 1 ? (v).m * kv_grow_factor : 2), (v).a = (type*)realloc((v).a, sizeof(type) * (v).m))


#define kv_copy(type, v1, v0) do { if ((v1).m < (v0).n) kv_resize(type, v1, (v0).n); (v1).n = (v0).n; memcpy((v1).a, (v0).a, sizeof(type) * (v0).n); } while (0)





#define kv_push(type, v, x) do { if ((v).n == (v).m) { kv_grow(type, v); } (v).a[(v).n++] = (x); } while (0)






#define kv_prepend(type, v, x) do { if ((v).n == (v).m) { kv_grow(type, v); } memmove((v).a + 1, (v).a, sizeof(type) * (v).n); (v).a[0] = (x); (v).n ++; } while (0)








#define kv_concat(type, v1, v0) do { if ((v1).m < (v0).n + (v1).n) kv_resize(type, v1, (v0).n + (v1).n); memcpy((v1).a + (v1).n, (v0).a, sizeof(type) * (v0).n); (v1).n = (v0).n + (v1).n; } while (0)





#endif
