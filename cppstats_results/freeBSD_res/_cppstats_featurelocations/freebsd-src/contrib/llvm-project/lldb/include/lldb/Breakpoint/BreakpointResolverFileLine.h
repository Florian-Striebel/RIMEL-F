







#if !defined(LLDB_BREAKPOINT_BREAKPOINTRESOLVERFILELINE_H)
#define LLDB_BREAKPOINT_BREAKPOINTRESOLVERFILELINE_H

#include "lldb/Breakpoint/BreakpointResolver.h"
#include "lldb/Core/SourceLocationSpec.h"

namespace lldb_private {






class BreakpointResolverFileLine : public BreakpointResolver {
public:
BreakpointResolverFileLine(const lldb::BreakpointSP &bkpt,
lldb::addr_t offset, bool skip_prologue,
const SourceLocationSpec &location_spec);

static BreakpointResolver *
CreateFromStructuredData(const lldb::BreakpointSP &bkpt,
const StructuredData::Dictionary &data_dict,
Status &error);

StructuredData::ObjectSP SerializeToStructuredData() override;

~BreakpointResolverFileLine() override = default;

Searcher::CallbackReturn SearchCallback(SearchFilter &filter,
SymbolContext &context,
Address *addr) override;

lldb::SearchDepth GetDepth() override;

void GetDescription(Stream *s) override;

void Dump(Stream *s) const override;


static inline bool classof(const BreakpointResolverFileLine *) {
return true;
}
static inline bool classof(const BreakpointResolver *V) {
return V->getResolverID() == BreakpointResolver::FileLineResolver;
}

lldb::BreakpointResolverSP
CopyForBreakpoint(lldb::BreakpointSP &breakpoint) override;

protected:
void FilterContexts(SymbolContextList &sc_list, bool is_relative);

friend class Breakpoint;
SourceLocationSpec m_location_spec;
bool m_skip_prologue;

private:
BreakpointResolverFileLine(const BreakpointResolverFileLine &) = delete;
const BreakpointResolverFileLine &
operator=(const BreakpointResolverFileLine &) = delete;
};

}

#endif
