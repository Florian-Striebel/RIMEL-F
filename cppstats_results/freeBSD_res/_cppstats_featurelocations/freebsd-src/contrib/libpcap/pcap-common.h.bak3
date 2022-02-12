



































#define SWAPLONG(y) (((((u_int)(y))&0xff)<<24) | ((((u_int)(y))&0xff00)<<8) | ((((u_int)(y))&0xff0000)>>8) | ((((u_int)(y))>>24)&0xff))




#define SWAPSHORT(y) ((u_short)(((((u_int)(y))&0xff)<<8) | ((((u_int)(y))&0xff00)>>8)))



extern int dlt_to_linktype(int dlt);

extern int linktype_to_dlt(int linktype);

extern void swap_pseudo_headers(int linktype, struct pcap_pkthdr *hdr,
u_char *data);

extern u_int max_snaplen_for_dlt(int dlt);
