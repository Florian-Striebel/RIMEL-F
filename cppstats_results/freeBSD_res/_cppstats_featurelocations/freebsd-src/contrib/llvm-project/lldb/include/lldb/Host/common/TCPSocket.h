







#if !defined(LLDB_HOST_COMMON_TCPSOCKET_H)
#define LLDB_HOST_COMMON_TCPSOCKET_H

#include "lldb/Host/Socket.h"
#include "lldb/Host/SocketAddress.h"
#include <map>

namespace lldb_private {
class TCPSocket : public Socket {
public:
TCPSocket(bool should_close, bool child_processes_inherit);
TCPSocket(NativeSocket socket, bool should_close,
bool child_processes_inherit);
~TCPSocket() override;


uint16_t GetLocalPortNumber() const;


std::string GetLocalIPAddress() const;



uint16_t GetRemotePortNumber() const;



std::string GetRemoteIPAddress() const;

int SetOptionNoDelay();
int SetOptionReuseAddress();

Status Connect(llvm::StringRef name) override;
Status Listen(llvm::StringRef name, int backlog) override;
Status Accept(Socket *&conn_socket) override;

Status CreateSocket(int domain);

bool IsValid() const override;

std::string GetRemoteConnectionURI() const override;

private:
TCPSocket(NativeSocket socket, const TCPSocket &listen_socket);

void CloseListenSockets();

std::map<int, SocketAddress> m_listen_sockets;
};
}

#endif
