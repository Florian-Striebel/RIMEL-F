







#if !defined(LLDB_CORE_ADDRESSRANGE_H)
#define LLDB_CORE_ADDRESSRANGE_H

#include "lldb/Core/Address.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-types.h"

#include <cstddef>

namespace lldb_private {
class SectionList;
class Stream;
class Target;



class AddressRange {
public:




AddressRange();















AddressRange(const lldb::SectionSP &section, lldb::addr_t offset,
lldb::addr_t byte_size);














AddressRange(lldb::addr_t file_addr, lldb::addr_t byte_size,
const SectionList *section_list = nullptr);











AddressRange(const Address &so_addr, lldb::addr_t byte_size);




~AddressRange();





void Clear();









bool Contains(const Address &so_addr) const;






















bool ContainsFileAddress(const Address &so_addr) const;











bool ContainsFileAddress(lldb::addr_t file_addr) const;











bool ContainsLoadAddress(const Address &so_addr, Target *target) const;








bool ContainsLoadAddress(lldb::addr_t load_addr, Target *target) const;













bool Extend(const AddressRange &rhs_range);





















bool
Dump(Stream *s, Target *target, Address::DumpStyle style,
Address::DumpStyle fallback_style = Address::DumpStyleInvalid) const;











void DumpDebug(Stream *s) const;





Address &GetBaseAddress() { return m_base_addr; }





const Address &GetBaseAddress() const { return m_base_addr; }





lldb::addr_t GetByteSize() const { return m_byte_size; }





size_t MemorySize() const {


return sizeof(AddressRange);
}





void SetByteSize(lldb::addr_t byte_size) { m_byte_size = byte_size; }

protected:

Address m_base_addr;
lldb::addr_t m_byte_size = 0;
};



}

#endif
