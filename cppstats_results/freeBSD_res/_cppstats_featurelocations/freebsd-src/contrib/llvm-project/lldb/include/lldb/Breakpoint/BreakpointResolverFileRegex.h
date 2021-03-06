








#if !defined(LLDB_BREAKPOINT_BREAKPOINTRESOLVERFILEREGEX_H)
#define LLDB_BREAKPOINT_BREAKPOINTRESOLVERFILEREGEX_H

#include <set>
#include "lldb/Breakpoint/BreakpointResolver.h"
#include "lldb/Utility/ConstString.h"

namespace lldb_private {






class BreakpointResolverFileRegex : public BreakpointResolver {
public:
BreakpointResolverFileRegex(
const lldb::BreakpointSP &bkpt, RegularExpression regex,
const std::unordered_set<std::string> &func_name_set, bool exact_match);

static BreakpointResolver *
CreateFromStructuredData(const lldb::BreakpointSP &bkpt,
const StructuredData::Dictionary &options_dict,
Status &error);

StructuredData::ObjectSP SerializeToStructuredData() override;

~BreakpointResolverFileRegex() override = default;

Searcher::CallbackReturn SearchCallback(SearchFilter &filter,
SymbolContext &context,
Address *addr) override;

lldb::SearchDepth GetDepth() override;

void GetDescription(Stream *s) override;

void Dump(Stream *s) const override;

void AddFunctionName(const char *func_name);


static inline bool classof(const BreakpointResolverFileRegex *) {
return true;
}
static inline bool classof(const BreakpointResolver *V) {
return V->getResolverID() == BreakpointResolver::FileRegexResolver;
}

lldb::BreakpointResolverSP
CopyForBreakpoint(lldb::BreakpointSP &breakpoint) override;

protected:
friend class Breakpoint;
RegularExpression
m_regex;
bool m_exact_match;

std::unordered_set<std::string> m_function_names;



private:
BreakpointResolverFileRegex(const BreakpointResolverFileRegex &) = delete;
const BreakpointResolverFileRegex &
operator=(const BreakpointResolverFileRegex &) = delete;
};

}

#endif
