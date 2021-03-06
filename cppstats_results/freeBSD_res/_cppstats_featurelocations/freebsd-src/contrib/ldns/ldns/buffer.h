











#if !defined(LDNS_BUFFER_H)
#define LDNS_BUFFER_H

#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include <ldns/error.h>
#include <ldns/common.h>

#include "ldns/util.h"

#if defined(__cplusplus)
extern "C" {
#endif





#define LDNS_MIN_BUFLEN 512
















struct ldns_struct_buffer
{

size_t _position;


size_t _limit;


size_t _capacity;


uint8_t *_data;


unsigned _fixed : 1;




ldns_status _status;
};
typedef struct ldns_struct_buffer ldns_buffer;


#if defined(NDEBUG)
INLINE void
ldns_buffer_invariant(const ldns_buffer *ATTR_UNUSED(buffer))
{
}
#else
INLINE void
ldns_buffer_invariant(const ldns_buffer *buffer)
{
assert(buffer != NULL);
assert(buffer->_position <= buffer->_limit);
assert(buffer->_limit <= buffer->_capacity);
assert(buffer->_data != NULL);
}
#endif







ldns_buffer *ldns_buffer_new(size_t capacity);










void ldns_buffer_new_frm_data(ldns_buffer *buffer, const void *data, size_t size);






INLINE void ldns_buffer_clear(ldns_buffer *buffer)
{
ldns_buffer_invariant(buffer);



buffer->_position = 0;
buffer->_limit = buffer->_capacity;
}









INLINE void ldns_buffer_flip(ldns_buffer *buffer)
{
ldns_buffer_invariant(buffer);

buffer->_limit = buffer->_position;
buffer->_position = 0;
}






INLINE void ldns_buffer_rewind(ldns_buffer *buffer)
{
ldns_buffer_invariant(buffer);

buffer->_position = 0;
}






INLINE size_t
ldns_buffer_position(const ldns_buffer *buffer)
{
return buffer->_position;
}







INLINE void
ldns_buffer_set_position(ldns_buffer *buffer, size_t mark)
{
assert(mark <= buffer->_limit);
buffer->_position = mark;
}








INLINE void
ldns_buffer_skip(ldns_buffer *buffer, ssize_t count)
{
assert(buffer->_position + count <= buffer->_limit);
buffer->_position += count;
}






INLINE size_t
ldns_buffer_limit(const ldns_buffer *buffer)
{
return buffer->_limit;
}







INLINE void
ldns_buffer_set_limit(ldns_buffer *buffer, size_t limit)
{
assert(limit <= buffer->_capacity);
buffer->_limit = limit;
if (buffer->_position > buffer->_limit)
buffer->_position = buffer->_limit;
}






INLINE size_t
ldns_buffer_capacity(const ldns_buffer *buffer)
{
return buffer->_capacity;
}









bool ldns_buffer_set_capacity(ldns_buffer *buffer, size_t capacity);











bool ldns_buffer_reserve(ldns_buffer *buffer, size_t amount);







INLINE uint8_t *
ldns_buffer_at(const ldns_buffer *buffer, size_t at)
{
assert(at <= buffer->_limit);
return buffer->_data + at;
}







INLINE uint8_t *
ldns_buffer_begin(const ldns_buffer *buffer)
{
return ldns_buffer_at(buffer, 0);
}







INLINE uint8_t *
ldns_buffer_end(const ldns_buffer *buffer)
{
return ldns_buffer_at(buffer, buffer->_limit);
}






INLINE uint8_t *
ldns_buffer_current(const ldns_buffer *buffer)
{
return ldns_buffer_at(buffer, buffer->_position);
}








INLINE size_t
ldns_buffer_remaining_at(const ldns_buffer *buffer, size_t at)
{
ldns_buffer_invariant(buffer);
assert(at <= buffer->_limit);
return buffer->_limit - at;
}







INLINE size_t
ldns_buffer_remaining(const ldns_buffer *buffer)
{
return ldns_buffer_remaining_at(buffer, buffer->_position);
}










INLINE int
ldns_buffer_available_at(const ldns_buffer *buffer, size_t at, size_t count)
{
return count <= ldns_buffer_remaining_at(buffer, at);
}







INLINE int
ldns_buffer_available(const ldns_buffer *buffer, size_t count)
{
return ldns_buffer_available_at(buffer, buffer->_position, count);
}








INLINE void
ldns_buffer_write_at(ldns_buffer *buffer, size_t at, const void *data, size_t count)
{
assert(ldns_buffer_available_at(buffer, at, count));
memcpy(buffer->_data + at, data, count);
}







INLINE void
ldns_buffer_write(ldns_buffer *buffer, const void *data, size_t count)
{
ldns_buffer_write_at(buffer, buffer->_position, data, count);
buffer->_position += count;
}







INLINE void
ldns_buffer_write_string_at(ldns_buffer *buffer, size_t at, const char *str)
{
ldns_buffer_write_at(buffer, at, str, strlen(str));
}






INLINE void
ldns_buffer_write_string(ldns_buffer *buffer, const char *str)
{
ldns_buffer_write(buffer, str, strlen(str));
}







INLINE void
ldns_buffer_write_u8_at(ldns_buffer *buffer, size_t at, uint8_t data)
{
assert(ldns_buffer_available_at(buffer, at, sizeof(data)));
buffer->_data[at] = data;
}






INLINE void
ldns_buffer_write_u8(ldns_buffer *buffer, uint8_t data)
{
ldns_buffer_write_u8_at(buffer, buffer->_position, data);
buffer->_position += sizeof(data);
}







INLINE void
ldns_buffer_write_u16_at(ldns_buffer *buffer, size_t at, uint16_t data)
{
assert(ldns_buffer_available_at(buffer, at, sizeof(data)));
ldns_write_uint16(buffer->_data + at, data);
}






INLINE void
ldns_buffer_write_u16(ldns_buffer *buffer, uint16_t data)
{
ldns_buffer_write_u16_at(buffer, buffer->_position, data);
buffer->_position += sizeof(data);
}







INLINE void
ldns_buffer_write_u32_at(ldns_buffer *buffer, size_t at, uint32_t data)
{
assert(ldns_buffer_available_at(buffer, at, sizeof(data)));
ldns_write_uint32(buffer->_data + at, data);
}






INLINE void
ldns_buffer_write_u32(ldns_buffer *buffer, uint32_t data)
{
ldns_buffer_write_u32_at(buffer, buffer->_position, data);
buffer->_position += sizeof(data);
}








INLINE void
ldns_buffer_read_at(const ldns_buffer *buffer, size_t at, void *data, size_t count)
{
assert(ldns_buffer_available_at(buffer, at, count));
memcpy(data, buffer->_data + at, count);
}







INLINE void
ldns_buffer_read(ldns_buffer *buffer, void *data, size_t count)
{
ldns_buffer_read_at(buffer, buffer->_position, data, count);
buffer->_position += count;
}







INLINE uint8_t
ldns_buffer_read_u8_at(const ldns_buffer *buffer, size_t at)
{
assert(ldns_buffer_available_at(buffer, at, sizeof(uint8_t)));
return buffer->_data[at];
}






INLINE uint8_t
ldns_buffer_read_u8(ldns_buffer *buffer)
{
uint8_t result = ldns_buffer_read_u8_at(buffer, buffer->_position);
buffer->_position += sizeof(uint8_t);
return result;
}







INLINE uint16_t
ldns_buffer_read_u16_at(ldns_buffer *buffer, size_t at)
{
assert(ldns_buffer_available_at(buffer, at, sizeof(uint16_t)));
return ldns_read_uint16(buffer->_data + at);
}






INLINE uint16_t
ldns_buffer_read_u16(ldns_buffer *buffer)
{
uint16_t result = ldns_buffer_read_u16_at(buffer, buffer->_position);
buffer->_position += sizeof(uint16_t);
return result;
}







INLINE uint32_t
ldns_buffer_read_u32_at(ldns_buffer *buffer, size_t at)
{
assert(ldns_buffer_available_at(buffer, at, sizeof(uint32_t)));
return ldns_read_uint32(buffer->_data + at);
}






INLINE uint32_t
ldns_buffer_read_u32(ldns_buffer *buffer)
{
uint32_t result = ldns_buffer_read_u32_at(buffer, buffer->_position);
buffer->_position += sizeof(uint32_t);
return result;
}






INLINE ldns_status
ldns_buffer_status(const ldns_buffer *buffer)
{
return buffer->_status;
}






INLINE bool
ldns_buffer_status_ok(const ldns_buffer *buffer)
{
if (buffer) {
return ldns_buffer_status(buffer) == LDNS_STATUS_OK;
} else {
return false;
}
}







int ldns_buffer_printf(ldns_buffer *buffer, const char *format, ...);







void ldns_buffer_free(ldns_buffer *buffer);







void *ldns_buffer_export(ldns_buffer *buffer);








void ldns_buffer_copy(ldns_buffer* result, const ldns_buffer* from);

#if defined(__cplusplus)
}
#endif

#endif
