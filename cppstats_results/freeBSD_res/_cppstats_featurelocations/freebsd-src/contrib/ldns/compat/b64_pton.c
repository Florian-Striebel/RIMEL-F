








































#include <ldns/config.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static const char Base64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char Pad64 = '=';






































































int
ldns_b64_pton(char const *origsrc, uint8_t *target, size_t targsize)
{
unsigned char const* src = (unsigned char*)origsrc;
int tarindex, state, ch;
char *pos;

state = 0;
tarindex = 0;

if (strlen(origsrc) == 0) {
return 0;
}

while ((ch = *src++) != '\0') {
if (isspace((unsigned char)ch))
continue;

if (ch == Pad64)
break;

pos = strchr(Base64, ch);
if (pos == 0) {

return (-1);
}

switch (state) {
case 0:
if (target) {
if ((size_t)tarindex >= targsize)
return (-1);
target[tarindex] = (pos - Base64) << 2;
}
state = 1;
break;
case 1:
if (target) {
if ((size_t)tarindex + 1 >= targsize)
return (-1);
target[tarindex] |= (pos - Base64) >> 4;
target[tarindex+1] = ((pos - Base64) & 0x0f)
<< 4 ;
}
tarindex++;
state = 2;
break;
case 2:
if (target) {
if ((size_t)tarindex + 1 >= targsize)
return (-1);
target[tarindex] |= (pos - Base64) >> 2;
target[tarindex+1] = ((pos - Base64) & 0x03)
<< 6;
}
tarindex++;
state = 3;
break;
case 3:
if (target) {
if ((size_t)tarindex >= targsize)
return (-1);
target[tarindex] |= (pos - Base64);
}
tarindex++;
state = 0;
break;
default:
abort();
}
}






if (ch == Pad64) {
ch = *src++;
switch (state) {
case 0:
case 1:
return (-1);

case 2:

for ((void)NULL; ch != '\0'; ch = *src++)
if (!isspace((unsigned char)ch))
break;

if (ch != Pad64)
return (-1);
ch = *src++;



case 3:




for ((void)NULL; ch != '\0'; ch = *src++)
if (!isspace((unsigned char)ch))
return (-1);







if (target && target[tarindex] != 0)
return (-1);
}
} else {




if (state != 0)
return (-1);
}

return (tarindex);
}
