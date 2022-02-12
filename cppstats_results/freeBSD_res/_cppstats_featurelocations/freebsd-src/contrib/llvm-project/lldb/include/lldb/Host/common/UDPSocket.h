







#if !defined(LLDB_HOST_COMMON_UDPSOCKET_H)
#define LLDB_HOST_COMMON_UDPSOCKET_H

#include "lldb/Host/Socket.h"

namespace lldb_private {
class UDPSocket : public Socket {
public:
UDPSocket(bool should_close, bool child_processes_inherit);

static llvm::Expected<std::unique_ptr<UDPSocket>>
Connect(llvm::StringRef name, bool child_processes_inherit);

std::string GetRemoteConnectionURI() const override;

private:
UDPSocket(NativeSocket socket);

size_t Send(const void *buf, const size_t num_bytes) override;
Status Connect(llvm::StringRef name) override;
Status Listen(llvm::StringRef name, int backlog) override;
Status Accept(Socket *&socket) override;

SocketAddress m_sockaddr;
};
}

#endif
