






































#if !defined(snmp_h_)
#define snmp_h_

#include <sys/types.h>

#define BSNMP_MAJOR 1
#define BSNMP_MINOR 13

#define SNMP_COMMUNITY_MAXLEN 128
#define SNMP_MAX_BINDINGS 100
#define SNMP_CONTEXT_NAME_SIZ (32 + 1)
#define SNMP_ENGINE_ID_SIZ 32
#define SNMP_TIME_WINDOW 150

enum snmp_syntax {
SNMP_SYNTAX_NULL = 0,
SNMP_SYNTAX_INTEGER,
SNMP_SYNTAX_OCTETSTRING,
SNMP_SYNTAX_OID,
SNMP_SYNTAX_IPADDRESS,
SNMP_SYNTAX_COUNTER,
SNMP_SYNTAX_GAUGE,
SNMP_SYNTAX_TIMETICKS,


SNMP_SYNTAX_COUNTER64,
SNMP_SYNTAX_NOSUCHOBJECT,
SNMP_SYNTAX_NOSUCHINSTANCE,
SNMP_SYNTAX_ENDOFMIBVIEW,
};

struct snmp_value {
struct asn_oid var;
enum snmp_syntax syntax;
union snmp_values {
int32_t integer;
struct {
u_int len;
u_char *octets;
} octetstring;
struct asn_oid oid;
u_char ipaddress[4];
uint32_t uint32;

uint64_t counter64;
} v;
};

enum snmp_version {
SNMP_Verr = 0,
SNMP_V1 = 1,
SNMP_V2c = 2,
SNMP_V3,
};

#define SNMP_MPM_SNMP_V1 0
#define SNMP_MPM_SNMP_V2c 1
#define SNMP_MPM_SNMP_V3 3

#define SNMP_ADM_STR32_SIZ (32 + 1)
#define SNMP_AUTH_KEY_SIZ 40
#define SNMP_PRIV_KEY_SIZ 32
#define SNMP_USM_AUTH_SIZE 12
#define SNMP_USM_PRIV_SIZE 8
#define SNMP_AUTH_HMACMD5_KEY_SIZ 16
#define SNMP_AUTH_HMACSHA_KEY_SIZ 20
#define SNMP_PRIV_AES_KEY_SIZ 16
#define SNMP_PRIV_DES_KEY_SIZ 8


enum snmp_secmodel {
SNMP_SECMODEL_ANY = 0,
SNMP_SECMODEL_SNMPv1 = 1,
SNMP_SECMODEL_SNMPv2c = 2,
SNMP_SECMODEL_USM = 3,
SNMP_SECMODEL_UNKNOWN
};

enum snmp_usm_level {
SNMP_noAuthNoPriv = 1,
SNMP_authNoPriv = 2,
SNMP_authPriv = 3
};

enum snmp_authentication {
SNMP_AUTH_NOAUTH = 0,
SNMP_AUTH_HMAC_MD5,
SNMP_AUTH_HMAC_SHA
};

enum snmp_privacy {
SNMP_PRIV_NOPRIV = 0,
SNMP_PRIV_DES = 1,
SNMP_PRIV_AES
};

struct snmp_engine {
uint8_t engine_id[SNMP_ENGINE_ID_SIZ];
uint32_t engine_len;
int32_t engine_boots;
int32_t engine_time;
int32_t max_msg_size;
};

struct snmp_user {
char sec_name[SNMP_ADM_STR32_SIZ];
enum snmp_authentication auth_proto;
enum snmp_privacy priv_proto;
uint8_t auth_key[SNMP_AUTH_KEY_SIZ];
uint8_t priv_key[SNMP_PRIV_KEY_SIZ];
};

struct snmp_pdu {
char community[SNMP_COMMUNITY_MAXLEN + 1];
enum snmp_version version;
u_int type;


int32_t identifier;
uint8_t flags;
int32_t security_model;
struct snmp_engine engine;


struct snmp_user user;
uint8_t msg_digest[SNMP_USM_AUTH_SIZE];
uint8_t msg_salt[SNMP_USM_PRIV_SIZE];



uint32_t context_engine_len;
uint8_t context_engine[SNMP_ENGINE_ID_SIZ];
char context_name[SNMP_CONTEXT_NAME_SIZ];


struct asn_oid enterprise;
u_char agent_addr[4];
int32_t generic_trap;
int32_t specific_trap;
uint32_t time_stamp;


int32_t request_id;
int32_t error_status;
int32_t error_index;


size_t outer_len;
asn_len_t scoped_len;
u_char *outer_ptr;
u_char *digest_ptr;
u_char *encrypted_ptr;
u_char *scoped_ptr;
u_char *pdu_ptr;
u_char *vars_ptr;


struct snmp_value bindings[SNMP_MAX_BINDINGS];
u_int nbindings;
};
#define snmp_v1_pdu snmp_pdu

