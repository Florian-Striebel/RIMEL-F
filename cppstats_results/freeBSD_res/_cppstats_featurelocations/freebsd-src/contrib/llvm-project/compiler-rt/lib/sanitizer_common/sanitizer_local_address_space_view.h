


























#if !defined(SANITIZER_LOCAL_ADDRES_SPACE_VIEW_H)
#define SANITIZER_LOCAL_ADDRES_SPACE_VIEW_H

namespace __sanitizer {
struct LocalAddressSpaceView {








template <typename T>
static const T *Load(const T *target_address, uptr num_elements = 1) {


return target_address;
}




















template <typename T>
static T *LoadWritable(T *target_address, uptr num_elements = 1) {


return target_address;
}
};
}

#endif
