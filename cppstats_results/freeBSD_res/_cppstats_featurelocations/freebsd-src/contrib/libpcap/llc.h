
























#define LLC_U_FMT 3
#define LLC_GSAP 1
#define LLC_IG 1
#define LLC_S_FMT 1

#define LLC_U_POLL 0x10
#define LLC_IS_POLL 0x0100
#define LLC_XID_FI 0x81

#define LLC_U_CMD_MASK 0xef
#define LLC_UI 0x03
#define LLC_UA 0x63
#define LLC_DISC 0x43
#define LLC_DM 0x0f
#define LLC_SABME 0x6f
#define LLC_TEST 0xe3
#define LLC_XID 0xaf
#define LLC_FRMR 0x87

#define LLC_S_CMD_MASK 0x0f
#define LLC_RR 0x0001
#define LLC_RNR 0x0005
#define LLC_REJ 0x0009

#define LLC_IS_NR(is) (((is) >> 9) & 0x7f)
#define LLC_I_NS(is) (((is) >> 1) & 0x7f)





#if !defined(LLCSAP_NULL)
#define LLCSAP_NULL 0x00
#endif
#if !defined(LLCSAP_GLOBAL)
#define LLCSAP_GLOBAL 0xff
#endif
#if !defined(LLCSAP_8021B_I)
#define LLCSAP_8021B_I 0x02
#endif
#if !defined(LLCSAP_8021B_G)
#define LLCSAP_8021B_G 0x03
#endif
#if !defined(LLCSAP_IP)
#define LLCSAP_IP 0x06
#endif
#if !defined(LLCSAP_PROWAYNM)
#define LLCSAP_PROWAYNM 0x0e
#endif
#if !defined(LLCSAP_8021D)
#define LLCSAP_8021D 0x42
#endif
#if !defined(LLCSAP_RS511)
#define LLCSAP_RS511 0x4e
#endif
#if !defined(LLCSAP_ISO8208)
#define LLCSAP_ISO8208 0x7e
#endif
#if !defined(LLCSAP_PROWAY)
#define LLCSAP_PROWAY 0x8e
#endif
#if !defined(LLCSAP_SNAP)
#define LLCSAP_SNAP 0xaa
#endif
#if !defined(LLCSAP_IPX)
#define LLCSAP_IPX 0xe0
#endif
#if !defined(LLCSAP_NETBEUI)
#define LLCSAP_NETBEUI 0xf0
#endif
#if !defined(LLCSAP_ISONS)
#define LLCSAP_ISONS 0xfe
#endif
