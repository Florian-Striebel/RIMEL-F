







#if !defined(LLDB_API_SBCOMMUNICATION_H)
#define LLDB_API_SBCOMMUNICATION_H

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBError.h"

namespace lldb {

class LLDB_API SBCommunication {
public:
FLAGS_ANONYMOUS_ENUM(){
eBroadcastBitDisconnected =
(1 << 0),
eBroadcastBitReadThreadGotBytes =
(1 << 1),
eBroadcastBitReadThreadDidExit =
(1
<< 2),
eBroadcastBitReadThreadShouldExit =
(1 << 3),
eBroadcastBitPacketAvailable =
(1 << 4),
eAllEventBits = 0xffffffff};

typedef void (*ReadThreadBytesReceived)(void *baton, const void *src,
size_t src_len);

SBCommunication();
SBCommunication(const char *broadcaster_name);
~SBCommunication();

explicit operator bool() const;

bool IsValid() const;

lldb::SBBroadcaster GetBroadcaster();

static const char *GetBroadcasterClass();

lldb::ConnectionStatus AdoptFileDesriptor(int fd, bool owns_fd);

lldb::ConnectionStatus Connect(const char *url);

lldb::ConnectionStatus Disconnect();

bool IsConnected() const;

bool GetCloseOnEOF();

void SetCloseOnEOF(bool b);

size_t Read(void *dst, size_t dst_len, uint32_t timeout_usec,
lldb::ConnectionStatus &status);

size_t Write(const void *src, size_t src_len, lldb::ConnectionStatus &status);

bool ReadThreadStart();

bool ReadThreadStop();

bool ReadThreadIsRunning();

bool SetReadThreadBytesReceivedCallback(ReadThreadBytesReceived callback,
void *callback_baton);

private:
SBCommunication(const SBCommunication &) = delete;
const SBCommunication &operator=(const SBCommunication &) = delete;

lldb_private::Communication *m_opaque = nullptr;
bool m_opaque_owned = false;
};

}

#endif
