






















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <pcap-types.h>

#include "pcap-int.h"
#include "extract.h"
#include "pcap/sll.h"
#include "pcap/usb.h"
#include "pcap/nflog.h"
#include "pcap/can_socketcan.h"

#include "pcap-common.h"





































































#define LINKTYPE_NULL DLT_NULL
#define LINKTYPE_ETHERNET DLT_EN10MB
#define LINKTYPE_EXP_ETHERNET DLT_EN3MB
#define LINKTYPE_AX25 DLT_AX25
#define LINKTYPE_PRONET DLT_PRONET
#define LINKTYPE_CHAOS DLT_CHAOS
#define LINKTYPE_IEEE802_5 DLT_IEEE802
#define LINKTYPE_ARCNET_BSD DLT_ARCNET
#define LINKTYPE_SLIP DLT_SLIP
#define LINKTYPE_PPP DLT_PPP
#define LINKTYPE_FDDI DLT_FDDI
















#define LINKTYPE_PPP_HDLC 50

#define LINKTYPE_PPP_ETHER 51

#define LINKTYPE_SYMANTEC_FIREWALL 99







#define LINKTYPE_ATM_RFC1483 100
#define LINKTYPE_RAW 101
#define LINKTYPE_SLIP_BSDOS 102
#define LINKTYPE_PPP_BSDOS 103











#define LINKTYPE_MATCHING_MIN 104

#define LINKTYPE_C_HDLC 104
#define LINKTYPE_IEEE802_11 105
#define LINKTYPE_ATM_CLIP 106
#define LINKTYPE_FRELAY 107
#define LINKTYPE_LOOP 108
#define LINKTYPE_ENC 109




#define LINKTYPE_LANE8023 110
#define LINKTYPE_HIPPI 111
#define LINKTYPE_HDLC 112

#define LINKTYPE_LINUX_SLL 113
#define LINKTYPE_LTALK 114
#define LINKTYPE_ECONET 115




#define LINKTYPE_IPFILTER 116

#define LINKTYPE_PFLOG 117
#define LINKTYPE_CISCO_IOS 118
#define LINKTYPE_IEEE802_11_PRISM 119
#define LINKTYPE_IEEE802_11_AIRONET 120




#define LINKTYPE_HHDLC 121

#define LINKTYPE_IP_OVER_FC 122
#define LINKTYPE_SUNATM 123





#define LINKTYPE_RIO 124
#define LINKTYPE_PCI_EXP 125
#define LINKTYPE_AURORA 126

#define LINKTYPE_IEEE802_11_RADIOTAP 127









#define LINKTYPE_TZSP 128

#define LINKTYPE_ARCNET_LINUX 129







#define LINKTYPE_JUNIPER_MLPPP 130
#define LINKTYPE_JUNIPER_MLFR 131
#define LINKTYPE_JUNIPER_ES 132
#define LINKTYPE_JUNIPER_GGSN 133
#define LINKTYPE_JUNIPER_MFR 134
#define LINKTYPE_JUNIPER_ATM2 135
#define LINKTYPE_JUNIPER_SERVICES 136
#define LINKTYPE_JUNIPER_ATM1 137

#define LINKTYPE_APPLE_IP_OVER_IEEE1394 138

#define LINKTYPE_MTP2_WITH_PHDR 139
#define LINKTYPE_MTP2 140
#define LINKTYPE_MTP3 141
#define LINKTYPE_SCCP 142

#define LINKTYPE_DOCSIS 143

#define LINKTYPE_LINUX_IRDA 144




#define LINKTYPE_IBM_SP 145
#define LINKTYPE_IBM_SN 146



























