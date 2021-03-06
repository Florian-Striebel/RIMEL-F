










#if !defined(SANITIZER_STACKTRACE_PRINTER_H)
#define SANITIZER_STACKTRACE_PRINTER_H

#include "sanitizer_common.h"
#include "sanitizer_symbolizer.h"

namespace __sanitizer {






























void RenderFrame(InternalScopedString *buffer, const char *format, int frame_no,
uptr address, const AddressInfo *info, bool vs_style,
const char *strip_path_prefix = "",
const char *strip_func_prefix = "");

bool RenderNeedsSymbolization(const char *format);

void RenderSourceLocation(InternalScopedString *buffer, const char *file,
int line, int column, bool vs_style,
const char *strip_path_prefix);

void RenderModuleLocation(InternalScopedString *buffer, const char *module,
uptr offset, ModuleArch arch,
const char *strip_path_prefix);





void RenderData(InternalScopedString *buffer, const char *format,
const DataInfo *DI, const char *strip_path_prefix = "");

}

#endif
