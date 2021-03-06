





#if !defined(_EXTERN_H_)
#define _EXTERN_H_

#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>

#if defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif


EC_KEY *read_ec_pubkey(const char *);
RSA *read_rsa_pubkey(const char *);
EVP_PKEY *read_eddsa_pubkey(const char *);
int base10(const char *, long long *);
int read_blob(const char *, unsigned char **, size_t *);
int write_blob(const char *, const unsigned char *, size_t);
int write_ec_pubkey(const char *, const void *, size_t);
int write_rsa_pubkey(const char *, const void *, size_t);
int write_eddsa_pubkey(const char *, const void *, size_t);
#if defined(SIGNAL_EXAMPLE)
void prepare_signal_handler(int);
extern volatile sig_atomic_t got_signal;
#endif

#endif
