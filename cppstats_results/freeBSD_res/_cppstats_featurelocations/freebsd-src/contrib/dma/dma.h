



































#if !defined(DMA_H)
#define DMA_H

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <arpa/nameser.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <netdb.h>
#include <sysexits.h>

#define VERSION "DragonFly Mail Agent " DMA_VERSION

#define BUF_SIZE 2048
#define ERRMSG_SIZE 1024
#define USERNAME_SIZE 50
#define EHLO_RESPONSE_SIZE BUF_SIZE
#define MIN_RETRY 300
#define MAX_RETRY (3*60*60)
#define MAX_TIMEOUT (5*24*60*60)
#define SLEEP_TIMEOUT 30
#if !defined(PATH_MAX)
#define PATH_MAX 1024
#endif
#define SMTP_PORT 25
#define CON_TIMEOUT (5*60)

#define STARTTLS 0x002
#define SECURETRANSFER 0x004
#define NOSSL 0x008
#define DEFER 0x010
#define INSECURE 0x020
#define FULLBOUNCE 0x040
#define TLS_OPP 0x080
#define NULLCLIENT 0x100

#if !defined(CONF_PATH)
#error Please define CONF_PATH
#endif

#if !defined(LIBEXEC_PATH)
#error Please define LIBEXEC_PATH
#endif

#define SPOOL_FLUSHFILE "flush"

#if !defined(DMA_ROOT_USER)
#define DMA_ROOT_USER "mail"
#endif
#if !defined(DMA_GROUP)
#define DMA_GROUP "mail"
#endif

#if !defined(MBOX_STRICT)
#define MBOX_STRICT 0
#endif


struct stritem {
SLIST_ENTRY(stritem) next;
char *str;
};
SLIST_HEAD(strlist, stritem);

struct alias {
LIST_ENTRY(alias) next;
char *alias;
struct strlist dests;
};
LIST_HEAD(aliases, alias);

struct qitem {
LIST_ENTRY(qitem) next;
const char *sender;
char *addr;
char *queuefn;
char *mailfn;
char *queueid;
FILE *queuef;
FILE *mailf;
int remote;
};
LIST_HEAD(queueh, qitem);

struct queue {
struct queueh queue;
char *id;
FILE *mailf;
char *tmpf;
const char *sender;
};

struct config {
const char *smarthost;
int port;
const char *aliases;
const char *spooldir;
const char *authpath;
const char *certfile;
int features;
const char *mailname;
const char *masquerade_host;
const char *masquerade_user;
const unsigned char *fingerprint;


SSL *ssl;
};


struct authuser {
SLIST_ENTRY(authuser) next;
char *login;
char *password;
char *host;
};
SLIST_HEAD(authusers, authuser);


struct mx_hostentry {
char host[MAXDNAME];
char addr[INET6_ADDRSTRLEN];
int pref;
struct addrinfo ai;
struct sockaddr_storage sa;
};

struct smtp_auth_mechanisms {
int cram_md5;
int login;
};

struct smtp_features {
struct smtp_auth_mechanisms auth;
int starttls;
};


extern struct aliases aliases;
extern struct config config;
extern struct strlist tmpfs;
extern struct authusers authusers;
extern char username[USERNAME_SIZE];
extern uid_t useruid;
extern const char *logident_base;

extern char neterr[ERRMSG_SIZE];
extern char errmsg[ERRMSG_SIZE];


int yyparse(void);
int yywrap(void);
int yylex(void);
extern FILE *yyin;


void trim_line(char *);
void parse_conf(const char *);
void parse_authfile(const char *);


void hmac_md5(unsigned char *, int, unsigned char *, int, unsigned char *);
int smtp_auth_md5(int, char *, char *);
int smtp_init_crypto(int, int, struct smtp_features*);


int dns_get_mx_list(const char *, int, struct mx_hostentry **, int);


char *ssl_errstr(void);
int read_remote(int, int, char *);
ssize_t send_remote_command(int, const char*, ...) __attribute__((__nonnull__(2), __format__ (__printf__, 2, 3)));
int perform_server_greeting(int, struct smtp_features*);
int deliver_remote(struct qitem *);


int base64_encode(const void *, int, char **);
int base64_decode(const char *, void *);


#define EXPAND_ADDR 1
#define EXPAND_WILDCARD 2
int add_recp(struct queue *, const char *, int);
void run_queue(struct queue *);


int newspoolf(struct queue *);
int linkspool(struct queue *);
int load_queue(struct queue *);
void delqueue(struct qitem *);
int acquirespool(struct qitem *);
void dropspool(struct queue *, struct qitem *);
int flushqueue_since(unsigned int);
int flushqueue_signal(void);


int deliver_local(struct qitem *);


void bounce(struct qitem *, const char *);
int readmail(struct queue *, int, int);


const char *hostname(void);
const char *systemhostname(void);
void setlogident(const char *, ...) __attribute__((__format__ (__printf__, 1, 2)));
void errlog(int, const char *, ...) __attribute__((__format__ (__printf__, 2, 3)));
void errlogx(int, const char *, ...) __attribute__((__format__ (__printf__, 2, 3)));
void set_username(void);
void deltmp(void);
int do_timeout(int, int);
int open_locked(const char *, int, ...);
char *rfc822date(void);
int strprefixcmp(const char *, const char *);
void init_random(void);

#endif
