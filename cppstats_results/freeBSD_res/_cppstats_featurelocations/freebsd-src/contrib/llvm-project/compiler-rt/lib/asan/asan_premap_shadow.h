













#if !defined(ASAN_PREMAP_SHADOW_H)
#define ASAN_PREMAP_SHADOW_H

#if ASAN_PREMAP_SHADOW
namespace __asan {

uptr PremapShadowSize();
bool PremapShadowFailed();
}
#endif

extern "C" INTERFACE_ATTRIBUTE void __asan_shadow();
extern "C" decltype(__asan_shadow)* __asan_premap_shadow();

#endif
