




















#if !defined(_WIN32)
#include <arpa/inet.h>
#endif

#include <pcap/pcap-inttypes.h>
#include <pcap/compiler-tests.h>




#if defined(LBL_ALIGN)



#if PCAP_IS_AT_LEAST_GNUC_VERSION(2,0) && (defined(__alpha) || defined(__alpha__) || defined(__mips) || defined(__mips__))






















































typedef struct {
uint16_t val;
} __attribute__((packed)) unaligned_uint16_t;

typedef struct {
uint32_t val;
} __attribute__((packed)) unaligned_uint32_t;

static inline uint16_t
EXTRACT_16BITS(const void *p)
{
return ((uint16_t)ntohs(((const unaligned_uint16_t *)(p))->val));
}

static inline uint32_t
EXTRACT_32BITS(const void *p)
{
return ((uint32_t)ntohl(((const unaligned_uint32_t *)(p))->val));
}

static inline uint64_t
EXTRACT_64BITS(const void *p)
{
return ((uint64_t)(((uint64_t)ntohl(((const unaligned_uint32_t *)(p) + 0)->val)) << 32 | \
((uint64_t)ntohl(((const unaligned_uint32_t *)(p) + 1)->val)) << 0));
}

#else







#define EXTRACT_16BITS(p) ((uint16_t)(((uint16_t)(*((const uint8_t *)(p) + 0)) << 8) | ((uint16_t)(*((const uint8_t *)(p) + 1)) << 0)))


#define EXTRACT_32BITS(p) ((uint32_t)(((uint32_t)(*((const uint8_t *)(p) + 0)) << 24) | ((uint32_t)(*((const uint8_t *)(p) + 1)) << 16) | ((uint32_t)(*((const uint8_t *)(p) + 2)) << 8) | ((uint32_t)(*((const uint8_t *)(p) + 3)) << 0)))




#define EXTRACT_64BITS(p) ((uint64_t)(((uint64_t)(*((const uint8_t *)(p) + 0)) << 56) | ((uint64_t)(*((const uint8_t *)(p) + 1)) << 48) | ((uint64_t)(*((const uint8_t *)(p) + 2)) << 40) | ((uint64_t)(*((const uint8_t *)(p) + 3)) << 32) | ((uint64_t)(*((const uint8_t *)(p) + 4)) << 24) | ((uint64_t)(*((const uint8_t *)(p) + 5)) << 16) | ((uint64_t)(*((const uint8_t *)(p) + 6)) << 8) | ((uint64_t)(*((const uint8_t *)(p) + 7)) << 0)))








#endif
#else




static inline uint16_t
EXTRACT_16BITS(const void *p)
{
return ((uint16_t)ntohs(*(const uint16_t *)(p)));
}

static inline uint32_t
EXTRACT_32BITS(const void *p)
{
return ((uint32_t)ntohl(*(const uint32_t *)(p)));
}

static inline uint64_t
EXTRACT_64BITS(const void *p)
{
return ((uint64_t)(((uint64_t)ntohl(*((const uint32_t *)(p) + 0))) << 32 | \
((uint64_t)ntohl(*((const uint32_t *)(p) + 1))) << 0));

}

#endif

#define EXTRACT_24BITS(p) ((uint32_t)(((uint32_t)(*((const uint8_t *)(p) + 0)) << 16) | ((uint32_t)(*((const uint8_t *)(p) + 1)) << 8) | ((uint32_t)(*((const uint8_t *)(p) + 2)) << 0)))




#define EXTRACT_40BITS(p) ((uint64_t)(((uint64_t)(*((const uint8_t *)(p) + 0)) << 32) | ((uint64_t)(*((const uint8_t *)(p) + 1)) << 24) | ((uint64_t)(*((const uint8_t *)(p) + 2)) << 16) | ((uint64_t)(*((const uint8_t *)(p) + 3)) << 8) | ((uint64_t)(*((const uint8_t *)(p) + 4)) << 0)))






#define EXTRACT_48BITS(p) ((uint64_t)(((uint64_t)(*((const uint8_t *)(p) + 0)) << 40) | ((uint64_t)(*((const uint8_t *)(p) + 1)) << 32) | ((uint64_t)(*((const uint8_t *)(p) + 2)) << 24) | ((uint64_t)(*((const uint8_t *)(p) + 3)) << 16) | ((uint64_t)(*((const uint8_t *)(p) + 4)) << 8) | ((uint64_t)(*((const uint8_t *)(p) + 5)) << 0)))







#define EXTRACT_56BITS(p) ((uint64_t)(((uint64_t)(*((const uint8_t *)(p) + 0)) << 48) | ((uint64_t)(*((const uint8_t *)(p) + 1)) << 40) | ((uint64_t)(*((const uint8_t *)(p) + 2)) << 32) | ((uint64_t)(*((const uint8_t *)(p) + 3)) << 24) | ((uint64_t)(*((const uint8_t *)(p) + 4)) << 16) | ((uint64_t)(*((const uint8_t *)(p) + 5)) << 8) | ((uint64_t)(*((const uint8_t *)(p) + 6)) << 0)))












#define EXTRACT_LE_8BITS(p) (*(p))
#define EXTRACT_LE_16BITS(p) ((uint16_t)(((uint16_t)(*((const uint8_t *)(p) + 1)) << 8) | ((uint16_t)(*((const uint8_t *)(p) + 0)) << 0)))


#define EXTRACT_LE_32BITS(p) ((uint32_t)(((uint32_t)(*((const uint8_t *)(p) + 3)) << 24) | ((uint32_t)(*((const uint8_t *)(p) + 2)) << 16) | ((uint32_t)(*((const uint8_t *)(p) + 1)) << 8) | ((uint32_t)(*((const uint8_t *)(p) + 0)) << 0)))




#define EXTRACT_LE_24BITS(p) ((uint32_t)(((uint32_t)(*((const uint8_t *)(p) + 2)) << 16) | ((uint32_t)(*((const uint8_t *)(p) + 1)) << 8) | ((uint32_t)(*((const uint8_t *)(p) + 0)) << 0)))



#define EXTRACT_LE_64BITS(p) ((uint64_t)(((uint64_t)(*((const uint8_t *)(p) + 7)) << 56) | ((uint64_t)(*((const uint8_t *)(p) + 6)) << 48) | ((uint64_t)(*((const uint8_t *)(p) + 5)) << 40) | ((uint64_t)(*((const uint8_t *)(p) + 4)) << 32) | ((uint64_t)(*((const uint8_t *)(p) + 3)) << 24) | ((uint64_t)(*((const uint8_t *)(p) + 2)) << 16) | ((uint64_t)(*((const uint8_t *)(p) + 1)) << 8) | ((uint64_t)(*((const uint8_t *)(p) + 0)) << 0)))








