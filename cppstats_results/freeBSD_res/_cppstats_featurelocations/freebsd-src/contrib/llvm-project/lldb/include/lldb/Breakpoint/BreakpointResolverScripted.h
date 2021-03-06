







#if !defined(LLDB_BREAKPOINT_BREAKPOINTRESOLVERSCRIPTED_H)
#define LLDB_BREAKPOINT_BREAKPOINTRESOLVERSCRIPTED_H

#include "lldb/lldb-forward.h"
#include "lldb/Breakpoint/BreakpointResolver.h"
#include "lldb/Core/ModuleSpec.h"


namespace lldb_private {






class BreakpointResolverScripted : public BreakpointResolver {
public:
BreakpointResolverScripted(const lldb::BreakpointSP &bkpt,
const llvm::StringRef class_name,
lldb::SearchDepth depth,
StructuredDataImpl *args_data);

~BreakpointResolverScripted() override = default;

static BreakpointResolver *
CreateFromStructuredData(const lldb::BreakpointSP &bkpt,
const StructuredData::Dictionary &options_dict,
Status &error);

StructuredData::ObjectSP SerializeToStructuredData() override;

Searcher::CallbackReturn SearchCallback(SearchFilter &filter,
SymbolContext &context,
Address *addr) override;

lldb::SearchDepth GetDepth() override;

void GetDescription(Stream *s) override;

void Dump(Stream *s) const override;


static inline bool classof(const BreakpointResolverScripted *) { return true; }
static inline bool classof(const BreakpointResolver *V) {
return V->getResolverID() == BreakpointResolver::PythonResolver;
}

lldb::BreakpointResolverSP
CopyForBreakpoint(lldb::BreakpointSP &breakpoint) override;

protected:
void NotifyBreakpointSet() override;
private:
void CreateImplementationIfNeeded(lldb::BreakpointSP bkpt);
ScriptInterpreter *GetScriptInterpreter();

std::string m_class_name;
lldb::SearchDepth m_depth;
StructuredDataImpl *m_args_ptr;



StructuredData::GenericSP m_implementation_sp;

BreakpointResolverScripted(const BreakpointResolverScripted &) = delete;
const BreakpointResolverScripted &
operator=(const BreakpointResolverScripted &) = delete;
};

}

#endif
