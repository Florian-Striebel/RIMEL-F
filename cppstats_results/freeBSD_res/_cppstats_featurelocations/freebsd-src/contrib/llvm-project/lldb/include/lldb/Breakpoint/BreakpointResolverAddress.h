







#if !defined(LLDB_BREAKPOINT_BREAKPOINTRESOLVERADDRESS_H)
#define LLDB_BREAKPOINT_BREAKPOINTRESOLVERADDRESS_H

#include "lldb/Breakpoint/BreakpointResolver.h"
#include "lldb/Core/ModuleSpec.h"

namespace lldb_private {






class BreakpointResolverAddress : public BreakpointResolver {
public:
BreakpointResolverAddress(const lldb::BreakpointSP &bkpt,
const Address &addr);

BreakpointResolverAddress(const lldb::BreakpointSP &bkpt,
const Address &addr,
const FileSpec &module_spec);

~BreakpointResolverAddress() override = default;

static BreakpointResolver *
CreateFromStructuredData(const lldb::BreakpointSP &bkpt,
const StructuredData::Dictionary &options_dict,
Status &error);

StructuredData::ObjectSP SerializeToStructuredData() override;

void ResolveBreakpoint(SearchFilter &filter) override;

void ResolveBreakpointInModules(SearchFilter &filter,
ModuleList &modules) override;

Searcher::CallbackReturn SearchCallback(SearchFilter &filter,
SymbolContext &context,
Address *addr) override;

lldb::SearchDepth GetDepth() override;

void GetDescription(Stream *s) override;

void Dump(Stream *s) const override;


static inline bool classof(const BreakpointResolverAddress *) { return true; }
static inline bool classof(const BreakpointResolver *V) {
return V->getResolverID() == BreakpointResolver::AddressResolver;
}

lldb::BreakpointResolverSP
CopyForBreakpoint(lldb::BreakpointSP &breakpoint) override;

protected:
Address m_addr;

lldb::addr_t m_resolved_addr;

FileSpec m_module_filespec;



private:
BreakpointResolverAddress(const BreakpointResolverAddress &) = delete;
const BreakpointResolverAddress &
operator=(const BreakpointResolverAddress &) = delete;
};

}

#endif
