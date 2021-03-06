











#if !defined(_UTIL_H)
#define _UTIL_H

#include <inttypes.h>
#include <sys/types.h>
#include <unistd.h>
#include <ldns/common.h>
#include <time.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define dprintf(X,Y) fprintf(stderr, (X), (Y))


#define LDNS_VERSION "1.7.0"
#define LDNS_REVISION ((1<<16)|(7<<8)|(0))




#if defined(S_SPLINT_S)
#define INLINE
#else
#if defined(SWIG)
#define INLINE static
#else
#define INLINE static inline
#endif
#endif




#define LDNS_MALLOC(type) LDNS_XMALLOC(type, 1)

#define LDNS_XMALLOC(type, count) ((type *) malloc((count) * sizeof(type)))

#define LDNS_CALLOC(type, count) ((type *) calloc((count), sizeof(type)))

#define LDNS_REALLOC(ptr, type) LDNS_XREALLOC((ptr), type, 1)

#define LDNS_XREALLOC(ptr, type, count) ((type *) realloc((ptr), (count) * sizeof(type)))


#define LDNS_FREE(ptr) do { free((ptr)); (ptr) = NULL; } while (0)


#define LDNS_DEP printf("DEPRECATED FUNCTION!\n");





INLINE uint16_t
ldns_read_uint16(const void *src)
{
#if defined(ALLOW_UNALIGNED_ACCESSES)
return ntohs(*(const uint16_t *) src);
#else
const uint8_t *p = (const uint8_t *) src;
return ((uint16_t) p[0] << 8) | (uint16_t) p[1];
#endif
}

INLINE uint32_t
ldns_read_uint32(const void *src)
{
#if defined(ALLOW_UNALIGNED_ACCESSES)
return ntohl(*(const uint32_t *) src);
#else
const uint8_t *p = (const uint8_t *) src;
return ( ((uint32_t) p[0] << 24)
| ((uint32_t) p[1] << 16)
| ((uint32_t) p[2] << 8)
| (uint32_t) p[3]);
#endif
}





INLINE void
ldns_write_uint16(void *dst, uint16_t data)
{
#if defined(ALLOW_UNALIGNED_ACCESSES)
* (uint16_t *) dst = htons(data);
#else
uint8_t *p = (uint8_t *) dst;
p[0] = (uint8_t) ((data >> 8) & 0xff);
p[1] = (uint8_t) (data & 0xff);
#endif
}

INLINE void
ldns_write_uint32(void *dst, uint32_t data)
{
#if defined(ALLOW_UNALIGNED_ACCESSES)
* (uint32_t *) dst = htonl(data);
#else
uint8_t *p = (uint8_t *) dst;
p[0] = (uint8_t) ((data >> 24) & 0xff);
p[1] = (uint8_t) ((data >> 16) & 0xff);
p[2] = (uint8_t) ((data >> 8) & 0xff);
p[3] = (uint8_t) (data & 0xff);
#endif
}


INLINE void
ldns_write_uint64_as_uint48(void *dst, uint64_t data)
{
uint8_t *p = (uint8_t *) dst;
p[0] = (uint8_t) ((data >> 40) & 0xff);
p[1] = (uint8_t) ((data >> 32) & 0xff);
p[2] = (uint8_t) ((data >> 24) & 0xff);
p[3] = (uint8_t) ((data >> 16) & 0xff);
p[4] = (uint8_t) ((data >> 8) & 0xff);
p[5] = (uint8_t) (data & 0xff);
}








struct ldns_schwartzian_compare_struct {
void *original_object;
void *transformed_object;
};








struct ldns_struct_lookup_table {
int id;
const char *name;
};
typedef struct ldns_struct_lookup_table ldns_lookup_table;







ldns_lookup_table *ldns_lookup_by_name(ldns_lookup_table table[],
const char *name);







ldns_lookup_table *ldns_lookup_by_id(ldns_lookup_table table[], int id);









int ldns_get_bit(uint8_t bits[], size_t index);










int ldns_get_bit_r(uint8_t bits[], size_t index);











void ldns_set_bit(uint8_t *byte, int bit_nr, bool value);






INLINE long
ldns_power(long a, long b) {
long result = 1;
while (b > 0) {
if (b & 1) {
result *= a;
if (b == 1) {
return result;
}
}
a *= a;
b /= 2;
}
return result;
}






int ldns_hexdigit_to_int(char ch);






char ldns_int_to_hexdigit(int ch);










int
ldns_hexstring_to_data(uint8_t *data, const char *str);





const char * ldns_version(void);







time_t ldns_mktime_from_utc(const struct tm *tm);

time_t mktime_from_utc(const struct tm *tm);















struct tm * ldns_serial_arithmitics_gmtime_r(int32_t time, time_t now, struct tm *result);




















int ldns_init_random(FILE *fd, unsigned int size);






uint16_t ldns_get_random(void);








char *ldns_bubblebabble(uint8_t *data, size_t len);


INLINE time_t ldns_time(time_t *t) { return time(t); }






INLINE size_t ldns_b32_ntop_calculate_size(size_t src_data_length)
{
return src_data_length == 0 ? 0 : ((src_data_length - 1) / 5 + 1) * 8;
}

INLINE size_t ldns_b32_ntop_calculate_size_no_padding(size_t src_data_length)
{
return ((src_data_length + 3) * 8 / 5) - 4;
}

int ldns_b32_ntop(const uint8_t* src_data, size_t src_data_length,
char* target_text_buffer, size_t target_text_buffer_size);

int ldns_b32_ntop_extended_hex(const uint8_t* src_data, size_t src_data_length,
char* target_text_buffer, size_t target_text_buffer_size);

#if ! LDNS_BUILD_CONFIG_HAVE_B32_NTOP

int b32_ntop(const uint8_t* src_data, size_t src_data_length,
char* target_text_buffer, size_t target_text_buffer_size);

int b32_ntop_extended_hex(const uint8_t* src_data, size_t src_data_length,
char* target_text_buffer, size_t target_text_buffer_size);

#endif






INLINE size_t ldns_b32_pton_calculate_size(size_t src_text_length)
{
return src_text_length * 5 / 8;
}

int ldns_b32_pton(const char* src_text, size_t src_text_length,
uint8_t* target_data_buffer, size_t target_data_buffer_size);

int ldns_b32_pton_extended_hex(const char* src_text, size_t src_text_length,
uint8_t* target_data_buffer, size_t target_data_buffer_size);

#if ! LDNS_BUILD_CONFIG_HAVE_B32_PTON

int b32_pton(const char* src_text, size_t src_text_length,
uint8_t* target_data_buffer, size_t target_data_buffer_size);

int b32_pton_extended_hex(const char* src_text, size_t src_text_length,
uint8_t* target_data_buffer, size_t target_data_buffer_size);

#endif


#if defined(__cplusplus)
}
#endif

#endif
