







#if !defined(LLDB_INTERPRETER_COMMANDHISTORY_H)
#define LLDB_INTERPRETER_COMMANDHISTORY_H

#include <mutex>
#include <string>
#include <vector>

#include "lldb/Utility/Stream.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class CommandHistory {
public:
CommandHistory() = default;

~CommandHistory() = default;

size_t GetSize() const;

bool IsEmpty() const;

llvm::Optional<llvm::StringRef> FindString(llvm::StringRef input_str) const;

llvm::StringRef GetStringAtIndex(size_t idx) const;

llvm::StringRef operator[](size_t idx) const;

llvm::StringRef GetRecentmostString() const;

void AppendString(llvm::StringRef str, bool reject_if_dupe = true);

void Clear();

void Dump(Stream &stream, size_t start_idx = 0,
size_t stop_idx = SIZE_MAX) const;

static const char g_repeat_char = '!';

private:
CommandHistory(const CommandHistory &) = delete;
const CommandHistory &operator=(const CommandHistory &) = delete;

typedef std::vector<std::string> History;
mutable std::recursive_mutex m_mutex;
History m_history;
};

}

#endif
