
































#if !defined(lib_pcap_bluetooth_h)
#define lib_pcap_bluetooth_h

#include <pcap/pcap-inttypes.h>





typedef struct _pcap_bluetooth_h4_header {
uint32_t direction;
} pcap_bluetooth_h4_header;





typedef struct _pcap_bluetooth_linux_monitor_header {
uint16_t adapter_id;
uint16_t opcode;
} pcap_bluetooth_linux_monitor_header;

#endif
