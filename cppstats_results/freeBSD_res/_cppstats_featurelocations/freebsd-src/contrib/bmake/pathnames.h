

































#if HAVE_CONFIG_H
#include "config.h"
#endif
#if !defined(MAKE_NATIVE)
#if HAVE_NBTOOL_CONFIG_H
#include "nbtool_config.h"
#endif
#endif
#if defined(HAVE_PATHS_H)
#include <paths.h>
#endif

#define _PATH_OBJDIR "obj"
#define _PATH_OBJDIRPREFIX "/usr/obj"
#if !defined(_PATH_DEFSHELLDIR)
#define _PATH_DEFSHELLDIR "/bin"
#endif
#define _PATH_DEFSYSMK "sys.mk"
#define _path_defsyspath "/usr/share/mk:/usr/local/share/mk:/opt/share/mk"
#if !defined(_PATH_DEFSYSPATH)
#if defined(_PATH_PREFIX_SYSPATH)
#define _PATH_DEFSYSPATH _PATH_PREFIX_SYSPATH ":" _path_defsyspath
#else
#define _PATH_DEFSYSPATH _path_defsyspath
#endif
#endif
#if !defined(_PATH_TMP)
#define _PATH_TMP "/tmp/"
#endif
