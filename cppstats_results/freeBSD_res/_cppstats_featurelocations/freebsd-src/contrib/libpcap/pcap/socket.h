

































#if !defined(lib_pcap_socket_h)
#define lib_pcap_socket_h






#if defined(_WIN32)

#if defined(__MINGW32__)
#include <windef.h>
#endif
#include <winsock2.h>
#include <ws2tcpip.h>








typedef int socklen_t;





typedef long suseconds_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>







#if !defined(SOCKET)
#define SOCKET int
#endif







#if !defined(INVALID_SOCKET)
#define INVALID_SOCKET -1
#endif
#endif

#endif
