




































#if defined(__APPLE__) && defined(__clang__)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <openssl/x509v3.h>
#include <openssl/ssl.h>
#include <string.h>

#include "openssl_hostname_validation.h"
#include "hostcheck.h"

#define HOSTNAME_MAX_SIZE 255

#if OPENSSL_VERSION_NUMBER < 0x10100000L
#define ASN1_STRING_get0_data ASN1_STRING_data
#endif









static HostnameValidationResult matches_common_name(const char *hostname, const X509 *server_cert) {
int common_name_loc = -1;
X509_NAME_ENTRY *common_name_entry = NULL;
ASN1_STRING *common_name_asn1 = NULL;
const char *common_name_str = NULL;


common_name_loc = X509_NAME_get_index_by_NID(X509_get_subject_name((X509 *) server_cert), NID_commonName, -1);
if (common_name_loc < 0) {
return Error;
}


common_name_entry = X509_NAME_get_entry(X509_get_subject_name((X509 *) server_cert), common_name_loc);
if (common_name_entry == NULL) {
return Error;
}


common_name_asn1 = X509_NAME_ENTRY_get_data(common_name_entry);
if (common_name_asn1 == NULL) {
return Error;
}
common_name_str = (char *) ASN1_STRING_get0_data(common_name_asn1);


if ((size_t)ASN1_STRING_length(common_name_asn1) != strlen(common_name_str)) {
return MalformedCertificate;
}


if (Curl_cert_hostcheck(common_name_str, hostname) == CURL_HOST_MATCH) {
return MatchFound;
}
else {
return MatchNotFound;
}
}










static HostnameValidationResult matches_subject_alternative_name(const char *hostname, const X509 *server_cert) {
HostnameValidationResult result = MatchNotFound;
int i;
int san_names_nb = -1;
STACK_OF(GENERAL_NAME) *san_names = NULL;


san_names = X509_get_ext_d2i((X509 *) server_cert, NID_subject_alt_name, NULL, NULL);
if (san_names == NULL) {
return NoSANPresent;
}
san_names_nb = sk_GENERAL_NAME_num(san_names);


for (i=0; i<san_names_nb; i++) {
const GENERAL_NAME *current_name = sk_GENERAL_NAME_value(san_names, i);

if (current_name->type == GEN_DNS) {

const char *dns_name = (char *) ASN1_STRING_get0_data(current_name->d.dNSName);


if ((size_t)ASN1_STRING_length(current_name->d.dNSName) != strlen(dns_name)) {
result = MalformedCertificate;
break;
}
else {
if (Curl_cert_hostcheck(dns_name, hostname)
== CURL_HOST_MATCH) {
result = MatchFound;
break;
}
}
}
}
sk_GENERAL_NAME_pop_free(san_names, GENERAL_NAME_free);

return result;
}













HostnameValidationResult validate_hostname(const char *hostname, const X509 *server_cert) {
HostnameValidationResult result;

if((hostname == NULL) || (server_cert == NULL))
return Error;


result = matches_subject_alternative_name(hostname, server_cert);
if (result == NoSANPresent) {

result = matches_common_name(hostname, server_cert);
}

return result;
}
