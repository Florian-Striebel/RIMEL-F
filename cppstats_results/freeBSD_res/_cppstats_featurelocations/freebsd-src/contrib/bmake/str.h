



































typedef struct FStr {
const char *str;
void *freeIt;
} FStr;


typedef struct MFStr {
char *str;
void *freeIt;
} MFStr;


typedef struct Substring {
const char *start;
const char *end;
} Substring;





typedef struct LazyBuf {
char *data;
size_t len;
size_t cap;
const char *expected;
} LazyBuf;


typedef struct Words {
char **words;
size_t len;
void *freeIt;
} Words;


typedef struct SubstringWords {
Substring *words;
size_t len;
void *freeIt;
} SubstringWords;


MAKE_INLINE FStr
FStr_Init(const char *str, void *freeIt)
{
FStr fstr;
fstr.str = str;
fstr.freeIt = freeIt;
return fstr;
}


MAKE_INLINE FStr
FStr_InitOwn(char *str)
{
return FStr_Init(str, str);
}


MAKE_INLINE FStr
FStr_InitRefer(const char *str)
{
return FStr_Init(str, NULL);
}

MAKE_INLINE void
FStr_Done(FStr *fstr)
{
free(fstr->freeIt);
#if defined(CLEANUP)
fstr->str = NULL;
fstr->freeIt = NULL;
#endif
}


MAKE_INLINE MFStr
MFStr_Init(char *str, void *freeIt)
{
MFStr mfstr;
mfstr.str = str;
mfstr.freeIt = freeIt;
return mfstr;
}


MAKE_INLINE MFStr
MFStr_InitOwn(char *str)
{
return MFStr_Init(str, str);
}


MAKE_INLINE MFStr
MFStr_InitRefer(char *str)
{
return MFStr_Init(str, NULL);
}

MAKE_INLINE void
MFStr_Done(MFStr *mfstr)
{
free(mfstr->freeIt);
#if defined(CLEANUP)
mfstr->str = NULL;
mfstr->freeIt = NULL;
#endif
}


MAKE_STATIC Substring
Substring_Init(const char *start, const char *end)
{
Substring sub;

sub.start = start;
sub.end = end;
return sub;
}

MAKE_INLINE Substring
Substring_InitStr(const char *str)
{
return Substring_Init(str, str + strlen(str));
}

MAKE_STATIC size_t
Substring_Length(Substring sub)
{
return (size_t)(sub.end - sub.start);
}

MAKE_STATIC bool
Substring_IsEmpty(Substring sub)
{
return sub.start == sub.end;
}

MAKE_INLINE bool
Substring_Equals(Substring sub, const char *str)
{
size_t len = strlen(str);
return Substring_Length(sub) == len &&
memcmp(sub.start, str, len) == 0;
}

MAKE_INLINE bool
Substring_Eq(Substring sub, Substring str)
{
size_t len = Substring_Length(sub);
return len == Substring_Length(str) &&
memcmp(sub.start, str.start, len) == 0;
}

MAKE_STATIC Substring
Substring_Sub(Substring sub, size_t start, size_t end)
{
assert(start <= Substring_Length(sub));
assert(end <= Substring_Length(sub));
return Substring_Init(sub.start + start, sub.start + end);
}

MAKE_STATIC bool
Substring_HasPrefix(Substring sub, Substring prefix)
{
return Substring_Length(sub) >= Substring_Length(prefix) &&
memcmp(sub.start, prefix.start, Substring_Length(prefix)) == 0;
}

MAKE_STATIC bool
Substring_HasSuffix(Substring sub, Substring suffix)
{
size_t suffixLen = Substring_Length(suffix);
return Substring_Length(sub) >= suffixLen &&
memcmp(sub.end - suffixLen, suffix.start, suffixLen) == 0;
}


MAKE_STATIC FStr
Substring_Str(Substring sub)
{
if (Substring_IsEmpty(sub))
return FStr_InitRefer("");
return FStr_InitOwn(bmake_strsedup(sub.start, sub.end));
}

MAKE_STATIC const char *
Substring_SkipFirst(Substring sub, char ch)
{
const char *p;

for (p = sub.start; p != sub.end; p++)
if (*p == ch)
return p + 1;
return sub.start;
}

MAKE_STATIC const char *
Substring_LastIndex(Substring sub, char ch)
{
const char *p;

for (p = sub.end; p != sub.start; p--)
if (p[-1] == ch)
return p - 1;
return NULL;
}

MAKE_STATIC Substring
Substring_Dirname(Substring pathname)
{
const char *p;

for (p = pathname.end; p != pathname.start; p--)
if (p[-1] == '/')
return Substring_Init(pathname.start, p - 1);
return Substring_InitStr(".");
}

MAKE_STATIC Substring
Substring_Basename(Substring pathname)
{
const char *p;

for (p = pathname.end; p != pathname.start; p--)
if (p[-1] == '/')
return Substring_Init(p, pathname.end);
return pathname;
}


MAKE_STATIC void
LazyBuf_Init(LazyBuf *buf, const char *expected)
{
buf->data = NULL;
buf->len = 0;
buf->cap = 0;
buf->expected = expected;
}

MAKE_INLINE void
LazyBuf_Done(LazyBuf *buf)
{
free(buf->data);
}

MAKE_STATIC void
LazyBuf_Add(LazyBuf *buf, char ch)
{

if (buf->data != NULL) {
if (buf->len == buf->cap) {
buf->cap *= 2;
buf->data = bmake_realloc(buf->data, buf->cap);
}
buf->data[buf->len++] = ch;

} else if (ch == buf->expected[buf->len]) {
buf->len++;
return;

} else {
buf->cap = buf->len + 16;
buf->data = bmake_malloc(buf->cap);
memcpy(buf->data, buf->expected, buf->len);
buf->data[buf->len++] = ch;
}
}

MAKE_STATIC void
LazyBuf_AddStr(LazyBuf *buf, const char *str)
{
const char *p;

for (p = str; *p != '\0'; p++)
LazyBuf_Add(buf, *p);
}

MAKE_STATIC void
LazyBuf_AddBytesBetween(LazyBuf *buf, const char *start, const char *end)
{
const char *p;

for (p = start; p != end; p++)
LazyBuf_Add(buf, *p);
}

MAKE_INLINE void
LazyBuf_AddSubstring(LazyBuf *buf, Substring sub)
{
LazyBuf_AddBytesBetween(buf, sub.start, sub.end);
}

MAKE_STATIC Substring
LazyBuf_Get(const LazyBuf *buf)
{
const char *start = buf->data != NULL ? buf->data : buf->expected;
return Substring_Init(start, start + buf->len);
}






MAKE_STATIC FStr
LazyBuf_DoneGet(LazyBuf *buf)
{
if (buf->data != NULL) {
LazyBuf_Add(buf, '\0');
return FStr_InitOwn(buf->data);
}
return Substring_Str(LazyBuf_Get(buf));
}


Words Str_Words(const char *, bool);

MAKE_INLINE void
Words_Free(Words w)
{
free(w.words);
free(w.freeIt);
}


SubstringWords Substring_Words(const char *, bool);

MAKE_INLINE void
SubstringWords_Init(SubstringWords *w)
{
w->words = NULL;
w->len = 0;
w->freeIt = NULL;
}

MAKE_INLINE void
SubstringWords_Free(SubstringWords w)
{
free(w.words);
free(w.freeIt);
}


char *str_concat2(const char *, const char *);
char *str_concat3(const char *, const char *, const char *);

bool Str_Match(const char *, const char *);