#define SNMP_PDU_GET 0
#define SNMP_PDU_GETNEXT 1
#define SNMP_PDU_RESPONSE 2
#define SNMP_PDU_SET 3
#define SNMP_PDU_TRAP 4
#define SNMP_PDU_GETBULK 5
#define SNMP_PDU_INFORM 6
#define SNMP_PDU_TRAP2 7
#define SNMP_PDU_REPORT 8

#define SNMP_ERR_NOERROR 0
#define SNMP_ERR_TOOBIG 1
#define SNMP_ERR_NOSUCHNAME 2
#define SNMP_ERR_BADVALUE 3
#define SNMP_ERR_READONLY 4
#define SNMP_ERR_GENERR 5
#define SNMP_ERR_NO_ACCESS 6
#define SNMP_ERR_WRONG_TYPE 7
#define SNMP_ERR_WRONG_LENGTH 8
#define SNMP_ERR_WRONG_ENCODING 9
#define SNMP_ERR_WRONG_VALUE 10
#define SNMP_ERR_NO_CREATION 11
#define SNMP_ERR_INCONS_VALUE 12
#define SNMP_ERR_RES_UNAVAIL 13
#define SNMP_ERR_COMMIT_FAILED 14
#define SNMP_ERR_UNDO_FAILED 15
#define SNMP_ERR_AUTH_ERR 16
#define SNMP_ERR_NOT_WRITEABLE 17
#define SNMP_ERR_INCONS_NAME 18

#define SNMP_TRAP_COLDSTART 0
#define SNMP_TRAP_WARMSTART 1
#define SNMP_TRAP_LINKDOWN 2
#define SNMP_TRAP_LINKUP 3
#define SNMP_TRAP_AUTHENTICATION_FAILURE 4
#define SNMP_TRAP_EGP_NEIGHBOR_LOSS 5
#define SNMP_TRAP_ENTERPRISE 6

enum snmp_code {
SNMP_CODE_OK = 0,
SNMP_CODE_FAILED,
SNMP_CODE_BADVERS,
SNMP_CODE_BADLEN,
SNMP_CODE_BADENC,
SNMP_CODE_OORANGE,
SNMP_CODE_BADSECLEVEL,
SNMP_CODE_NOTINTIME,
SNMP_CODE_BADUSER,
SNMP_CODE_BADENGINE,
SNMP_CODE_BADDIGEST,
SNMP_CODE_EDECRYPT
};

#define SNMP_MSG_AUTH_FLAG 0x1
#define SNMP_MSG_PRIV_FLAG 0x2
#define SNMP_MSG_REPORT_FLAG 0x4
#define SNMP_MSG_AUTODISCOVER 0x80

void snmp_value_free(struct snmp_value *);
int snmp_value_parse(const char *, enum snmp_syntax, union snmp_values *);
int snmp_value_copy(struct snmp_value *, const struct snmp_value *);

void snmp_pdu_free(struct snmp_pdu *);
void snmp_pdu_init_secparams(struct snmp_pdu *);
enum snmp_code snmp_pdu_decode(struct asn_buf *b, struct snmp_pdu *pdu, int32_t *);
enum snmp_code snmp_pdu_decode_header(struct asn_buf *, struct snmp_pdu *);
enum snmp_code snmp_pdu_decode_scoped(struct asn_buf *, struct snmp_pdu *, int32_t *);
enum snmp_code snmp_pdu_encode(struct snmp_pdu *, struct asn_buf *);
enum snmp_code snmp_pdu_decode_secmode(struct asn_buf *, struct snmp_pdu *);

int snmp_pdu_snoop(const struct asn_buf *);

void snmp_pdu_dump(const struct snmp_pdu *pdu);

enum snmp_code snmp_passwd_to_keys(struct snmp_user *, char *);
enum snmp_code snmp_get_local_keys(struct snmp_user *, uint8_t *, uint32_t);
enum snmp_code snmp_calc_keychange(struct snmp_user *, uint8_t *);

extern void (*snmp_error)(const char *, ...);
extern void (*snmp_printf)(const char *, ...);

#define TRUTH_MK(F) ((F) ? 1 : 2)
#define TRUTH_GET(T) (((T) == 1) ? 1 : 0)
#define TRUTH_OK(T) ((T) == 1 || (T) == 2)

#endif
