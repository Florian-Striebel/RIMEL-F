







#if !defined(LLDB_TARGET_STACKID_H)
#define LLDB_TARGET_STACKID_H

#include "lldb/Core/AddressRange.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class StackID {
public:

StackID()

{}

explicit StackID(lldb::addr_t pc, lldb::addr_t cfa,
SymbolContextScope *symbol_scope)
: m_pc(pc), m_cfa(cfa), m_symbol_scope(symbol_scope) {}

StackID(const StackID &rhs)
: m_pc(rhs.m_pc), m_cfa(rhs.m_cfa), m_symbol_scope(rhs.m_symbol_scope) {}

~StackID() = default;

lldb::addr_t GetPC() const { return m_pc; }

lldb::addr_t GetCallFrameAddress() const { return m_cfa; }

SymbolContextScope *GetSymbolContextScope() const { return m_symbol_scope; }

void SetSymbolContextScope(SymbolContextScope *symbol_scope) {
m_symbol_scope = symbol_scope;
}

void Clear() {
m_pc = LLDB_INVALID_ADDRESS;
m_cfa = LLDB_INVALID_ADDRESS;
m_symbol_scope = nullptr;
}

bool IsValid() const {
return m_pc != LLDB_INVALID_ADDRESS || m_cfa != LLDB_INVALID_ADDRESS;
}

void Dump(Stream *s);


const StackID &operator=(const StackID &rhs) {
if (this != &rhs) {
m_pc = rhs.m_pc;
m_cfa = rhs.m_cfa;
m_symbol_scope = rhs.m_symbol_scope;
}
return *this;
}

protected:
friend class StackFrame;

void SetPC(lldb::addr_t pc) { m_pc = pc; }

void SetCFA(lldb::addr_t cfa) { m_cfa = cfa; }

lldb::addr_t m_pc =
LLDB_INVALID_ADDRESS;




lldb::addr_t m_cfa =
LLDB_INVALID_ADDRESS;



SymbolContextScope *m_symbol_scope =
nullptr;






};

bool operator==(const StackID &lhs, const StackID &rhs);
bool operator!=(const StackID &lhs, const StackID &rhs);


bool operator<(const StackID &lhs, const StackID &rhs);

}

#endif
