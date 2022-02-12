


























#if !defined(lib_pcap_nflog_h)
#define lib_pcap_nflog_h

#include <pcap/pcap-inttypes.h>












typedef struct nflog_hdr {
uint8_t nflog_family;
uint8_t nflog_version;
uint16_t nflog_rid;
} nflog_hdr_t;

typedef struct nflog_tlv {
uint16_t tlv_length;
uint16_t tlv_type;

} nflog_tlv_t;

typedef struct nflog_packet_hdr {
uint16_t hw_protocol;
uint8_t hook;
uint8_t pad;
} nflog_packet_hdr_t;

typedef struct nflog_hwaddr {
uint16_t hw_addrlen;
uint16_t pad;
uint8_t hw_addr[8];
} nflog_hwaddr_t;

typedef struct nflog_timestamp {
uint64_t sec;
uint64_t usec;
} nflog_timestamp_t;




#define NFULA_PACKET_HDR 1
#define NFULA_MARK 2
#define NFULA_TIMESTAMP 3
#define NFULA_IFINDEX_INDEV 4
#define NFULA_IFINDEX_OUTDEV 5
#define NFULA_IFINDEX_PHYSINDEV 6
#define NFULA_IFINDEX_PHYSOUTDEV 7
#define NFULA_HWADDR 8
#define NFULA_PAYLOAD 9
#define NFULA_PREFIX 10
#define NFULA_UID 11
#define NFULA_SEQ 12
#define NFULA_SEQ_GLOBAL 13
#define NFULA_GID 14
#define NFULA_HWTYPE 15
#define NFULA_HWHEADER 16
#define NFULA_HWLEN 17

#endif
