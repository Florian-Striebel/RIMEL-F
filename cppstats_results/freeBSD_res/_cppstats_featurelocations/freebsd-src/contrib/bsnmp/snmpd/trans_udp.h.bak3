































struct udp_port {
struct tport tport;

uint8_t addr[4];
uint16_t port;

struct port_input input;

struct sockaddr_in ret;

bool recvdstaddr;
struct in_addr dstaddr;
};


struct udp_open {
uint8_t addr[4];
uint16_t port;
};

extern const struct transport_def udp_trans;
