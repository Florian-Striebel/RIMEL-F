








#if !defined(_LIBCPP_FENV_H)
#define _LIBCPP_FENV_H









































#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#include_next <fenv.h>

#if defined(__cplusplus)

extern "C++" {

#if defined(feclearexcept)
#undef feclearexcept
#endif

#if defined(fegetexceptflag)
#undef fegetexceptflag
#endif


#if defined(feraiseexcept)
#undef feraiseexcept
#endif

#if defined(fesetexceptflag)
#undef fesetexceptflag
#endif


#if defined(fetestexcept)
#undef fetestexcept
#endif

#if defined(fegetround)
#undef fegetround
#endif

#if defined(fesetround)
#undef fesetround
#endif

#if defined(fegetenv)
#undef fegetenv
#endif

#if defined(feholdexcept)
#undef feholdexcept
#endif


#if defined(fesetenv)
#undef fesetenv
#endif

#if defined(feupdateenv)
#undef feupdateenv
#endif

}

#endif

#endif