#define LINKTYPE_USER0 147
#define LINKTYPE_USER1 148
#define LINKTYPE_USER2 149
#define LINKTYPE_USER3 150
#define LINKTYPE_USER4 151
#define LINKTYPE_USER5 152
#define LINKTYPE_USER6 153
#define LINKTYPE_USER7 154
#define LINKTYPE_USER8 155
#define LINKTYPE_USER9 156
#define LINKTYPE_USER10 157
#define LINKTYPE_USER11 158
#define LINKTYPE_USER12 159
#define LINKTYPE_USER13 160
#define LINKTYPE_USER14 161
#define LINKTYPE_USER15 162








#define LINKTYPE_IEEE802_11_AVS 163







#define LINKTYPE_JUNIPER_MONITOR 164




#define LINKTYPE_BACNET_MS_TP 165















#define LINKTYPE_PPP_PPPD 166







#define LINKTYPE_JUNIPER_PPPOE 167
#define LINKTYPE_JUNIPER_PPPOE_ATM 168

#define LINKTYPE_GPRS_LLC 169
#define LINKTYPE_GPF_T 170
#define LINKTYPE_GPF_F 171





#define LINKTYPE_GCOM_T1E1 172
#define LINKTYPE_GCOM_SERIAL 173






#define LINKTYPE_JUNIPER_PIC_PEER 174







#define LINKTYPE_ERF_ETH 175
#define LINKTYPE_ERF_POS 176







#define LINKTYPE_LINUX_LAPD 177








#define LINKTYPE_JUNIPER_ETHER 178
#define LINKTYPE_JUNIPER_PPP 179
#define LINKTYPE_JUNIPER_FRELAY 180
#define LINKTYPE_JUNIPER_CHDLC 181




#define LINKTYPE_MFR 182







#define LINKTYPE_JUNIPER_VP 183








#define LINKTYPE_A429 184






#define LINKTYPE_A653_ICM 185












#define LINKTYPE_USB_FREEBSD 186





#define LINKTYPE_BLUETOOTH_HCI_H4 187





#define LINKTYPE_IEEE802_16_MAC_CPS 188





#define LINKTYPE_USB_LINUX 189








#define LINKTYPE_CAN20B 190





#define LINKTYPE_IEEE802_15_4_LINUX 191





#define LINKTYPE_PPI 192





#define LINKTYPE_IEEE802_16_MAC_CPS_RADIO 193







#define LINKTYPE_JUNIPER_ISM 194










#define LINKTYPE_IEEE802_15_4_WITHFCS 195





#define LINKTYPE_SITA 196






#define LINKTYPE_ERF 197






#define LINKTYPE_RAIF1 198















#define LINKTYPE_IPMB_KONTRON 199






#define LINKTYPE_JUNIPER_ST 200





#define LINKTYPE_BLUETOOTH_HCI_H4_WITH_PHDR 201








#define LINKTYPE_AX25_KISS 202






#define LINKTYPE_LAPD 203







#define LINKTYPE_PPP_WITH_DIR 204






#define LINKTYPE_C_HDLC_WITH_DIR 205







#define LINKTYPE_FRELAY_WITH_DIR 206







#define LINKTYPE_LAPB_WITH_DIR 207










#define LINKTYPE_IPMB_LINUX 209





#define LINKTYPE_FLEXRAY 210






#define LINKTYPE_MOST 211






#define LINKTYPE_LIN 212





#define LINKTYPE_X2E_SERIAL 213





#define LINKTYPE_X2E_XORAYA 214










#define LINKTYPE_IEEE802_15_4_NONASK_PHY 215







#define LINKTYPE_LINUX_EVDEV 216






#define LINKTYPE_GSMTAP_UM 217
#define LINKTYPE_GSMTAP_ABIS 218






#define LINKTYPE_MPLS 219





#define LINKTYPE_USB_LINUX_MMAPPED 220





#define LINKTYPE_DECT 221










#define LINKTYPE_AOS 222








#define LINKTYPE_WIHART 223





#define LINKTYPE_FC_2 224













#define LINKTYPE_FC_2_WITH_FRAME_DELIMS 225















































#define LINKTYPE_IPNET 226










