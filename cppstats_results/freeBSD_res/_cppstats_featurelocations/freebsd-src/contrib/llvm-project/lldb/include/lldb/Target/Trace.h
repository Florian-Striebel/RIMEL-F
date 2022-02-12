







#if !defined(LLDB_TARGET_TRACE_H)
#define LLDB_TARGET_TRACE_H

#include <unordered_map>

#include "llvm/Support/JSON.h"

#include "lldb/Core/PluginInterface.h"
#include "lldb/Target/Thread.h"
#include "lldb/Target/TraceCursor.h"
#include "lldb/Utility/ArchSpec.h"
#include "lldb/Utility/TraceGDBRemotePackets.h"
#include "lldb/Utility/UnimplementedError.h"
#include "lldb/lldb-private.h"

namespace lldb_private {




















class Trace : public PluginInterface,
public std::enable_shared_from_this<Trace> {
public:








virtual void Dump(Stream *s) const = 0;







































static llvm::Expected<lldb::TraceSP>
FindPluginForPostMortemProcess(Debugger &debugger,
const llvm::json::Value &trace_session_file,
llvm::StringRef session_file_dir);













static llvm::Expected<lldb::TraceSP>
FindPluginForLiveProcess(llvm::StringRef plugin_name, Process &process);





static llvm::Expected<llvm::StringRef>
FindPluginSchema(llvm::StringRef plugin_name);


virtual lldb::CommandObjectSP
GetProcessTraceStartCommand(CommandInterpreter &interpreter) = 0;


virtual lldb::CommandObjectSP
GetThreadTraceStartCommand(CommandInterpreter &interpreter) = 0;



virtual llvm::StringRef GetSchema() = 0;







virtual lldb::TraceCursorUP GetCursor(Thread &thread) = 0;













virtual void DumpTraceInfo(Thread &thread, Stream &s, bool verbose) = 0;








virtual bool IsTraced(const Thread &thread) = 0;



virtual const char *GetStartConfigurationHelp() = 0;










virtual llvm::Error Start(
StructuredData::ObjectSP configuration = StructuredData::ObjectSP()) = 0;














virtual llvm::Error Start(
llvm::ArrayRef<lldb::tid_t> tids,
StructuredData::ObjectSP configuration = StructuredData::ObjectSP()) = 0;









llvm::Error Stop(llvm::ArrayRef<lldb::tid_t> tids);









llvm::Error Stop();


llvm::Expected<const FileSpec &> GetPostMortemTraceFile(lldb::tid_t tid);




uint32_t GetStopID();

protected:











llvm::Expected<llvm::ArrayRef<uint8_t>>
GetLiveThreadBinaryData(lldb::tid_t tid, llvm::StringRef kind);









llvm::Expected<llvm::ArrayRef<uint8_t>>
GetLiveProcessBinaryData(llvm::StringRef kind);


llvm::Optional<size_t> GetLiveThreadBinaryDataSize(lldb::tid_t tid,
llvm::StringRef kind);


llvm::Optional<size_t> GetLiveProcessBinaryDataSize(llvm::StringRef kind);

Trace() = default;


Trace(Process &live_process) : m_live_process(&live_process) {}











llvm::Error Start(const llvm::json::Value &request);






llvm::Expected<std::string> GetLiveProcessState();







virtual void
DoRefreshLiveProcessState(llvm::Expected<TraceGetStateResponse> state) = 0;




void RefreshLiveProcessState();

uint32_t m_stop_id = LLDB_INVALID_STOP_ID;

Process *m_live_process = nullptr;

std::map<lldb::tid_t, std::unordered_map<std::string, size_t>>
m_live_thread_data;

std::unordered_map<std::string, size_t> m_live_process_data;
};

}

#endif
