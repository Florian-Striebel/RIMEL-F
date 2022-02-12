







#if !defined(LLDB_UTILITY_SOURCELOCATIONSPEC_H)
#define LLDB_UTILITY_SOURCELOCATIONSPEC_H

#include "lldb/Core/Declaration.h"
#include "lldb/lldb-defines.h"
#include "llvm/ADT/Optional.h"

#include <string>

namespace lldb_private {








class SourceLocationSpec {
public:




















explicit SourceLocationSpec(FileSpec file_spec, uint32_t line,
llvm::Optional<uint16_t> column = llvm::None,
bool check_inlines = false,
bool exact_match = false);

SourceLocationSpec() = delete;















explicit operator bool() const;















bool operator!() const;












bool operator==(const SourceLocationSpec &rhs) const;












bool operator!=(const SourceLocationSpec &rhs) const;












bool operator<(const SourceLocationSpec &rhs) const;























static int Compare(const SourceLocationSpec &lhs,
const SourceLocationSpec &rhs);

static bool Equal(const SourceLocationSpec &lhs,
const SourceLocationSpec &rhs, bool full);








void Dump(Stream &s) const;

std::string GetString() const;

FileSpec GetFileSpec() const { return m_declaration.GetFile(); }

llvm::Optional<uint32_t> GetLine() const;

llvm::Optional<uint16_t> GetColumn() const;

bool GetCheckInlines() const { return m_check_inlines; }

bool GetExactMatch() const { return m_exact_match; }

protected:
Declaration m_declaration;

bool m_check_inlines;

bool m_exact_match;
};


Stream &operator<<(Stream &s, const SourceLocationSpec &loc);
}

#endif
