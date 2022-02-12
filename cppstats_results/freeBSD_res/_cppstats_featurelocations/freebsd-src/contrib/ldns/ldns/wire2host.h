
















#if !defined(LDNS_WIRE2HOST_H)
#define LDNS_WIRE2HOST_H

#include <ldns/rdata.h>
#include <ldns/common.h>
#include <ldns/error.h>
#include <ldns/rr.h>
#include <ldns/packet.h>

#if defined(__cplusplus)
extern "C" {
#endif


#define LDNS_HEADER_SIZE 12


#define LDNS_RD_MASK 0x01U
#define LDNS_RD_SHIFT 0
#define LDNS_RD_WIRE(wirebuf) (*(wirebuf+2) & LDNS_RD_MASK)
#define LDNS_RD_SET(wirebuf) (*(wirebuf+2) |= LDNS_RD_MASK)
#define LDNS_RD_CLR(wirebuf) (*(wirebuf+2) &= ~LDNS_RD_MASK)

#define LDNS_TC_MASK 0x02U
#define LDNS_TC_SHIFT 1
#define LDNS_TC_WIRE(wirebuf) (*(wirebuf+2) & LDNS_TC_MASK)
#define LDNS_TC_SET(wirebuf) (*(wirebuf+2) |= LDNS_TC_MASK)
#define LDNS_TC_CLR(wirebuf) (*(wirebuf+2) &= ~LDNS_TC_MASK)

#define LDNS_AA_MASK 0x04U
#define LDNS_AA_SHIFT 2
#define LDNS_AA_WIRE(wirebuf) (*(wirebuf+2) & LDNS_AA_MASK)
#define LDNS_AA_SET(wirebuf) (*(wirebuf+2) |= LDNS_AA_MASK)
#define LDNS_AA_CLR(wirebuf) (*(wirebuf+2) &= ~LDNS_AA_MASK)

#define LDNS_OPCODE_MASK 0x78U
#define LDNS_OPCODE_SHIFT 3
#define LDNS_OPCODE_WIRE(wirebuf) ((*(wirebuf+2) & LDNS_OPCODE_MASK) >> LDNS_OPCODE_SHIFT)
#define LDNS_OPCODE_SET(wirebuf, opcode) (*(wirebuf+2) = ((*(wirebuf+2)) & ~LDNS_OPCODE_MASK) | ((opcode) << LDNS_OPCODE_SHIFT))


#define LDNS_QR_MASK 0x80U
#define LDNS_QR_SHIFT 7
#define LDNS_QR_WIRE(wirebuf) (*(wirebuf+2) & LDNS_QR_MASK)
#define LDNS_QR_SET(wirebuf) (*(wirebuf+2) |= LDNS_QR_MASK)
#define LDNS_QR_CLR(wirebuf) (*(wirebuf+2) &= ~LDNS_QR_MASK)


#define LDNS_RCODE_MASK 0x0fU
#define LDNS_RCODE_SHIFT 0
#define LDNS_RCODE_WIRE(wirebuf) (*(wirebuf+3) & LDNS_RCODE_MASK)
#define LDNS_RCODE_SET(wirebuf, rcode) (*(wirebuf+3) = ((*(wirebuf+3)) & ~LDNS_RCODE_MASK) | (rcode))


#define LDNS_CD_MASK 0x10U
#define LDNS_CD_SHIFT 4
#define LDNS_CD_WIRE(wirebuf) (*(wirebuf+3) & LDNS_CD_MASK)
#define LDNS_CD_SET(wirebuf) (*(wirebuf+3) |= LDNS_CD_MASK)
#define LDNS_CD_CLR(wirebuf) (*(wirebuf+3) &= ~LDNS_CD_MASK)

#define LDNS_AD_MASK 0x20U
#define LDNS_AD_SHIFT 5
#define LDNS_AD_WIRE(wirebuf) (*(wirebuf+3) & LDNS_AD_MASK)
#define LDNS_AD_SET(wirebuf) (*(wirebuf+3) |= LDNS_AD_MASK)
#define LDNS_AD_CLR(wirebuf) (*(wirebuf+3) &= ~LDNS_AD_MASK)

#define LDNS_Z_MASK 0x40U
#define LDNS_Z_SHIFT 6
#define LDNS_Z_WIRE(wirebuf) (*(wirebuf+3) & LDNS_Z_MASK)
#define LDNS_Z_SET(wirebuf) (*(wirebuf+3) |= LDNS_Z_MASK)
#define LDNS_Z_CLR(wirebuf) (*(wirebuf+3) &= ~LDNS_Z_MASK)

#define LDNS_RA_MASK 0x80U
#define LDNS_RA_SHIFT 7
#define LDNS_RA_WIRE(wirebuf) (*(wirebuf+3) & LDNS_RA_MASK)
#define LDNS_RA_SET(wirebuf) (*(wirebuf+3) |= LDNS_RA_MASK)
#define LDNS_RA_CLR(wirebuf) (*(wirebuf+3) &= ~LDNS_RA_MASK)


#define LDNS_ID_WIRE(wirebuf) (ldns_read_uint16(wirebuf))
#define LDNS_ID_SET(wirebuf, id) (ldns_write_uint16(wirebuf, id))


#define LDNS_QDCOUNT_OFF 4
#define LDNS_QDCOUNT(wirebuf) (ldns_read_uint16(wirebuf+LDNS_QDCOUNT_OFF))


#define LDNS_ANCOUNT_OFF 6
#define LDNS_ANCOUNT(wirebuf) (ldns_read_uint16(wirebuf+LDNS_ANCOUNT_OFF))


#define LDNS_NSCOUNT_OFF 8
#define LDNS_NSCOUNT(wirebuf) (ldns_read_uint16(wirebuf+LDNS_NSCOUNT_OFF))


#define LDNS_ARCOUNT_OFF 10
#define LDNS_ARCOUNT(wirebuf) (ldns_read_uint16(wirebuf+LDNS_ARCOUNT_OFF))











ldns_status ldns_wire2pkt(ldns_pkt **packet, const uint8_t *data, size_t len);










ldns_status ldns_buffer2pkt_wire(ldns_pkt **packet, const ldns_buffer *buffer);














ldns_status ldns_wire2dname(ldns_rdf **dname, const uint8_t *wire, size_t max, size_t *pos);


















ldns_status ldns_wire2rdf(ldns_rr *rr, const uint8_t *wire, size_t max, size_t *pos);
















ldns_status ldns_wire2rr(ldns_rr **rr, const uint8_t *wire, size_t max, size_t *pos, ldns_pkt_section section);

#if defined(__cplusplus)
}
#endif

#endif
