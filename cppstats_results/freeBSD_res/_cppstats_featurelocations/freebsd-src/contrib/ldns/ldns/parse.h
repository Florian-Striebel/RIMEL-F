








#if !defined(LDNS_PARSE_H)
#define LDNS_PARSE_H

#include <ldns/common.h>
#include <ldns/buffer.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define LDNS_PARSE_SKIP_SPACE "\f\n\r\v"
#define LDNS_PARSE_NORMAL " \f\n\r\t\v"
#define LDNS_PARSE_NO_NL " \t"
#define LDNS_MAX_LINELEN 10230
#define LDNS_MAX_KEYWORDLEN 32














enum ldns_enum_directive
{
LDNS_DIR_TTL,
LDNS_DIR_ORIGIN,
LDNS_DIR_INCLUDE
};
typedef enum ldns_enum_directive ldns_directive;











ssize_t ldns_fget_token(FILE *f, char *token, const char *delim, size_t limit);












ssize_t ldns_fget_token_l(FILE *f, char *token, const char *delim, size_t limit, int *line_nr);











ssize_t ldns_bget_token(ldns_buffer *b, char *token, const char *delim, size_t limit);












ssize_t ldns_fget_keyword_data(FILE *f, const char *keyword, const char *k_del, char *data, const char *d_del, size_t data_limit);














ssize_t ldns_fget_keyword_data_l(FILE *f, const char *keyword, const char *k_del, char *data, const char *d_del, size_t data_limit, int *line_nr);












ssize_t ldns_bget_keyword_data(ldns_buffer *b, const char *keyword, const char *k_del, char *data, const char *d_del, size_t data_limit);








int ldns_bgetc(ldns_buffer *buffer);








void ldns_bskipcs(ldns_buffer *buffer, const char *s);








void ldns_fskipcs(FILE *fp, const char *s);










void ldns_fskipcs_l(FILE *fp, const char *s, int *line_nr);

#if defined(__cplusplus)
}
#endif

#endif
