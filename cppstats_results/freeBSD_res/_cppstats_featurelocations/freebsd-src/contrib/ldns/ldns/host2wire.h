















#if !defined(LDNS_HOST2WIRE_H)
#define LDNS_HOST2WIRE_H

#include <ldns/common.h>
#include <ldns/error.h>
#include <ldns/rr.h>
#include <ldns/rdata.h>
#include <ldns/packet.h>
#include <ldns/buffer.h>
#include <ctype.h>

#include "ldns/util.h"

#if defined(__cplusplus)
extern "C" {
#endif







ldns_status ldns_dname2buffer_wire(ldns_buffer *buffer, const ldns_rdf *name);








ldns_status ldns_dname2buffer_wire_compress(ldns_buffer *buffer, const ldns_rdf *name, ldns_rbtree_t *compression_data);







ldns_status ldns_rdf2buffer_wire(ldns_buffer *output, const ldns_rdf *rdf);








ldns_status ldns_rdf2buffer_wire_compress(ldns_buffer *output, const ldns_rdf *rdf, ldns_rbtree_t *compression_data);









ldns_status ldns_rdf2buffer_wire_canonical(ldns_buffer *output,
const ldns_rdf *rdf);









ldns_status ldns_rr2buffer_wire(ldns_buffer *output,
const ldns_rr *rr,
int section);










ldns_status ldns_rr2buffer_wire_compress(ldns_buffer *output,
const ldns_rr *rr,
int section,
ldns_rbtree_t *compression_data);











ldns_status ldns_rr2buffer_wire_canonical(ldns_buffer *output,
const ldns_rr *rr,
int section);









ldns_status ldns_rrsig2buffer_wire(ldns_buffer *output, const ldns_rr *sigrr);











ldns_status ldns_rr_rdata2buffer_wire(ldns_buffer *output, const ldns_rr *rr);







ldns_status ldns_pkt2buffer_wire(ldns_buffer *output, const ldns_pkt *pkt);







ldns_status ldns_rr_list2buffer_wire(ldns_buffer *output, const ldns_rr_list *rrlist);











ldns_status ldns_rdf2wire(uint8_t **dest, const ldns_rdf *rdf, size_t *size);















ldns_status ldns_rr2wire(uint8_t **dest, const ldns_rr *rr, int section, size_t *size);







ldns_status ldns_pkt2wire(uint8_t **dest, const ldns_pkt *p, size_t *size);

#if defined(__cplusplus)
}
#endif

#endif
