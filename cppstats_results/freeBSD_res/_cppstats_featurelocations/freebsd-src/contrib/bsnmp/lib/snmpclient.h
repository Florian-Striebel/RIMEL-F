






























#if !defined(_BSNMP_SNMPCLIENT_H)
#define _BSNMP_SNMPCLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stddef.h>


#define SNMP_STRERROR_LEN 200

#define SNMP_LOCAL_PATH "/tmp/snmpXXXXXXXXXXXXXX"




#define SNMP_TRANS_UDP 0
#define SNMP_TRANS_LOC_DGRAM 1
#define SNMP_TRANS_LOC_STREAM 2
#define SNMP_TRANS_UDP6 3




typedef void (*snmp_send_cb_f)(struct snmp_pdu *, struct snmp_pdu *, void *);


typedef void (*snmp_timeout_cb_f)(void * );


typedef void *(*snmp_timeout_start_f)(struct timeval *timeout,
snmp_timeout_cb_f callback, void *);


typedef void (*snmp_timeout_stop_f)(void *timeout_id);




struct snmp_client {
enum snmp_version version;
int trans;


char *cport;
char *chost;

char read_community[SNMP_COMMUNITY_MAXLEN + 1];
char write_community[SNMP_COMMUNITY_MAXLEN + 1];


int32_t identifier;
int32_t security_model;
struct snmp_engine engine;
struct snmp_user user;


uint32_t clen;
uint8_t cengine[SNMP_ENGINE_ID_SIZ];
char cname[SNMP_CONTEXT_NAME_SIZ];

struct timeval timeout;
u_int retries;

int dump_pdus;

size_t txbuflen;
size_t rxbuflen;

int fd;

int32_t next_reqid;
int32_t max_reqid;
int32_t min_reqid;

char error[SNMP_STRERROR_LEN];

snmp_timeout_start_f timeout_start;
snmp_timeout_stop_f timeout_stop;

char local_path[sizeof(SNMP_LOCAL_PATH)];
};


extern struct snmp_client snmp_client;


void snmp_client_init(struct snmp_client *);


int snmp_client_set_host(struct snmp_client *, const char *);
int snmp_client_set_port(struct snmp_client *, const char *);


int snmp_open(const char *_hostname, const char *_portname,
const char *_read_community, const char *_write_community);


void snmp_close(void);


void snmp_pdu_create(struct snmp_pdu *, u_int _op);


int snmp_add_binding(struct snmp_pdu *, ...);


int snmp_pdu_check(const struct snmp_pdu *_req, const struct snmp_pdu *_resp);

int32_t snmp_pdu_send(struct snmp_pdu *_pdu, snmp_send_cb_f _func, void *_arg);


int snmp_oid_append(struct asn_oid *_oid, const char *_fmt, ...);


int snmp_receive(int _blocking);






struct snmp_table {

struct asn_oid table;

struct asn_oid last_change;

u_int max_iter;

size_t entry_size;

u_int index_size;

uint64_t req_mask;


struct snmp_table_entry {

asn_subid_t subid;

enum snmp_syntax syntax;


off_t offset;
#if defined(__GNUC__) && __GNUC__ < 3
} entries[0];
#else
} entries[];
#endif
};


typedef void (*snmp_table_cb_f)(void *_list, void *_arg, int _res);


int snmp_table_fetch(const struct snmp_table *descr, void *);
int snmp_table_fetch_async(const struct snmp_table *, void *,
snmp_table_cb_f, void *);


int snmp_dialog(struct snmp_pdu *_req, struct snmp_pdu *_resp);


int snmp_discover_engine(char *);


int snmp_parse_server(struct snmp_client *, const char *);

#endif
