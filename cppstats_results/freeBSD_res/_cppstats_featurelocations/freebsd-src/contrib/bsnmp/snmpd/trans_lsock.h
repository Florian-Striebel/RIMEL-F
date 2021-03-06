
































enum locp {
LOCP_DGRAM_UNPRIV = 1,
LOCP_DGRAM_PRIV = 2,
LOCP_STREAM_UNPRIV = 3,
LOCP_STREAM_PRIV = 4,
};
struct lsock_peer {
LIST_ENTRY(lsock_peer) link;
struct port_input input;
struct sockaddr_un peer;
struct lsock_port *port;
};

struct lsock_port {
struct tport tport;

char *name;
enum locp type;

int str_sock;
void *str_id;

LIST_HEAD(, lsock_peer) peers;
};

extern const struct transport_def lsock_trans;
