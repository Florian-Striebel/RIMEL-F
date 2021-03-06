







#if !defined(LLDB_SYMBOL_BLOCK_H)
#define LLDB_SYMBOL_BLOCK_H

#include "lldb/Core/AddressRange.h"
#include "lldb/Symbol/CompilerType.h"
#include "lldb/Symbol/LineEntry.h"
#include "lldb/Symbol/SymbolContext.h"
#include "lldb/Symbol/SymbolContextScope.h"
#include "lldb/Utility/RangeMap.h"
#include "lldb/Utility/Stream.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-private.h"
#include <vector>

namespace lldb_private {

















class Block : public UserID, public SymbolContextScope {
public:
typedef RangeVector<uint32_t, uint32_t, 1> RangeList;
typedef RangeList::Entry Range;
















Block(lldb::user_id_t uid);


~Block() override;






void AddChild(const lldb::BlockSP &child_block_sp);


void AddRange(const Range &range);

void FinalizeRanges();




void CalculateSymbolContext(SymbolContext *sc) override;

lldb::ModuleSP CalculateSymbolContextModule() override;

CompileUnit *CalculateSymbolContextCompileUnit() override;

Function *CalculateSymbolContextFunction() override;

Block *CalculateSymbolContextBlock() override;









bool Contains(lldb::addr_t range_offset) const;









bool Contains(const Range &range) const;









bool Contains(const Block *block) const;


















void Dump(Stream *s, lldb::addr_t base_addr, int32_t depth,
bool show_context) const;




void DumpSymbolContext(Stream *s) override;

void DumpAddressRanges(Stream *s, lldb::addr_t base_addr);

void GetDescription(Stream *s, Function *function,
lldb::DescriptionLevel level, Target *target) const;






Block *GetParent() const;








Block *GetContainingInlinedBlock();






Block *GetInlinedParent();














Block *
GetContainingInlinedBlockWithCallSite(const Declaration &find_call_site);






Block *GetSibling() const;






Block *GetFirstChild() const {
return (m_children.empty() ? nullptr : m_children.front().get());
}











lldb::VariableListSP GetBlockVariableList(bool can_create);


















uint32_t AppendBlockVariables(bool can_create, bool get_child_block_variables,
bool stop_if_child_block_is_inlined_function,
const std::function<bool(Variable *)> &filter,
VariableList *variable_list);



























uint32_t AppendVariables(bool can_create, bool get_parent_variables,
bool stop_if_block_is_inlined_function,
const std::function<bool(Variable *)> &filter,
VariableList *variable_list);






const InlineFunctionInfo *GetInlinedFunctionInfo() const {
return m_inlineInfoSP.get();
}





SymbolFile *GetSymbolFile();

CompilerDeclContext GetDeclContext();








size_t MemorySize() const;




















void SetInlinedFunctionInfo(const char *name, const char *mangled,
const Declaration *decl_ptr,
const Declaration *call_decl_ptr);

void SetParentScope(SymbolContextScope *parent_scope) {
m_parent_scope = parent_scope;
}








void SetVariableList(lldb::VariableListSP &variable_list_sp) {
m_variable_list_sp = variable_list_sp;
}

bool BlockInfoHasBeenParsed() const { return m_parsed_block_info; }

void SetBlockInfoHasBeenParsed(bool b, bool set_children);

Block *FindBlockByID(lldb::user_id_t block_id);

size_t GetNumRanges() const { return m_ranges.GetSize(); }

bool GetRangeContainingOffset(const lldb::addr_t offset, Range &range);

bool GetRangeContainingAddress(const Address &addr, AddressRange &range);

bool GetRangeContainingLoadAddress(lldb::addr_t load_addr, Target &target,
AddressRange &range);

uint32_t GetRangeIndexContainingAddress(const Address &addr);



bool GetRangeAtIndex(uint32_t range_idx, AddressRange &range);

bool GetStartAddress(Address &addr);

void SetDidParseVariables(bool b, bool set_children);

protected:
typedef std::vector<lldb::BlockSP> collection;

SymbolContextScope *m_parent_scope;
collection m_children;
RangeList m_ranges;
lldb::InlineFunctionInfoSP m_inlineInfoSP;
lldb::VariableListSP m_variable_list_sp;


bool m_parsed_block_info : 1,

m_parsed_block_variables : 1, m_parsed_child_blocks : 1;



Block *GetSiblingForChild(const Block *child_block) const;

private:
Block(const Block &) = delete;
const Block &operator=(const Block &) = delete;
};

}

#endif
