








































































#if !defined(lib_pcap_sll_h)
#define lib_pcap_sll_h

#include <pcap/pcap-inttypes.h>




#define SLL_HDR_LEN 16
#define SLL_ADDRLEN 8

struct sll_header {
uint16_t sll_pkttype;
uint16_t sll_hatype;
uint16_t sll_halen;
uint8_t sll_addr[SLL_ADDRLEN];
uint16_t sll_protocol;
};




#define SLL2_HDR_LEN 20

struct sll2_header {
uint16_t sll2_protocol;
uint16_t sll2_reserved_mbz;
uint32_t sll2_if_index;
uint16_t sll2_hatype;
uint8_t sll2_pkttype;
uint8_t sll2_halen;
uint8_t sll2_addr[SLL_ADDRLEN];
};









#define LINUX_SLL_HOST 0
#define LINUX_SLL_BROADCAST 1
#define LINUX_SLL_MULTICAST 2
#define LINUX_SLL_OTHERHOST 3
#define LINUX_SLL_OUTGOING 4























#define LINUX_SLL_P_802_3 0x0001
#define LINUX_SLL_P_802_2 0x0004
#define LINUX_SLL_P_CAN 0x000C
#define LINUX_SLL_P_CANFD 0x000D

#endif
