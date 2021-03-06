








#include <ldns/config.h>
#include <ldns/ldns.h>

#include <limits.h>
#include <strings.h>

ldns_lookup_table ldns_directive_types[] = {
{ LDNS_DIR_TTL, "$TTL" },
{ LDNS_DIR_ORIGIN, "$ORIGIN" },
{ LDNS_DIR_INCLUDE, "$INCLUDE" },
{ 0, NULL }
};


ssize_t
ldns_fget_token(FILE *f, char *token, const char *delim, size_t limit)
{
return ldns_fget_token_l(f, token, delim, limit, NULL);
}

ssize_t
ldns_fget_token_l(FILE *f, char *token, const char *delim, size_t limit, int *line_nr)
{
int c, prev_c;
int p;
int com, quoted;
char *t;
size_t i;
const char *d;
const char *del;


if (!delim) {

del = LDNS_PARSE_NORMAL;
} else {
del = delim;
}

p = 0;
i = 0;
com = 0;
quoted = 0;
prev_c = 0;
t = token;
if (del[0] == '"') {
quoted = 1;
}
while ((c = getc(f)) != EOF) {
if (c == '\r')
c = ' ';
if (c == '(' && prev_c != '\\' && !quoted) {

if (com == 0) {
p++;
}
prev_c = c;
continue;
}

if (c == ')' && prev_c != '\\' && !quoted) {

if (com == 0) {
p--;
}
prev_c = c;
continue;
}

if (p < 0) {

*t = '\0';
return 0;
}


if (c == ';' && quoted == 0) {
if (prev_c != '\\') {
com = 1;
}
}
if (c == '\"' && com == 0 && prev_c != '\\') {
quoted = 1 - quoted;
}

if (c == '\n' && com != 0) {

com = 0;
*t = ' ';
if (line_nr) {
*line_nr = *line_nr + 1;
}
if (p == 0 && i > 0) {
goto tokenread;
} else {
prev_c = c;
continue;
}
}

if (com == 1) {
*t = ' ';
prev_c = c;
continue;
}

if (c == '\n' && p != 0 && t > token) {

if (line_nr) {
*line_nr = *line_nr + 1;
}
*t++ = ' ';
prev_c = c;
continue;
}


for (d = del; *d; d++) {
if (c == *d && i > 0 && prev_c != '\\' && p == 0) {
if (c == '\n' && line_nr) {
*line_nr = *line_nr + 1;
}
goto tokenread;
}
}
if (c != '\0' && c != '\n') {
i++;
}
if (limit > 0 && (i >= limit || (size_t)(t-token) >= limit)) {
*t = '\0';
return -1;
}
if (c != '\0' && c != '\n') {
*t++ = c;
}
if (c == '\\' && prev_c == '\\')
prev_c = 0;
else prev_c = c;
}
*t = '\0';
if (c == EOF) {
return (ssize_t)i;
}

if (i == 0) {

return -1;
}
if (p != 0) {
return -1;
}
return (ssize_t)i;

tokenread:
if(*del == '"')
ldns_fskipcs_l(f, del+1, line_nr);
else ldns_fskipcs_l(f, del, line_nr);
*t = '\0';
if (p != 0) {
return -1;
}

return (ssize_t)i;
}

ssize_t
ldns_fget_keyword_data(FILE *f, const char *keyword, const char *k_del, char *data,
const char *d_del, size_t data_limit)
{
return ldns_fget_keyword_data_l(f, keyword, k_del, data, d_del,
data_limit, NULL);
}

ssize_t
ldns_fget_keyword_data_l(FILE *f, const char *keyword, const char *k_del, char *data,
const char *d_del, size_t data_limit, int *line_nr)
{

char *fkeyword;
ssize_t i;

if(strlen(keyword) >= LDNS_MAX_KEYWORDLEN)
return -1;
fkeyword = LDNS_XMALLOC(char, LDNS_MAX_KEYWORDLEN);
if(!fkeyword)
return -1;

i = ldns_fget_token(f, fkeyword, k_del, LDNS_MAX_KEYWORDLEN);
if(i==0 || i==-1) {
LDNS_FREE(fkeyword);
return -1;
}


if (strncmp(fkeyword, keyword, LDNS_MAX_KEYWORDLEN - 1) == 0) {


i = ldns_fget_token_l(f, data, d_del, data_limit, line_nr);
LDNS_FREE(fkeyword);
return i;
} else {

LDNS_FREE(fkeyword);
return -1;
}
}


