












#if !defined(SANITIZER_SYMBOLIZER_MAC_H)
#define SANITIZER_SYMBOLIZER_MAC_H

#include "sanitizer_platform.h"
#if SANITIZER_MAC

#include "sanitizer_symbolizer_internal.h"

namespace __sanitizer {

class DlAddrSymbolizer final : public SymbolizerTool {
public:
bool SymbolizePC(uptr addr, SymbolizedStack *stack) override;
bool SymbolizeData(uptr addr, DataInfo *info) override;
};

class AtosSymbolizerProcess;

class AtosSymbolizer final : public SymbolizerTool {
public:
explicit AtosSymbolizer(const char *path, LowLevelAllocator *allocator);

bool SymbolizePC(uptr addr, SymbolizedStack *stack) override;
bool SymbolizeData(uptr addr, DataInfo *info) override;
void LateInitialize() override;

private:
AtosSymbolizerProcess *process_;
};

}

#endif

#endif
