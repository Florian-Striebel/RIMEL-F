







#if !defined(LLDB_BREAKPOINT_STOPPOINT_H)
#define LLDB_BREAKPOINT_STOPPOINT_H

#include "lldb/Utility/UserID.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class Stoppoint {
public:

Stoppoint();

virtual ~Stoppoint();


virtual void Dump(Stream *) = 0;

virtual bool IsEnabled() = 0;

virtual void SetEnabled(bool enable) = 0;

lldb::break_id_t GetID() const;

void SetID(lldb::break_id_t bid);

protected:
lldb::break_id_t m_bid = LLDB_INVALID_BREAK_ID;

private:

Stoppoint(const Stoppoint &) = delete;
const Stoppoint &operator=(const Stoppoint &) = delete;
};

}

#endif
