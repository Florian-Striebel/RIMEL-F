







#if !defined(LLDB_CORE_PROGRESS_H)
#define LLDB_CORE_PROGRESS_H

#include "lldb/Utility/ConstString.h"
#include "lldb/lldb-types.h"
#include <atomic>
#include <mutex>

namespace lldb_private {





































class Progress {
public:














Progress(std::string title, uint64_t total = UINT64_MAX,
lldb_private::Debugger *debugger = nullptr);






~Progress();









void Increment(uint64_t amount = 1);

private:
void ReportProgress();
static std::atomic<uint64_t> g_id;

std::string m_title;
std::mutex m_mutex;

const uint64_t m_id;

uint64_t m_completed;

const uint64_t m_total;


llvm::Optional<lldb::user_id_t> m_debugger_id;



bool m_complete = false;
};

}

#endif
