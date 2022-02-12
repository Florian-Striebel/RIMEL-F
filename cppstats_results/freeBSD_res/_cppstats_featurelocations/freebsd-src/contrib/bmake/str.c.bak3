





































































#include "make.h"


MAKE_RCSID("$NetBSD: str.c,v 1.86 2021/06/21 16:59:18 rillig Exp $");


char *
str_concat2(const char *s1, const char *s2)
{
size_t len1 = strlen(s1);
size_t len2 = strlen(s2);
char *result = bmake_malloc(len1 + len2 + 1);
memcpy(result, s1, len1);
memcpy(result + len1, s2, len2 + 1);
return result;
}


char *
str_concat3(const char *s1, const char *s2, const char *s3)
{
size_t len1 = strlen(s1);
size_t len2 = strlen(s2);
size_t len3 = strlen(s3);
char *result = bmake_malloc(len1 + len2 + len3 + 1);
memcpy(result, s1, len1);
memcpy(result + len1, s2, len2);
memcpy(result + len1 + len2, s3, len3 + 1);
return result;
}











SubstringWords
Substring_Words(const char *str, bool expand)
{
size_t str_len;
char *words_buf;
size_t words_cap;
Substring *words;
size_t words_len;
char inquote;
char *word_start;
char *word_end;
const char *str_p;


cpp_skip_hspace(&str);


str_len = strlen(str);
words_buf = bmake_malloc(str_len + 1);

words_cap = str_len / 5 > 50 ? str_len / 5 : 50;
words = bmake_malloc((words_cap + 1) * sizeof(words[0]));





words_len = 0;
inquote = '\0';
word_start = words_buf;
word_end = words_buf;
for (str_p = str;; str_p++) {
char ch = *str_p;
switch (ch) {
case '"':
case '\'':
if (inquote != '\0') {
if (inquote == ch)
inquote = '\0';
else
break;
} else {
inquote = ch;

if (word_start == NULL && str_p[1] == inquote) {
if (!expand) {
word_start = word_end;
*word_end++ = ch;
} else
word_start = word_end + 1;
str_p++;
inquote = '\0';
break;
}
}
if (!expand) {
if (word_start == NULL)
word_start = word_end;
*word_end++ = ch;
}
continue;
case ' ':
case '\t':
case '\n':
if (inquote != '\0')
break;
if (word_start == NULL)
continue;

case '\0':




if (word_start == NULL)
goto done;

*word_end++ = '\0';
if (words_len == words_cap) {
words_cap *= 2;
words = bmake_realloc(words,
(words_cap + 1) * sizeof(words[0]));
}
words[words_len++] =
Substring_Init(word_start, word_end - 1);
word_start = NULL;
if (ch == '\n' || ch == '\0') {
if (expand && inquote != '\0') {
SubstringWords res;

free(words);
free(words_buf);

res.words = NULL;
res.len = 0;
res.freeIt = NULL;
return res;
}
goto done;
}
continue;
case '\\':
if (!expand) {
if (word_start == NULL)
word_start = word_end;
*word_end++ = '\\';

if (str_p[1] == '\0')
continue;
ch = *++str_p;
break;
}

switch (ch = *++str_p) {
case '\0':
case '\n':

ch = '\\';
str_p--;
break;
case 'b':
ch = '\b';
break;
case 'f':
ch = '\f';
break;
case 'n':
ch = '\n';
break;
case 'r':
ch = '\r';
break;
case 't':
ch = '\t';
break;
}
break;
}
if (word_start == NULL)
word_start = word_end;
*word_end++ = ch;
}
done:
words[words_len] = Substring_Init(NULL, NULL);

{
SubstringWords result;

result.words = words;
result.len = words_len;
result.freeIt = words_buf;
return result;
}
}

Words
Str_Words(const char *str, bool expand)
{
SubstringWords swords;
Words words;
size_t i;

swords = Substring_Words(str, expand);
if (swords.words == NULL) {
words.words = NULL;
words.len = 0;
words.freeIt = NULL;
return words;
}

words.words = bmake_malloc((swords.len + 1) * sizeof(words.words[0]));
words.len = swords.len;
words.freeIt = swords.freeIt;
for (i = 0; i < swords.len + 1; i++)
words.words[i] = UNCONST(swords.words[i].start);
free(swords.words);
return words;
}







bool
Str_Match(const char *str, const char *pat)
{
for (;;) {





if (*pat == '\0')
return *str == '\0';
if (*str == '\0' && *pat != '*')
return false;





if (*pat == '*') {
pat++;
while (*pat == '*')
pat++;
if (*pat == '\0')
return true;
while (*str != '\0') {
if (Str_Match(str, pat))
return true;
str++;
}
return false;
}


if (*pat == '?')
goto thisCharOK;







if (*pat == '[') {
bool neg = pat[1] == '^';
pat += neg ? 2 : 1;

for (;;) {
if (*pat == ']' || *pat == '\0') {
if (neg)
break;
return false;
}









if (*pat == *str)
break;
if (pat[1] == '-') {
if (pat[2] == '\0')
return neg;
if (*pat <= *str && pat[2] >= *str)
break;
if (*pat >= *str && pat[2] <= *str)
break;
pat += 2;
}
pat++;
}
if (neg && *pat != ']' && *pat != '\0')
return false;
while (*pat != ']' && *pat != '\0')
pat++;
if (*pat == '\0')
pat--;
goto thisCharOK;
}





if (*pat == '\\') {
pat++;
if (*pat == '\0')
return false;
}

if (*pat != *str)
return false;

thisCharOK:
pat++;
str++;
}
}
