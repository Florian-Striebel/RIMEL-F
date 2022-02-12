







#if !defined(LLDB_CORE_ADDRESS_H)
#define LLDB_CORE_ADDRESS_H

#include "lldb/lldb-defines.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private-enumerations.h"
#include "lldb/lldb-types.h"

#include <cstddef>
#include <cstdint>

namespace lldb_private {
class Block;
class CompileUnit;
class ExecutionContextScope;
class Function;
class SectionList;
class Stream;
class Symbol;
class SymbolContext;
class Target;
struct LineEntry;




























class Address {
public:


enum DumpStyle {

DumpStyleInvalid,





DumpStyleSectionNameOffset,





DumpStyleSectionPointerOffset,






DumpStyleFileAddress,





DumpStyleModuleWithFileAddress,





DumpStyleLoadAddress,




DumpStyleResolvedDescription,
DumpStyleResolvedDescriptionNoModule,
DumpStyleResolvedDescriptionNoFunctionArguments,


DumpStyleNoFunctionName,


DumpStyleDetailedSymbolContext,


DumpStyleResolvedPointerDescription
};





Address() : m_section_wp() {}







Address(const Address &rhs)
: m_section_wp(rhs.m_section_wp), m_offset(rhs.m_offset) {}











Address(const lldb::SectionSP &section_sp, lldb::addr_t offset)
: m_section_wp(),

m_offset(offset) {
if (section_sp)
m_section_wp = section_sp;
}











Address(lldb::addr_t file_addr, const SectionList *section_list);

Address(lldb::addr_t abs_addr);











const Address &operator=(const Address &rhs);





void Clear() {
m_section_wp.reset();
m_offset = LLDB_INVALID_ADDRESS;
}













static int CompareFileAddress(const Address &lhs, const Address &rhs);

static int CompareLoadAddress(const Address &lhs, const Address &rhs,
Target *target);

static int CompareModulePointerAndOffset(const Address &lhs,
const Address &rhs);


class ModulePointerAndOffsetLessThanFunctionObject {
public:
ModulePointerAndOffsetLessThanFunctionObject() = default;

bool operator()(const Address &a, const Address &b) const {
return Address::CompareModulePointerAndOffset(a, b) < 0;
}
};























bool Dump(Stream *s, ExecutionContextScope *exe_scope, DumpStyle style,
DumpStyle fallback_style = DumpStyleInvalid,
uint32_t addr_byte_size = UINT32_MAX) const;

AddressClass GetAddressClass() const;











lldb::addr_t GetFileAddress() const;













lldb::addr_t GetLoadAddress(Target *target) const;













lldb::addr_t GetCallableLoadAddress(Target *target,
bool is_indirect = false) const;
















lldb::addr_t GetOpcodeLoadAddress(
Target *target,
AddressClass addr_class = AddressClass::eInvalid) const;






lldb::addr_t GetOffset() const { return m_offset; }












bool IsSectionOffset() const {
return IsValid() && (GetSection().get() != nullptr);
}










bool IsValid() const { return m_offset != LLDB_INVALID_ADDRESS; }





size_t MemorySize() const;









bool ResolveAddressUsingFileSections(lldb::addr_t addr,
const SectionList *sections);















bool ResolveFunctionScope(lldb_private::SymbolContext &sym_ctx,
lldb_private::AddressRange *addr_range_ptr = nullptr);


























bool SetLoadAddress(lldb::addr_t load_addr, Target *target,
bool allow_section_end = false);

bool SetOpcodeLoadAddress(
lldb::addr_t load_addr, Target *target,
AddressClass addr_class = AddressClass::eInvalid,
bool allow_section_end = false);

bool SetCallableLoadAddress(lldb::addr_t load_addr, Target *target);







lldb::ModuleSP GetModule() const;






lldb::SectionSP GetSection() const { return m_section_wp.lock(); }








bool SetOffset(lldb::addr_t offset) {
bool changed = m_offset != offset;
m_offset = offset;
return changed;
}

void SetRawAddress(lldb::addr_t addr) {
m_section_wp.reset();
m_offset = addr;
}

bool Slide(int64_t offset) {
if (m_offset != LLDB_INVALID_ADDRESS) {
m_offset += offset;
return true;
}
return false;
}







void SetSection(const lldb::SectionSP &section_sp) {
m_section_wp = section_sp;
}

void ClearSection() { m_section_wp.reset(); }









uint32_t CalculateSymbolContext(SymbolContext *sc,
lldb::SymbolContextItem resolve_scope =
lldb::eSymbolContextEverything) const;

lldb::ModuleSP CalculateSymbolContextModule() const;

CompileUnit *CalculateSymbolContextCompileUnit() const;

Function *CalculateSymbolContextFunction() const;

Block *CalculateSymbolContextBlock() const;

Symbol *CalculateSymbolContextSymbol() const;

bool CalculateSymbolContextLineEntry(LineEntry &line_entry) const;






bool SectionWasDeleted() const;

protected:

lldb::SectionWP m_section_wp;
lldb::addr_t m_offset = LLDB_INVALID_ADDRESS;







bool SectionWasDeletedPrivate() const;
};












bool operator<(const Address &lhs, const Address &rhs);
bool operator>(const Address &lhs, const Address &rhs);
bool operator==(const Address &lhs, const Address &rhs);
bool operator!=(const Address &lhs, const Address &rhs);

}

#endif
