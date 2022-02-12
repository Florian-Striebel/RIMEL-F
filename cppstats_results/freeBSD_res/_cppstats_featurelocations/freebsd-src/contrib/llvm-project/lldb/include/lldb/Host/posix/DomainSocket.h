







#if !defined(LLDB_HOST_POSIX_DOMAINSOCKET_H)
#define LLDB_HOST_POSIX_DOMAINSOCKET_H

#include "lldb/Host/Socket.h"

namespace lldb_private {
class DomainSocket : public Socket {
public:
DomainSocket(bool should_close, bool child_processes_inherit);

Status Connect(llvm::StringRef name) override;
Status Listen(llvm::StringRef name, int backlog) override;
Status Accept(Socket *&socket) override;

std::string GetRemoteConnectionURI() const override;

protected:
DomainSocket(SocketProtocol protocol, bool child_processes_inherit);

virtual size_t GetNameOffset() const;
virtual void DeleteSocketFile(llvm::StringRef name);
std::string GetSocketName() const;

private:
DomainSocket(NativeSocket socket, const DomainSocket &listen_socket);
};
}

#endif
