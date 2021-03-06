







#if !defined(LLDB_HOST_SOCKETADDRESS_H)
#define LLDB_HOST_SOCKETADDRESS_H

#include <cstdint>

#if defined(_WIN32)
#include "lldb/Host/windows/windows.h"
#include <winsock2.h>
#include <ws2tcpip.h>
typedef ADDRESS_FAMILY sa_family_t;
#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#if defined(__FreeBSD__)
#include <sys/types.h>
#endif

#include <string>
#include <vector>

namespace lldb_private {

class SocketAddress {
public:

static std::vector<SocketAddress>
GetAddressInfo(const char *hostname, const char *servname, int ai_family,
int ai_socktype, int ai_protocol, int ai_flags = 0);


SocketAddress();
SocketAddress(const struct addrinfo *addr_info);
SocketAddress(const struct sockaddr &s);
SocketAddress(const struct sockaddr_in &s);
SocketAddress(const struct sockaddr_in6 &s);
SocketAddress(const struct sockaddr_storage &s);
~SocketAddress();


const SocketAddress &operator=(const struct addrinfo *addr_info);

const SocketAddress &operator=(const struct sockaddr &s);

const SocketAddress &operator=(const struct sockaddr_in &s);

const SocketAddress &operator=(const struct sockaddr_in6 &s);

const SocketAddress &operator=(const struct sockaddr_storage &s);

bool operator==(const SocketAddress &rhs) const;
bool operator!=(const SocketAddress &rhs) const;


void Clear();


socklen_t GetLength() const;


static socklen_t GetMaxLength();


sa_family_t GetFamily() const;


void SetFamily(sa_family_t family);


std::string GetIPAddress() const;


uint16_t GetPort() const;



bool SetPort(uint16_t port);





bool
getaddrinfo(const char *host,


const char *service,

int ai_family = PF_UNSPEC, int ai_socktype = 0,
int ai_protocol = 0, int ai_flags = 0);




bool SetToLocalhost(sa_family_t family, uint16_t port);

bool SetToAnyAddress(sa_family_t family, uint16_t port);


bool IsValid() const;


bool IsAnyAddr() const;


bool IsLocalhost() const;


struct sockaddr &sockaddr() {
return m_socket_addr.sa;
}

const struct sockaddr &sockaddr() const { return m_socket_addr.sa; }

struct sockaddr_in &sockaddr_in() {
return m_socket_addr.sa_ipv4;
}

const struct sockaddr_in &sockaddr_in() const {
return m_socket_addr.sa_ipv4;
}

struct sockaddr_in6 &sockaddr_in6() {
return m_socket_addr.sa_ipv6;
}

const struct sockaddr_in6 &sockaddr_in6() const {
return m_socket_addr.sa_ipv6;
}

struct sockaddr_storage &sockaddr_storage() {
return m_socket_addr.sa_storage;
}

const struct sockaddr_storage &sockaddr_storage() const {
return m_socket_addr.sa_storage;
}






operator struct sockaddr *() { return &m_socket_addr.sa; }

operator const struct sockaddr *() const { return &m_socket_addr.sa; }

operator struct sockaddr_in *() { return &m_socket_addr.sa_ipv4; }

operator const struct sockaddr_in *() const { return &m_socket_addr.sa_ipv4; }

operator struct sockaddr_in6 *() { return &m_socket_addr.sa_ipv6; }

operator const struct sockaddr_in6 *() const {
return &m_socket_addr.sa_ipv6;
}

operator const struct sockaddr_storage *() const {
return &m_socket_addr.sa_storage;
}

operator struct sockaddr_storage *() { return &m_socket_addr.sa_storage; }

protected:
typedef union sockaddr_tag {
struct sockaddr sa;
struct sockaddr_in sa_ipv4;
struct sockaddr_in6 sa_ipv6;
struct sockaddr_storage sa_storage;
} sockaddr_t;


sockaddr_t m_socket_addr;
};

}

#endif
