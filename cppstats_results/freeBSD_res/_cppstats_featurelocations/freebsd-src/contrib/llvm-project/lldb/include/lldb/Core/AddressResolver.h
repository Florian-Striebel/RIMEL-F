







#if !defined(LLDB_CORE_ADDRESSRESOLVER_H)
#define LLDB_CORE_ADDRESSRESOLVER_H

#include "lldb/Core/AddressRange.h"
#include "lldb/Core/SearchFilter.h"
#include "lldb/lldb-defines.h"

#include <cstddef>
#include <vector>

namespace lldb_private {
class ModuleList;
class Stream;











class AddressResolver : public Searcher {
public:
enum MatchType { Exact, Regexp, Glob };

AddressResolver();

~AddressResolver() override;

virtual void ResolveAddress(SearchFilter &filter);

virtual void ResolveAddressInModules(SearchFilter &filter,
ModuleList &modules);

void GetDescription(Stream *s) override = 0;

std::vector<AddressRange> &GetAddressRanges();

size_t GetNumberOfAddresses();

AddressRange &GetAddressRangeAtIndex(size_t idx);

protected:
std::vector<AddressRange> m_address_ranges;

private:
AddressResolver(const AddressResolver &) = delete;
const AddressResolver &operator=(const AddressResolver &) = delete;
};

}

#endif
