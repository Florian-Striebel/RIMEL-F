#include <stddef.h>
typedef struct ReplyParser ReplyParser;
typedef struct ReplyParserCallbacks {
void (*null_array_callback)(void *ctx, const char *proto, size_t proto_len);
void (*null_bulk_string_callback)(void *ctx, const char *proto, size_t proto_len);
void (*bulk_string_callback)(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len);
void (*error_callback)(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len);
void (*simple_str_callback)(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len);
void (*long_callback)(void *ctx, long long val, const char *proto, size_t proto_len);
void (*array_callback)(struct ReplyParser *parser, void *ctx, size_t len, const char *proto);
void (*set_callback)(struct ReplyParser *parser, void *ctx, size_t len, const char *proto);
void (*map_callback)(struct ReplyParser *parser, void *ctx, size_t len, const char *proto);
void (*bool_callback)(void *ctx, int val, const char *proto, size_t proto_len);
void (*double_callback)(void *ctx, double val, const char *proto, size_t proto_len);
void (*big_number_callback)(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len);
void (*verbatim_string_callback)(void *ctx, const char *format, const char *str, size_t len, const char *proto, size_t proto_len);
void (*attribute_callback)(struct ReplyParser *parser, void *ctx, size_t len, const char *proto);
void (*null_callback)(void *ctx, const char *proto, size_t proto_len);
void (*error)(void *ctx);
} ReplyParserCallbacks;
struct ReplyParser {
const char *curr_location;
ReplyParserCallbacks callbacks;
};
int parseReply(ReplyParser *parser, void *p_ctx);
