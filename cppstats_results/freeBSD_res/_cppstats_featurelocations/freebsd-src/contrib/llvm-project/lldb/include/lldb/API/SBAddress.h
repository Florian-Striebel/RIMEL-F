







#if !defined(LLDB_API_SBADDRESS_H)
#define LLDB_API_SBADDRESS_H

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBModule.h"

namespace lldb {

class LLDB_API SBAddress {
public:
SBAddress();

SBAddress(const lldb::SBAddress &rhs);

SBAddress(lldb::SBSection section, lldb::addr_t offset);


SBAddress(lldb::addr_t load_addr, lldb::SBTarget &target);

~SBAddress();

const lldb::SBAddress &operator=(const lldb::SBAddress &rhs);

explicit operator bool() const;



bool operator!=(const SBAddress &rhs) const;

bool IsValid() const;

void Clear();

addr_t GetFileAddress() const;

addr_t GetLoadAddress(const lldb::SBTarget &target) const;

void SetAddress(lldb::SBSection section, lldb::addr_t offset);

void SetLoadAddress(lldb::addr_t load_addr, lldb::SBTarget &target);
bool OffsetAddress(addr_t offset);

bool GetDescription(lldb::SBStream &description);







lldb::SBSymbolContext GetSymbolContext(uint32_t resolve_scope);












lldb::SBSection GetSection();

lldb::addr_t GetOffset();

lldb::SBModule GetModule();

lldb::SBCompileUnit GetCompileUnit();

lldb::SBFunction GetFunction();

lldb::SBBlock GetBlock();

lldb::SBSymbol GetSymbol();

lldb::SBLineEntry GetLineEntry();

protected:
friend class SBBlock;
friend class SBBreakpoint;
friend class SBBreakpointLocation;
friend class SBFrame;
friend class SBFunction;
friend class SBLineEntry;
friend class SBInstruction;
friend class SBModule;
friend class SBSection;
friend class SBSymbol;
friend class SBSymbolContext;
friend class SBTarget;
friend class SBThread;
friend class SBThreadPlan;
friend class SBValue;
friend class SBQueueItem;

lldb_private::Address *operator->();

const lldb_private::Address *operator->() const;

friend bool LLDB_API operator==(const SBAddress &lhs, const SBAddress &rhs);

lldb_private::Address *get();

lldb_private::Address &ref();

const lldb_private::Address &ref() const;

SBAddress(const lldb_private::Address &address);

void SetAddress(const lldb_private::Address &address);

private:
std::unique_ptr<lldb_private::Address> m_opaque_up;
};

bool LLDB_API operator==(const SBAddress &lhs, const SBAddress &rhs);

}

#endif