#define LINKTYPE_CAN_SOCKETCAN 227





#define LINKTYPE_IPV4 228
#define LINKTYPE_IPV6 229






#define LINKTYPE_IEEE802_15_4_NOFCS 230

















#define LINKTYPE_DBUS 231





#define LINKTYPE_JUNIPER_VS 232
#define LINKTYPE_JUNIPER_SRX_E2E 233
#define LINKTYPE_JUNIPER_FIBRECHANNEL 234











#define LINKTYPE_DVB_CI 235





#define LINKTYPE_MUX27010 236





#define LINKTYPE_STANAG_5066_D_PDU 237





#define LINKTYPE_JUNIPER_ATM_CEMIC 238







#define LINKTYPE_NFLOG 239









#define LINKTYPE_NETANALYZER 240









#define LINKTYPE_NETANALYZER_TRANSPARENT 241






#define LINKTYPE_IPOIB 242






#define LINKTYPE_MPEG_2_TS 243







#define LINKTYPE_NG40 244









#define LINKTYPE_NFC_LLCP 245










#define LINKTYPE_PFSYNC 246






#define LINKTYPE_INFINIBAND 247






#define LINKTYPE_SCTP 248






#define LINKTYPE_USBPCAP 249







#define DLT_RTAC_SERIAL 250






#define LINKTYPE_BLUETOOTH_LE_LL 251












#define LINKTYPE_WIRESHARK_UPPER_PDU 252




#define LINKTYPE_NETLINK 253




#define LINKTYPE_BLUETOOTH_LINUX_MONITOR 254





#define LINKTYPE_BLUETOOTH_BREDR_BB 255




#define LINKTYPE_BLUETOOTH_LE_LL_WITH_PHDR 256




#define LINKTYPE_PROFIBUS_DL 257




















#define LINKTYPE_PKTAP 258






#define LINKTYPE_EPON 259





#define LINKTYPE_IPMI_HPM_2 260




#define LINKTYPE_ZWAVE_R1_R2 261
#define LINKTYPE_ZWAVE_R3 262





#define LINKTYPE_WATTSTOPPER_DLM 263




#define LINKTYPE_ISO_14443 264





#define LINKTYPE_RDS 265




#define LINKTYPE_USB_DARWIN 266




#define LINKTYPE_OPENFLOW 267




#define LINKTYPE_SDLC 268





#define LINKTYPE_TI_LLN_SNIFFER 269





#define LINKTYPE_LORATAP 270







#define LINKTYPE_VSOCK 271




#define LINKTYPE_NORDIC_BLE 272






#define LINKTYPE_DOCSIS31_XRA31 273





#define LINKTYPE_ETHERNET_MPACKET 274






#define LINKTYPE_DISPLAYPORT_AUX 275




#define LINKTYPE_LINUX_SLL2 276

#define LINKTYPE_MATCHING_MAX 276






#if LINKTYPE_MATCHING_MAX != DLT_MATCHING_MAX
#error The LINKTYPE_ matching range does not match the DLT_ matching range
#endif

