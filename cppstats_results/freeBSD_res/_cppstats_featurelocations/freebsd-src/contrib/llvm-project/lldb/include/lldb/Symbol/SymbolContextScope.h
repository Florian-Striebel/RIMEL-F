







#if !defined(LLDB_SYMBOL_SYMBOLCONTEXTSCOPE_H)
#define LLDB_SYMBOL_SYMBOLCONTEXTSCOPE_H

#include "lldb/lldb-private.h"

namespace lldb_private {

















































class SymbolContextScope {
public:
virtual ~SymbolContextScope() = default;









virtual void CalculateSymbolContext(SymbolContext *sc) = 0;

virtual lldb::ModuleSP CalculateSymbolContextModule() {
return lldb::ModuleSP();
}

virtual CompileUnit *CalculateSymbolContextCompileUnit() { return nullptr; }

virtual Function *CalculateSymbolContextFunction() { return nullptr; }

virtual Block *CalculateSymbolContextBlock() { return nullptr; }

virtual Symbol *CalculateSymbolContextSymbol() { return nullptr; }









virtual void DumpSymbolContext(Stream *s) = 0;
};

}

#endif
