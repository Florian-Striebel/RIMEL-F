





































#if !defined(lib_pcap_can_socketcan_h)
#define lib_pcap_can_socketcan_h

#include <pcap/pcap-inttypes.h>





typedef struct {
uint32_t can_id;
uint8_t payload_length;
uint8_t pad;
uint8_t reserved1;
uint8_t reserved2;
} pcap_can_socketcan_hdr;

#endif