static struct linktype_map {
int dlt;
int linktype;
} map[] = {




{ DLT_NULL, LINKTYPE_NULL },
{ DLT_EN10MB, LINKTYPE_ETHERNET },
{ DLT_EN3MB, LINKTYPE_EXP_ETHERNET },
{ DLT_AX25, LINKTYPE_AX25 },
{ DLT_PRONET, LINKTYPE_PRONET },
{ DLT_CHAOS, LINKTYPE_CHAOS },
{ DLT_IEEE802, LINKTYPE_IEEE802_5 },
{ DLT_ARCNET, LINKTYPE_ARCNET_BSD },
{ DLT_SLIP, LINKTYPE_SLIP },
{ DLT_PPP, LINKTYPE_PPP },
{ DLT_FDDI, LINKTYPE_FDDI },
{ DLT_SYMANTEC_FIREWALL, LINKTYPE_SYMANTEC_FIREWALL },







#if defined(DLT_FR)

{ DLT_FR, LINKTYPE_FRELAY },
#endif

{ DLT_ATM_RFC1483, LINKTYPE_ATM_RFC1483 },
{ DLT_RAW, LINKTYPE_RAW },
{ DLT_SLIP_BSDOS, LINKTYPE_SLIP_BSDOS },
{ DLT_PPP_BSDOS, LINKTYPE_PPP_BSDOS },


{ DLT_C_HDLC, LINKTYPE_C_HDLC },









{ DLT_ATM_CLIP, LINKTYPE_ATM_CLIP },


{ DLT_PPP_SERIAL, LINKTYPE_PPP_HDLC },


{ DLT_PPP_ETHER, LINKTYPE_PPP_ETHER },







{ -1, -1 }
};

int
dlt_to_linktype(int dlt)
{
int i;








if (dlt == DLT_PFSYNC)
return (LINKTYPE_PFSYNC);
if (dlt == DLT_PKTAP)
return (LINKTYPE_PKTAP);





if (dlt >= DLT_MATCHING_MIN && dlt <= DLT_MATCHING_MAX)
return (dlt);




for (i = 0; map[i].dlt != -1; i++) {
if (map[i].dlt == dlt)
return (map[i].linktype);
}






return (-1);
}

int
linktype_to_dlt(int linktype)
{
int i;







if (linktype == LINKTYPE_PFSYNC)
return (DLT_PFSYNC);
if (linktype == LINKTYPE_PKTAP)
return (DLT_PKTAP);





if (linktype >= LINKTYPE_MATCHING_MIN &&
linktype <= LINKTYPE_MATCHING_MAX)
return (linktype);




for (i = 0; map[i].linktype != -1; i++) {
if (map[i].linktype == linktype)
return (map[i].dlt);
}






return linktype;
}














u_int
max_snaplen_for_dlt(int dlt)
{
switch (dlt) {

case DLT_DBUS:
return 128*1024*1024;

case DLT_USBPCAP:
return 1024*1024;

default:
return MAXIMUM_SNAPLEN;
}
}










static void
swap_linux_sll_header(const struct pcap_pkthdr *hdr, u_char *buf)
{
u_int caplen = hdr->caplen;
u_int length = hdr->len;
struct sll_header *shdr = (struct sll_header *)buf;
uint16_t protocol;
pcap_can_socketcan_hdr *chdr;

if (caplen < (u_int) sizeof(struct sll_header) ||
length < (u_int) sizeof(struct sll_header)) {

return;
}

protocol = EXTRACT_16BITS(&shdr->sll_protocol);
if (protocol != LINUX_SLL_P_CAN && protocol != LINUX_SLL_P_CANFD)
return;




chdr = (pcap_can_socketcan_hdr *)(buf + sizeof(struct sll_header));
if (caplen < (u_int) sizeof(struct sll_header) + sizeof(chdr->can_id) ||
length < (u_int) sizeof(struct sll_header) + sizeof(chdr->can_id)) {

return;
}
chdr->can_id = SWAPLONG(chdr->can_id);
}










