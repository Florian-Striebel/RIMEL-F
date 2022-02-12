





#if !defined(_FIDO_PARAM_H)
#define _FIDO_PARAM_H


#define CTAP_AUTHDATA_USER_PRESENT 0x01
#define CTAP_AUTHDATA_USER_VERIFIED 0x04
#define CTAP_AUTHDATA_ATT_CRED 0x40
#define CTAP_AUTHDATA_EXT_DATA 0x80


#define CTAP_CMD_PING 0x01
#define CTAP_CMD_MSG 0x03
#define CTAP_CMD_LOCK 0x04
#define CTAP_CMD_INIT 0x06
#define CTAP_CMD_WINK 0x08
#define CTAP_CMD_CBOR 0x10
#define CTAP_CMD_CANCEL 0x11
#define CTAP_KEEPALIVE 0x3b
#define CTAP_FRAME_INIT 0x80


#define CTAP_CBOR_MAKECRED 0x01
#define CTAP_CBOR_ASSERT 0x02
#define CTAP_CBOR_GETINFO 0x04
#define CTAP_CBOR_CLIENT_PIN 0x06
#define CTAP_CBOR_RESET 0x07
#define CTAP_CBOR_NEXT_ASSERT 0x08
#define CTAP_CBOR_LARGEBLOB 0x0c
#define CTAP_CBOR_CONFIG 0x0d
#define CTAP_CBOR_BIO_ENROLL_PRE 0x40
#define CTAP_CBOR_CRED_MGMT_PRE 0x41


#define CTAP_PIN_PROTOCOL1 1
#define CTAP_PIN_PROTOCOL2 2


#define U2F_CMD_REGISTER 0x01
#define U2F_CMD_AUTH 0x02


#define U2F_AUTH_SIGN 0x03
#define U2F_AUTH_CHECK 0x07


#define SW1_MORE_DATA 0x61
#define SW_CONDITIONS_NOT_SATISFIED 0x6985
#define SW_WRONG_DATA 0x6a80
#define SW_NO_ERROR 0x9000


#define CTAP_CID_BROADCAST 0xffffffff

#define CTAP_INIT_HEADER_LEN 7
#define CTAP_CONT_HEADER_LEN 5


#define CTAP_MAX_REPORT_LEN 64


#define CTAP_MIN_REPORT_LEN (CTAP_INIT_HEADER_LEN + 1)


#if !defined(FIDO_RANDOM_DEV)
#define FIDO_RANDOM_DEV "/dev/urandom"
#endif


#if !defined(FIDO_MAXMSG)
#define FIDO_MAXMSG 2048
#endif


#define FIDO_CAP_WINK 0x01
#define FIDO_CAP_CBOR 0x04
#define FIDO_CAP_NMSG 0x08


#define COSE_ES256 -7
#define COSE_EDDSA -8
#define COSE_ECDH_ES256 -25
#define COSE_RS256 -257


#define COSE_KTY_OKP 1
#define COSE_KTY_EC2 2
#define COSE_KTY_RSA 3


#define COSE_P256 1
#define COSE_ED25519 6


#define FIDO_EXT_HMAC_SECRET 0x01
#define FIDO_EXT_CRED_PROTECT 0x02
#define FIDO_EXT_LARGEBLOB_KEY 0x04
#define FIDO_EXT_CRED_BLOB 0x08


#define FIDO_CRED_PROT_UV_OPTIONAL 0x01
#define FIDO_CRED_PROT_UV_OPTIONAL_WITH_ID 0x02
#define FIDO_CRED_PROT_UV_REQUIRED 0x03

#if defined(_FIDO_INTERNAL)
#define FIDO_EXT_ASSERT_MASK (FIDO_EXT_HMAC_SECRET|FIDO_EXT_LARGEBLOB_KEY| FIDO_EXT_CRED_BLOB)

#define FIDO_EXT_CRED_MASK (FIDO_EXT_HMAC_SECRET|FIDO_EXT_CRED_PROTECT| FIDO_EXT_LARGEBLOB_KEY|FIDO_EXT_CRED_BLOB)

#endif

#endif
