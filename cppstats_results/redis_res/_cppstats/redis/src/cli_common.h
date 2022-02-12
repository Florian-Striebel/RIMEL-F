#include <hiredis.h>
#include <sdscompat.h>
typedef struct cliSSLconfig {
char *sni;
char *cacert;
char *cacertdir;
int skip_cert_verify;
char *cert;
char *key;
char* ciphers;
char* ciphersuites;
} cliSSLconfig;
typedef struct cliConnInfo {
char *hostip;
int hostport;
int input_dbnum;
char *auth;
char *user;
} cliConnInfo;
int cliSecureConnection(redisContext *c, cliSSLconfig config, const char **err);
ssize_t cliWriteConn(redisContext *c, const char *buf, size_t buf_len);
int cliSecureInit();
sds readArgFromStdin(void);
sds *getSdsArrayFromArgv(int argc,char **argv, int quoted);
sds unquoteCString(char *str);
void parseRedisUri(const char *uri, const char* tool_name, cliConnInfo *connInfo, int *tls_flag);
void freeCliConnInfo(cliConnInfo connInfo);
