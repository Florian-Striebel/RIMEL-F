































#include <sys/cdefs.h>

enum asn_err snmp_binding_encode(struct asn_buf *, const struct snmp_value *);
enum snmp_code snmp_pdu_encode_header(struct asn_buf *, struct snmp_pdu *);
enum snmp_code snmp_fix_encoding(struct asn_buf *, struct snmp_pdu *);
enum asn_err snmp_parse_pdus_hdr(struct asn_buf *b, struct snmp_pdu *pdu,
asn_len_t *lenp);

enum snmp_code snmp_pdu_calc_digest(const struct snmp_pdu *, uint8_t *);
enum snmp_code snmp_pdu_encrypt(const struct snmp_pdu *);
enum snmp_code snmp_pdu_decrypt(const struct snmp_pdu *);

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "snmp"
#define DEFAULT_LOCAL "/var/run/snmp.sock"
