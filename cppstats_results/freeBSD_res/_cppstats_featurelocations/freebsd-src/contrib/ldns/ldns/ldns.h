























































































#if !defined(LDNS_DNS_H)
#define LDNS_DNS_H

#include <stdio.h>
#include <stdlib.h>

#include <ldns/util.h>
#include <ldns/buffer.h>
#include <ldns/common.h>
#include <ldns/dane.h>
#include <ldns/dname.h>
#include <ldns/dnssec.h>
#include <ldns/dnssec_verify.h>
#include <ldns/dnssec_sign.h>
#include <ldns/duration.h>
#include <ldns/error.h>
#include <ldns/higher.h>
#include <ldns/host2str.h>
#include <ldns/host2wire.h>
#include <ldns/net.h>
#include <ldns/packet.h>
#include <ldns/rdata.h>
#include <ldns/resolver.h>
#include <ldns/rr.h>
#include <ldns/str2host.h>
#include <ldns/tsig.h>
#include <ldns/update.h>
#include <ldns/wire2host.h>
#include <ldns/rr_functions.h>
#include <ldns/keys.h>
#include <ldns/parse.h>
#include <ldns/zone.h>
#include <ldns/dnssec_zone.h>
#include <ldns/radix.h>
#include <ldns/rbtree.h>
#include <ldns/sha1.h>
#include <ldns/sha2.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define LDNS_IP4ADDRLEN (32/8)
#define LDNS_IP6ADDRLEN (128/8)
#define LDNS_PORT 53
#define LDNS_ROOT_LABEL_STR "."
#define LDNS_DEFAULT_TTL 3600




extern ldns_lookup_table ldns_certificate_types[];

extern ldns_lookup_table ldns_algorithms[];

extern ldns_lookup_table ldns_cert_algorithms[];

extern ldns_lookup_table ldns_rr_classes[];

extern ldns_lookup_table ldns_rcodes[];

extern ldns_lookup_table ldns_opcodes[];

extern ldns_lookup_table ldns_edns_flags[];

#if defined(__cplusplus)
}
#endif

#endif
