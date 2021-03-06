







#if !defined(LLDB_BREAKPOINT_BREAKPOINTRESOLVER_H)
#define LLDB_BREAKPOINT_BREAKPOINTRESOLVER_H

#include "lldb/Breakpoint/Breakpoint.h"
#include "lldb/Core/Address.h"
#include "lldb/Core/SearchFilter.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/RegularExpression.h"
#include "lldb/lldb-private.h"

namespace lldb_private {














class BreakpointResolver : public Searcher {
friend class Breakpoint;

public:








BreakpointResolver(const lldb::BreakpointSP &bkpt,
unsigned char resolverType,
lldb::addr_t offset = 0);



~BreakpointResolver() override;





void SetBreakpoint(const lldb::BreakpointSP &bkpt);


lldb::BreakpointSP GetBreakpoint() const {
auto breakpoint_sp = m_breakpoint.expired() ? lldb::BreakpointSP() :
m_breakpoint.lock();
assert(breakpoint_sp);
return breakpoint_sp;
}







void SetOffset(lldb::addr_t offset);

lldb::addr_t GetOffset() const { return m_offset; }






virtual void ResolveBreakpoint(SearchFilter &filter);






virtual void ResolveBreakpointInModules(SearchFilter &filter,
ModuleList &modules);





void GetDescription(Stream *s) override = 0;


virtual void Dump(Stream *s) const = 0;




static lldb::BreakpointResolverSP
CreateFromStructuredData(const StructuredData::Dictionary &resolver_dict,
Status &error);

virtual StructuredData::ObjectSP SerializeToStructuredData() {
return StructuredData::ObjectSP();
}

static const char *GetSerializationKey() { return "BKPTResolver"; }

static const char *GetSerializationSubclassKey() { return "Type"; }

static const char *GetSerializationSubclassOptionsKey() { return "Options"; }

StructuredData::DictionarySP
WrapOptionsDict(StructuredData::DictionarySP options_dict_sp);





enum ResolverTy {
FileLineResolver = 0,
AddressResolver,
NameResolver,
FileRegexResolver,
PythonResolver,
ExceptionResolver,
LastKnownResolverType = ExceptionResolver,
UnknownResolver
};



static const char *g_ty_to_name[LastKnownResolverType + 2];




unsigned getResolverID() const { return SubclassID; }

enum ResolverTy GetResolverTy() {
if (SubclassID > ResolverTy::LastKnownResolverType)
return ResolverTy::UnknownResolver;
else
return (enum ResolverTy)SubclassID;
}

const char *GetResolverName() { return ResolverTyToName(GetResolverTy()); }

static const char *ResolverTyToName(enum ResolverTy);

static ResolverTy NameToResolverTy(llvm::StringRef name);

virtual lldb::BreakpointResolverSP
CopyForBreakpoint(lldb::BreakpointSP &breakpoint) = 0;

protected:



enum class OptionNames : uint32_t {
AddressOffset = 0,
ExactMatch,
FileName,
Inlines,
LanguageName,
LineNumber,
Column,
ModuleName,
NameMaskArray,
Offset,
PythonClassName,
RegexString,
ScriptArgs,
SectionName,
SearchDepth,
SkipPrologue,
SymbolNameArray,
LastOptionName
};
static const char
*g_option_names[static_cast<uint32_t>(OptionNames::LastOptionName)];

virtual void NotifyBreakpointSet() {};

public:
static const char *GetKey(OptionNames enum_value) {
return g_option_names[static_cast<uint32_t>(enum_value)];
}

protected:







void SetSCMatchesByLine(SearchFilter &filter, SymbolContextList &sc_list,
bool skip_prologue, llvm::StringRef log_ident,
uint32_t line = 0,
llvm::Optional<uint16_t> column = llvm::None);
void SetSCMatchesByLine(SearchFilter &, SymbolContextList &, bool,
const char *) = delete;

lldb::BreakpointLocationSP AddLocation(Address loc_addr,
bool *new_location = nullptr);

private:

void AddLocation(SearchFilter &filter, const SymbolContext &sc,
bool skip_prologue, llvm::StringRef log_ident);

lldb::BreakpointWP m_breakpoint;
lldb::addr_t m_offset;



const unsigned char SubclassID;
BreakpointResolver(const BreakpointResolver &) = delete;
const BreakpointResolver &operator=(const BreakpointResolver &) = delete;
};

}

#endif
