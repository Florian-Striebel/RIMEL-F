





















#if !defined(LDNS_DANE_H)
#define LDNS_DANE_H

#include <ldns/common.h>
#include <ldns/rdata.h>
#include <ldns/rr.h>
#if LDNS_BUILD_CONFIG_HAVE_SSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif




enum ldns_enum_tlsa_certificate_usage
{

LDNS_TLSA_USAGE_PKIX_TA = 0,
LDNS_TLSA_USAGE_CA_CONSTRAINT = 0,

LDNS_TLSA_USAGE_PKIX_EE = 1,
LDNS_TLSA_USAGE_SERVICE_CERTIFICATE_CONSTRAINT = 1,

LDNS_TLSA_USAGE_DANE_TA = 2,
LDNS_TLSA_USAGE_TRUST_ANCHOR_ASSERTION = 2,

LDNS_TLSA_USAGE_DANE_EE = 3,
LDNS_TLSA_USAGE_DOMAIN_ISSUED_CERTIFICATE = 3,

LDNS_TLSA_USAGE_PRIVCERT = 255
};
typedef enum ldns_enum_tlsa_certificate_usage ldns_tlsa_certificate_usage;




enum ldns_enum_tlsa_selector
{




LDNS_TLSA_SELECTOR_CERT = 0,
LDNS_TLSA_SELECTOR_FULL_CERTIFICATE = 0,





LDNS_TLSA_SELECTOR_SPKI = 1,
LDNS_TLSA_SELECTOR_SUBJECTPUBLICKEYINFO = 1,


LDNS_TLSA_SELECTOR_PRIVSEL = 255
};
typedef enum ldns_enum_tlsa_selector ldns_tlsa_selector;




enum ldns_enum_tlsa_matching_type
{

LDNS_TLSA_MATCHING_TYPE_FULL = 0,
LDNS_TLSA_MATCHING_TYPE_NO_HASH_USED = 0,

LDNS_TLSA_MATCHING_TYPE_SHA2_256 = 1,
LDNS_TLSA_MATCHING_TYPE_SHA256 = 1,

LDNS_TLSA_MATCHING_TYPE_SHA2_512 = 2,
LDNS_TLSA_MATCHING_TYPE_SHA512 = 2,

LDNS_TLSA_MATCHING_TYPE_PRIVMATCH = 255
};
typedef enum ldns_enum_tlsa_matching_type ldns_tlsa_matching_type;




enum ldns_enum_dane_transport
{

LDNS_DANE_TRANSPORT_TCP = 0,

LDNS_DANE_TRANSPORT_UDP = 1,

LDNS_DANE_TRANSPORT_SCTP = 2
};
typedef enum ldns_enum_dane_transport ldns_dane_transport;


#if LDNS_BUILD_CONFIG_USE_DANE










ldns_status ldns_dane_create_tlsa_owner(ldns_rdf** tlsa_owner,
const ldns_rdf* name, uint16_t port,
ldns_dane_transport transport);


#if LDNS_BUILD_CONFIG_HAVE_SSL











ldns_status ldns_dane_cert2rdf(ldns_rdf** rdf, X509* cert,
ldns_tlsa_selector selector,
ldns_tlsa_matching_type matching_type);






























ldns_status ldns_dane_select_certificate(X509** selected_cert,
X509* cert, STACK_OF(X509)* extra_certs,
X509_STORE* pkix_validation_store,
ldns_tlsa_certificate_usage cert_usage, int index);














ldns_status ldns_dane_create_tlsa_rr(ldns_rr** tlsa,
ldns_tlsa_certificate_usage certificate_usage,
ldns_tlsa_selector selector,
ldns_tlsa_matching_type matching_type,
X509* cert);








































ldns_status ldns_dane_verify_rr(const ldns_rr* tlsa_rr,
X509* cert, STACK_OF(X509)* extra_certs,
X509_STORE* pkix_validation_store);


































ldns_status ldns_dane_verify(const ldns_rr_list* tlsas,
X509* cert, STACK_OF(X509)* extra_certs,
X509_STORE* pkix_validation_store);
#endif
#endif

#if defined(__cplusplus)
}
#endif

#endif

