
































#if !defined(lib_pcap_vlan_h)
#define lib_pcap_vlan_h

#include <pcap/pcap-inttypes.h>

struct vlan_tag {
uint16_t vlan_tpid;
uint16_t vlan_tci;
};

#define VLAN_TAG_LEN 4

#endif
