







#if !defined(LLDB_TARGET_REGISTERCHECKPOINT_H)
#define LLDB_TARGET_REGISTERCHECKPOINT_H

#include "lldb/Target/StackID.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-private.h"

namespace lldb_private {




class RegisterCheckpoint : public UserID {
public:
enum class Reason {




eExpression,


eDataBackup
};

RegisterCheckpoint(Reason reason)
: UserID(0), m_data_sp(), m_reason(reason) {}

~RegisterCheckpoint() = default;

lldb::DataBufferSP &GetData() { return m_data_sp; }

const lldb::DataBufferSP &GetData() const { return m_data_sp; }

protected:
lldb::DataBufferSP m_data_sp;
Reason m_reason;


RegisterCheckpoint(const RegisterCheckpoint &) = delete;
const RegisterCheckpoint &operator=(const RegisterCheckpoint &) = delete;
};

}

#endif
