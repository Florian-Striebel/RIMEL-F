






























#if !defined(__HIREDIS_SSL_H)
#define __HIREDIS_SSL_H

#if defined(__cplusplus)
extern "C" {
#endif




struct ssl_st;




typedef struct redisSSLContext redisSSLContext;





typedef enum {
REDIS_SSL_CTX_NONE = 0,
REDIS_SSL_CTX_CREATE_FAILED,
REDIS_SSL_CTX_CERT_KEY_REQUIRED,
REDIS_SSL_CTX_CA_CERT_LOAD_FAILED,
REDIS_SSL_CTX_CLIENT_CERT_LOAD_FAILED,
REDIS_SSL_CTX_PRIVATE_KEY_LOAD_FAILED
} redisSSLContextError;





const char *redisSSLContextGetError(redisSSLContextError error);








int redisInitOpenSSL(void);






















redisSSLContext *redisCreateSSLContext(const char *cacert_filename, const char *capath,
const char *cert_filename, const char *private_key_filename,
const char *server_name, redisSSLContextError *error);




void redisFreeSSLContext(redisSSLContext *redis_ssl_ctx);









int redisInitiateSSLWithContext(redisContext *c, redisSSLContext *redis_ssl_ctx);





int redisInitiateSSL(redisContext *c, struct ssl_st *ssl);

#if defined(__cplusplus)
}
#endif

#endif
