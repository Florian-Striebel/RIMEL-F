











#if !defined(SANITIZER_SYMBOLIZER_LIBBACKTRACE_H)
#define SANITIZER_SYMBOLIZER_LIBBACKTRACE_H

#include "sanitizer_platform.h"
#include "sanitizer_common.h"
#include "sanitizer_allocator_internal.h"
#include "sanitizer_symbolizer_internal.h"

#if !defined(SANITIZER_LIBBACKTRACE)
#define SANITIZER_LIBBACKTRACE 0
#endif

#if !defined(SANITIZER_CP_DEMANGLE)
#define SANITIZER_CP_DEMANGLE 0
#endif

namespace __sanitizer {

class LibbacktraceSymbolizer final : public SymbolizerTool {
public:
static LibbacktraceSymbolizer *get(LowLevelAllocator *alloc);

bool SymbolizePC(uptr addr, SymbolizedStack *stack) override;

bool SymbolizeData(uptr addr, DataInfo *info) override;


const char *Demangle(const char *name) override;

private:
explicit LibbacktraceSymbolizer(void *state) : state_(state) {}

void *state_;
};

}
#endif
