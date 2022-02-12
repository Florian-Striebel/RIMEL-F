

































#include <stdio.h>
#include <expat.h>

#if defined(XML_LARGE_SIZE)
#define XML_FMT_INT_MOD "ll"
#else
#define XML_FMT_INT_MOD "l"
#endif

#if defined(XML_UNICODE_WCHAR_T)
#define XML_FMT_STR "ls"
#else
#define XML_FMT_STR "s"
#endif

#define BUFFSIZE 8192

char Buff[BUFFSIZE];

int Depth;

static void XMLCALL
start(void *data, const XML_Char *el, const XML_Char **attr) {
int i;
(void)data;

for (i = 0; i < Depth; i++)
printf(" ");

printf("%" XML_FMT_STR, el);

for (i = 0; attr[i]; i += 2) {
printf(" %" XML_FMT_STR "='%" XML_FMT_STR "'", attr[i], attr[i + 1]);
}

printf("\n");
Depth++;
}

static void XMLCALL
end(void *data, const XML_Char *el) {
(void)data;
(void)el;

Depth--;
}

int
main(int argc, char *argv[]) {
XML_Parser p = XML_ParserCreate(NULL);
(void)argc;
(void)argv;

if (! p) {
fprintf(stderr, "Couldn't allocate memory for parser\n");
exit(-1);
}

XML_SetElementHandler(p, start, end);

for (;;) {
int done;
int len;

len = (int)fread(Buff, 1, BUFFSIZE, stdin);
if (ferror(stdin)) {
fprintf(stderr, "Read error\n");
exit(-1);
}
done = feof(stdin);

if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {
fprintf(stderr,
"Parse error at line %" XML_FMT_INT_MOD "u:\n%" XML_FMT_STR "\n",
XML_GetCurrentLineNumber(p),
XML_ErrorString(XML_GetErrorCode(p)));
exit(-1);
}

if (done)
break;
}
XML_ParserFree(p);
return 0;
}
