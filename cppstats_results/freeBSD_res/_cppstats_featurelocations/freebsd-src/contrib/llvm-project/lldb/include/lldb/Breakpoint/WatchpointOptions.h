







#if !defined(LLDB_BREAKPOINT_WATCHPOINTOPTIONS_H)
#define LLDB_BREAKPOINT_WATCHPOINTOPTIONS_H

#include <memory>
#include <string>

#include "lldb/Utility/Baton.h"
#include "lldb/Utility/StringList.h"
#include "lldb/lldb-private.h"

namespace lldb_private {





class WatchpointOptions {
public:



WatchpointOptions();
WatchpointOptions(const WatchpointOptions &rhs);

static WatchpointOptions *CopyOptionsNoCallback(WatchpointOptions &rhs);











WatchpointOptions(WatchpointHitCallback callback, void *baton,
lldb::tid_t thread_id = LLDB_INVALID_THREAD_ID);

virtual ~WatchpointOptions();


const WatchpointOptions &operator=(const WatchpointOptions &rhs);








































void SetCallback(WatchpointHitCallback callback,
const lldb::BatonSP &baton_sp, bool synchronous = false);


void ClearCallback();



















bool InvokeCallback(StoppointCallbackContext *context,
lldb::user_id_t watch_id);






bool IsCallbackSynchronous() { return m_callback_is_synchronous; }





Baton *GetBaton();





const Baton *GetBaton() const;







const ThreadSpec *GetThreadSpecNoCreate() const;




ThreadSpec *GetThreadSpec();

void SetThreadID(lldb::tid_t thread_id);

void GetDescription(Stream *s, lldb::DescriptionLevel level) const;


void GetCallbackDescription(Stream *s, lldb::DescriptionLevel level) const;


bool HasCallback();





static bool NullCallback(void *baton, StoppointCallbackContext *context,
lldb::user_id_t watch_id);

struct CommandData {
CommandData() : user_source(), script_source() {}

~CommandData() = default;

StringList user_source;
std::string script_source;
bool stop_on_error = true;
};

class CommandBaton : public TypedBaton<CommandData> {
public:
CommandBaton(std::unique_ptr<CommandData> Data)
: TypedBaton(std::move(Data)) {}

void GetDescription(llvm::raw_ostream &s, lldb::DescriptionLevel level,
unsigned indentation) const override;
};

protected:


private:

WatchpointHitCallback m_callback;
lldb::BatonSP m_callback_baton_sp;
bool m_callback_is_synchronous = false;
std::unique_ptr<ThreadSpec>
m_thread_spec_up;
};

}

#endif