static void
swap_linux_usb_header(const struct pcap_pkthdr *hdr, u_char *buf,
int header_len_64_bytes)
{
pcap_usb_header_mmapped *uhdr = (pcap_usb_header_mmapped *)buf;
bpf_u_int32 offset = 0;











offset += 8;
if (hdr->caplen < offset)
return;
uhdr->id = SWAPLL(uhdr->id);

offset += 4;

offset += 2;
if (hdr->caplen < offset)
return;
uhdr->bus_id = SWAPSHORT(uhdr->bus_id);

offset += 2;

offset += 8;
if (hdr->caplen < offset)
return;
uhdr->ts_sec = SWAPLL(uhdr->ts_sec);

offset += 4;
if (hdr->caplen < offset)
return;
uhdr->ts_usec = SWAPLONG(uhdr->ts_usec);

offset += 4;
if (hdr->caplen < offset)
return;
uhdr->status = SWAPLONG(uhdr->status);

offset += 4;
if (hdr->caplen < offset)
return;
uhdr->urb_len = SWAPLONG(uhdr->urb_len);

offset += 4;
if (hdr->caplen < offset)
return;
uhdr->data_len = SWAPLONG(uhdr->data_len);

if (uhdr->transfer_type == URB_ISOCHRONOUS) {
offset += 4;
if (hdr->caplen < offset)
return;
uhdr->s.iso.error_count = SWAPLONG(uhdr->s.iso.error_count);

offset += 4;
if (hdr->caplen < offset)
return;
uhdr->s.iso.numdesc = SWAPLONG(uhdr->s.iso.numdesc);
} else
offset += 8;












if (header_len_64_bytes) {








offset += 4;
if (hdr->caplen < offset)
return;
uhdr->interval = SWAPLONG(uhdr->interval);

offset += 4;
if (hdr->caplen < offset)
return;
uhdr->start_frame = SWAPLONG(uhdr->start_frame);

offset += 4;
if (hdr->caplen < offset)
return;
uhdr->xfer_flags = SWAPLONG(uhdr->xfer_flags);

offset += 4;
if (hdr->caplen < offset)
return;
uhdr->ndesc = SWAPLONG(uhdr->ndesc);

if (uhdr->transfer_type == URB_ISOCHRONOUS) {

usb_isodesc *pisodesc;
uint32_t i;

pisodesc = (usb_isodesc *)(void *)(buf+offset);
for (i = 0; i < uhdr->ndesc; i++) {
offset += 4;
if (hdr->caplen < offset)
return;
pisodesc->status = SWAPLONG(pisodesc->status);

offset += 4;
if (hdr->caplen < offset)
return;
pisodesc->offset = SWAPLONG(pisodesc->offset);

offset += 4;
if (hdr->caplen < offset)
return;
pisodesc->len = SWAPLONG(pisodesc->len);

offset += 4;

pisodesc++;
}
}
}
}












static void
swap_nflog_header(const struct pcap_pkthdr *hdr, u_char *buf)
{
u_char *p = buf;
nflog_hdr_t *nfhdr = (nflog_hdr_t *)buf;
nflog_tlv_t *tlv;
u_int caplen = hdr->caplen;
u_int length = hdr->len;
uint16_t size;

if (caplen < (u_int) sizeof(nflog_hdr_t) ||
length < (u_int) sizeof(nflog_hdr_t)) {

return;
}

if (nfhdr->nflog_version != 0) {

return;
}

length -= sizeof(nflog_hdr_t);
caplen -= sizeof(nflog_hdr_t);
p += sizeof(nflog_hdr_t);

while (caplen >= sizeof(nflog_tlv_t)) {
tlv = (nflog_tlv_t *) p;


tlv->tlv_type = SWAPSHORT(tlv->tlv_type);
tlv->tlv_length = SWAPSHORT(tlv->tlv_length);


size = tlv->tlv_length;
if (size % 4 != 0)
size += 4 - size % 4;


if (size < sizeof(nflog_tlv_t)) {

return;
}


if (caplen < size || length < size) {

return;
}


length -= size;
caplen -= size;
p += size;
}
}

void
swap_pseudo_headers(int linktype, struct pcap_pkthdr *hdr, u_char *data)
{





switch (linktype) {

case DLT_LINUX_SLL:
swap_linux_sll_header(hdr, data);
break;

case DLT_USB_LINUX:
swap_linux_usb_header(hdr, data, 0);
break;

case DLT_USB_LINUX_MMAPPED:
swap_linux_usb_header(hdr, data, 1);
break;

case DLT_NFLOG:
swap_nflog_header(hdr, data);
break;
}
}
