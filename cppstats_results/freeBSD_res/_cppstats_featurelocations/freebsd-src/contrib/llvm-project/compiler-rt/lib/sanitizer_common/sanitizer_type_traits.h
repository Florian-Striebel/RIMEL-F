











#if !defined(SANITIZER_TYPE_TRAITS_H)
#define SANITIZER_TYPE_TRAITS_H

namespace __sanitizer {

struct true_type {
static const bool value = true;
};

struct false_type {
static const bool value = false;
};










template <typename T, typename U>
struct is_same : public false_type {};

template <typename T>
struct is_same<T, T> : public true_type {};










template <bool B, class T, class F>
struct conditional {
using type = T;
};

template <class T, class F>
struct conditional<false, T, F> {
using type = F;
};

}

#endif
