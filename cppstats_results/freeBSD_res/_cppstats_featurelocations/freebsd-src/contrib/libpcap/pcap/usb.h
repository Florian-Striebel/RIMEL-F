
































#if !defined(lib_pcap_usb_h)
#define lib_pcap_usb_h

#include <pcap/pcap-inttypes.h>




#define URB_TRANSFER_IN 0x80
#define URB_ISOCHRONOUS 0x0
#define URB_INTERRUPT 0x1
#define URB_CONTROL 0x2
#define URB_BULK 0x3




#define URB_SUBMIT 'S'
#define URB_COMPLETE 'C'
#define URB_ERROR 'E'





typedef struct _usb_setup {
uint8_t bmRequestType;
uint8_t bRequest;
uint16_t wValue;
uint16_t wIndex;
uint16_t wLength;
} pcap_usb_setup;




typedef struct _iso_rec {
int32_t error_count;
int32_t numdesc;
} iso_rec;





typedef struct _usb_header {
uint64_t id;
uint8_t event_type;
uint8_t transfer_type;
uint8_t endpoint_number;
uint8_t device_address;
uint16_t bus_id;
char setup_flag;
char data_flag;
int64_t ts_sec;
int32_t ts_usec;
int32_t status;
uint32_t urb_len;
uint32_t data_len;
pcap_usb_setup setup;
} pcap_usb_header;









typedef struct _usb_header_mmapped {
uint64_t id;
uint8_t event_type;
uint8_t transfer_type;
uint8_t endpoint_number;
uint8_t device_address;
uint16_t bus_id;
char setup_flag;
char data_flag;
int64_t ts_sec;
int32_t ts_usec;
int32_t status;
uint32_t urb_len;
uint32_t data_len;
union {
pcap_usb_setup setup;
iso_rec iso;
} s;
int32_t interval;
int32_t start_frame;
uint32_t xfer_flags;
uint32_t ndesc;
} pcap_usb_header_mmapped;









typedef struct _usb_isodesc {
int32_t status;
uint32_t offset;
uint32_t len;
uint8_t pad[4];
} usb_isodesc;

#endif
