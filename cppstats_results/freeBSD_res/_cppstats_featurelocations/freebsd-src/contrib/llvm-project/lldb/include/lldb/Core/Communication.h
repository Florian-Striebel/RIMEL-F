







#if !defined(LLDB_CORE_COMMUNICATION_H)
#define LLDB_CORE_COMMUNICATION_H

#include "lldb/Host/HostThread.h"
#include "lldb/Utility/Broadcaster.h"
#include "lldb/Utility/Timeout.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-types.h"

#include <atomic>
#include <mutex>
#include <ratio>
#include <string>

#include <cstddef>
#include <cstdint>

namespace lldb_private {
class Connection;
class ConstString;
class Status;




















































class Communication : public Broadcaster {
public:
FLAGS_ANONYMOUS_ENUM(){
eBroadcastBitDisconnected =
(1u << 0),
eBroadcastBitReadThreadGotBytes =
(1u << 1),
eBroadcastBitReadThreadDidExit =
(1u
<< 2),
eBroadcastBitReadThreadShouldExit =
(1u << 3),
eBroadcastBitPacketAvailable =
(1u << 4),
eBroadcastBitNoMorePendingInput = (1u << 5),


kLoUserBroadcastBit =
(1u << 16),
kHiUserBroadcastBit = (1u << 31),
eAllEventBits = 0xffffffff};

typedef void (*ReadThreadBytesReceived)(void *baton, const void *src,
size_t src_len);









Communication(const char *broadcaster_name);




~Communication() override;

void Clear();















lldb::ConnectionStatus Connect(const char *url, Status *error_ptr);










lldb::ConnectionStatus Disconnect(Status *error_ptr = nullptr);






bool IsConnected() const;

bool HasConnection() const;

lldb_private::Connection *GetConnection() { return m_connection_sp.get(); }




























size_t Read(void *dst, size_t dst_len, const Timeout<std::micro> &timeout,
lldb::ConnectionStatus &status, Status *error_ptr);
















size_t Write(const void *src, size_t src_len, lldb::ConnectionStatus &status,
Status *error_ptr);













void SetConnection(std::unique_ptr<Connection> connection);























virtual bool StartReadThread(Status *error_ptr = nullptr);






virtual bool StopReadThread(Status *error_ptr = nullptr);

virtual bool JoinReadThread(Status *error_ptr = nullptr);




bool ReadThreadIsRunning();













static lldb::thread_result_t ReadThread(lldb::thread_arg_t comm_ptr);

void SetReadThreadBytesReceivedCallback(ReadThreadBytesReceived callback,
void *callback_baton);






void SynchronizeWithReadThread();

static std::string ConnectionStatusAsString(lldb::ConnectionStatus status);

bool GetCloseOnEOF() const { return m_close_on_eof; }

void SetCloseOnEOF(bool b) { m_close_on_eof = b; }

static ConstString &GetStaticBroadcasterClass();

ConstString &GetBroadcasterClass() const override {
return GetStaticBroadcasterClass();
}

protected:
lldb::ConnectionSP m_connection_sp;

HostThread m_read_thread;

std::atomic<bool> m_read_thread_enabled;
std::atomic<bool> m_read_thread_did_exit;
std::string
m_bytes;
std::recursive_mutex m_bytes_mutex;

std::mutex
m_write_mutex;
std::mutex m_synchronize_mutex;
ReadThreadBytesReceived m_callback;
void *m_callback_baton;
bool m_close_on_eof;

size_t ReadFromConnection(void *dst, size_t dst_len,
const Timeout<std::micro> &timeout,
lldb::ConnectionStatus &status, Status *error_ptr);



















virtual void AppendBytesToCache(const uint8_t *src, size_t src_len,
bool broadcast,
lldb::ConnectionStatus status);
















size_t GetCachedBytes(void *dst, size_t dst_len);

private:
Communication(const Communication &) = delete;
const Communication &operator=(const Communication &) = delete;
};

}

#endif