ssize_t
ldns_bget_token(ldns_buffer *b, char *token, const char *delim, size_t limit)
{
int c, lc;
int p;
int com, quoted;
char *t;
size_t i;
const char *d;
const char *del;


if (!delim) {

del = LDNS_PARSE_NORMAL;
} else {
del = delim;
}

p = 0;
i = 0;
com = 0;
quoted = 0;
t = token;
lc = 0;
if (del[0] == '"') {
quoted = 1;
}

while ((c = ldns_bgetc(b)) != EOF) {
if (c == '\r')
c = ' ';
if (c == '(' && lc != '\\' && !quoted) {

if (com == 0) {
p++;
}
lc = c;
continue;
}

if (c == ')' && lc != '\\' && !quoted) {

if (com == 0) {
p--;
}
lc = c;
continue;
}

if (p < 0) {

*t = '\0';
return 0;
}


if (c == ';' && quoted == 0) {
if (lc != '\\') {
com = 1;
}
}
if (c == '"' && com == 0 && lc != '\\') {
quoted = 1 - quoted;
}

if (c == '\n' && com != 0) {

com = 0;
*t = ' ';
lc = c;
continue;
}

if (com == 1) {
*t = ' ';
lc = c;
continue;
}

if (c == '\n' && p != 0) {

*t++ = ' ';
lc = c;
continue;
}


for (d = del; *d; d++) {
if (c == *d && lc != '\\' && p == 0) {
goto tokenread;
}
}

i++;
if (limit > 0 && (i >= limit || (size_t)(t-token) >= limit)) {
*t = '\0';
return -1;
}
*t++ = c;

if (c == '\\' && lc == '\\') {
lc = 0;
} else {
lc = c;
}
}
*t = '\0';
if (i == 0) {

return -1;
}
if (p != 0) {
return -1;
}
return (ssize_t)i;

tokenread:
if(*del == '"')
ldns_bskipcs(b, del+1);
else ldns_bskipcs(b, del);
*t = '\0';

if (p != 0) {
return -1;
}
return (ssize_t)i;
}


void
ldns_bskipcs(ldns_buffer *buffer, const char *s)
{
bool found;
char c;
const char *d;

while(ldns_buffer_available_at(buffer, buffer->_position, sizeof(char))) {
c = (char) ldns_buffer_read_u8_at(buffer, buffer->_position);
found = false;
for (d = s; *d; d++) {
if (*d == c) {
found = true;
}
}
if (found && buffer->_limit > buffer->_position) {
buffer->_position += sizeof(char);
} else {
return;
}
}
}

void
ldns_fskipcs(FILE *fp, const char *s)
{
ldns_fskipcs_l(fp, s, NULL);
}

void
ldns_fskipcs_l(FILE *fp, const char *s, int *line_nr)
{
bool found;
int c;
const char *d;

while ((c = fgetc(fp)) != EOF) {
if (line_nr && c == '\n') {
*line_nr = *line_nr + 1;
}
found = false;
for (d = s; *d; d++) {
if (*d == c) {
found = true;
}
}
if (!found) {

ungetc(c, fp);
return;
}
}
}

ssize_t
ldns_bget_keyword_data(ldns_buffer *b, const char *keyword, const char *k_del, char
*data, const char *d_del, size_t data_limit)
{

char *fkeyword;
ssize_t i;

if(strlen(keyword) >= LDNS_MAX_KEYWORDLEN)
return -1;
fkeyword = LDNS_XMALLOC(char, LDNS_MAX_KEYWORDLEN);
if(!fkeyword)
return -1;

i = ldns_bget_token(b, fkeyword, k_del, data_limit);
if(i==0 || i==-1) {
LDNS_FREE(fkeyword);
return -1;
}


if (strncmp(fkeyword, keyword, strlen(keyword)) == 0) {
LDNS_FREE(fkeyword);


i = ldns_bget_token(b, data, d_del, 0);
return i;
} else {
LDNS_FREE(fkeyword);
return -1;
}
}

