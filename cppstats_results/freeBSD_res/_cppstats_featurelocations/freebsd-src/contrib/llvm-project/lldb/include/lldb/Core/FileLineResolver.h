







#if !defined(LLDB_CORE_FILELINERESOLVER_H)
#define LLDB_CORE_FILELINERESOLVER_H

#include "lldb/Core/SearchFilter.h"
#include "lldb/Symbol/SymbolContext.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/lldb-defines.h"

#include <cstdint>

namespace lldb_private {
class Address;
class Stream;





class FileLineResolver : public Searcher {
public:
FileLineResolver()
: m_file_spec(),

m_sc_list() {}

FileLineResolver(const FileSpec &resolver, uint32_t line_no,
bool check_inlines);

~FileLineResolver() override;

Searcher::CallbackReturn SearchCallback(SearchFilter &filter,
SymbolContext &context,
Address *addr) override;

lldb::SearchDepth GetDepth() override;

void GetDescription(Stream *s) override;

const SymbolContextList &GetFileLineMatches() { return m_sc_list; }

void Clear();

void Reset(const FileSpec &file_spec, uint32_t line, bool check_inlines);

protected:
FileSpec m_file_spec;
uint32_t m_line_number =
UINT32_MAX;
SymbolContextList m_sc_list;
bool m_inlines = true;


private:
FileLineResolver(const FileLineResolver &) = delete;
const FileLineResolver &operator=(const FileLineResolver &) = delete;
};

}

#endif
