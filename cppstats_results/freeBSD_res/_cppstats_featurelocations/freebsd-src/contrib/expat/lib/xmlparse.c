































#if ! defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#endif

#if defined(_WIN32)

#if ! defined(_CRT_RAND_S)
#define _CRT_RAND_S
#endif
#endif

#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#define getpid GetCurrentProcessId
#else
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif

#define XML_BUILDING_EXPAT 1

#if defined(_WIN32)
#include "winconfig.h"
#elif defined(HAVE_EXPAT_CONFIG_H)
#include <expat_config.h>
#endif

#include "ascii.h"
#include "expat.h"
#include "siphash.h"

#if defined(HAVE_GETRANDOM) || defined(HAVE_SYSCALL_GETRANDOM)
#if defined(HAVE_GETRANDOM)
#include <sys/random.h>
#else
#include <unistd.h>
#include <sys/syscall.h>
#endif
#if ! defined(GRND_NONBLOCK)
#define GRND_NONBLOCK 0x0001
#endif
#endif

#if defined(HAVE_LIBBSD) && (defined(HAVE_ARC4RANDOM_BUF) || defined(HAVE_ARC4RANDOM))

#include <bsd/stdlib.h>
#endif

#if defined(_WIN32) && ! defined(LOAD_LIBRARY_SEARCH_SYSTEM32)
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif

#if ! defined(HAVE_GETRANDOM) && ! defined(HAVE_SYSCALL_GETRANDOM) && ! defined(HAVE_ARC4RANDOM_BUF) && ! defined(HAVE_ARC4RANDOM) && ! defined(XML_DEV_URANDOM) && ! defined(_WIN32) && ! defined(XML_POOR_ENTROPY)



#error You do not have support for any sources of high quality entropy enabled. For end user security, that is probably not what you want. Your options include: * Linux + glibc >=2.25 (getrandom): HAVE_GETRANDOM, * Linux + glibc <2.25 (syscall SYS_getrandom): HAVE_SYSCALL_GETRANDOM, * BSD / macOS >=10.7 (arc4random_buf): HAVE_ARC4RANDOM_BUF, * BSD / macOS <10.7 (arc4random): HAVE_ARC4RANDOM, * libbsd (arc4random_buf): HAVE_ARC4RANDOM_BUF + HAVE_LIBBSD, * libbsd (arc4random): HAVE_ARC4RANDOM + HAVE_LIBBSD, * Linux / BSD / macOS (/dev/urandom): XML_DEV_URANDOM * Windows (rand_s): _WIN32. If insist on not using any of these, bypass this error by defining XML_POOR_ENTROPY; you have been warned. If you have reasons to patch this detection code away or need changes to the build system, please open a bug. Thank you!

















#endif

#if defined(XML_UNICODE)
#define XML_ENCODE_MAX XML_UTF16_ENCODE_MAX
#define XmlConvert XmlUtf16Convert
#define XmlGetInternalEncoding XmlGetUtf16InternalEncoding
#define XmlGetInternalEncodingNS XmlGetUtf16InternalEncodingNS
#define XmlEncode XmlUtf16Encode

#define MUST_CONVERT(enc, s) (! (enc)->isUtf16 || (((char *)(s) - (char *)NULL) & 1))

typedef unsigned short ICHAR;
#else
#define XML_ENCODE_MAX XML_UTF8_ENCODE_MAX
#define XmlConvert XmlUtf8Convert
#define XmlGetInternalEncoding XmlGetUtf8InternalEncoding
#define XmlGetInternalEncodingNS XmlGetUtf8InternalEncodingNS
#define XmlEncode XmlUtf8Encode
#define MUST_CONVERT(enc, s) (! (enc)->isUtf8)
typedef char ICHAR;
#endif

#if !defined(XML_NS)

#define XmlInitEncodingNS XmlInitEncoding
#define XmlInitUnknownEncodingNS XmlInitUnknownEncoding
#undef XmlGetInternalEncodingNS
#define XmlGetInternalEncodingNS XmlGetInternalEncoding
#define XmlParseXmlDeclNS XmlParseXmlDecl

#endif

#if defined(XML_UNICODE)

#if defined(XML_UNICODE_WCHAR_T)
#define XML_T(x) (const wchar_t) x
#define XML_L(x) L##x
#else
#define XML_T(x) (const unsigned short)x
#define XML_L(x) x
#endif

#else

#define XML_T(x) x
#define XML_L(x) x

#endif


#define ROUND_UP(n, sz) (((n) + ((sz)-1)) & ~((sz)-1))


#define EXPAT_SAFE_PTR_DIFF(p, q) (((p) && (q)) ? ((p) - (q)) : 0)

#include "internal.h"
#include "xmltok.h"
#include "xmlrole.h"

typedef const XML_Char *KEY;

typedef struct {
KEY name;
} NAMED;

typedef struct {
NAMED **v;
unsigned char power;
size_t size;
size_t used;
const XML_Memory_Handling_Suite *mem;
} HASH_TABLE;

static size_t keylen(KEY s);

static void copy_salt_to_sipkey(XML_Parser parser, struct sipkey *key);









#define SECOND_HASH(hash, mask, power) ((((hash) & ~(mask)) >> ((power)-1)) & ((mask) >> 2))

#define PROBE_STEP(hash, mask, power) ((unsigned char)((SECOND_HASH(hash, mask, power)) | 1))


typedef struct {
NAMED **p;
NAMED **end;
} HASH_TABLE_ITER;

#define INIT_TAG_BUF_SIZE 32
#define INIT_DATA_BUF_SIZE 1024
#define INIT_ATTS_SIZE 16
#define INIT_ATTS_VERSION 0xFFFFFFFF
#define INIT_BLOCK_SIZE 1024
#define INIT_BUFFER_SIZE 1024

#define EXPAND_SPARE 24

typedef struct binding {
struct prefix *prefix;
struct binding *nextTagBinding;
struct binding *prevPrefixBinding;
const struct attribute_id *attId;
XML_Char *uri;
int uriLen;
int uriAlloc;
} BINDING;

typedef struct prefix {
const XML_Char *name;
BINDING *binding;
} PREFIX;

typedef struct {
const XML_Char *str;
const XML_Char *localPart;
const XML_Char *prefix;
int strLen;
int uriLen;
int prefixLen;
} TAG_NAME;














typedef struct tag {
struct tag *parent;
const char *rawName;
int rawNameLength;
TAG_NAME name;
char *buf;
char *bufEnd;
BINDING *bindings;
} TAG;

typedef struct {
const XML_Char *name;
const XML_Char *textPtr;
int textLen;
int processed;
const XML_Char *systemId;
const XML_Char *base;
const XML_Char *publicId;
const XML_Char *notation;
XML_Bool open;
XML_Bool is_param;
XML_Bool is_internal;
} ENTITY;

typedef struct {
enum XML_Content_Type type;
enum XML_Content_Quant quant;
const XML_Char *name;
int firstchild;
int lastchild;
int childcnt;
int nextsib;
} CONTENT_SCAFFOLD;

#define INIT_SCAFFOLD_ELEMENTS 32

typedef struct block {
struct block *next;
int size;
XML_Char s[1];
} BLOCK;

typedef struct {
BLOCK *blocks;
BLOCK *freeBlocks;
const XML_Char *end;
XML_Char *ptr;
XML_Char *start;
const XML_Memory_Handling_Suite *mem;
} STRING_POOL;



typedef struct attribute_id {
XML_Char *name;
PREFIX *prefix;
XML_Bool maybeTokenized;
XML_Bool xmlns;
} ATTRIBUTE_ID;

typedef struct {
const ATTRIBUTE_ID *id;
XML_Bool isCdata;
const XML_Char *value;
} DEFAULT_ATTRIBUTE;

typedef struct {
unsigned long version;
unsigned long hash;
const XML_Char *uriName;
} NS_ATT;

typedef struct {
const XML_Char *name;
PREFIX *prefix;
const ATTRIBUTE_ID *idAtt;
int nDefaultAtts;
int allocDefaultAtts;
DEFAULT_ATTRIBUTE *defaultAtts;
} ELEMENT_TYPE;

typedef struct {
HASH_TABLE generalEntities;
HASH_TABLE elementTypes;
HASH_TABLE attributeIds;
HASH_TABLE prefixes;
STRING_POOL pool;
STRING_POOL entityValuePool;

XML_Bool keepProcessing;


XML_Bool hasParamEntityRefs;
XML_Bool standalone;
#if defined(XML_DTD)

XML_Bool paramEntityRead;
HASH_TABLE paramEntities;
#endif
PREFIX defaultPrefix;

XML_Bool in_eldecl;
CONTENT_SCAFFOLD *scaffold;
unsigned contentStringLen;
unsigned scaffSize;
unsigned scaffCount;
int scaffLevel;
int *scaffIndex;
} DTD;

typedef struct open_internal_entity {
const char *internalEventPtr;
const char *internalEventEndPtr;
struct open_internal_entity *next;
ENTITY *entity;
int startTagLevel;
XML_Bool betweenDecl;
} OPEN_INTERNAL_ENTITY;

typedef enum XML_Error PTRCALL Processor(XML_Parser parser, const char *start,
const char *end, const char **endPtr);

static Processor prologProcessor;
static Processor prologInitProcessor;
static Processor contentProcessor;
static Processor cdataSectionProcessor;
#if defined(XML_DTD)
static Processor ignoreSectionProcessor;
static Processor externalParEntProcessor;
static Processor externalParEntInitProcessor;
static Processor entityValueProcessor;
static Processor entityValueInitProcessor;
#endif
static Processor epilogProcessor;
static Processor errorProcessor;
static Processor externalEntityInitProcessor;
static Processor externalEntityInitProcessor2;
static Processor externalEntityInitProcessor3;
static Processor externalEntityContentProcessor;
static Processor internalEntityProcessor;

static enum XML_Error handleUnknownEncoding(XML_Parser parser,
const XML_Char *encodingName);
static enum XML_Error processXmlDecl(XML_Parser parser, int isGeneralTextEntity,
const char *s, const char *next);
static enum XML_Error initializeEncoding(XML_Parser parser);
static enum XML_Error doProlog(XML_Parser parser, const ENCODING *enc,
const char *s, const char *end, int tok,
const char *next, const char **nextPtr,
XML_Bool haveMore, XML_Bool allowClosingDoctype);
static enum XML_Error processInternalEntity(XML_Parser parser, ENTITY *entity,
XML_Bool betweenDecl);
static enum XML_Error doContent(XML_Parser parser, int startTagLevel,
const ENCODING *enc, const char *start,
const char *end, const char **endPtr,
XML_Bool haveMore);
static enum XML_Error doCdataSection(XML_Parser parser, const ENCODING *,
const char **startPtr, const char *end,
const char **nextPtr, XML_Bool haveMore);
#if defined(XML_DTD)
static enum XML_Error doIgnoreSection(XML_Parser parser, const ENCODING *,
const char **startPtr, const char *end,
const char **nextPtr, XML_Bool haveMore);
#endif

static void freeBindings(XML_Parser parser, BINDING *bindings);
static enum XML_Error storeAtts(XML_Parser parser, const ENCODING *,
const char *s, TAG_NAME *tagNamePtr,
BINDING **bindingsPtr);
static enum XML_Error addBinding(XML_Parser parser, PREFIX *prefix,
const ATTRIBUTE_ID *attId, const XML_Char *uri,
BINDING **bindingsPtr);
static int defineAttribute(ELEMENT_TYPE *type, ATTRIBUTE_ID *, XML_Bool isCdata,
XML_Bool isId, const XML_Char *dfltValue,
XML_Parser parser);
static enum XML_Error storeAttributeValue(XML_Parser parser, const ENCODING *,
XML_Bool isCdata, const char *,
const char *, STRING_POOL *);
static enum XML_Error appendAttributeValue(XML_Parser parser, const ENCODING *,
XML_Bool isCdata, const char *,
const char *, STRING_POOL *);
static ATTRIBUTE_ID *getAttributeId(XML_Parser parser, const ENCODING *enc,
const char *start, const char *end);
static int setElementTypePrefix(XML_Parser parser, ELEMENT_TYPE *);
static enum XML_Error storeEntityValue(XML_Parser parser, const ENCODING *enc,
const char *start, const char *end);
static int reportProcessingInstruction(XML_Parser parser, const ENCODING *enc,
const char *start, const char *end);
static int reportComment(XML_Parser parser, const ENCODING *enc,
const char *start, const char *end);
static void reportDefault(XML_Parser parser, const ENCODING *enc,
const char *start, const char *end);

static const XML_Char *getContext(XML_Parser parser);
static XML_Bool setContext(XML_Parser parser, const XML_Char *context);

static void FASTCALL normalizePublicId(XML_Char *s);

static DTD *dtdCreate(const XML_Memory_Handling_Suite *ms);

static void dtdReset(DTD *p, const XML_Memory_Handling_Suite *ms);
static void dtdDestroy(DTD *p, XML_Bool isDocEntity,
const XML_Memory_Handling_Suite *ms);
static int dtdCopy(XML_Parser oldParser, DTD *newDtd, const DTD *oldDtd,
const XML_Memory_Handling_Suite *ms);
static int copyEntityTable(XML_Parser oldParser, HASH_TABLE *, STRING_POOL *,
const HASH_TABLE *);
static NAMED *lookup(XML_Parser parser, HASH_TABLE *table, KEY name,
size_t createSize);
static void FASTCALL hashTableInit(HASH_TABLE *,
const XML_Memory_Handling_Suite *ms);
static void FASTCALL hashTableClear(HASH_TABLE *);
static void FASTCALL hashTableDestroy(HASH_TABLE *);
static void FASTCALL hashTableIterInit(HASH_TABLE_ITER *, const HASH_TABLE *);
static NAMED *FASTCALL hashTableIterNext(HASH_TABLE_ITER *);

static void FASTCALL poolInit(STRING_POOL *,
const XML_Memory_Handling_Suite *ms);
static void FASTCALL poolClear(STRING_POOL *);
static void FASTCALL poolDestroy(STRING_POOL *);
static XML_Char *poolAppend(STRING_POOL *pool, const ENCODING *enc,
const char *ptr, const char *end);
static XML_Char *poolStoreString(STRING_POOL *pool, const ENCODING *enc,
const char *ptr, const char *end);
static XML_Bool FASTCALL poolGrow(STRING_POOL *pool);
static const XML_Char *FASTCALL poolCopyString(STRING_POOL *pool,
const XML_Char *s);
static const XML_Char *poolCopyStringN(STRING_POOL *pool, const XML_Char *s,
int n);
static const XML_Char *FASTCALL poolAppendString(STRING_POOL *pool,
const XML_Char *s);

static int FASTCALL nextScaffoldPart(XML_Parser parser);
static XML_Content *build_model(XML_Parser parser);
static ELEMENT_TYPE *getElementType(XML_Parser parser, const ENCODING *enc,
const char *ptr, const char *end);

static XML_Char *copyString(const XML_Char *s,
const XML_Memory_Handling_Suite *memsuite);

static unsigned long generate_hash_secret_salt(XML_Parser parser);
static XML_Bool startParsing(XML_Parser parser);

static XML_Parser parserCreate(const XML_Char *encodingName,
const XML_Memory_Handling_Suite *memsuite,
const XML_Char *nameSep, DTD *dtd);

static void parserInit(XML_Parser parser, const XML_Char *encodingName);

#define poolStart(pool) ((pool)->start)
#define poolEnd(pool) ((pool)->ptr)
#define poolLength(pool) ((pool)->ptr - (pool)->start)
#define poolChop(pool) ((void)--(pool->ptr))
#define poolLastChar(pool) (((pool)->ptr)[-1])
#define poolDiscard(pool) ((pool)->ptr = (pool)->start)
#define poolFinish(pool) ((pool)->start = (pool)->ptr)
#define poolAppendChar(pool, c) (((pool)->ptr == (pool)->end && ! poolGrow(pool)) ? 0 : ((*((pool)->ptr)++ = c), 1))




struct XML_ParserStruct {


void *m_userData;
void *m_handlerArg;
char *m_buffer;
const XML_Memory_Handling_Suite m_mem;

const char *m_bufferPtr;

char *m_bufferEnd;

const char *m_bufferLim;
XML_Index m_parseEndByteIndex;
const char *m_parseEndPtr;
XML_Char *m_dataBuf;
XML_Char *m_dataBufEnd;
XML_StartElementHandler m_startElementHandler;
XML_EndElementHandler m_endElementHandler;
XML_CharacterDataHandler m_characterDataHandler;
XML_ProcessingInstructionHandler m_processingInstructionHandler;
XML_CommentHandler m_commentHandler;
XML_StartCdataSectionHandler m_startCdataSectionHandler;
XML_EndCdataSectionHandler m_endCdataSectionHandler;
XML_DefaultHandler m_defaultHandler;
XML_StartDoctypeDeclHandler m_startDoctypeDeclHandler;
XML_EndDoctypeDeclHandler m_endDoctypeDeclHandler;
XML_UnparsedEntityDeclHandler m_unparsedEntityDeclHandler;
XML_NotationDeclHandler m_notationDeclHandler;
XML_StartNamespaceDeclHandler m_startNamespaceDeclHandler;
XML_EndNamespaceDeclHandler m_endNamespaceDeclHandler;
XML_NotStandaloneHandler m_notStandaloneHandler;
XML_ExternalEntityRefHandler m_externalEntityRefHandler;
XML_Parser m_externalEntityRefHandlerArg;
XML_SkippedEntityHandler m_skippedEntityHandler;
XML_UnknownEncodingHandler m_unknownEncodingHandler;
XML_ElementDeclHandler m_elementDeclHandler;
XML_AttlistDeclHandler m_attlistDeclHandler;
XML_EntityDeclHandler m_entityDeclHandler;
XML_XmlDeclHandler m_xmlDeclHandler;
const ENCODING *m_encoding;
INIT_ENCODING m_initEncoding;
const ENCODING *m_internalEncoding;
const XML_Char *m_protocolEncodingName;
XML_Bool m_ns;
XML_Bool m_ns_triplets;
void *m_unknownEncodingMem;
void *m_unknownEncodingData;
void *m_unknownEncodingHandlerData;
void(XMLCALL *m_unknownEncodingRelease)(void *);
PROLOG_STATE m_prologState;
Processor *m_processor;
enum XML_Error m_errorCode;
const char *m_eventPtr;
const char *m_eventEndPtr;
const char *m_positionPtr;
OPEN_INTERNAL_ENTITY *m_openInternalEntities;
OPEN_INTERNAL_ENTITY *m_freeInternalEntities;
XML_Bool m_defaultExpandInternalEntities;
int m_tagLevel;
ENTITY *m_declEntity;
const XML_Char *m_doctypeName;
const XML_Char *m_doctypeSysid;
const XML_Char *m_doctypePubid;
const XML_Char *m_declAttributeType;
const XML_Char *m_declNotationName;
const XML_Char *m_declNotationPublicId;
ELEMENT_TYPE *m_declElementType;
ATTRIBUTE_ID *m_declAttributeId;
XML_Bool m_declAttributeIsCdata;
XML_Bool m_declAttributeIsId;
DTD *m_dtd;
const XML_Char *m_curBase;
TAG *m_tagStack;
TAG *m_freeTagList;
BINDING *m_inheritedBindings;
BINDING *m_freeBindingList;
int m_attsSize;
int m_nSpecifiedAtts;
int m_idAttIndex;
ATTRIBUTE *m_atts;
NS_ATT *m_nsAtts;
unsigned long m_nsAttsVersion;
unsigned char m_nsAttsPower;
#if defined(XML_ATTR_INFO)
XML_AttrInfo *m_attInfo;
#endif
POSITION m_position;
STRING_POOL m_tempPool;
STRING_POOL m_temp2Pool;
char *m_groupConnector;
unsigned int m_groupSize;
XML_Char m_namespaceSeparator;
XML_Parser m_parentParser;
XML_ParsingStatus m_parsingStatus;
#if defined(XML_DTD)
XML_Bool m_isParamEntity;
XML_Bool m_useForeignDTD;
enum XML_ParamEntityParsing m_paramEntityParsing;
#endif
unsigned long m_hash_secret_salt;
};

#define MALLOC(parser, s) (parser->m_mem.malloc_fcn((s)))
#define REALLOC(parser, p, s) (parser->m_mem.realloc_fcn((p), (s)))
#define FREE(parser, p) (parser->m_mem.free_fcn((p)))

XML_Parser XMLCALL
XML_ParserCreate(const XML_Char *encodingName) {
return XML_ParserCreate_MM(encodingName, NULL, NULL);
}

XML_Parser XMLCALL
XML_ParserCreateNS(const XML_Char *encodingName, XML_Char nsSep) {
XML_Char tmp[2];
*tmp = nsSep;
return XML_ParserCreate_MM(encodingName, NULL, tmp);
}

static const XML_Char implicitContext[]
= {ASCII_x, ASCII_m, ASCII_l, ASCII_EQUALS, ASCII_h,
ASCII_t, ASCII_t, ASCII_p, ASCII_COLON, ASCII_SLASH,
ASCII_SLASH, ASCII_w, ASCII_w, ASCII_w, ASCII_PERIOD,
ASCII_w, ASCII_3, ASCII_PERIOD, ASCII_o, ASCII_r,
ASCII_g, ASCII_SLASH, ASCII_X, ASCII_M, ASCII_L,
ASCII_SLASH, ASCII_1, ASCII_9, ASCII_9, ASCII_8,
ASCII_SLASH, ASCII_n, ASCII_a, ASCII_m, ASCII_e,
ASCII_s, ASCII_p, ASCII_a, ASCII_c, ASCII_e,
'\0'};


#if ! defined(HAVE_ARC4RANDOM_BUF) && ! defined(HAVE_ARC4RANDOM)

#if defined(HAVE_GETRANDOM) || defined(HAVE_SYSCALL_GETRANDOM)


static int
writeRandomBytes_getrandom_nonblock(void *target, size_t count) {
int success = 0;
size_t bytesWrittenTotal = 0;
const unsigned int getrandomFlags = GRND_NONBLOCK;

do {
void *const currentTarget = (void *)((char *)target + bytesWrittenTotal);
const size_t bytesToWrite = count - bytesWrittenTotal;

const int bytesWrittenMore =
#if defined(HAVE_GETRANDOM)
getrandom(currentTarget, bytesToWrite, getrandomFlags);
#else
syscall(SYS_getrandom, currentTarget, bytesToWrite, getrandomFlags);
#endif

if (bytesWrittenMore > 0) {
bytesWrittenTotal += bytesWrittenMore;
if (bytesWrittenTotal >= count)
success = 1;
}
} while (! success && (errno == EINTR));

return success;
}

#endif

#if ! defined(_WIN32) && defined(XML_DEV_URANDOM)


static int
writeRandomBytes_dev_urandom(void *target, size_t count) {
int success = 0;
size_t bytesWrittenTotal = 0;

const int fd = open("/dev/urandom", O_RDONLY);
if (fd < 0) {
return 0;
}

do {
void *const currentTarget = (void *)((char *)target + bytesWrittenTotal);
const size_t bytesToWrite = count - bytesWrittenTotal;

const ssize_t bytesWrittenMore = read(fd, currentTarget, bytesToWrite);

if (bytesWrittenMore > 0) {
bytesWrittenTotal += bytesWrittenMore;
if (bytesWrittenTotal >= count)
success = 1;
}
} while (! success && (errno == EINTR));

close(fd);
return success;
}

#endif

#endif

#if defined(HAVE_ARC4RANDOM) && ! defined(HAVE_ARC4RANDOM_BUF)

static void
writeRandomBytes_arc4random(void *target, size_t count) {
size_t bytesWrittenTotal = 0;

while (bytesWrittenTotal < count) {
const uint32_t random32 = arc4random();
size_t i = 0;

for (; (i < sizeof(random32)) && (bytesWrittenTotal < count);
i++, bytesWrittenTotal++) {
const uint8_t random8 = (uint8_t)(random32 >> (i * 8));
((uint8_t *)target)[bytesWrittenTotal] = random8;
}
}
}

#endif

#if defined(_WIN32)





static int
writeRandomBytes_rand_s(void *target, size_t count) {
size_t bytesWrittenTotal = 0;

while (bytesWrittenTotal < count) {
unsigned int random32 = 0;
size_t i = 0;

if (rand_s(&random32))
return 0;

for (; (i < sizeof(random32)) && (bytesWrittenTotal < count);
i++, bytesWrittenTotal++) {
const uint8_t random8 = (uint8_t)(random32 >> (i * 8));
((uint8_t *)target)[bytesWrittenTotal] = random8;
}
}
return 1;
}

#endif

#if ! defined(HAVE_ARC4RANDOM_BUF) && ! defined(HAVE_ARC4RANDOM)

static unsigned long
gather_time_entropy(void) {
#if defined(_WIN32)
FILETIME ft;
GetSystemTimeAsFileTime(&ft);
return ft.dwHighDateTime ^ ft.dwLowDateTime;
#else
struct timeval tv;
int gettimeofday_res;

gettimeofday_res = gettimeofday(&tv, NULL);

#if defined(NDEBUG)
(void)gettimeofday_res;
#else
assert(gettimeofday_res == 0);
#endif


return tv.tv_usec;
#endif
}

#endif

static unsigned long
ENTROPY_DEBUG(const char *label, unsigned long entropy) {
const char *const EXPAT_ENTROPY_DEBUG = getenv("EXPAT_ENTROPY_DEBUG");
if (EXPAT_ENTROPY_DEBUG && ! strcmp(EXPAT_ENTROPY_DEBUG, "1")) {
fprintf(stderr, "Entropy: %s --> 0x%0*lx (%lu bytes)\n", label,
(int)sizeof(entropy) * 2, entropy, (unsigned long)sizeof(entropy));
}
return entropy;
}

static unsigned long
generate_hash_secret_salt(XML_Parser parser) {
unsigned long entropy;
(void)parser;


#if defined(HAVE_ARC4RANDOM_BUF)
arc4random_buf(&entropy, sizeof(entropy));
return ENTROPY_DEBUG("arc4random_buf", entropy);
#elif defined(HAVE_ARC4RANDOM)
writeRandomBytes_arc4random((void *)&entropy, sizeof(entropy));
return ENTROPY_DEBUG("arc4random", entropy);
#else

#if defined(_WIN32)
if (writeRandomBytes_rand_s((void *)&entropy, sizeof(entropy))) {
return ENTROPY_DEBUG("rand_s", entropy);
}
#elif defined(HAVE_GETRANDOM) || defined(HAVE_SYSCALL_GETRANDOM)
if (writeRandomBytes_getrandom_nonblock((void *)&entropy, sizeof(entropy))) {
return ENTROPY_DEBUG("getrandom", entropy);
}
#endif
#if ! defined(_WIN32) && defined(XML_DEV_URANDOM)
if (writeRandomBytes_dev_urandom((void *)&entropy, sizeof(entropy))) {
return ENTROPY_DEBUG("/dev/urandom", entropy);
}
#endif



entropy = gather_time_entropy() ^ getpid();


if (sizeof(unsigned long) == 4) {
return ENTROPY_DEBUG("fallback(4)", entropy * 2147483647);
} else {
return ENTROPY_DEBUG("fallback(8)",
entropy * (unsigned long)2305843009213693951ULL);
}
#endif
}

static unsigned long
get_hash_secret_salt(XML_Parser parser) {
if (parser->m_parentParser != NULL)
return get_hash_secret_salt(parser->m_parentParser);
return parser->m_hash_secret_salt;
}

static XML_Bool
startParsing(XML_Parser parser) {

if (parser->m_hash_secret_salt == 0)
parser->m_hash_secret_salt = generate_hash_secret_salt(parser);
if (parser->m_ns) {



return setContext(parser, implicitContext);
}
return XML_TRUE;
}

XML_Parser XMLCALL
XML_ParserCreate_MM(const XML_Char *encodingName,
const XML_Memory_Handling_Suite *memsuite,
const XML_Char *nameSep) {
return parserCreate(encodingName, memsuite, nameSep, NULL);
}

static XML_Parser
parserCreate(const XML_Char *encodingName,
const XML_Memory_Handling_Suite *memsuite, const XML_Char *nameSep,
DTD *dtd) {
XML_Parser parser;

if (memsuite) {
XML_Memory_Handling_Suite *mtemp;
parser = (XML_Parser)memsuite->malloc_fcn(sizeof(struct XML_ParserStruct));
if (parser != NULL) {
mtemp = (XML_Memory_Handling_Suite *)&(parser->m_mem);
mtemp->malloc_fcn = memsuite->malloc_fcn;
mtemp->realloc_fcn = memsuite->realloc_fcn;
mtemp->free_fcn = memsuite->free_fcn;
}
} else {
XML_Memory_Handling_Suite *mtemp;
parser = (XML_Parser)malloc(sizeof(struct XML_ParserStruct));
if (parser != NULL) {
mtemp = (XML_Memory_Handling_Suite *)&(parser->m_mem);
mtemp->malloc_fcn = malloc;
mtemp->realloc_fcn = realloc;
mtemp->free_fcn = free;
}
}

if (! parser)
return parser;

parser->m_buffer = NULL;
parser->m_bufferLim = NULL;

parser->m_attsSize = INIT_ATTS_SIZE;
parser->m_atts
= (ATTRIBUTE *)MALLOC(parser, parser->m_attsSize * sizeof(ATTRIBUTE));
if (parser->m_atts == NULL) {
FREE(parser, parser);
return NULL;
}
#if defined(XML_ATTR_INFO)
parser->m_attInfo = (XML_AttrInfo *)MALLOC(
parser, parser->m_attsSize * sizeof(XML_AttrInfo));
if (parser->m_attInfo == NULL) {
FREE(parser, parser->m_atts);
FREE(parser, parser);
return NULL;
}
#endif
parser->m_dataBuf
= (XML_Char *)MALLOC(parser, INIT_DATA_BUF_SIZE * sizeof(XML_Char));
if (parser->m_dataBuf == NULL) {
FREE(parser, parser->m_atts);
#if defined(XML_ATTR_INFO)
FREE(parser, parser->m_attInfo);
#endif
FREE(parser, parser);
return NULL;
}
parser->m_dataBufEnd = parser->m_dataBuf + INIT_DATA_BUF_SIZE;

if (dtd)
parser->m_dtd = dtd;
else {
parser->m_dtd = dtdCreate(&parser->m_mem);
if (parser->m_dtd == NULL) {
FREE(parser, parser->m_dataBuf);
FREE(parser, parser->m_atts);
#if defined(XML_ATTR_INFO)
FREE(parser, parser->m_attInfo);
#endif
FREE(parser, parser);
return NULL;
}
}

parser->m_freeBindingList = NULL;
parser->m_freeTagList = NULL;
parser->m_freeInternalEntities = NULL;

parser->m_groupSize = 0;
parser->m_groupConnector = NULL;

parser->m_unknownEncodingHandler = NULL;
parser->m_unknownEncodingHandlerData = NULL;

parser->m_namespaceSeparator = ASCII_EXCL;
parser->m_ns = XML_FALSE;
parser->m_ns_triplets = XML_FALSE;

parser->m_nsAtts = NULL;
parser->m_nsAttsVersion = 0;
parser->m_nsAttsPower = 0;

parser->m_protocolEncodingName = NULL;

poolInit(&parser->m_tempPool, &(parser->m_mem));
poolInit(&parser->m_temp2Pool, &(parser->m_mem));
parserInit(parser, encodingName);

if (encodingName && ! parser->m_protocolEncodingName) {
XML_ParserFree(parser);
return NULL;
}

if (nameSep) {
parser->m_ns = XML_TRUE;
parser->m_internalEncoding = XmlGetInternalEncodingNS();
parser->m_namespaceSeparator = *nameSep;
} else {
parser->m_internalEncoding = XmlGetInternalEncoding();
}

return parser;
}

static void
parserInit(XML_Parser parser, const XML_Char *encodingName) {
parser->m_processor = prologInitProcessor;
XmlPrologStateInit(&parser->m_prologState);
if (encodingName != NULL) {
parser->m_protocolEncodingName = copyString(encodingName, &(parser->m_mem));
}
parser->m_curBase = NULL;
XmlInitEncoding(&parser->m_initEncoding, &parser->m_encoding, 0);
parser->m_userData = NULL;
parser->m_handlerArg = NULL;
parser->m_startElementHandler = NULL;
parser->m_endElementHandler = NULL;
parser->m_characterDataHandler = NULL;
parser->m_processingInstructionHandler = NULL;
parser->m_commentHandler = NULL;
parser->m_startCdataSectionHandler = NULL;
parser->m_endCdataSectionHandler = NULL;
parser->m_defaultHandler = NULL;
parser->m_startDoctypeDeclHandler = NULL;
parser->m_endDoctypeDeclHandler = NULL;
parser->m_unparsedEntityDeclHandler = NULL;
parser->m_notationDeclHandler = NULL;
parser->m_startNamespaceDeclHandler = NULL;
parser->m_endNamespaceDeclHandler = NULL;
parser->m_notStandaloneHandler = NULL;
parser->m_externalEntityRefHandler = NULL;
parser->m_externalEntityRefHandlerArg = parser;
parser->m_skippedEntityHandler = NULL;
parser->m_elementDeclHandler = NULL;
parser->m_attlistDeclHandler = NULL;
parser->m_entityDeclHandler = NULL;
parser->m_xmlDeclHandler = NULL;
parser->m_bufferPtr = parser->m_buffer;
parser->m_bufferEnd = parser->m_buffer;
parser->m_parseEndByteIndex = 0;
parser->m_parseEndPtr = NULL;
parser->m_declElementType = NULL;
parser->m_declAttributeId = NULL;
parser->m_declEntity = NULL;
parser->m_doctypeName = NULL;
parser->m_doctypeSysid = NULL;
parser->m_doctypePubid = NULL;
parser->m_declAttributeType = NULL;
parser->m_declNotationName = NULL;
parser->m_declNotationPublicId = NULL;
parser->m_declAttributeIsCdata = XML_FALSE;
parser->m_declAttributeIsId = XML_FALSE;
memset(&parser->m_position, 0, sizeof(POSITION));
parser->m_errorCode = XML_ERROR_NONE;
parser->m_eventPtr = NULL;
parser->m_eventEndPtr = NULL;
parser->m_positionPtr = NULL;
parser->m_openInternalEntities = NULL;
parser->m_defaultExpandInternalEntities = XML_TRUE;
parser->m_tagLevel = 0;
parser->m_tagStack = NULL;
parser->m_inheritedBindings = NULL;
parser->m_nSpecifiedAtts = 0;
parser->m_unknownEncodingMem = NULL;
parser->m_unknownEncodingRelease = NULL;
parser->m_unknownEncodingData = NULL;
parser->m_parentParser = NULL;
parser->m_parsingStatus.parsing = XML_INITIALIZED;
#if defined(XML_DTD)
parser->m_isParamEntity = XML_FALSE;
parser->m_useForeignDTD = XML_FALSE;
parser->m_paramEntityParsing = XML_PARAM_ENTITY_PARSING_NEVER;
#endif
parser->m_hash_secret_salt = 0;
}


static void FASTCALL
moveToFreeBindingList(XML_Parser parser, BINDING *bindings) {
while (bindings) {
BINDING *b = bindings;
bindings = bindings->nextTagBinding;
b->nextTagBinding = parser->m_freeBindingList;
parser->m_freeBindingList = b;
}
}

XML_Bool XMLCALL
XML_ParserReset(XML_Parser parser, const XML_Char *encodingName) {
TAG *tStk;
OPEN_INTERNAL_ENTITY *openEntityList;

if (parser == NULL)
return XML_FALSE;

if (parser->m_parentParser)
return XML_FALSE;

tStk = parser->m_tagStack;
while (tStk) {
TAG *tag = tStk;
tStk = tStk->parent;
tag->parent = parser->m_freeTagList;
moveToFreeBindingList(parser, tag->bindings);
tag->bindings = NULL;
parser->m_freeTagList = tag;
}

openEntityList = parser->m_openInternalEntities;
while (openEntityList) {
OPEN_INTERNAL_ENTITY *openEntity = openEntityList;
openEntityList = openEntity->next;
openEntity->next = parser->m_freeInternalEntities;
parser->m_freeInternalEntities = openEntity;
}
moveToFreeBindingList(parser, parser->m_inheritedBindings);
FREE(parser, parser->m_unknownEncodingMem);
if (parser->m_unknownEncodingRelease)
parser->m_unknownEncodingRelease(parser->m_unknownEncodingData);
poolClear(&parser->m_tempPool);
poolClear(&parser->m_temp2Pool);
FREE(parser, (void *)parser->m_protocolEncodingName);
parser->m_protocolEncodingName = NULL;
parserInit(parser, encodingName);
dtdReset(parser->m_dtd, &parser->m_mem);
return XML_TRUE;
}

enum XML_Status XMLCALL
XML_SetEncoding(XML_Parser parser, const XML_Char *encodingName) {
if (parser == NULL)
return XML_STATUS_ERROR;




if (parser->m_parsingStatus.parsing == XML_PARSING
|| parser->m_parsingStatus.parsing == XML_SUSPENDED)
return XML_STATUS_ERROR;


FREE(parser, (void *)parser->m_protocolEncodingName);

if (encodingName == NULL)

parser->m_protocolEncodingName = NULL;
else {

parser->m_protocolEncodingName = copyString(encodingName, &(parser->m_mem));
if (! parser->m_protocolEncodingName)
return XML_STATUS_ERROR;
}
return XML_STATUS_OK;
}

XML_Parser XMLCALL
XML_ExternalEntityParserCreate(XML_Parser oldParser, const XML_Char *context,
const XML_Char *encodingName) {
XML_Parser parser = oldParser;
DTD *newDtd = NULL;
DTD *oldDtd;
XML_StartElementHandler oldStartElementHandler;
XML_EndElementHandler oldEndElementHandler;
XML_CharacterDataHandler oldCharacterDataHandler;
XML_ProcessingInstructionHandler oldProcessingInstructionHandler;
XML_CommentHandler oldCommentHandler;
XML_StartCdataSectionHandler oldStartCdataSectionHandler;
XML_EndCdataSectionHandler oldEndCdataSectionHandler;
XML_DefaultHandler oldDefaultHandler;
XML_UnparsedEntityDeclHandler oldUnparsedEntityDeclHandler;
XML_NotationDeclHandler oldNotationDeclHandler;
XML_StartNamespaceDeclHandler oldStartNamespaceDeclHandler;
XML_EndNamespaceDeclHandler oldEndNamespaceDeclHandler;
XML_NotStandaloneHandler oldNotStandaloneHandler;
XML_ExternalEntityRefHandler oldExternalEntityRefHandler;
XML_SkippedEntityHandler oldSkippedEntityHandler;
XML_UnknownEncodingHandler oldUnknownEncodingHandler;
XML_ElementDeclHandler oldElementDeclHandler;
XML_AttlistDeclHandler oldAttlistDeclHandler;
XML_EntityDeclHandler oldEntityDeclHandler;
XML_XmlDeclHandler oldXmlDeclHandler;
ELEMENT_TYPE *oldDeclElementType;

void *oldUserData;
void *oldHandlerArg;
XML_Bool oldDefaultExpandInternalEntities;
XML_Parser oldExternalEntityRefHandlerArg;
#if defined(XML_DTD)
enum XML_ParamEntityParsing oldParamEntityParsing;
int oldInEntityValue;
#endif
XML_Bool oldns_triplets;





unsigned long oldhash_secret_salt;


if (oldParser == NULL)
return NULL;


oldDtd = parser->m_dtd;
oldStartElementHandler = parser->m_startElementHandler;
oldEndElementHandler = parser->m_endElementHandler;
oldCharacterDataHandler = parser->m_characterDataHandler;
oldProcessingInstructionHandler = parser->m_processingInstructionHandler;
oldCommentHandler = parser->m_commentHandler;
oldStartCdataSectionHandler = parser->m_startCdataSectionHandler;
oldEndCdataSectionHandler = parser->m_endCdataSectionHandler;
oldDefaultHandler = parser->m_defaultHandler;
oldUnparsedEntityDeclHandler = parser->m_unparsedEntityDeclHandler;
oldNotationDeclHandler = parser->m_notationDeclHandler;
oldStartNamespaceDeclHandler = parser->m_startNamespaceDeclHandler;
oldEndNamespaceDeclHandler = parser->m_endNamespaceDeclHandler;
oldNotStandaloneHandler = parser->m_notStandaloneHandler;
oldExternalEntityRefHandler = parser->m_externalEntityRefHandler;
oldSkippedEntityHandler = parser->m_skippedEntityHandler;
oldUnknownEncodingHandler = parser->m_unknownEncodingHandler;
oldElementDeclHandler = parser->m_elementDeclHandler;
oldAttlistDeclHandler = parser->m_attlistDeclHandler;
oldEntityDeclHandler = parser->m_entityDeclHandler;
oldXmlDeclHandler = parser->m_xmlDeclHandler;
oldDeclElementType = parser->m_declElementType;

oldUserData = parser->m_userData;
oldHandlerArg = parser->m_handlerArg;
oldDefaultExpandInternalEntities = parser->m_defaultExpandInternalEntities;
oldExternalEntityRefHandlerArg = parser->m_externalEntityRefHandlerArg;
#if defined(XML_DTD)
oldParamEntityParsing = parser->m_paramEntityParsing;
oldInEntityValue = parser->m_prologState.inEntityValue;
#endif
oldns_triplets = parser->m_ns_triplets;





oldhash_secret_salt = parser->m_hash_secret_salt;

#if defined(XML_DTD)
if (! context)
newDtd = oldDtd;
#endif






if (parser->m_ns) {
XML_Char tmp[2];
*tmp = parser->m_namespaceSeparator;
parser = parserCreate(encodingName, &parser->m_mem, tmp, newDtd);
} else {
parser = parserCreate(encodingName, &parser->m_mem, NULL, newDtd);
}

if (! parser)
return NULL;

parser->m_startElementHandler = oldStartElementHandler;
parser->m_endElementHandler = oldEndElementHandler;
parser->m_characterDataHandler = oldCharacterDataHandler;
parser->m_processingInstructionHandler = oldProcessingInstructionHandler;
parser->m_commentHandler = oldCommentHandler;
parser->m_startCdataSectionHandler = oldStartCdataSectionHandler;
parser->m_endCdataSectionHandler = oldEndCdataSectionHandler;
parser->m_defaultHandler = oldDefaultHandler;
parser->m_unparsedEntityDeclHandler = oldUnparsedEntityDeclHandler;
parser->m_notationDeclHandler = oldNotationDeclHandler;
parser->m_startNamespaceDeclHandler = oldStartNamespaceDeclHandler;
parser->m_endNamespaceDeclHandler = oldEndNamespaceDeclHandler;
parser->m_notStandaloneHandler = oldNotStandaloneHandler;
parser->m_externalEntityRefHandler = oldExternalEntityRefHandler;
parser->m_skippedEntityHandler = oldSkippedEntityHandler;
parser->m_unknownEncodingHandler = oldUnknownEncodingHandler;
parser->m_elementDeclHandler = oldElementDeclHandler;
parser->m_attlistDeclHandler = oldAttlistDeclHandler;
parser->m_entityDeclHandler = oldEntityDeclHandler;
parser->m_xmlDeclHandler = oldXmlDeclHandler;
parser->m_declElementType = oldDeclElementType;
parser->m_userData = oldUserData;
if (oldUserData == oldHandlerArg)
parser->m_handlerArg = parser->m_userData;
else
parser->m_handlerArg = parser;
if (oldExternalEntityRefHandlerArg != oldParser)
parser->m_externalEntityRefHandlerArg = oldExternalEntityRefHandlerArg;
parser->m_defaultExpandInternalEntities = oldDefaultExpandInternalEntities;
parser->m_ns_triplets = oldns_triplets;
parser->m_hash_secret_salt = oldhash_secret_salt;
parser->m_parentParser = oldParser;
#if defined(XML_DTD)
parser->m_paramEntityParsing = oldParamEntityParsing;
parser->m_prologState.inEntityValue = oldInEntityValue;
if (context) {
#endif
if (! dtdCopy(oldParser, parser->m_dtd, oldDtd, &parser->m_mem)
|| ! setContext(parser, context)) {
XML_ParserFree(parser);
return NULL;
}
parser->m_processor = externalEntityInitProcessor;
#if defined(XML_DTD)
} else {







parser->m_isParamEntity = XML_TRUE;
XmlPrologStateInitExternalEntity(&parser->m_prologState);
parser->m_processor = externalParEntInitProcessor;
}
#endif
return parser;
}

static void FASTCALL
destroyBindings(BINDING *bindings, XML_Parser parser) {
for (;;) {
BINDING *b = bindings;
if (! b)
break;
bindings = b->nextTagBinding;
FREE(parser, b->uri);
FREE(parser, b);
}
}

void XMLCALL
XML_ParserFree(XML_Parser parser) {
TAG *tagList;
OPEN_INTERNAL_ENTITY *entityList;
if (parser == NULL)
return;

tagList = parser->m_tagStack;
for (;;) {
TAG *p;
if (tagList == NULL) {
if (parser->m_freeTagList == NULL)
break;
tagList = parser->m_freeTagList;
parser->m_freeTagList = NULL;
}
p = tagList;
tagList = tagList->parent;
FREE(parser, p->buf);
destroyBindings(p->bindings, parser);
FREE(parser, p);
}

entityList = parser->m_openInternalEntities;
for (;;) {
OPEN_INTERNAL_ENTITY *openEntity;
if (entityList == NULL) {
if (parser->m_freeInternalEntities == NULL)
break;
entityList = parser->m_freeInternalEntities;
parser->m_freeInternalEntities = NULL;
}
openEntity = entityList;
entityList = entityList->next;
FREE(parser, openEntity);
}

destroyBindings(parser->m_freeBindingList, parser);
destroyBindings(parser->m_inheritedBindings, parser);
poolDestroy(&parser->m_tempPool);
poolDestroy(&parser->m_temp2Pool);
FREE(parser, (void *)parser->m_protocolEncodingName);
#if defined(XML_DTD)



if (! parser->m_isParamEntity && parser->m_dtd)
#else
if (parser->m_dtd)
#endif
dtdDestroy(parser->m_dtd, (XML_Bool)! parser->m_parentParser,
&parser->m_mem);
FREE(parser, (void *)parser->m_atts);
#if defined(XML_ATTR_INFO)
FREE(parser, (void *)parser->m_attInfo);
#endif
FREE(parser, parser->m_groupConnector);
FREE(parser, parser->m_buffer);
FREE(parser, parser->m_dataBuf);
FREE(parser, parser->m_nsAtts);
FREE(parser, parser->m_unknownEncodingMem);
if (parser->m_unknownEncodingRelease)
parser->m_unknownEncodingRelease(parser->m_unknownEncodingData);
FREE(parser, parser);
}

void XMLCALL
XML_UseParserAsHandlerArg(XML_Parser parser) {
if (parser != NULL)
parser->m_handlerArg = parser;
}

enum XML_Error XMLCALL
XML_UseForeignDTD(XML_Parser parser, XML_Bool useDTD) {
if (parser == NULL)
return XML_ERROR_INVALID_ARGUMENT;
#if defined(XML_DTD)

if (parser->m_parsingStatus.parsing == XML_PARSING
|| parser->m_parsingStatus.parsing == XML_SUSPENDED)
return XML_ERROR_CANT_CHANGE_FEATURE_ONCE_PARSING;
parser->m_useForeignDTD = useDTD;
return XML_ERROR_NONE;
#else
return XML_ERROR_FEATURE_REQUIRES_XML_DTD;
#endif
}

void XMLCALL
XML_SetReturnNSTriplet(XML_Parser parser, int do_nst) {
if (parser == NULL)
return;

if (parser->m_parsingStatus.parsing == XML_PARSING
|| parser->m_parsingStatus.parsing == XML_SUSPENDED)
return;
parser->m_ns_triplets = do_nst ? XML_TRUE : XML_FALSE;
}

void XMLCALL
XML_SetUserData(XML_Parser parser, void *p) {
if (parser == NULL)
return;
if (parser->m_handlerArg == parser->m_userData)
parser->m_handlerArg = parser->m_userData = p;
else
parser->m_userData = p;
}

enum XML_Status XMLCALL
XML_SetBase(XML_Parser parser, const XML_Char *p) {
if (parser == NULL)
return XML_STATUS_ERROR;
if (p) {
p = poolCopyString(&parser->m_dtd->pool, p);
if (! p)
return XML_STATUS_ERROR;
parser->m_curBase = p;
} else
parser->m_curBase = NULL;
return XML_STATUS_OK;
}

const XML_Char *XMLCALL
XML_GetBase(XML_Parser parser) {
if (parser == NULL)
return NULL;
return parser->m_curBase;
}

int XMLCALL
XML_GetSpecifiedAttributeCount(XML_Parser parser) {
if (parser == NULL)
return -1;
return parser->m_nSpecifiedAtts;
}

int XMLCALL
XML_GetIdAttributeIndex(XML_Parser parser) {
if (parser == NULL)
return -1;
return parser->m_idAttIndex;
}

#if defined(XML_ATTR_INFO)
const XML_AttrInfo *XMLCALL
XML_GetAttributeInfo(XML_Parser parser) {
if (parser == NULL)
return NULL;
return parser->m_attInfo;
}
#endif

void XMLCALL
XML_SetElementHandler(XML_Parser parser, XML_StartElementHandler start,
XML_EndElementHandler end) {
if (parser == NULL)
return;
parser->m_startElementHandler = start;
parser->m_endElementHandler = end;
}

void XMLCALL
XML_SetStartElementHandler(XML_Parser parser, XML_StartElementHandler start) {
if (parser != NULL)
parser->m_startElementHandler = start;
}

void XMLCALL
XML_SetEndElementHandler(XML_Parser parser, XML_EndElementHandler end) {
if (parser != NULL)
parser->m_endElementHandler = end;
}

void XMLCALL
XML_SetCharacterDataHandler(XML_Parser parser,
XML_CharacterDataHandler handler) {
if (parser != NULL)
parser->m_characterDataHandler = handler;
}

void XMLCALL
XML_SetProcessingInstructionHandler(XML_Parser parser,
XML_ProcessingInstructionHandler handler) {
if (parser != NULL)
parser->m_processingInstructionHandler = handler;
}

void XMLCALL
XML_SetCommentHandler(XML_Parser parser, XML_CommentHandler handler) {
if (parser != NULL)
parser->m_commentHandler = handler;
}

void XMLCALL
XML_SetCdataSectionHandler(XML_Parser parser,
XML_StartCdataSectionHandler start,
XML_EndCdataSectionHandler end) {
if (parser == NULL)
return;
parser->m_startCdataSectionHandler = start;
parser->m_endCdataSectionHandler = end;
}

void XMLCALL
XML_SetStartCdataSectionHandler(XML_Parser parser,
XML_StartCdataSectionHandler start) {
if (parser != NULL)
parser->m_startCdataSectionHandler = start;
}

void XMLCALL
XML_SetEndCdataSectionHandler(XML_Parser parser,
XML_EndCdataSectionHandler end) {
if (parser != NULL)
parser->m_endCdataSectionHandler = end;
}

void XMLCALL
XML_SetDefaultHandler(XML_Parser parser, XML_DefaultHandler handler) {
if (parser == NULL)
return;
parser->m_defaultHandler = handler;
parser->m_defaultExpandInternalEntities = XML_FALSE;
}

void XMLCALL
XML_SetDefaultHandlerExpand(XML_Parser parser, XML_DefaultHandler handler) {
if (parser == NULL)
return;
parser->m_defaultHandler = handler;
parser->m_defaultExpandInternalEntities = XML_TRUE;
}

void XMLCALL
XML_SetDoctypeDeclHandler(XML_Parser parser, XML_StartDoctypeDeclHandler start,
XML_EndDoctypeDeclHandler end) {
if (parser == NULL)
return;
parser->m_startDoctypeDeclHandler = start;
parser->m_endDoctypeDeclHandler = end;
}

void XMLCALL
XML_SetStartDoctypeDeclHandler(XML_Parser parser,
XML_StartDoctypeDeclHandler start) {
if (parser != NULL)
parser->m_startDoctypeDeclHandler = start;
}

void XMLCALL
XML_SetEndDoctypeDeclHandler(XML_Parser parser, XML_EndDoctypeDeclHandler end) {
if (parser != NULL)
parser->m_endDoctypeDeclHandler = end;
}

void XMLCALL
XML_SetUnparsedEntityDeclHandler(XML_Parser parser,
XML_UnparsedEntityDeclHandler handler) {
if (parser != NULL)
parser->m_unparsedEntityDeclHandler = handler;
}

void XMLCALL
XML_SetNotationDeclHandler(XML_Parser parser, XML_NotationDeclHandler handler) {
if (parser != NULL)
parser->m_notationDeclHandler = handler;
}

void XMLCALL
XML_SetNamespaceDeclHandler(XML_Parser parser,
XML_StartNamespaceDeclHandler start,
XML_EndNamespaceDeclHandler end) {
if (parser == NULL)
return;
parser->m_startNamespaceDeclHandler = start;
parser->m_endNamespaceDeclHandler = end;
}

void XMLCALL
XML_SetStartNamespaceDeclHandler(XML_Parser parser,
XML_StartNamespaceDeclHandler start) {
if (parser != NULL)
parser->m_startNamespaceDeclHandler = start;
}

void XMLCALL
XML_SetEndNamespaceDeclHandler(XML_Parser parser,
XML_EndNamespaceDeclHandler end) {
if (parser != NULL)
parser->m_endNamespaceDeclHandler = end;
}

void XMLCALL
XML_SetNotStandaloneHandler(XML_Parser parser,
XML_NotStandaloneHandler handler) {
if (parser != NULL)
parser->m_notStandaloneHandler = handler;
}

void XMLCALL
XML_SetExternalEntityRefHandler(XML_Parser parser,
XML_ExternalEntityRefHandler handler) {
if (parser != NULL)
parser->m_externalEntityRefHandler = handler;
}

void XMLCALL
XML_SetExternalEntityRefHandlerArg(XML_Parser parser, void *arg) {
if (parser == NULL)
return;
if (arg)
parser->m_externalEntityRefHandlerArg = (XML_Parser)arg;
else
parser->m_externalEntityRefHandlerArg = parser;
}

void XMLCALL
XML_SetSkippedEntityHandler(XML_Parser parser,
XML_SkippedEntityHandler handler) {
if (parser != NULL)
parser->m_skippedEntityHandler = handler;
}

void XMLCALL
XML_SetUnknownEncodingHandler(XML_Parser parser,
XML_UnknownEncodingHandler handler, void *data) {
if (parser == NULL)
return;
parser->m_unknownEncodingHandler = handler;
parser->m_unknownEncodingHandlerData = data;
}

void XMLCALL
XML_SetElementDeclHandler(XML_Parser parser, XML_ElementDeclHandler eldecl) {
if (parser != NULL)
parser->m_elementDeclHandler = eldecl;
}

void XMLCALL
XML_SetAttlistDeclHandler(XML_Parser parser, XML_AttlistDeclHandler attdecl) {
if (parser != NULL)
parser->m_attlistDeclHandler = attdecl;
}

void XMLCALL
XML_SetEntityDeclHandler(XML_Parser parser, XML_EntityDeclHandler handler) {
if (parser != NULL)
parser->m_entityDeclHandler = handler;
}

void XMLCALL
XML_SetXmlDeclHandler(XML_Parser parser, XML_XmlDeclHandler handler) {
if (parser != NULL)
parser->m_xmlDeclHandler = handler;
}

int XMLCALL
XML_SetParamEntityParsing(XML_Parser parser,
enum XML_ParamEntityParsing peParsing) {
if (parser == NULL)
return 0;

if (parser->m_parsingStatus.parsing == XML_PARSING
|| parser->m_parsingStatus.parsing == XML_SUSPENDED)
return 0;
#if defined(XML_DTD)
parser->m_paramEntityParsing = peParsing;
return 1;
#else
return peParsing == XML_PARAM_ENTITY_PARSING_NEVER;
#endif
}

int XMLCALL
XML_SetHashSalt(XML_Parser parser, unsigned long hash_salt) {
if (parser == NULL)
return 0;
if (parser->m_parentParser)
return XML_SetHashSalt(parser->m_parentParser, hash_salt);

if (parser->m_parsingStatus.parsing == XML_PARSING
|| parser->m_parsingStatus.parsing == XML_SUSPENDED)
return 0;
parser->m_hash_secret_salt = hash_salt;
return 1;
}

enum XML_Status XMLCALL
XML_Parse(XML_Parser parser, const char *s, int len, int isFinal) {
if ((parser == NULL) || (len < 0) || ((s == NULL) && (len != 0))) {
if (parser != NULL)
parser->m_errorCode = XML_ERROR_INVALID_ARGUMENT;
return XML_STATUS_ERROR;
}
switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:
parser->m_errorCode = XML_ERROR_SUSPENDED;
return XML_STATUS_ERROR;
case XML_FINISHED:
parser->m_errorCode = XML_ERROR_FINISHED;
return XML_STATUS_ERROR;
case XML_INITIALIZED:
if (parser->m_parentParser == NULL && ! startParsing(parser)) {
parser->m_errorCode = XML_ERROR_NO_MEMORY;
return XML_STATUS_ERROR;
}

default:
parser->m_parsingStatus.parsing = XML_PARSING;
}

if (len == 0) {
parser->m_parsingStatus.finalBuffer = (XML_Bool)isFinal;
if (! isFinal)
return XML_STATUS_OK;
parser->m_positionPtr = parser->m_bufferPtr;
parser->m_parseEndPtr = parser->m_bufferEnd;





parser->m_errorCode
= parser->m_processor(parser, parser->m_bufferPtr,
parser->m_parseEndPtr, &parser->m_bufferPtr);

if (parser->m_errorCode == XML_ERROR_NONE) {
switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:












XmlUpdatePosition(parser->m_encoding, parser->m_positionPtr,
parser->m_bufferPtr, &parser->m_position);
parser->m_positionPtr = parser->m_bufferPtr;
return XML_STATUS_SUSPENDED;

case XML_INITIALIZED:
case XML_PARSING:
parser->m_parsingStatus.parsing = XML_FINISHED;

default:
return XML_STATUS_OK;
}
}
parser->m_eventEndPtr = parser->m_eventPtr;
parser->m_processor = errorProcessor;
return XML_STATUS_ERROR;
}
#if !defined(XML_CONTEXT_BYTES)
else if (parser->m_bufferPtr == parser->m_bufferEnd) {
const char *end;
int nLeftOver;
enum XML_Status result;

if (len > ((XML_Size)-1) / 2 - parser->m_parseEndByteIndex) {
parser->m_errorCode = XML_ERROR_NO_MEMORY;
parser->m_eventPtr = parser->m_eventEndPtr = NULL;
parser->m_processor = errorProcessor;
return XML_STATUS_ERROR;
}
parser->m_parseEndByteIndex += len;
parser->m_positionPtr = s;
parser->m_parsingStatus.finalBuffer = (XML_Bool)isFinal;

parser->m_errorCode
= parser->m_processor(parser, s, parser->m_parseEndPtr = s + len, &end);

if (parser->m_errorCode != XML_ERROR_NONE) {
parser->m_eventEndPtr = parser->m_eventPtr;
parser->m_processor = errorProcessor;
return XML_STATUS_ERROR;
} else {
switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:
result = XML_STATUS_SUSPENDED;
break;
case XML_INITIALIZED:
case XML_PARSING:
if (isFinal) {
parser->m_parsingStatus.parsing = XML_FINISHED;
return XML_STATUS_OK;
}

default:
result = XML_STATUS_OK;
}
}

XmlUpdatePosition(parser->m_encoding, parser->m_positionPtr, end,
&parser->m_position);
nLeftOver = s + len - end;
if (nLeftOver) {
if (parser->m_buffer == NULL
|| nLeftOver > parser->m_bufferLim - parser->m_buffer) {

char *temp = NULL;
const int bytesToAllocate = (int)((unsigned)len * 2U);
if (bytesToAllocate > 0) {
temp = (char *)REALLOC(parser, parser->m_buffer, bytesToAllocate);
}
if (temp == NULL) {
parser->m_errorCode = XML_ERROR_NO_MEMORY;
parser->m_eventPtr = parser->m_eventEndPtr = NULL;
parser->m_processor = errorProcessor;
return XML_STATUS_ERROR;
}
parser->m_buffer = temp;
parser->m_bufferLim = parser->m_buffer + bytesToAllocate;
}
memcpy(parser->m_buffer, end, nLeftOver);
}
parser->m_bufferPtr = parser->m_buffer;
parser->m_bufferEnd = parser->m_buffer + nLeftOver;
parser->m_positionPtr = parser->m_bufferPtr;
parser->m_parseEndPtr = parser->m_bufferEnd;
parser->m_eventPtr = parser->m_bufferPtr;
parser->m_eventEndPtr = parser->m_bufferPtr;
return result;
}
#endif
else {
void *buff = XML_GetBuffer(parser, len);
if (buff == NULL)
return XML_STATUS_ERROR;
else {
memcpy(buff, s, len);
return XML_ParseBuffer(parser, len, isFinal);
}
}
}

enum XML_Status XMLCALL
XML_ParseBuffer(XML_Parser parser, int len, int isFinal) {
const char *start;
enum XML_Status result = XML_STATUS_OK;

if (parser == NULL)
return XML_STATUS_ERROR;
switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:
parser->m_errorCode = XML_ERROR_SUSPENDED;
return XML_STATUS_ERROR;
case XML_FINISHED:
parser->m_errorCode = XML_ERROR_FINISHED;
return XML_STATUS_ERROR;
case XML_INITIALIZED:
if (parser->m_parentParser == NULL && ! startParsing(parser)) {
parser->m_errorCode = XML_ERROR_NO_MEMORY;
return XML_STATUS_ERROR;
}

default:
parser->m_parsingStatus.parsing = XML_PARSING;
}

start = parser->m_bufferPtr;
parser->m_positionPtr = start;
parser->m_bufferEnd += len;
parser->m_parseEndPtr = parser->m_bufferEnd;
parser->m_parseEndByteIndex += len;
parser->m_parsingStatus.finalBuffer = (XML_Bool)isFinal;

parser->m_errorCode = parser->m_processor(
parser, start, parser->m_parseEndPtr, &parser->m_bufferPtr);

if (parser->m_errorCode != XML_ERROR_NONE) {
parser->m_eventEndPtr = parser->m_eventPtr;
parser->m_processor = errorProcessor;
return XML_STATUS_ERROR;
} else {
switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:
result = XML_STATUS_SUSPENDED;
break;
case XML_INITIALIZED:
case XML_PARSING:
if (isFinal) {
parser->m_parsingStatus.parsing = XML_FINISHED;
return result;
}
default:;
}
}

XmlUpdatePosition(parser->m_encoding, parser->m_positionPtr,
parser->m_bufferPtr, &parser->m_position);
parser->m_positionPtr = parser->m_bufferPtr;
return result;
}

void *XMLCALL
XML_GetBuffer(XML_Parser parser, int len) {
if (parser == NULL)
return NULL;
if (len < 0) {
parser->m_errorCode = XML_ERROR_NO_MEMORY;
return NULL;
}
switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:
parser->m_errorCode = XML_ERROR_SUSPENDED;
return NULL;
case XML_FINISHED:
parser->m_errorCode = XML_ERROR_FINISHED;
return NULL;
default:;
}

if (len > EXPAT_SAFE_PTR_DIFF(parser->m_bufferLim, parser->m_bufferEnd)) {
#if defined(XML_CONTEXT_BYTES)
int keep;
#endif

int neededSize = (int)((unsigned)len
+ (unsigned)EXPAT_SAFE_PTR_DIFF(
parser->m_bufferEnd, parser->m_bufferPtr));
if (neededSize < 0) {
parser->m_errorCode = XML_ERROR_NO_MEMORY;
return NULL;
}
#if defined(XML_CONTEXT_BYTES)
keep = (int)EXPAT_SAFE_PTR_DIFF(parser->m_bufferPtr, parser->m_buffer);
if (keep > XML_CONTEXT_BYTES)
keep = XML_CONTEXT_BYTES;
neededSize += keep;
#endif
if (neededSize
<= EXPAT_SAFE_PTR_DIFF(parser->m_bufferLim, parser->m_buffer)) {
#if defined(XML_CONTEXT_BYTES)
if (keep < EXPAT_SAFE_PTR_DIFF(parser->m_bufferPtr, parser->m_buffer)) {
int offset
= (int)EXPAT_SAFE_PTR_DIFF(parser->m_bufferPtr, parser->m_buffer)
- keep;


memmove(parser->m_buffer, &parser->m_buffer[offset],
parser->m_bufferEnd - parser->m_bufferPtr + keep);
parser->m_bufferEnd -= offset;
parser->m_bufferPtr -= offset;
}
#else
if (parser->m_buffer && parser->m_bufferPtr) {
memmove(parser->m_buffer, parser->m_bufferPtr,
EXPAT_SAFE_PTR_DIFF(parser->m_bufferEnd, parser->m_bufferPtr));
parser->m_bufferEnd
= parser->m_buffer
+ EXPAT_SAFE_PTR_DIFF(parser->m_bufferEnd, parser->m_bufferPtr);
parser->m_bufferPtr = parser->m_buffer;
}
#endif
} else {
char *newBuf;
int bufferSize
= (int)EXPAT_SAFE_PTR_DIFF(parser->m_bufferLim, parser->m_bufferPtr);
if (bufferSize == 0)
bufferSize = INIT_BUFFER_SIZE;
do {

bufferSize = (int)(2U * (unsigned)bufferSize);
} while (bufferSize < neededSize && bufferSize > 0);
if (bufferSize <= 0) {
parser->m_errorCode = XML_ERROR_NO_MEMORY;
return NULL;
}
newBuf = (char *)MALLOC(parser, bufferSize);
if (newBuf == 0) {
parser->m_errorCode = XML_ERROR_NO_MEMORY;
return NULL;
}
parser->m_bufferLim = newBuf + bufferSize;
#if defined(XML_CONTEXT_BYTES)
if (parser->m_bufferPtr) {
memcpy(newBuf, &parser->m_bufferPtr[-keep],
EXPAT_SAFE_PTR_DIFF(parser->m_bufferEnd, parser->m_bufferPtr)
+ keep);
FREE(parser, parser->m_buffer);
parser->m_buffer = newBuf;
parser->m_bufferEnd
= parser->m_buffer
+ EXPAT_SAFE_PTR_DIFF(parser->m_bufferEnd, parser->m_bufferPtr)
+ keep;
parser->m_bufferPtr = parser->m_buffer + keep;
} else {

parser->m_bufferEnd = newBuf;
parser->m_bufferPtr = parser->m_buffer = newBuf;
}
#else
if (parser->m_bufferPtr) {
memcpy(newBuf, parser->m_bufferPtr,
EXPAT_SAFE_PTR_DIFF(parser->m_bufferEnd, parser->m_bufferPtr));
FREE(parser, parser->m_buffer);
parser->m_bufferEnd
= newBuf
+ EXPAT_SAFE_PTR_DIFF(parser->m_bufferEnd, parser->m_bufferPtr);
} else {

parser->m_bufferEnd = newBuf;
}
parser->m_bufferPtr = parser->m_buffer = newBuf;
#endif
}
parser->m_eventPtr = parser->m_eventEndPtr = NULL;
parser->m_positionPtr = NULL;
}
return parser->m_bufferEnd;
}

enum XML_Status XMLCALL
XML_StopParser(XML_Parser parser, XML_Bool resumable) {
if (parser == NULL)
return XML_STATUS_ERROR;
switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:
if (resumable) {
parser->m_errorCode = XML_ERROR_SUSPENDED;
return XML_STATUS_ERROR;
}
parser->m_parsingStatus.parsing = XML_FINISHED;
break;
case XML_FINISHED:
parser->m_errorCode = XML_ERROR_FINISHED;
return XML_STATUS_ERROR;
default:
if (resumable) {
#if defined(XML_DTD)
if (parser->m_isParamEntity) {
parser->m_errorCode = XML_ERROR_SUSPEND_PE;
return XML_STATUS_ERROR;
}
#endif
parser->m_parsingStatus.parsing = XML_SUSPENDED;
} else
parser->m_parsingStatus.parsing = XML_FINISHED;
}
return XML_STATUS_OK;
}

enum XML_Status XMLCALL
XML_ResumeParser(XML_Parser parser) {
enum XML_Status result = XML_STATUS_OK;

if (parser == NULL)
return XML_STATUS_ERROR;
if (parser->m_parsingStatus.parsing != XML_SUSPENDED) {
parser->m_errorCode = XML_ERROR_NOT_SUSPENDED;
return XML_STATUS_ERROR;
}
parser->m_parsingStatus.parsing = XML_PARSING;

parser->m_errorCode = parser->m_processor(
parser, parser->m_bufferPtr, parser->m_parseEndPtr, &parser->m_bufferPtr);

if (parser->m_errorCode != XML_ERROR_NONE) {
parser->m_eventEndPtr = parser->m_eventPtr;
parser->m_processor = errorProcessor;
return XML_STATUS_ERROR;
} else {
switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:
result = XML_STATUS_SUSPENDED;
break;
case XML_INITIALIZED:
case XML_PARSING:
if (parser->m_parsingStatus.finalBuffer) {
parser->m_parsingStatus.parsing = XML_FINISHED;
return result;
}
default:;
}
}

XmlUpdatePosition(parser->m_encoding, parser->m_positionPtr,
parser->m_bufferPtr, &parser->m_position);
parser->m_positionPtr = parser->m_bufferPtr;
return result;
}

void XMLCALL
XML_GetParsingStatus(XML_Parser parser, XML_ParsingStatus *status) {
if (parser == NULL)
return;
assert(status != NULL);
*status = parser->m_parsingStatus;
}

enum XML_Error XMLCALL
XML_GetErrorCode(XML_Parser parser) {
if (parser == NULL)
return XML_ERROR_INVALID_ARGUMENT;
return parser->m_errorCode;
}

XML_Index XMLCALL
XML_GetCurrentByteIndex(XML_Parser parser) {
if (parser == NULL)
return -1;
if (parser->m_eventPtr)
return (XML_Index)(parser->m_parseEndByteIndex
- (parser->m_parseEndPtr - parser->m_eventPtr));
return -1;
}

int XMLCALL
XML_GetCurrentByteCount(XML_Parser parser) {
if (parser == NULL)
return 0;
if (parser->m_eventEndPtr && parser->m_eventPtr)
return (int)(parser->m_eventEndPtr - parser->m_eventPtr);
return 0;
}

const char *XMLCALL
XML_GetInputContext(XML_Parser parser, int *offset, int *size) {
#if defined(XML_CONTEXT_BYTES)
if (parser == NULL)
return NULL;
if (parser->m_eventPtr && parser->m_buffer) {
if (offset != NULL)
*offset = (int)(parser->m_eventPtr - parser->m_buffer);
if (size != NULL)
*size = (int)(parser->m_bufferEnd - parser->m_buffer);
return parser->m_buffer;
}
#else
(void)parser;
(void)offset;
(void)size;
#endif
return (char *)0;
}

XML_Size XMLCALL
XML_GetCurrentLineNumber(XML_Parser parser) {
if (parser == NULL)
return 0;
if (parser->m_eventPtr && parser->m_eventPtr >= parser->m_positionPtr) {
XmlUpdatePosition(parser->m_encoding, parser->m_positionPtr,
parser->m_eventPtr, &parser->m_position);
parser->m_positionPtr = parser->m_eventPtr;
}
return parser->m_position.lineNumber + 1;
}

XML_Size XMLCALL
XML_GetCurrentColumnNumber(XML_Parser parser) {
if (parser == NULL)
return 0;
if (parser->m_eventPtr && parser->m_eventPtr >= parser->m_positionPtr) {
XmlUpdatePosition(parser->m_encoding, parser->m_positionPtr,
parser->m_eventPtr, &parser->m_position);
parser->m_positionPtr = parser->m_eventPtr;
}
return parser->m_position.columnNumber;
}

void XMLCALL
XML_FreeContentModel(XML_Parser parser, XML_Content *model) {
if (parser != NULL)
FREE(parser, model);
}

void *XMLCALL
XML_MemMalloc(XML_Parser parser, size_t size) {
if (parser == NULL)
return NULL;
return MALLOC(parser, size);
}

void *XMLCALL
XML_MemRealloc(XML_Parser parser, void *ptr, size_t size) {
if (parser == NULL)
return NULL;
return REALLOC(parser, ptr, size);
}

void XMLCALL
XML_MemFree(XML_Parser parser, void *ptr) {
if (parser != NULL)
FREE(parser, ptr);
}

void XMLCALL
XML_DefaultCurrent(XML_Parser parser) {
if (parser == NULL)
return;
if (parser->m_defaultHandler) {
if (parser->m_openInternalEntities)
reportDefault(parser, parser->m_internalEncoding,
parser->m_openInternalEntities->internalEventPtr,
parser->m_openInternalEntities->internalEventEndPtr);
else
reportDefault(parser, parser->m_encoding, parser->m_eventPtr,
parser->m_eventEndPtr);
}
}

const XML_LChar *XMLCALL
XML_ErrorString(enum XML_Error code) {
switch (code) {
case XML_ERROR_NONE:
return NULL;
case XML_ERROR_NO_MEMORY:
return XML_L("out of memory");
case XML_ERROR_SYNTAX:
return XML_L("syntax error");
case XML_ERROR_NO_ELEMENTS:
return XML_L("no element found");
case XML_ERROR_INVALID_TOKEN:
return XML_L("not well-formed (invalid token)");
case XML_ERROR_UNCLOSED_TOKEN:
return XML_L("unclosed token");
case XML_ERROR_PARTIAL_CHAR:
return XML_L("partial character");
case XML_ERROR_TAG_MISMATCH:
return XML_L("mismatched tag");
case XML_ERROR_DUPLICATE_ATTRIBUTE:
return XML_L("duplicate attribute");
case XML_ERROR_JUNK_AFTER_DOC_ELEMENT:
return XML_L("junk after document element");
case XML_ERROR_PARAM_ENTITY_REF:
return XML_L("illegal parameter entity reference");
case XML_ERROR_UNDEFINED_ENTITY:
return XML_L("undefined entity");
case XML_ERROR_RECURSIVE_ENTITY_REF:
return XML_L("recursive entity reference");
case XML_ERROR_ASYNC_ENTITY:
return XML_L("asynchronous entity");
case XML_ERROR_BAD_CHAR_REF:
return XML_L("reference to invalid character number");
case XML_ERROR_BINARY_ENTITY_REF:
return XML_L("reference to binary entity");
case XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF:
return XML_L("reference to external entity in attribute");
case XML_ERROR_MISPLACED_XML_PI:
return XML_L("XML or text declaration not at start of entity");
case XML_ERROR_UNKNOWN_ENCODING:
return XML_L("unknown encoding");
case XML_ERROR_INCORRECT_ENCODING:
return XML_L("encoding specified in XML declaration is incorrect");
case XML_ERROR_UNCLOSED_CDATA_SECTION:
return XML_L("unclosed CDATA section");
case XML_ERROR_EXTERNAL_ENTITY_HANDLING:
return XML_L("error in processing external entity reference");
case XML_ERROR_NOT_STANDALONE:
return XML_L("document is not standalone");
case XML_ERROR_UNEXPECTED_STATE:
return XML_L("unexpected parser state - please send a bug report");
case XML_ERROR_ENTITY_DECLARED_IN_PE:
return XML_L("entity declared in parameter entity");
case XML_ERROR_FEATURE_REQUIRES_XML_DTD:
return XML_L("requested feature requires XML_DTD support in Expat");
case XML_ERROR_CANT_CHANGE_FEATURE_ONCE_PARSING:
return XML_L("cannot change setting once parsing has begun");

case XML_ERROR_UNBOUND_PREFIX:
return XML_L("unbound prefix");

case XML_ERROR_UNDECLARING_PREFIX:
return XML_L("must not undeclare prefix");
case XML_ERROR_INCOMPLETE_PE:
return XML_L("incomplete markup in parameter entity");
case XML_ERROR_XML_DECL:
return XML_L("XML declaration not well-formed");
case XML_ERROR_TEXT_DECL:
return XML_L("text declaration not well-formed");
case XML_ERROR_PUBLICID:
return XML_L("illegal character(s) in public id");
case XML_ERROR_SUSPENDED:
return XML_L("parser suspended");
case XML_ERROR_NOT_SUSPENDED:
return XML_L("parser not suspended");
case XML_ERROR_ABORTED:
return XML_L("parsing aborted");
case XML_ERROR_FINISHED:
return XML_L("parsing finished");
case XML_ERROR_SUSPEND_PE:
return XML_L("cannot suspend in external parameter entity");

case XML_ERROR_RESERVED_PREFIX_XML:
return XML_L(
"reserved prefix (xml) must not be undeclared or bound to another namespace name");
case XML_ERROR_RESERVED_PREFIX_XMLNS:
return XML_L("reserved prefix (xmlns) must not be declared or undeclared");
case XML_ERROR_RESERVED_NAMESPACE_URI:
return XML_L(
"prefix must not be bound to one of the reserved namespace names");

case XML_ERROR_INVALID_ARGUMENT:
return XML_L("invalid argument");
}
return NULL;
}

const XML_LChar *XMLCALL
XML_ExpatVersion(void) {








#define V1(a, b, c) XML_L(#a) XML_L(".") XML_L(#b) XML_L(".") XML_L(#c)
#define V2(a, b, c) XML_L("expat_") V1(a, b, c)

return V2(XML_MAJOR_VERSION, XML_MINOR_VERSION, XML_MICRO_VERSION);

#undef V1
#undef V2
}

XML_Expat_Version XMLCALL
XML_ExpatVersionInfo(void) {
XML_Expat_Version version;

version.major = XML_MAJOR_VERSION;
version.minor = XML_MINOR_VERSION;
version.micro = XML_MICRO_VERSION;

return version;
}

const XML_Feature *XMLCALL
XML_GetFeatureList(void) {
static const XML_Feature features[]
= {{XML_FEATURE_SIZEOF_XML_CHAR, XML_L("sizeof(XML_Char)"),
sizeof(XML_Char)},
{XML_FEATURE_SIZEOF_XML_LCHAR, XML_L("sizeof(XML_LChar)"),
sizeof(XML_LChar)},
#if defined(XML_UNICODE)
{XML_FEATURE_UNICODE, XML_L("XML_UNICODE"), 0},
#endif
#if defined(XML_UNICODE_WCHAR_T)
{XML_FEATURE_UNICODE_WCHAR_T, XML_L("XML_UNICODE_WCHAR_T"), 0},
#endif
#if defined(XML_DTD)
{XML_FEATURE_DTD, XML_L("XML_DTD"), 0},
#endif
#if defined(XML_CONTEXT_BYTES)
{XML_FEATURE_CONTEXT_BYTES, XML_L("XML_CONTEXT_BYTES"),
XML_CONTEXT_BYTES},
#endif
#if defined(XML_MIN_SIZE)
{XML_FEATURE_MIN_SIZE, XML_L("XML_MIN_SIZE"), 0},
#endif
#if defined(XML_NS)
{XML_FEATURE_NS, XML_L("XML_NS"), 0},
#endif
#if defined(XML_LARGE_SIZE)
{XML_FEATURE_LARGE_SIZE, XML_L("XML_LARGE_SIZE"), 0},
#endif
#if defined(XML_ATTR_INFO)
{XML_FEATURE_ATTR_INFO, XML_L("XML_ATTR_INFO"), 0},
#endif
{XML_FEATURE_END, NULL, 0}};

return features;
}






static XML_Bool
storeRawNames(XML_Parser parser) {
TAG *tag = parser->m_tagStack;
while (tag) {
int bufSize;
int nameLen = sizeof(XML_Char) * (tag->name.strLen + 1);
char *rawNameBuf = tag->buf + nameLen;





if (tag->rawName == rawNameBuf)
break;



bufSize = nameLen + ROUND_UP(tag->rawNameLength, sizeof(XML_Char));
if (bufSize > tag->bufEnd - tag->buf) {
char *temp = (char *)REALLOC(parser, tag->buf, bufSize);
if (temp == NULL)
return XML_FALSE;



if (tag->name.str == (XML_Char *)tag->buf)
tag->name.str = (XML_Char *)temp;



if (tag->name.localPart)
tag->name.localPart
= (XML_Char *)temp + (tag->name.localPart - (XML_Char *)tag->buf);
tag->buf = temp;
tag->bufEnd = temp + bufSize;
rawNameBuf = temp + nameLen;
}
memcpy(rawNameBuf, tag->rawName, tag->rawNameLength);
tag->rawName = rawNameBuf;
tag = tag->parent;
}
return XML_TRUE;
}

static enum XML_Error PTRCALL
contentProcessor(XML_Parser parser, const char *start, const char *end,
const char **endPtr) {
enum XML_Error result
= doContent(parser, 0, parser->m_encoding, start, end, endPtr,
(XML_Bool)! parser->m_parsingStatus.finalBuffer);
if (result == XML_ERROR_NONE) {
if (! storeRawNames(parser))
return XML_ERROR_NO_MEMORY;
}
return result;
}

static enum XML_Error PTRCALL
externalEntityInitProcessor(XML_Parser parser, const char *start,
const char *end, const char **endPtr) {
enum XML_Error result = initializeEncoding(parser);
if (result != XML_ERROR_NONE)
return result;
parser->m_processor = externalEntityInitProcessor2;
return externalEntityInitProcessor2(parser, start, end, endPtr);
}

static enum XML_Error PTRCALL
externalEntityInitProcessor2(XML_Parser parser, const char *start,
const char *end, const char **endPtr) {
const char *next = start;
int tok = XmlContentTok(parser->m_encoding, start, end, &next);
switch (tok) {
case XML_TOK_BOM:





if (next == end && ! parser->m_parsingStatus.finalBuffer) {
*endPtr = next;
return XML_ERROR_NONE;
}
start = next;
break;
case XML_TOK_PARTIAL:
if (! parser->m_parsingStatus.finalBuffer) {
*endPtr = start;
return XML_ERROR_NONE;
}
parser->m_eventPtr = start;
return XML_ERROR_UNCLOSED_TOKEN;
case XML_TOK_PARTIAL_CHAR:
if (! parser->m_parsingStatus.finalBuffer) {
*endPtr = start;
return XML_ERROR_NONE;
}
parser->m_eventPtr = start;
return XML_ERROR_PARTIAL_CHAR;
}
parser->m_processor = externalEntityInitProcessor3;
return externalEntityInitProcessor3(parser, start, end, endPtr);
}

static enum XML_Error PTRCALL
externalEntityInitProcessor3(XML_Parser parser, const char *start,
const char *end, const char **endPtr) {
int tok;
const char *next = start;
parser->m_eventPtr = start;
tok = XmlContentTok(parser->m_encoding, start, end, &next);
parser->m_eventEndPtr = next;

switch (tok) {
case XML_TOK_XML_DECL: {
enum XML_Error result;
result = processXmlDecl(parser, 1, start, next);
if (result != XML_ERROR_NONE)
return result;
switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:
*endPtr = next;
return XML_ERROR_NONE;
case XML_FINISHED:
return XML_ERROR_ABORTED;
default:
start = next;
}
} break;
case XML_TOK_PARTIAL:
if (! parser->m_parsingStatus.finalBuffer) {
*endPtr = start;
return XML_ERROR_NONE;
}
return XML_ERROR_UNCLOSED_TOKEN;
case XML_TOK_PARTIAL_CHAR:
if (! parser->m_parsingStatus.finalBuffer) {
*endPtr = start;
return XML_ERROR_NONE;
}
return XML_ERROR_PARTIAL_CHAR;
}
parser->m_processor = externalEntityContentProcessor;
parser->m_tagLevel = 1;
return externalEntityContentProcessor(parser, start, end, endPtr);
}

static enum XML_Error PTRCALL
externalEntityContentProcessor(XML_Parser parser, const char *start,
const char *end, const char **endPtr) {
enum XML_Error result
= doContent(parser, 1, parser->m_encoding, start, end, endPtr,
(XML_Bool)! parser->m_parsingStatus.finalBuffer);
if (result == XML_ERROR_NONE) {
if (! storeRawNames(parser))
return XML_ERROR_NO_MEMORY;
}
return result;
}

static enum XML_Error
doContent(XML_Parser parser, int startTagLevel, const ENCODING *enc,
const char *s, const char *end, const char **nextPtr,
XML_Bool haveMore) {

DTD *const dtd = parser->m_dtd;

const char **eventPP;
const char **eventEndPP;
if (enc == parser->m_encoding) {
eventPP = &parser->m_eventPtr;
eventEndPP = &parser->m_eventEndPtr;
} else {
eventPP = &(parser->m_openInternalEntities->internalEventPtr);
eventEndPP = &(parser->m_openInternalEntities->internalEventEndPtr);
}
*eventPP = s;

for (;;) {
const char *next = s;
int tok = XmlContentTok(enc, s, end, &next);
*eventEndPP = next;
switch (tok) {
case XML_TOK_TRAILING_CR:
if (haveMore) {
*nextPtr = s;
return XML_ERROR_NONE;
}
*eventEndPP = end;
if (parser->m_characterDataHandler) {
XML_Char c = 0xA;
parser->m_characterDataHandler(parser->m_handlerArg, &c, 1);
} else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, end);



if (startTagLevel == 0)
return XML_ERROR_NO_ELEMENTS;
if (parser->m_tagLevel != startTagLevel)
return XML_ERROR_ASYNC_ENTITY;
*nextPtr = end;
return XML_ERROR_NONE;
case XML_TOK_NONE:
if (haveMore) {
*nextPtr = s;
return XML_ERROR_NONE;
}
if (startTagLevel > 0) {
if (parser->m_tagLevel != startTagLevel)
return XML_ERROR_ASYNC_ENTITY;
*nextPtr = s;
return XML_ERROR_NONE;
}
return XML_ERROR_NO_ELEMENTS;
case XML_TOK_INVALID:
*eventPP = next;
return XML_ERROR_INVALID_TOKEN;
case XML_TOK_PARTIAL:
if (haveMore) {
*nextPtr = s;
return XML_ERROR_NONE;
}
return XML_ERROR_UNCLOSED_TOKEN;
case XML_TOK_PARTIAL_CHAR:
if (haveMore) {
*nextPtr = s;
return XML_ERROR_NONE;
}
return XML_ERROR_PARTIAL_CHAR;
case XML_TOK_ENTITY_REF: {
const XML_Char *name;
ENTITY *entity;
XML_Char ch = (XML_Char)XmlPredefinedEntityName(
enc, s + enc->minBytesPerChar, next - enc->minBytesPerChar);
if (ch) {
if (parser->m_characterDataHandler)
parser->m_characterDataHandler(parser->m_handlerArg, &ch, 1);
else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
break;
}
name = poolStoreString(&dtd->pool, enc, s + enc->minBytesPerChar,
next - enc->minBytesPerChar);
if (! name)
return XML_ERROR_NO_MEMORY;
entity = (ENTITY *)lookup(parser, &dtd->generalEntities, name, 0);
poolDiscard(&dtd->pool);




if (! dtd->hasParamEntityRefs || dtd->standalone) {
if (! entity)
return XML_ERROR_UNDEFINED_ENTITY;
else if (! entity->is_internal)
return XML_ERROR_ENTITY_DECLARED_IN_PE;
} else if (! entity) {
if (parser->m_skippedEntityHandler)
parser->m_skippedEntityHandler(parser->m_handlerArg, name, 0);
else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
break;
}
if (entity->open)
return XML_ERROR_RECURSIVE_ENTITY_REF;
if (entity->notation)
return XML_ERROR_BINARY_ENTITY_REF;
if (entity->textPtr) {
enum XML_Error result;
if (! parser->m_defaultExpandInternalEntities) {
if (parser->m_skippedEntityHandler)
parser->m_skippedEntityHandler(parser->m_handlerArg, entity->name,
0);
else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
break;
}
result = processInternalEntity(parser, entity, XML_FALSE);
if (result != XML_ERROR_NONE)
return result;
} else if (parser->m_externalEntityRefHandler) {
const XML_Char *context;
entity->open = XML_TRUE;
context = getContext(parser);
entity->open = XML_FALSE;
if (! context)
return XML_ERROR_NO_MEMORY;
if (! parser->m_externalEntityRefHandler(
parser->m_externalEntityRefHandlerArg, context, entity->base,
entity->systemId, entity->publicId))
return XML_ERROR_EXTERNAL_ENTITY_HANDLING;
poolDiscard(&parser->m_tempPool);
} else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
break;
}
case XML_TOK_START_TAG_NO_ATTS:

case XML_TOK_START_TAG_WITH_ATTS: {
TAG *tag;
enum XML_Error result;
XML_Char *toPtr;
if (parser->m_freeTagList) {
tag = parser->m_freeTagList;
parser->m_freeTagList = parser->m_freeTagList->parent;
} else {
tag = (TAG *)MALLOC(parser, sizeof(TAG));
if (! tag)
return XML_ERROR_NO_MEMORY;
tag->buf = (char *)MALLOC(parser, INIT_TAG_BUF_SIZE);
if (! tag->buf) {
FREE(parser, tag);
return XML_ERROR_NO_MEMORY;
}
tag->bufEnd = tag->buf + INIT_TAG_BUF_SIZE;
}
tag->bindings = NULL;
tag->parent = parser->m_tagStack;
parser->m_tagStack = tag;
tag->name.localPart = NULL;
tag->name.prefix = NULL;
tag->rawName = s + enc->minBytesPerChar;
tag->rawNameLength = XmlNameLength(enc, tag->rawName);
++parser->m_tagLevel;
{
const char *rawNameEnd = tag->rawName + tag->rawNameLength;
const char *fromPtr = tag->rawName;
toPtr = (XML_Char *)tag->buf;
for (;;) {
int bufSize;
int convLen;
const enum XML_Convert_Result convert_res
= XmlConvert(enc, &fromPtr, rawNameEnd, (ICHAR **)&toPtr,
(ICHAR *)tag->bufEnd - 1);
convLen = (int)(toPtr - (XML_Char *)tag->buf);
if ((fromPtr >= rawNameEnd)
|| (convert_res == XML_CONVERT_INPUT_INCOMPLETE)) {
tag->name.strLen = convLen;
break;
}
bufSize = (int)(tag->bufEnd - tag->buf) << 1;
{
char *temp = (char *)REALLOC(parser, tag->buf, bufSize);
if (temp == NULL)
return XML_ERROR_NO_MEMORY;
tag->buf = temp;
tag->bufEnd = temp + bufSize;
toPtr = (XML_Char *)temp + convLen;
}
}
}
tag->name.str = (XML_Char *)tag->buf;
*toPtr = XML_T('\0');
result = storeAtts(parser, enc, s, &(tag->name), &(tag->bindings));
if (result)
return result;
if (parser->m_startElementHandler)
parser->m_startElementHandler(parser->m_handlerArg, tag->name.str,
(const XML_Char **)parser->m_atts);
else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
poolClear(&parser->m_tempPool);
break;
}
case XML_TOK_EMPTY_ELEMENT_NO_ATTS:

case XML_TOK_EMPTY_ELEMENT_WITH_ATTS: {
const char *rawName = s + enc->minBytesPerChar;
enum XML_Error result;
BINDING *bindings = NULL;
XML_Bool noElmHandlers = XML_TRUE;
TAG_NAME name;
name.str = poolStoreString(&parser->m_tempPool, enc, rawName,
rawName + XmlNameLength(enc, rawName));
if (! name.str)
return XML_ERROR_NO_MEMORY;
poolFinish(&parser->m_tempPool);
result = storeAtts(parser, enc, s, &name, &bindings);
if (result != XML_ERROR_NONE) {
freeBindings(parser, bindings);
return result;
}
poolFinish(&parser->m_tempPool);
if (parser->m_startElementHandler) {
parser->m_startElementHandler(parser->m_handlerArg, name.str,
(const XML_Char **)parser->m_atts);
noElmHandlers = XML_FALSE;
}
if (parser->m_endElementHandler) {
if (parser->m_startElementHandler)
*eventPP = *eventEndPP;
parser->m_endElementHandler(parser->m_handlerArg, name.str);
noElmHandlers = XML_FALSE;
}
if (noElmHandlers && parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
poolClear(&parser->m_tempPool);
freeBindings(parser, bindings);
}
if ((parser->m_tagLevel == 0)
&& (parser->m_parsingStatus.parsing != XML_FINISHED)) {
if (parser->m_parsingStatus.parsing == XML_SUSPENDED)
parser->m_processor = epilogProcessor;
else
return epilogProcessor(parser, next, end, nextPtr);
}
break;
case XML_TOK_END_TAG:
if (parser->m_tagLevel == startTagLevel)
return XML_ERROR_ASYNC_ENTITY;
else {
int len;
const char *rawName;
TAG *tag = parser->m_tagStack;
parser->m_tagStack = tag->parent;
tag->parent = parser->m_freeTagList;
parser->m_freeTagList = tag;
rawName = s + enc->minBytesPerChar * 2;
len = XmlNameLength(enc, rawName);
if (len != tag->rawNameLength
|| memcmp(tag->rawName, rawName, len) != 0) {
*eventPP = rawName;
return XML_ERROR_TAG_MISMATCH;
}
--parser->m_tagLevel;
if (parser->m_endElementHandler) {
const XML_Char *localPart;
const XML_Char *prefix;
XML_Char *uri;
localPart = tag->name.localPart;
if (parser->m_ns && localPart) {




uri = (XML_Char *)tag->name.str + tag->name.uriLen;

while (*localPart)
*uri++ = *localPart++;
prefix = (XML_Char *)tag->name.prefix;
if (parser->m_ns_triplets && prefix) {
*uri++ = parser->m_namespaceSeparator;
while (*prefix)
*uri++ = *prefix++;
}
*uri = XML_T('\0');
}
parser->m_endElementHandler(parser->m_handlerArg, tag->name.str);
} else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
while (tag->bindings) {
BINDING *b = tag->bindings;
if (parser->m_endNamespaceDeclHandler)
parser->m_endNamespaceDeclHandler(parser->m_handlerArg,
b->prefix->name);
tag->bindings = tag->bindings->nextTagBinding;
b->nextTagBinding = parser->m_freeBindingList;
parser->m_freeBindingList = b;
b->prefix->binding = b->prevPrefixBinding;
}
if ((parser->m_tagLevel == 0)
&& (parser->m_parsingStatus.parsing != XML_FINISHED)) {
if (parser->m_parsingStatus.parsing == XML_SUSPENDED)
parser->m_processor = epilogProcessor;
else
return epilogProcessor(parser, next, end, nextPtr);
}
}
break;
case XML_TOK_CHAR_REF: {
int n = XmlCharRefNumber(enc, s);
if (n < 0)
return XML_ERROR_BAD_CHAR_REF;
if (parser->m_characterDataHandler) {
XML_Char buf[XML_ENCODE_MAX];
parser->m_characterDataHandler(parser->m_handlerArg, buf,
XmlEncode(n, (ICHAR *)buf));
} else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
} break;
case XML_TOK_XML_DECL:
return XML_ERROR_MISPLACED_XML_PI;
case XML_TOK_DATA_NEWLINE:
if (parser->m_characterDataHandler) {
XML_Char c = 0xA;
parser->m_characterDataHandler(parser->m_handlerArg, &c, 1);
} else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
break;
case XML_TOK_CDATA_SECT_OPEN: {
enum XML_Error result;
if (parser->m_startCdataSectionHandler)
parser->m_startCdataSectionHandler(parser->m_handlerArg);













else if (0 && parser->m_characterDataHandler)
parser->m_characterDataHandler(parser->m_handlerArg, parser->m_dataBuf,
0);

else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
result = doCdataSection(parser, enc, &next, end, nextPtr, haveMore);
if (result != XML_ERROR_NONE)
return result;
else if (! next) {
parser->m_processor = cdataSectionProcessor;
return result;
}
} break;
case XML_TOK_TRAILING_RSQB:
if (haveMore) {
*nextPtr = s;
return XML_ERROR_NONE;
}
if (parser->m_characterDataHandler) {
if (MUST_CONVERT(enc, s)) {
ICHAR *dataPtr = (ICHAR *)parser->m_dataBuf;
XmlConvert(enc, &s, end, &dataPtr, (ICHAR *)parser->m_dataBufEnd);
parser->m_characterDataHandler(
parser->m_handlerArg, parser->m_dataBuf,
(int)(dataPtr - (ICHAR *)parser->m_dataBuf));
} else
parser->m_characterDataHandler(
parser->m_handlerArg, (XML_Char *)s,
(int)((XML_Char *)end - (XML_Char *)s));
} else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, end);



if (startTagLevel == 0) {
*eventPP = end;
return XML_ERROR_NO_ELEMENTS;
}
if (parser->m_tagLevel != startTagLevel) {
*eventPP = end;
return XML_ERROR_ASYNC_ENTITY;
}
*nextPtr = end;
return XML_ERROR_NONE;
case XML_TOK_DATA_CHARS: {
XML_CharacterDataHandler charDataHandler = parser->m_characterDataHandler;
if (charDataHandler) {
if (MUST_CONVERT(enc, s)) {
for (;;) {
ICHAR *dataPtr = (ICHAR *)parser->m_dataBuf;
const enum XML_Convert_Result convert_res = XmlConvert(
enc, &s, next, &dataPtr, (ICHAR *)parser->m_dataBufEnd);
*eventEndPP = s;
charDataHandler(parser->m_handlerArg, parser->m_dataBuf,
(int)(dataPtr - (ICHAR *)parser->m_dataBuf));
if ((convert_res == XML_CONVERT_COMPLETED)
|| (convert_res == XML_CONVERT_INPUT_INCOMPLETE))
break;
*eventPP = s;
}
} else
charDataHandler(parser->m_handlerArg, (XML_Char *)s,
(int)((XML_Char *)next - (XML_Char *)s));
} else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
} break;
case XML_TOK_PI:
if (! reportProcessingInstruction(parser, enc, s, next))
return XML_ERROR_NO_MEMORY;
break;
case XML_TOK_COMMENT:
if (! reportComment(parser, enc, s, next))
return XML_ERROR_NO_MEMORY;
break;
default:







if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
break;

}
*eventPP = s = next;
switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:
*nextPtr = next;
return XML_ERROR_NONE;
case XML_FINISHED:
return XML_ERROR_ABORTED;
default:;
}
}

}





static void
freeBindings(XML_Parser parser, BINDING *bindings) {
while (bindings) {
BINDING *b = bindings;




if (parser->m_endNamespaceDeclHandler)
parser->m_endNamespaceDeclHandler(parser->m_handlerArg, b->prefix->name);

bindings = bindings->nextTagBinding;
b->nextTagBinding = parser->m_freeBindingList;
parser->m_freeBindingList = b;
b->prefix->binding = b->prevPrefixBinding;
}
}











static enum XML_Error
storeAtts(XML_Parser parser, const ENCODING *enc, const char *attStr,
TAG_NAME *tagNamePtr, BINDING **bindingsPtr) {
DTD *const dtd = parser->m_dtd;
ELEMENT_TYPE *elementType;
int nDefaultAtts;
const XML_Char **appAtts;
int attIndex = 0;
int prefixLen;
int i;
int n;
XML_Char *uri;
int nPrefixes = 0;
BINDING *binding;
const XML_Char *localPart;


elementType
= (ELEMENT_TYPE *)lookup(parser, &dtd->elementTypes, tagNamePtr->str, 0);
if (! elementType) {
const XML_Char *name = poolCopyString(&dtd->pool, tagNamePtr->str);
if (! name)
return XML_ERROR_NO_MEMORY;
elementType = (ELEMENT_TYPE *)lookup(parser, &dtd->elementTypes, name,
sizeof(ELEMENT_TYPE));
if (! elementType)
return XML_ERROR_NO_MEMORY;
if (parser->m_ns && ! setElementTypePrefix(parser, elementType))
return XML_ERROR_NO_MEMORY;
}
nDefaultAtts = elementType->nDefaultAtts;


n = XmlGetAttributes(enc, attStr, parser->m_attsSize, parser->m_atts);
if (n + nDefaultAtts > parser->m_attsSize) {
int oldAttsSize = parser->m_attsSize;
ATTRIBUTE *temp;
#if defined(XML_ATTR_INFO)
XML_AttrInfo *temp2;
#endif
parser->m_attsSize = n + nDefaultAtts + INIT_ATTS_SIZE;
temp = (ATTRIBUTE *)REALLOC(parser, (void *)parser->m_atts,
parser->m_attsSize * sizeof(ATTRIBUTE));
if (temp == NULL) {
parser->m_attsSize = oldAttsSize;
return XML_ERROR_NO_MEMORY;
}
parser->m_atts = temp;
#if defined(XML_ATTR_INFO)
temp2 = (XML_AttrInfo *)REALLOC(parser, (void *)parser->m_attInfo,
parser->m_attsSize * sizeof(XML_AttrInfo));
if (temp2 == NULL) {
parser->m_attsSize = oldAttsSize;
return XML_ERROR_NO_MEMORY;
}
parser->m_attInfo = temp2;
#endif
if (n > oldAttsSize)
XmlGetAttributes(enc, attStr, n, parser->m_atts);
}

appAtts = (const XML_Char **)parser->m_atts;
for (i = 0; i < n; i++) {
ATTRIBUTE *currAtt = &parser->m_atts[i];
#if defined(XML_ATTR_INFO)
XML_AttrInfo *currAttInfo = &parser->m_attInfo[i];
#endif

ATTRIBUTE_ID *attId
= getAttributeId(parser, enc, currAtt->name,
currAtt->name + XmlNameLength(enc, currAtt->name));
if (! attId)
return XML_ERROR_NO_MEMORY;
#if defined(XML_ATTR_INFO)
currAttInfo->nameStart
= parser->m_parseEndByteIndex - (parser->m_parseEndPtr - currAtt->name);
currAttInfo->nameEnd
= currAttInfo->nameStart + XmlNameLength(enc, currAtt->name);
currAttInfo->valueStart = parser->m_parseEndByteIndex
- (parser->m_parseEndPtr - currAtt->valuePtr);
currAttInfo->valueEnd = parser->m_parseEndByteIndex
- (parser->m_parseEndPtr - currAtt->valueEnd);
#endif




if ((attId->name)[-1]) {
if (enc == parser->m_encoding)
parser->m_eventPtr = parser->m_atts[i].name;
return XML_ERROR_DUPLICATE_ATTRIBUTE;
}
(attId->name)[-1] = 1;
appAtts[attIndex++] = attId->name;
if (! parser->m_atts[i].normalized) {
enum XML_Error result;
XML_Bool isCdata = XML_TRUE;


if (attId->maybeTokenized) {
int j;
for (j = 0; j < nDefaultAtts; j++) {
if (attId == elementType->defaultAtts[j].id) {
isCdata = elementType->defaultAtts[j].isCdata;
break;
}
}
}


result = storeAttributeValue(
parser, enc, isCdata, parser->m_atts[i].valuePtr,
parser->m_atts[i].valueEnd, &parser->m_tempPool);
if (result)
return result;
appAtts[attIndex] = poolStart(&parser->m_tempPool);
poolFinish(&parser->m_tempPool);
} else {

appAtts[attIndex] = poolStoreString(&parser->m_tempPool, enc,
parser->m_atts[i].valuePtr,
parser->m_atts[i].valueEnd);
if (appAtts[attIndex] == 0)
return XML_ERROR_NO_MEMORY;
poolFinish(&parser->m_tempPool);
}

if (attId->prefix) {
if (attId->xmlns) {

enum XML_Error result = addBinding(parser, attId->prefix, attId,
appAtts[attIndex], bindingsPtr);
if (result)
return result;
--attIndex;
} else {

attIndex++;
nPrefixes++;
(attId->name)[-1] = 2;
}
} else
attIndex++;
}


parser->m_nSpecifiedAtts = attIndex;
if (elementType->idAtt && (elementType->idAtt->name)[-1]) {
for (i = 0; i < attIndex; i += 2)
if (appAtts[i] == elementType->idAtt->name) {
parser->m_idAttIndex = i;
break;
}
} else
parser->m_idAttIndex = -1;


for (i = 0; i < nDefaultAtts; i++) {
const DEFAULT_ATTRIBUTE *da = elementType->defaultAtts + i;
if (! (da->id->name)[-1] && da->value) {
if (da->id->prefix) {
if (da->id->xmlns) {
enum XML_Error result = addBinding(parser, da->id->prefix, da->id,
da->value, bindingsPtr);
if (result)
return result;
} else {
(da->id->name)[-1] = 2;
nPrefixes++;
appAtts[attIndex++] = da->id->name;
appAtts[attIndex++] = da->value;
}
} else {
(da->id->name)[-1] = 1;
appAtts[attIndex++] = da->id->name;
appAtts[attIndex++] = da->value;
}
}
}
appAtts[attIndex] = 0;



i = 0;
if (nPrefixes) {
int j;
unsigned long version = parser->m_nsAttsVersion;
int nsAttsSize = (int)1 << parser->m_nsAttsPower;
unsigned char oldNsAttsPower = parser->m_nsAttsPower;

if ((nPrefixes << 1)
>> parser->m_nsAttsPower) {
NS_ATT *temp;

while (nPrefixes >> parser->m_nsAttsPower++)
;
if (parser->m_nsAttsPower < 3)
parser->m_nsAttsPower = 3;
nsAttsSize = (int)1 << parser->m_nsAttsPower;
temp = (NS_ATT *)REALLOC(parser, parser->m_nsAtts,
nsAttsSize * sizeof(NS_ATT));
if (! temp) {

parser->m_nsAttsPower = oldNsAttsPower;
return XML_ERROR_NO_MEMORY;
}
parser->m_nsAtts = temp;
version = 0;
}

if (! version) {
version = INIT_ATTS_VERSION;
for (j = nsAttsSize; j != 0;)
parser->m_nsAtts[--j].version = version;
}
parser->m_nsAttsVersion = --version;


for (; i < attIndex; i += 2) {
const XML_Char *s = appAtts[i];
if (s[-1] == 2) {
ATTRIBUTE_ID *id;
const BINDING *b;
unsigned long uriHash;
struct siphash sip_state;
struct sipkey sip_key;

copy_salt_to_sipkey(parser, &sip_key);
sip24_init(&sip_state, &sip_key);

((XML_Char *)s)[-1] = 0;
id = (ATTRIBUTE_ID *)lookup(parser, &dtd->attributeIds, s, 0);
if (! id || ! id->prefix) {














return XML_ERROR_NO_MEMORY;
}
b = id->prefix->binding;
if (! b)
return XML_ERROR_UNBOUND_PREFIX;

for (j = 0; j < b->uriLen; j++) {
const XML_Char c = b->uri[j];
if (! poolAppendChar(&parser->m_tempPool, c))
return XML_ERROR_NO_MEMORY;
}

sip24_update(&sip_state, b->uri, b->uriLen * sizeof(XML_Char));

while (*s++ != XML_T(ASCII_COLON))
;

sip24_update(&sip_state, s, keylen(s) * sizeof(XML_Char));

do {
if (! poolAppendChar(&parser->m_tempPool, *s))
return XML_ERROR_NO_MEMORY;
} while (*s++);

uriHash = (unsigned long)sip24_final(&sip_state);

{


unsigned char step = 0;
unsigned long mask = nsAttsSize - 1;
j = uriHash & mask;
while (parser->m_nsAtts[j].version == version) {

if (uriHash == parser->m_nsAtts[j].hash) {
const XML_Char *s1 = poolStart(&parser->m_tempPool);
const XML_Char *s2 = parser->m_nsAtts[j].uriName;

for (; *s1 == *s2 && *s1 != 0; s1++, s2++)
;
if (*s1 == 0)
return XML_ERROR_DUPLICATE_ATTRIBUTE;
}
if (! step)
step = PROBE_STEP(uriHash, mask, parser->m_nsAttsPower);
j < step ? (j += nsAttsSize - step) : (j -= step);
}
}

if (parser->m_ns_triplets) {
parser->m_tempPool.ptr[-1] = parser->m_namespaceSeparator;
s = b->prefix->name;
do {
if (! poolAppendChar(&parser->m_tempPool, *s))
return XML_ERROR_NO_MEMORY;
} while (*s++);
}


s = poolStart(&parser->m_tempPool);
poolFinish(&parser->m_tempPool);
appAtts[i] = s;


parser->m_nsAtts[j].version = version;
parser->m_nsAtts[j].hash = uriHash;
parser->m_nsAtts[j].uriName = s;

if (! --nPrefixes) {
i += 2;
break;
}
} else
((XML_Char *)s)[-1] = 0;
}
}

for (; i < attIndex; i += 2)
((XML_Char *)(appAtts[i]))[-1] = 0;
for (binding = *bindingsPtr; binding; binding = binding->nextTagBinding)
binding->attId->name[-1] = 0;

if (! parser->m_ns)
return XML_ERROR_NONE;


if (elementType->prefix) {
binding = elementType->prefix->binding;
if (! binding)
return XML_ERROR_UNBOUND_PREFIX;
localPart = tagNamePtr->str;
while (*localPart++ != XML_T(ASCII_COLON))
;
} else if (dtd->defaultPrefix.binding) {
binding = dtd->defaultPrefix.binding;
localPart = tagNamePtr->str;
} else
return XML_ERROR_NONE;
prefixLen = 0;
if (parser->m_ns_triplets && binding->prefix->name) {
for (; binding->prefix->name[prefixLen++];)
;
}
tagNamePtr->localPart = localPart;
tagNamePtr->uriLen = binding->uriLen;
tagNamePtr->prefix = binding->prefix->name;
tagNamePtr->prefixLen = prefixLen;
for (i = 0; localPart[i++];)
;
n = i + binding->uriLen + prefixLen;
if (n > binding->uriAlloc) {
TAG *p;
uri = (XML_Char *)MALLOC(parser, (n + EXPAND_SPARE) * sizeof(XML_Char));
if (! uri)
return XML_ERROR_NO_MEMORY;
binding->uriAlloc = n + EXPAND_SPARE;
memcpy(uri, binding->uri, binding->uriLen * sizeof(XML_Char));
for (p = parser->m_tagStack; p; p = p->parent)
if (p->name.str == binding->uri)
p->name.str = uri;
FREE(parser, binding->uri);
binding->uri = uri;
}

uri = binding->uri + binding->uriLen;
memcpy(uri, localPart, i * sizeof(XML_Char));

if (prefixLen) {
uri += i - 1;
*uri = parser->m_namespaceSeparator;
memcpy(uri + 1, binding->prefix->name, prefixLen * sizeof(XML_Char));
}
tagNamePtr->str = binding->uri;
return XML_ERROR_NONE;
}




static enum XML_Error
addBinding(XML_Parser parser, PREFIX *prefix, const ATTRIBUTE_ID *attId,
const XML_Char *uri, BINDING **bindingsPtr) {
static const XML_Char xmlNamespace[]
= {ASCII_h, ASCII_t, ASCII_t, ASCII_p, ASCII_COLON,
ASCII_SLASH, ASCII_SLASH, ASCII_w, ASCII_w, ASCII_w,
ASCII_PERIOD, ASCII_w, ASCII_3, ASCII_PERIOD, ASCII_o,
ASCII_r, ASCII_g, ASCII_SLASH, ASCII_X, ASCII_M,
ASCII_L, ASCII_SLASH, ASCII_1, ASCII_9, ASCII_9,
ASCII_8, ASCII_SLASH, ASCII_n, ASCII_a, ASCII_m,
ASCII_e, ASCII_s, ASCII_p, ASCII_a, ASCII_c,
ASCII_e, '\0'};
static const int xmlLen = (int)sizeof(xmlNamespace) / sizeof(XML_Char) - 1;
static const XML_Char xmlnsNamespace[]
= {ASCII_h, ASCII_t, ASCII_t, ASCII_p, ASCII_COLON, ASCII_SLASH,
ASCII_SLASH, ASCII_w, ASCII_w, ASCII_w, ASCII_PERIOD, ASCII_w,
ASCII_3, ASCII_PERIOD, ASCII_o, ASCII_r, ASCII_g, ASCII_SLASH,
ASCII_2, ASCII_0, ASCII_0, ASCII_0, ASCII_SLASH, ASCII_x,
ASCII_m, ASCII_l, ASCII_n, ASCII_s, ASCII_SLASH, '\0'};
static const int xmlnsLen
= (int)sizeof(xmlnsNamespace) / sizeof(XML_Char) - 1;

XML_Bool mustBeXML = XML_FALSE;
XML_Bool isXML = XML_TRUE;
XML_Bool isXMLNS = XML_TRUE;

BINDING *b;
int len;


if (*uri == XML_T('\0') && prefix->name)
return XML_ERROR_UNDECLARING_PREFIX;

if (prefix->name && prefix->name[0] == XML_T(ASCII_x)
&& prefix->name[1] == XML_T(ASCII_m)
&& prefix->name[2] == XML_T(ASCII_l)) {

if (prefix->name[3] == XML_T(ASCII_n) && prefix->name[4] == XML_T(ASCII_s)
&& prefix->name[5] == XML_T('\0'))
return XML_ERROR_RESERVED_PREFIX_XMLNS;

if (prefix->name[3] == XML_T('\0'))
mustBeXML = XML_TRUE;
}

for (len = 0; uri[len]; len++) {
if (isXML && (len > xmlLen || uri[len] != xmlNamespace[len]))
isXML = XML_FALSE;

if (! mustBeXML && isXMLNS
&& (len > xmlnsLen || uri[len] != xmlnsNamespace[len]))
isXMLNS = XML_FALSE;
}
isXML = isXML && len == xmlLen;
isXMLNS = isXMLNS && len == xmlnsLen;

if (mustBeXML != isXML)
return mustBeXML ? XML_ERROR_RESERVED_PREFIX_XML
: XML_ERROR_RESERVED_NAMESPACE_URI;

if (isXMLNS)
return XML_ERROR_RESERVED_NAMESPACE_URI;

if (parser->m_namespaceSeparator)
len++;
if (parser->m_freeBindingList) {
b = parser->m_freeBindingList;
if (len > b->uriAlloc) {
XML_Char *temp = (XML_Char *)REALLOC(
parser, b->uri, sizeof(XML_Char) * (len + EXPAND_SPARE));
if (temp == NULL)
return XML_ERROR_NO_MEMORY;
b->uri = temp;
b->uriAlloc = len + EXPAND_SPARE;
}
parser->m_freeBindingList = b->nextTagBinding;
} else {
b = (BINDING *)MALLOC(parser, sizeof(BINDING));
if (! b)
return XML_ERROR_NO_MEMORY;
b->uri
= (XML_Char *)MALLOC(parser, sizeof(XML_Char) * (len + EXPAND_SPARE));
if (! b->uri) {
FREE(parser, b);
return XML_ERROR_NO_MEMORY;
}
b->uriAlloc = len + EXPAND_SPARE;
}
b->uriLen = len;
memcpy(b->uri, uri, len * sizeof(XML_Char));
if (parser->m_namespaceSeparator)
b->uri[len - 1] = parser->m_namespaceSeparator;
b->prefix = prefix;
b->attId = attId;
b->prevPrefixBinding = prefix->binding;

if (*uri == XML_T('\0') && prefix == &parser->m_dtd->defaultPrefix)
prefix->binding = NULL;
else
prefix->binding = b;
b->nextTagBinding = *bindingsPtr;
*bindingsPtr = b;

if (attId && parser->m_startNamespaceDeclHandler)
parser->m_startNamespaceDeclHandler(parser->m_handlerArg, prefix->name,
prefix->binding ? uri : 0);
return XML_ERROR_NONE;
}




static enum XML_Error PTRCALL
cdataSectionProcessor(XML_Parser parser, const char *start, const char *end,
const char **endPtr) {
enum XML_Error result
= doCdataSection(parser, parser->m_encoding, &start, end, endPtr,
(XML_Bool)! parser->m_parsingStatus.finalBuffer);
if (result != XML_ERROR_NONE)
return result;
if (start) {
if (parser->m_parentParser) {
parser->m_processor = externalEntityContentProcessor;
return externalEntityContentProcessor(parser, start, end, endPtr);
} else {
parser->m_processor = contentProcessor;
return contentProcessor(parser, start, end, endPtr);
}
}
return result;
}




static enum XML_Error
doCdataSection(XML_Parser parser, const ENCODING *enc, const char **startPtr,
const char *end, const char **nextPtr, XML_Bool haveMore) {
const char *s = *startPtr;
const char **eventPP;
const char **eventEndPP;
if (enc == parser->m_encoding) {
eventPP = &parser->m_eventPtr;
*eventPP = s;
eventEndPP = &parser->m_eventEndPtr;
} else {
eventPP = &(parser->m_openInternalEntities->internalEventPtr);
eventEndPP = &(parser->m_openInternalEntities->internalEventEndPtr);
}
*eventPP = s;
*startPtr = NULL;

for (;;) {
const char *next;
int tok = XmlCdataSectionTok(enc, s, end, &next);
*eventEndPP = next;
switch (tok) {
case XML_TOK_CDATA_SECT_CLOSE:
if (parser->m_endCdataSectionHandler)
parser->m_endCdataSectionHandler(parser->m_handlerArg);


else if (0 && parser->m_characterDataHandler)
parser->m_characterDataHandler(parser->m_handlerArg, parser->m_dataBuf,
0);

else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
*startPtr = next;
*nextPtr = next;
if (parser->m_parsingStatus.parsing == XML_FINISHED)
return XML_ERROR_ABORTED;
else
return XML_ERROR_NONE;
case XML_TOK_DATA_NEWLINE:
if (parser->m_characterDataHandler) {
XML_Char c = 0xA;
parser->m_characterDataHandler(parser->m_handlerArg, &c, 1);
} else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
break;
case XML_TOK_DATA_CHARS: {
XML_CharacterDataHandler charDataHandler = parser->m_characterDataHandler;
if (charDataHandler) {
if (MUST_CONVERT(enc, s)) {
for (;;) {
ICHAR *dataPtr = (ICHAR *)parser->m_dataBuf;
const enum XML_Convert_Result convert_res = XmlConvert(
enc, &s, next, &dataPtr, (ICHAR *)parser->m_dataBufEnd);
*eventEndPP = next;
charDataHandler(parser->m_handlerArg, parser->m_dataBuf,
(int)(dataPtr - (ICHAR *)parser->m_dataBuf));
if ((convert_res == XML_CONVERT_COMPLETED)
|| (convert_res == XML_CONVERT_INPUT_INCOMPLETE))
break;
*eventPP = s;
}
} else
charDataHandler(parser->m_handlerArg, (XML_Char *)s,
(int)((XML_Char *)next - (XML_Char *)s));
} else if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
} break;
case XML_TOK_INVALID:
*eventPP = next;
return XML_ERROR_INVALID_TOKEN;
case XML_TOK_PARTIAL_CHAR:
if (haveMore) {
*nextPtr = s;
return XML_ERROR_NONE;
}
return XML_ERROR_PARTIAL_CHAR;
case XML_TOK_PARTIAL:
case XML_TOK_NONE:
if (haveMore) {
*nextPtr = s;
return XML_ERROR_NONE;
}
return XML_ERROR_UNCLOSED_CDATA_SECTION;
default:







*eventPP = next;
return XML_ERROR_UNEXPECTED_STATE;

}

*eventPP = s = next;
switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:
*nextPtr = next;
return XML_ERROR_NONE;
case XML_FINISHED:
return XML_ERROR_ABORTED;
default:;
}
}

}

#if defined(XML_DTD)




static enum XML_Error PTRCALL
ignoreSectionProcessor(XML_Parser parser, const char *start, const char *end,
const char **endPtr) {
enum XML_Error result
= doIgnoreSection(parser, parser->m_encoding, &start, end, endPtr,
(XML_Bool)! parser->m_parsingStatus.finalBuffer);
if (result != XML_ERROR_NONE)
return result;
if (start) {
parser->m_processor = prologProcessor;
return prologProcessor(parser, start, end, endPtr);
}
return result;
}




static enum XML_Error
doIgnoreSection(XML_Parser parser, const ENCODING *enc, const char **startPtr,
const char *end, const char **nextPtr, XML_Bool haveMore) {
const char *next;
int tok;
const char *s = *startPtr;
const char **eventPP;
const char **eventEndPP;
if (enc == parser->m_encoding) {
eventPP = &parser->m_eventPtr;
*eventPP = s;
eventEndPP = &parser->m_eventEndPtr;
} else {











eventPP = &(parser->m_openInternalEntities->internalEventPtr);
eventEndPP = &(parser->m_openInternalEntities->internalEventEndPtr);

}
*eventPP = s;
*startPtr = NULL;
tok = XmlIgnoreSectionTok(enc, s, end, &next);
*eventEndPP = next;
switch (tok) {
case XML_TOK_IGNORE_SECT:
if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
*startPtr = next;
*nextPtr = next;
if (parser->m_parsingStatus.parsing == XML_FINISHED)
return XML_ERROR_ABORTED;
else
return XML_ERROR_NONE;
case XML_TOK_INVALID:
*eventPP = next;
return XML_ERROR_INVALID_TOKEN;
case XML_TOK_PARTIAL_CHAR:
if (haveMore) {
*nextPtr = s;
return XML_ERROR_NONE;
}
return XML_ERROR_PARTIAL_CHAR;
case XML_TOK_PARTIAL:
case XML_TOK_NONE:
if (haveMore) {
*nextPtr = s;
return XML_ERROR_NONE;
}
return XML_ERROR_SYNTAX;
default:







*eventPP = next;
return XML_ERROR_UNEXPECTED_STATE;

}

}

#endif

static enum XML_Error
initializeEncoding(XML_Parser parser) {
const char *s;
#if defined(XML_UNICODE)
char encodingBuf[128];

if (! parser->m_protocolEncodingName)
s = NULL;
else {
int i;
for (i = 0; parser->m_protocolEncodingName[i]; i++) {
if (i == sizeof(encodingBuf) - 1
|| (parser->m_protocolEncodingName[i] & ~0x7f) != 0) {
encodingBuf[0] = '\0';
break;
}
encodingBuf[i] = (char)parser->m_protocolEncodingName[i];
}
encodingBuf[i] = '\0';
s = encodingBuf;
}
#else
s = parser->m_protocolEncodingName;
#endif
if ((parser->m_ns ? XmlInitEncodingNS : XmlInitEncoding)(
&parser->m_initEncoding, &parser->m_encoding, s))
return XML_ERROR_NONE;
return handleUnknownEncoding(parser, parser->m_protocolEncodingName);
}

static enum XML_Error
processXmlDecl(XML_Parser parser, int isGeneralTextEntity, const char *s,
const char *next) {
const char *encodingName = NULL;
const XML_Char *storedEncName = NULL;
const ENCODING *newEncoding = NULL;
const char *version = NULL;
const char *versionend;
const XML_Char *storedversion = NULL;
int standalone = -1;
if (! (parser->m_ns ? XmlParseXmlDeclNS : XmlParseXmlDecl)(
isGeneralTextEntity, parser->m_encoding, s, next, &parser->m_eventPtr,
&version, &versionend, &encodingName, &newEncoding, &standalone)) {
if (isGeneralTextEntity)
return XML_ERROR_TEXT_DECL;
else
return XML_ERROR_XML_DECL;
}
if (! isGeneralTextEntity && standalone == 1) {
parser->m_dtd->standalone = XML_TRUE;
#if defined(XML_DTD)
if (parser->m_paramEntityParsing
== XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE)
parser->m_paramEntityParsing = XML_PARAM_ENTITY_PARSING_NEVER;
#endif
}
if (parser->m_xmlDeclHandler) {
if (encodingName != NULL) {
storedEncName = poolStoreString(
&parser->m_temp2Pool, parser->m_encoding, encodingName,
encodingName + XmlNameLength(parser->m_encoding, encodingName));
if (! storedEncName)
return XML_ERROR_NO_MEMORY;
poolFinish(&parser->m_temp2Pool);
}
if (version) {
storedversion
= poolStoreString(&parser->m_temp2Pool, parser->m_encoding, version,
versionend - parser->m_encoding->minBytesPerChar);
if (! storedversion)
return XML_ERROR_NO_MEMORY;
}
parser->m_xmlDeclHandler(parser->m_handlerArg, storedversion, storedEncName,
standalone);
} else if (parser->m_defaultHandler)
reportDefault(parser, parser->m_encoding, s, next);
if (parser->m_protocolEncodingName == NULL) {
if (newEncoding) {





if (newEncoding->minBytesPerChar != parser->m_encoding->minBytesPerChar
|| (newEncoding->minBytesPerChar == 2
&& newEncoding != parser->m_encoding)) {
parser->m_eventPtr = encodingName;
return XML_ERROR_INCORRECT_ENCODING;
}
parser->m_encoding = newEncoding;
} else if (encodingName) {
enum XML_Error result;
if (! storedEncName) {
storedEncName = poolStoreString(
&parser->m_temp2Pool, parser->m_encoding, encodingName,
encodingName + XmlNameLength(parser->m_encoding, encodingName));
if (! storedEncName)
return XML_ERROR_NO_MEMORY;
}
result = handleUnknownEncoding(parser, storedEncName);
poolClear(&parser->m_temp2Pool);
if (result == XML_ERROR_UNKNOWN_ENCODING)
parser->m_eventPtr = encodingName;
return result;
}
}

if (storedEncName || storedversion)
poolClear(&parser->m_temp2Pool);

return XML_ERROR_NONE;
}

static enum XML_Error
handleUnknownEncoding(XML_Parser parser, const XML_Char *encodingName) {
if (parser->m_unknownEncodingHandler) {
XML_Encoding info;
int i;
for (i = 0; i < 256; i++)
info.map[i] = -1;
info.convert = NULL;
info.data = NULL;
info.release = NULL;
if (parser->m_unknownEncodingHandler(parser->m_unknownEncodingHandlerData,
encodingName, &info)) {
ENCODING *enc;
parser->m_unknownEncodingMem = MALLOC(parser, XmlSizeOfUnknownEncoding());
if (! parser->m_unknownEncodingMem) {
if (info.release)
info.release(info.data);
return XML_ERROR_NO_MEMORY;
}
enc = (parser->m_ns ? XmlInitUnknownEncodingNS : XmlInitUnknownEncoding)(
parser->m_unknownEncodingMem, info.map, info.convert, info.data);
if (enc) {
parser->m_unknownEncodingData = info.data;
parser->m_unknownEncodingRelease = info.release;
parser->m_encoding = enc;
return XML_ERROR_NONE;
}
}
if (info.release != NULL)
info.release(info.data);
}
return XML_ERROR_UNKNOWN_ENCODING;
}

static enum XML_Error PTRCALL
prologInitProcessor(XML_Parser parser, const char *s, const char *end,
const char **nextPtr) {
enum XML_Error result = initializeEncoding(parser);
if (result != XML_ERROR_NONE)
return result;
parser->m_processor = prologProcessor;
return prologProcessor(parser, s, end, nextPtr);
}

#if defined(XML_DTD)

static enum XML_Error PTRCALL
externalParEntInitProcessor(XML_Parser parser, const char *s, const char *end,
const char **nextPtr) {
enum XML_Error result = initializeEncoding(parser);
if (result != XML_ERROR_NONE)
return result;



parser->m_dtd->paramEntityRead = XML_TRUE;

if (parser->m_prologState.inEntityValue) {
parser->m_processor = entityValueInitProcessor;
return entityValueInitProcessor(parser, s, end, nextPtr);
} else {
parser->m_processor = externalParEntProcessor;
return externalParEntProcessor(parser, s, end, nextPtr);
}
}

static enum XML_Error PTRCALL
entityValueInitProcessor(XML_Parser parser, const char *s, const char *end,
const char **nextPtr) {
int tok;
const char *start = s;
const char *next = start;
parser->m_eventPtr = start;

for (;;) {
tok = XmlPrologTok(parser->m_encoding, start, end, &next);
parser->m_eventEndPtr = next;
if (tok <= 0) {
if (! parser->m_parsingStatus.finalBuffer && tok != XML_TOK_INVALID) {
*nextPtr = s;
return XML_ERROR_NONE;
}
switch (tok) {
case XML_TOK_INVALID:
return XML_ERROR_INVALID_TOKEN;
case XML_TOK_PARTIAL:
return XML_ERROR_UNCLOSED_TOKEN;
case XML_TOK_PARTIAL_CHAR:
return XML_ERROR_PARTIAL_CHAR;
case XML_TOK_NONE:
default:
break;
}

return storeEntityValue(parser, parser->m_encoding, s, end);
} else if (tok == XML_TOK_XML_DECL) {
enum XML_Error result;
result = processXmlDecl(parser, 0, start, next);
if (result != XML_ERROR_NONE)
return result;





if (parser->m_parsingStatus.parsing == XML_FINISHED)
return XML_ERROR_ABORTED;
*nextPtr = next;

parser->m_processor = entityValueProcessor;
return entityValueProcessor(parser, next, end, nextPtr);
}







else if (tok == XML_TOK_BOM && next == end
&& ! parser->m_parsingStatus.finalBuffer) {
*nextPtr = next;
return XML_ERROR_NONE;
}




else if (tok == XML_TOK_INSTANCE_START) {
*nextPtr = next;
return XML_ERROR_SYNTAX;
}
start = next;
parser->m_eventPtr = start;
}
}

static enum XML_Error PTRCALL
externalParEntProcessor(XML_Parser parser, const char *s, const char *end,
const char **nextPtr) {
const char *next = s;
int tok;

tok = XmlPrologTok(parser->m_encoding, s, end, &next);
if (tok <= 0) {
if (! parser->m_parsingStatus.finalBuffer && tok != XML_TOK_INVALID) {
*nextPtr = s;
return XML_ERROR_NONE;
}
switch (tok) {
case XML_TOK_INVALID:
return XML_ERROR_INVALID_TOKEN;
case XML_TOK_PARTIAL:
return XML_ERROR_UNCLOSED_TOKEN;
case XML_TOK_PARTIAL_CHAR:
return XML_ERROR_PARTIAL_CHAR;
case XML_TOK_NONE:
default:
break;
}
}




else if (tok == XML_TOK_BOM) {
s = next;
tok = XmlPrologTok(parser->m_encoding, s, end, &next);
}

parser->m_processor = prologProcessor;
return doProlog(parser, parser->m_encoding, s, end, tok, next, nextPtr,
(XML_Bool)! parser->m_parsingStatus.finalBuffer, XML_TRUE);
}

static enum XML_Error PTRCALL
entityValueProcessor(XML_Parser parser, const char *s, const char *end,
const char **nextPtr) {
const char *start = s;
const char *next = s;
const ENCODING *enc = parser->m_encoding;
int tok;

for (;;) {
tok = XmlPrologTok(enc, start, end, &next);
if (tok <= 0) {
if (! parser->m_parsingStatus.finalBuffer && tok != XML_TOK_INVALID) {
*nextPtr = s;
return XML_ERROR_NONE;
}
switch (tok) {
case XML_TOK_INVALID:
return XML_ERROR_INVALID_TOKEN;
case XML_TOK_PARTIAL:
return XML_ERROR_UNCLOSED_TOKEN;
case XML_TOK_PARTIAL_CHAR:
return XML_ERROR_PARTIAL_CHAR;
case XML_TOK_NONE:
default:
break;
}

return storeEntityValue(parser, enc, s, end);
}
start = next;
}
}

#endif

static enum XML_Error PTRCALL
prologProcessor(XML_Parser parser, const char *s, const char *end,
const char **nextPtr) {
const char *next = s;
int tok = XmlPrologTok(parser->m_encoding, s, end, &next);
return doProlog(parser, parser->m_encoding, s, end, tok, next, nextPtr,
(XML_Bool)! parser->m_parsingStatus.finalBuffer, XML_TRUE);
}

static enum XML_Error
doProlog(XML_Parser parser, const ENCODING *enc, const char *s, const char *end,
int tok, const char *next, const char **nextPtr, XML_Bool haveMore,
XML_Bool allowClosingDoctype) {
#if defined(XML_DTD)
static const XML_Char externalSubsetName[] = {ASCII_HASH, '\0'};
#endif
static const XML_Char atypeCDATA[]
= {ASCII_C, ASCII_D, ASCII_A, ASCII_T, ASCII_A, '\0'};
static const XML_Char atypeID[] = {ASCII_I, ASCII_D, '\0'};
static const XML_Char atypeIDREF[]
= {ASCII_I, ASCII_D, ASCII_R, ASCII_E, ASCII_F, '\0'};
static const XML_Char atypeIDREFS[]
= {ASCII_I, ASCII_D, ASCII_R, ASCII_E, ASCII_F, ASCII_S, '\0'};
static const XML_Char atypeENTITY[]
= {ASCII_E, ASCII_N, ASCII_T, ASCII_I, ASCII_T, ASCII_Y, '\0'};
static const XML_Char atypeENTITIES[]
= {ASCII_E, ASCII_N, ASCII_T, ASCII_I, ASCII_T,
ASCII_I, ASCII_E, ASCII_S, '\0'};
static const XML_Char atypeNMTOKEN[]
= {ASCII_N, ASCII_M, ASCII_T, ASCII_O, ASCII_K, ASCII_E, ASCII_N, '\0'};
static const XML_Char atypeNMTOKENS[]
= {ASCII_N, ASCII_M, ASCII_T, ASCII_O, ASCII_K,
ASCII_E, ASCII_N, ASCII_S, '\0'};
static const XML_Char notationPrefix[]
= {ASCII_N, ASCII_O, ASCII_T, ASCII_A, ASCII_T,
ASCII_I, ASCII_O, ASCII_N, ASCII_LPAREN, '\0'};
static const XML_Char enumValueSep[] = {ASCII_PIPE, '\0'};
static const XML_Char enumValueStart[] = {ASCII_LPAREN, '\0'};


DTD *const dtd = parser->m_dtd;

const char **eventPP;
const char **eventEndPP;
enum XML_Content_Quant quant;

if (enc == parser->m_encoding) {
eventPP = &parser->m_eventPtr;
eventEndPP = &parser->m_eventEndPtr;
} else {
eventPP = &(parser->m_openInternalEntities->internalEventPtr);
eventEndPP = &(parser->m_openInternalEntities->internalEventEndPtr);
}

for (;;) {
int role;
XML_Bool handleDefault = XML_TRUE;
*eventPP = s;
*eventEndPP = next;
if (tok <= 0) {
if (haveMore && tok != XML_TOK_INVALID) {
*nextPtr = s;
return XML_ERROR_NONE;
}
switch (tok) {
case XML_TOK_INVALID:
*eventPP = next;
return XML_ERROR_INVALID_TOKEN;
case XML_TOK_PARTIAL:
return XML_ERROR_UNCLOSED_TOKEN;
case XML_TOK_PARTIAL_CHAR:
return XML_ERROR_PARTIAL_CHAR;
case -XML_TOK_PROLOG_S:
tok = -tok;
break;
case XML_TOK_NONE:
#if defined(XML_DTD)

if (enc != parser->m_encoding
&& ! parser->m_openInternalEntities->betweenDecl) {
*nextPtr = s;
return XML_ERROR_NONE;
}




if (parser->m_isParamEntity || enc != parser->m_encoding) {
if (XmlTokenRole(&parser->m_prologState, XML_TOK_NONE, end, end, enc)
== XML_ROLE_ERROR)
return XML_ERROR_INCOMPLETE_PE;
*nextPtr = s;
return XML_ERROR_NONE;
}
#endif
return XML_ERROR_NO_ELEMENTS;
default:
tok = -tok;
next = end;
break;
}
}
role = XmlTokenRole(&parser->m_prologState, tok, s, next, enc);
switch (role) {
case XML_ROLE_XML_DECL: {
enum XML_Error result = processXmlDecl(parser, 0, s, next);
if (result != XML_ERROR_NONE)
return result;
enc = parser->m_encoding;
handleDefault = XML_FALSE;
} break;
case XML_ROLE_DOCTYPE_NAME:
if (parser->m_startDoctypeDeclHandler) {
parser->m_doctypeName
= poolStoreString(&parser->m_tempPool, enc, s, next);
if (! parser->m_doctypeName)
return XML_ERROR_NO_MEMORY;
poolFinish(&parser->m_tempPool);
parser->m_doctypePubid = NULL;
handleDefault = XML_FALSE;
}
parser->m_doctypeSysid = NULL;
break;
case XML_ROLE_DOCTYPE_INTERNAL_SUBSET:
if (parser->m_startDoctypeDeclHandler) {
parser->m_startDoctypeDeclHandler(
parser->m_handlerArg, parser->m_doctypeName, parser->m_doctypeSysid,
parser->m_doctypePubid, 1);
parser->m_doctypeName = NULL;
poolClear(&parser->m_tempPool);
handleDefault = XML_FALSE;
}
break;
#if defined(XML_DTD)
case XML_ROLE_TEXT_DECL: {
enum XML_Error result = processXmlDecl(parser, 1, s, next);
if (result != XML_ERROR_NONE)
return result;
enc = parser->m_encoding;
handleDefault = XML_FALSE;
} break;
#endif
case XML_ROLE_DOCTYPE_PUBLIC_ID:
#if defined(XML_DTD)
parser->m_useForeignDTD = XML_FALSE;
parser->m_declEntity = (ENTITY *)lookup(
parser, &dtd->paramEntities, externalSubsetName, sizeof(ENTITY));
if (! parser->m_declEntity)
return XML_ERROR_NO_MEMORY;
#endif
dtd->hasParamEntityRefs = XML_TRUE;
if (parser->m_startDoctypeDeclHandler) {
XML_Char *pubId;
if (! XmlIsPublicId(enc, s, next, eventPP))
return XML_ERROR_PUBLICID;
pubId = poolStoreString(&parser->m_tempPool, enc,
s + enc->minBytesPerChar,
next - enc->minBytesPerChar);
if (! pubId)
return XML_ERROR_NO_MEMORY;
normalizePublicId(pubId);
poolFinish(&parser->m_tempPool);
parser->m_doctypePubid = pubId;
handleDefault = XML_FALSE;
goto alreadyChecked;
}

case XML_ROLE_ENTITY_PUBLIC_ID:
if (! XmlIsPublicId(enc, s, next, eventPP))
return XML_ERROR_PUBLICID;
alreadyChecked:
if (dtd->keepProcessing && parser->m_declEntity) {
XML_Char *tem
= poolStoreString(&dtd->pool, enc, s + enc->minBytesPerChar,
next - enc->minBytesPerChar);
if (! tem)
return XML_ERROR_NO_MEMORY;
normalizePublicId(tem);
parser->m_declEntity->publicId = tem;
poolFinish(&dtd->pool);



if (parser->m_entityDeclHandler && role == XML_ROLE_ENTITY_PUBLIC_ID)
handleDefault = XML_FALSE;
}
break;
case XML_ROLE_DOCTYPE_CLOSE:
if (allowClosingDoctype != XML_TRUE) {

return XML_ERROR_INVALID_TOKEN;
}

if (parser->m_doctypeName) {
parser->m_startDoctypeDeclHandler(
parser->m_handlerArg, parser->m_doctypeName, parser->m_doctypeSysid,
parser->m_doctypePubid, 0);
poolClear(&parser->m_tempPool);
handleDefault = XML_FALSE;
}




#if defined(XML_DTD)
if (parser->m_doctypeSysid || parser->m_useForeignDTD) {
XML_Bool hadParamEntityRefs = dtd->hasParamEntityRefs;
dtd->hasParamEntityRefs = XML_TRUE;
if (parser->m_paramEntityParsing
&& parser->m_externalEntityRefHandler) {
ENTITY *entity = (ENTITY *)lookup(parser, &dtd->paramEntities,
externalSubsetName, sizeof(ENTITY));
if (! entity) {





return XML_ERROR_NO_MEMORY;
}
if (parser->m_useForeignDTD)
entity->base = parser->m_curBase;
dtd->paramEntityRead = XML_FALSE;
if (! parser->m_externalEntityRefHandler(
parser->m_externalEntityRefHandlerArg, 0, entity->base,
entity->systemId, entity->publicId))
return XML_ERROR_EXTERNAL_ENTITY_HANDLING;
if (dtd->paramEntityRead) {
if (! dtd->standalone && parser->m_notStandaloneHandler
&& ! parser->m_notStandaloneHandler(parser->m_handlerArg))
return XML_ERROR_NOT_STANDALONE;
}



else if (! parser->m_doctypeSysid)
dtd->hasParamEntityRefs = hadParamEntityRefs;

}
parser->m_useForeignDTD = XML_FALSE;
}
#endif
if (parser->m_endDoctypeDeclHandler) {
parser->m_endDoctypeDeclHandler(parser->m_handlerArg);
handleDefault = XML_FALSE;
}
break;
case XML_ROLE_INSTANCE_START:
#if defined(XML_DTD)



if (parser->m_useForeignDTD) {
XML_Bool hadParamEntityRefs = dtd->hasParamEntityRefs;
dtd->hasParamEntityRefs = XML_TRUE;
if (parser->m_paramEntityParsing
&& parser->m_externalEntityRefHandler) {
ENTITY *entity = (ENTITY *)lookup(parser, &dtd->paramEntities,
externalSubsetName, sizeof(ENTITY));
if (! entity)
return XML_ERROR_NO_MEMORY;
entity->base = parser->m_curBase;
dtd->paramEntityRead = XML_FALSE;
if (! parser->m_externalEntityRefHandler(
parser->m_externalEntityRefHandlerArg, 0, entity->base,
entity->systemId, entity->publicId))
return XML_ERROR_EXTERNAL_ENTITY_HANDLING;
if (dtd->paramEntityRead) {
if (! dtd->standalone && parser->m_notStandaloneHandler
&& ! parser->m_notStandaloneHandler(parser->m_handlerArg))
return XML_ERROR_NOT_STANDALONE;
}



else
dtd->hasParamEntityRefs = hadParamEntityRefs;

}
}
#endif
parser->m_processor = contentProcessor;
return contentProcessor(parser, s, end, nextPtr);
case XML_ROLE_ATTLIST_ELEMENT_NAME:
parser->m_declElementType = getElementType(parser, enc, s, next);
if (! parser->m_declElementType)
return XML_ERROR_NO_MEMORY;
goto checkAttListDeclHandler;
case XML_ROLE_ATTRIBUTE_NAME:
parser->m_declAttributeId = getAttributeId(parser, enc, s, next);
if (! parser->m_declAttributeId)
return XML_ERROR_NO_MEMORY;
parser->m_declAttributeIsCdata = XML_FALSE;
parser->m_declAttributeType = NULL;
parser->m_declAttributeIsId = XML_FALSE;
goto checkAttListDeclHandler;
case XML_ROLE_ATTRIBUTE_TYPE_CDATA:
parser->m_declAttributeIsCdata = XML_TRUE;
parser->m_declAttributeType = atypeCDATA;
goto checkAttListDeclHandler;
case XML_ROLE_ATTRIBUTE_TYPE_ID:
parser->m_declAttributeIsId = XML_TRUE;
parser->m_declAttributeType = atypeID;
goto checkAttListDeclHandler;
case XML_ROLE_ATTRIBUTE_TYPE_IDREF:
parser->m_declAttributeType = atypeIDREF;
goto checkAttListDeclHandler;
case XML_ROLE_ATTRIBUTE_TYPE_IDREFS:
parser->m_declAttributeType = atypeIDREFS;
goto checkAttListDeclHandler;
case XML_ROLE_ATTRIBUTE_TYPE_ENTITY:
parser->m_declAttributeType = atypeENTITY;
goto checkAttListDeclHandler;
case XML_ROLE_ATTRIBUTE_TYPE_ENTITIES:
parser->m_declAttributeType = atypeENTITIES;
goto checkAttListDeclHandler;
case XML_ROLE_ATTRIBUTE_TYPE_NMTOKEN:
parser->m_declAttributeType = atypeNMTOKEN;
goto checkAttListDeclHandler;
case XML_ROLE_ATTRIBUTE_TYPE_NMTOKENS:
parser->m_declAttributeType = atypeNMTOKENS;
checkAttListDeclHandler:
if (dtd->keepProcessing && parser->m_attlistDeclHandler)
handleDefault = XML_FALSE;
break;
case XML_ROLE_ATTRIBUTE_ENUM_VALUE:
case XML_ROLE_ATTRIBUTE_NOTATION_VALUE:
if (dtd->keepProcessing && parser->m_attlistDeclHandler) {
const XML_Char *prefix;
if (parser->m_declAttributeType) {
prefix = enumValueSep;
} else {
prefix = (role == XML_ROLE_ATTRIBUTE_NOTATION_VALUE ? notationPrefix
: enumValueStart);
}
if (! poolAppendString(&parser->m_tempPool, prefix))
return XML_ERROR_NO_MEMORY;
if (! poolAppend(&parser->m_tempPool, enc, s, next))
return XML_ERROR_NO_MEMORY;
parser->m_declAttributeType = parser->m_tempPool.start;
handleDefault = XML_FALSE;
}
break;
case XML_ROLE_IMPLIED_ATTRIBUTE_VALUE:
case XML_ROLE_REQUIRED_ATTRIBUTE_VALUE:
if (dtd->keepProcessing) {
if (! defineAttribute(parser->m_declElementType,
parser->m_declAttributeId,
parser->m_declAttributeIsCdata,
parser->m_declAttributeIsId, 0, parser))
return XML_ERROR_NO_MEMORY;
if (parser->m_attlistDeclHandler && parser->m_declAttributeType) {
if (*parser->m_declAttributeType == XML_T(ASCII_LPAREN)
|| (*parser->m_declAttributeType == XML_T(ASCII_N)
&& parser->m_declAttributeType[1] == XML_T(ASCII_O))) {

if (! poolAppendChar(&parser->m_tempPool, XML_T(ASCII_RPAREN))
|| ! poolAppendChar(&parser->m_tempPool, XML_T('\0')))
return XML_ERROR_NO_MEMORY;
parser->m_declAttributeType = parser->m_tempPool.start;
poolFinish(&parser->m_tempPool);
}
*eventEndPP = s;
parser->m_attlistDeclHandler(
parser->m_handlerArg, parser->m_declElementType->name,
parser->m_declAttributeId->name, parser->m_declAttributeType, 0,
role == XML_ROLE_REQUIRED_ATTRIBUTE_VALUE);
poolClear(&parser->m_tempPool);
handleDefault = XML_FALSE;
}
}
break;
case XML_ROLE_DEFAULT_ATTRIBUTE_VALUE:
case XML_ROLE_FIXED_ATTRIBUTE_VALUE:
if (dtd->keepProcessing) {
const XML_Char *attVal;
enum XML_Error result = storeAttributeValue(
parser, enc, parser->m_declAttributeIsCdata,
s + enc->minBytesPerChar, next - enc->minBytesPerChar, &dtd->pool);
if (result)
return result;
attVal = poolStart(&dtd->pool);
poolFinish(&dtd->pool);

if (! defineAttribute(
parser->m_declElementType, parser->m_declAttributeId,
parser->m_declAttributeIsCdata, XML_FALSE, attVal, parser))
return XML_ERROR_NO_MEMORY;
if (parser->m_attlistDeclHandler && parser->m_declAttributeType) {
if (*parser->m_declAttributeType == XML_T(ASCII_LPAREN)
|| (*parser->m_declAttributeType == XML_T(ASCII_N)
&& parser->m_declAttributeType[1] == XML_T(ASCII_O))) {

if (! poolAppendChar(&parser->m_tempPool, XML_T(ASCII_RPAREN))
|| ! poolAppendChar(&parser->m_tempPool, XML_T('\0')))
return XML_ERROR_NO_MEMORY;
parser->m_declAttributeType = parser->m_tempPool.start;
poolFinish(&parser->m_tempPool);
}
*eventEndPP = s;
parser->m_attlistDeclHandler(
parser->m_handlerArg, parser->m_declElementType->name,
parser->m_declAttributeId->name, parser->m_declAttributeType,
attVal, role == XML_ROLE_FIXED_ATTRIBUTE_VALUE);
poolClear(&parser->m_tempPool);
handleDefault = XML_FALSE;
}
}
break;
case XML_ROLE_ENTITY_VALUE:
if (dtd->keepProcessing) {
enum XML_Error result = storeEntityValue(
parser, enc, s + enc->minBytesPerChar, next - enc->minBytesPerChar);
if (parser->m_declEntity) {
parser->m_declEntity->textPtr = poolStart(&dtd->entityValuePool);
parser->m_declEntity->textLen
= (int)(poolLength(&dtd->entityValuePool));
poolFinish(&dtd->entityValuePool);
if (parser->m_entityDeclHandler) {
*eventEndPP = s;
parser->m_entityDeclHandler(
parser->m_handlerArg, parser->m_declEntity->name,
parser->m_declEntity->is_param, parser->m_declEntity->textPtr,
parser->m_declEntity->textLen, parser->m_curBase, 0, 0, 0);
handleDefault = XML_FALSE;
}
} else
poolDiscard(&dtd->entityValuePool);
if (result != XML_ERROR_NONE)
return result;
}
break;
case XML_ROLE_DOCTYPE_SYSTEM_ID:
#if defined(XML_DTD)
parser->m_useForeignDTD = XML_FALSE;
#endif
dtd->hasParamEntityRefs = XML_TRUE;
if (parser->m_startDoctypeDeclHandler) {
parser->m_doctypeSysid = poolStoreString(&parser->m_tempPool, enc,
s + enc->minBytesPerChar,
next - enc->minBytesPerChar);
if (parser->m_doctypeSysid == NULL)
return XML_ERROR_NO_MEMORY;
poolFinish(&parser->m_tempPool);
handleDefault = XML_FALSE;
}
#if defined(XML_DTD)
else


parser->m_doctypeSysid = externalSubsetName;
#endif
if (! dtd->standalone
#if defined(XML_DTD)
&& ! parser->m_paramEntityParsing
#endif
&& parser->m_notStandaloneHandler
&& ! parser->m_notStandaloneHandler(parser->m_handlerArg))
return XML_ERROR_NOT_STANDALONE;
#if !defined(XML_DTD)
break;
#else
if (! parser->m_declEntity) {
parser->m_declEntity = (ENTITY *)lookup(
parser, &dtd->paramEntities, externalSubsetName, sizeof(ENTITY));
if (! parser->m_declEntity)
return XML_ERROR_NO_MEMORY;
parser->m_declEntity->publicId = NULL;
}
#endif

case XML_ROLE_ENTITY_SYSTEM_ID:
if (dtd->keepProcessing && parser->m_declEntity) {
parser->m_declEntity->systemId
= poolStoreString(&dtd->pool, enc, s + enc->minBytesPerChar,
next - enc->minBytesPerChar);
if (! parser->m_declEntity->systemId)
return XML_ERROR_NO_MEMORY;
parser->m_declEntity->base = parser->m_curBase;
poolFinish(&dtd->pool);



if (parser->m_entityDeclHandler && role == XML_ROLE_ENTITY_SYSTEM_ID)
handleDefault = XML_FALSE;
}
break;
case XML_ROLE_ENTITY_COMPLETE:
if (dtd->keepProcessing && parser->m_declEntity
&& parser->m_entityDeclHandler) {
*eventEndPP = s;
parser->m_entityDeclHandler(
parser->m_handlerArg, parser->m_declEntity->name,
parser->m_declEntity->is_param, 0, 0, parser->m_declEntity->base,
parser->m_declEntity->systemId, parser->m_declEntity->publicId, 0);
handleDefault = XML_FALSE;
}
break;
case XML_ROLE_ENTITY_NOTATION_NAME:
if (dtd->keepProcessing && parser->m_declEntity) {
parser->m_declEntity->notation
= poolStoreString(&dtd->pool, enc, s, next);
if (! parser->m_declEntity->notation)
return XML_ERROR_NO_MEMORY;
poolFinish(&dtd->pool);
if (parser->m_unparsedEntityDeclHandler) {
*eventEndPP = s;
parser->m_unparsedEntityDeclHandler(
parser->m_handlerArg, parser->m_declEntity->name,
parser->m_declEntity->base, parser->m_declEntity->systemId,
parser->m_declEntity->publicId, parser->m_declEntity->notation);
handleDefault = XML_FALSE;
} else if (parser->m_entityDeclHandler) {
*eventEndPP = s;
parser->m_entityDeclHandler(
parser->m_handlerArg, parser->m_declEntity->name, 0, 0, 0,
parser->m_declEntity->base, parser->m_declEntity->systemId,
parser->m_declEntity->publicId, parser->m_declEntity->notation);
handleDefault = XML_FALSE;
}
}
break;
case XML_ROLE_GENERAL_ENTITY_NAME: {
if (XmlPredefinedEntityName(enc, s, next)) {
parser->m_declEntity = NULL;
break;
}
if (dtd->keepProcessing) {
const XML_Char *name = poolStoreString(&dtd->pool, enc, s, next);
if (! name)
return XML_ERROR_NO_MEMORY;
parser->m_declEntity = (ENTITY *)lookup(parser, &dtd->generalEntities,
name, sizeof(ENTITY));
if (! parser->m_declEntity)
return XML_ERROR_NO_MEMORY;
if (parser->m_declEntity->name != name) {
poolDiscard(&dtd->pool);
parser->m_declEntity = NULL;
} else {
poolFinish(&dtd->pool);
parser->m_declEntity->publicId = NULL;
parser->m_declEntity->is_param = XML_FALSE;



parser->m_declEntity->is_internal
= ! (parser->m_parentParser || parser->m_openInternalEntities);
if (parser->m_entityDeclHandler)
handleDefault = XML_FALSE;
}
} else {
poolDiscard(&dtd->pool);
parser->m_declEntity = NULL;
}
} break;
case XML_ROLE_PARAM_ENTITY_NAME:
#if defined(XML_DTD)
if (dtd->keepProcessing) {
const XML_Char *name = poolStoreString(&dtd->pool, enc, s, next);
if (! name)
return XML_ERROR_NO_MEMORY;
parser->m_declEntity = (ENTITY *)lookup(parser, &dtd->paramEntities,
name, sizeof(ENTITY));
if (! parser->m_declEntity)
return XML_ERROR_NO_MEMORY;
if (parser->m_declEntity->name != name) {
poolDiscard(&dtd->pool);
parser->m_declEntity = NULL;
} else {
poolFinish(&dtd->pool);
parser->m_declEntity->publicId = NULL;
parser->m_declEntity->is_param = XML_TRUE;



parser->m_declEntity->is_internal
= ! (parser->m_parentParser || parser->m_openInternalEntities);
if (parser->m_entityDeclHandler)
handleDefault = XML_FALSE;
}
} else {
poolDiscard(&dtd->pool);
parser->m_declEntity = NULL;
}
#else
parser->m_declEntity = NULL;
#endif
break;
case XML_ROLE_NOTATION_NAME:
parser->m_declNotationPublicId = NULL;
parser->m_declNotationName = NULL;
if (parser->m_notationDeclHandler) {
parser->m_declNotationName
= poolStoreString(&parser->m_tempPool, enc, s, next);
if (! parser->m_declNotationName)
return XML_ERROR_NO_MEMORY;
poolFinish(&parser->m_tempPool);
handleDefault = XML_FALSE;
}
break;
case XML_ROLE_NOTATION_PUBLIC_ID:
if (! XmlIsPublicId(enc, s, next, eventPP))
return XML_ERROR_PUBLICID;
if (parser
->m_declNotationName) {
XML_Char *tem = poolStoreString(&parser->m_tempPool, enc,
s + enc->minBytesPerChar,
next - enc->minBytesPerChar);
if (! tem)
return XML_ERROR_NO_MEMORY;
normalizePublicId(tem);
parser->m_declNotationPublicId = tem;
poolFinish(&parser->m_tempPool);
handleDefault = XML_FALSE;
}
break;
case XML_ROLE_NOTATION_SYSTEM_ID:
if (parser->m_declNotationName && parser->m_notationDeclHandler) {
const XML_Char *systemId = poolStoreString(&parser->m_tempPool, enc,
s + enc->minBytesPerChar,
next - enc->minBytesPerChar);
if (! systemId)
return XML_ERROR_NO_MEMORY;
*eventEndPP = s;
parser->m_notationDeclHandler(
parser->m_handlerArg, parser->m_declNotationName, parser->m_curBase,
systemId, parser->m_declNotationPublicId);
handleDefault = XML_FALSE;
}
poolClear(&parser->m_tempPool);
break;
case XML_ROLE_NOTATION_NO_SYSTEM_ID:
if (parser->m_declNotationPublicId && parser->m_notationDeclHandler) {
*eventEndPP = s;
parser->m_notationDeclHandler(
parser->m_handlerArg, parser->m_declNotationName, parser->m_curBase,
0, parser->m_declNotationPublicId);
handleDefault = XML_FALSE;
}
poolClear(&parser->m_tempPool);
break;
case XML_ROLE_ERROR:
switch (tok) {
case XML_TOK_PARAM_ENTITY_REF:


return XML_ERROR_PARAM_ENTITY_REF;
case XML_TOK_XML_DECL:
return XML_ERROR_MISPLACED_XML_PI;
default:
return XML_ERROR_SYNTAX;
}
#if defined(XML_DTD)
case XML_ROLE_IGNORE_SECT: {
enum XML_Error result;
if (parser->m_defaultHandler)
reportDefault(parser, enc, s, next);
handleDefault = XML_FALSE;
result = doIgnoreSection(parser, enc, &next, end, nextPtr, haveMore);
if (result != XML_ERROR_NONE)
return result;
else if (! next) {
parser->m_processor = ignoreSectionProcessor;
return result;
}
} break;
#endif
case XML_ROLE_GROUP_OPEN:
if (parser->m_prologState.level >= parser->m_groupSize) {
if (parser->m_groupSize) {
{
char *const new_connector = (char *)REALLOC(
parser, parser->m_groupConnector, parser->m_groupSize *= 2);
if (new_connector == NULL) {
parser->m_groupSize /= 2;
return XML_ERROR_NO_MEMORY;
}
parser->m_groupConnector = new_connector;
}

if (dtd->scaffIndex) {
int *const new_scaff_index = (int *)REALLOC(
parser, dtd->scaffIndex, parser->m_groupSize * sizeof(int));
if (new_scaff_index == NULL)
return XML_ERROR_NO_MEMORY;
dtd->scaffIndex = new_scaff_index;
}
} else {
parser->m_groupConnector
= (char *)MALLOC(parser, parser->m_groupSize = 32);
if (! parser->m_groupConnector) {
parser->m_groupSize = 0;
return XML_ERROR_NO_MEMORY;
}
}
}
parser->m_groupConnector[parser->m_prologState.level] = 0;
if (dtd->in_eldecl) {
int myindex = nextScaffoldPart(parser);
if (myindex < 0)
return XML_ERROR_NO_MEMORY;
assert(dtd->scaffIndex != NULL);
dtd->scaffIndex[dtd->scaffLevel] = myindex;
dtd->scaffLevel++;
dtd->scaffold[myindex].type = XML_CTYPE_SEQ;
if (parser->m_elementDeclHandler)
handleDefault = XML_FALSE;
}
break;
case XML_ROLE_GROUP_SEQUENCE:
if (parser->m_groupConnector[parser->m_prologState.level] == ASCII_PIPE)
return XML_ERROR_SYNTAX;
parser->m_groupConnector[parser->m_prologState.level] = ASCII_COMMA;
if (dtd->in_eldecl && parser->m_elementDeclHandler)
handleDefault = XML_FALSE;
break;
case XML_ROLE_GROUP_CHOICE:
if (parser->m_groupConnector[parser->m_prologState.level] == ASCII_COMMA)
return XML_ERROR_SYNTAX;
if (dtd->in_eldecl
&& ! parser->m_groupConnector[parser->m_prologState.level]
&& (dtd->scaffold[dtd->scaffIndex[dtd->scaffLevel - 1]].type
!= XML_CTYPE_MIXED)) {
dtd->scaffold[dtd->scaffIndex[dtd->scaffLevel - 1]].type
= XML_CTYPE_CHOICE;
if (parser->m_elementDeclHandler)
handleDefault = XML_FALSE;
}
parser->m_groupConnector[parser->m_prologState.level] = ASCII_PIPE;
break;
case XML_ROLE_PARAM_ENTITY_REF:
#if defined(XML_DTD)
case XML_ROLE_INNER_PARAM_ENTITY_REF:
dtd->hasParamEntityRefs = XML_TRUE;
if (! parser->m_paramEntityParsing)
dtd->keepProcessing = dtd->standalone;
else {
const XML_Char *name;
ENTITY *entity;
name = poolStoreString(&dtd->pool, enc, s + enc->minBytesPerChar,
next - enc->minBytesPerChar);
if (! name)
return XML_ERROR_NO_MEMORY;
entity = (ENTITY *)lookup(parser, &dtd->paramEntities, name, 0);
poolDiscard(&dtd->pool);




if (parser->m_prologState.documentEntity
&& (dtd->standalone ? ! parser->m_openInternalEntities
: ! dtd->hasParamEntityRefs)) {
if (! entity)
return XML_ERROR_UNDEFINED_ENTITY;
else if (! entity->is_internal) {




















return XML_ERROR_ENTITY_DECLARED_IN_PE;
}
} else if (! entity) {
dtd->keepProcessing = dtd->standalone;

if ((role == XML_ROLE_PARAM_ENTITY_REF)
&& parser->m_skippedEntityHandler) {
parser->m_skippedEntityHandler(parser->m_handlerArg, name, 1);
handleDefault = XML_FALSE;
}
break;
}
if (entity->open)
return XML_ERROR_RECURSIVE_ENTITY_REF;
if (entity->textPtr) {
enum XML_Error result;
XML_Bool betweenDecl
= (role == XML_ROLE_PARAM_ENTITY_REF ? XML_TRUE : XML_FALSE);
result = processInternalEntity(parser, entity, betweenDecl);
if (result != XML_ERROR_NONE)
return result;
handleDefault = XML_FALSE;
break;
}
if (parser->m_externalEntityRefHandler) {
dtd->paramEntityRead = XML_FALSE;
entity->open = XML_TRUE;
if (! parser->m_externalEntityRefHandler(
parser->m_externalEntityRefHandlerArg, 0, entity->base,
entity->systemId, entity->publicId)) {
entity->open = XML_FALSE;
return XML_ERROR_EXTERNAL_ENTITY_HANDLING;
}
entity->open = XML_FALSE;
handleDefault = XML_FALSE;
if (! dtd->paramEntityRead) {
dtd->keepProcessing = dtd->standalone;
break;
}
} else {
dtd->keepProcessing = dtd->standalone;
break;
}
}
#endif
if (! dtd->standalone && parser->m_notStandaloneHandler
&& ! parser->m_notStandaloneHandler(parser->m_handlerArg))
return XML_ERROR_NOT_STANDALONE;
break;



case XML_ROLE_ELEMENT_NAME:
if (parser->m_elementDeclHandler) {
parser->m_declElementType = getElementType(parser, enc, s, next);
if (! parser->m_declElementType)
return XML_ERROR_NO_MEMORY;
dtd->scaffLevel = 0;
dtd->scaffCount = 0;
dtd->in_eldecl = XML_TRUE;
handleDefault = XML_FALSE;
}
break;

case XML_ROLE_CONTENT_ANY:
case XML_ROLE_CONTENT_EMPTY:
if (dtd->in_eldecl) {
if (parser->m_elementDeclHandler) {
XML_Content *content
= (XML_Content *)MALLOC(parser, sizeof(XML_Content));
if (! content)
return XML_ERROR_NO_MEMORY;
content->quant = XML_CQUANT_NONE;
content->name = NULL;
content->numchildren = 0;
content->children = NULL;
content->type = ((role == XML_ROLE_CONTENT_ANY) ? XML_CTYPE_ANY
: XML_CTYPE_EMPTY);
*eventEndPP = s;
parser->m_elementDeclHandler(
parser->m_handlerArg, parser->m_declElementType->name, content);
handleDefault = XML_FALSE;
}
dtd->in_eldecl = XML_FALSE;
}
break;

case XML_ROLE_CONTENT_PCDATA:
if (dtd->in_eldecl) {
dtd->scaffold[dtd->scaffIndex[dtd->scaffLevel - 1]].type
= XML_CTYPE_MIXED;
if (parser->m_elementDeclHandler)
handleDefault = XML_FALSE;
}
break;

case XML_ROLE_CONTENT_ELEMENT:
quant = XML_CQUANT_NONE;
goto elementContent;
case XML_ROLE_CONTENT_ELEMENT_OPT:
quant = XML_CQUANT_OPT;
goto elementContent;
case XML_ROLE_CONTENT_ELEMENT_REP:
quant = XML_CQUANT_REP;
goto elementContent;
case XML_ROLE_CONTENT_ELEMENT_PLUS:
quant = XML_CQUANT_PLUS;
elementContent:
if (dtd->in_eldecl) {
ELEMENT_TYPE *el;
const XML_Char *name;
int nameLen;
const char *nxt
= (quant == XML_CQUANT_NONE ? next : next - enc->minBytesPerChar);
int myindex = nextScaffoldPart(parser);
if (myindex < 0)
return XML_ERROR_NO_MEMORY;
dtd->scaffold[myindex].type = XML_CTYPE_NAME;
dtd->scaffold[myindex].quant = quant;
el = getElementType(parser, enc, s, nxt);
if (! el)
return XML_ERROR_NO_MEMORY;
name = el->name;
dtd->scaffold[myindex].name = name;
nameLen = 0;
for (; name[nameLen++];)
;
dtd->contentStringLen += nameLen;
if (parser->m_elementDeclHandler)
handleDefault = XML_FALSE;
}
break;

case XML_ROLE_GROUP_CLOSE:
quant = XML_CQUANT_NONE;
goto closeGroup;
case XML_ROLE_GROUP_CLOSE_OPT:
quant = XML_CQUANT_OPT;
goto closeGroup;
case XML_ROLE_GROUP_CLOSE_REP:
quant = XML_CQUANT_REP;
goto closeGroup;
case XML_ROLE_GROUP_CLOSE_PLUS:
quant = XML_CQUANT_PLUS;
closeGroup:
if (dtd->in_eldecl) {
if (parser->m_elementDeclHandler)
handleDefault = XML_FALSE;
dtd->scaffLevel--;
dtd->scaffold[dtd->scaffIndex[dtd->scaffLevel]].quant = quant;
if (dtd->scaffLevel == 0) {
if (! handleDefault) {
XML_Content *model = build_model(parser);
if (! model)
return XML_ERROR_NO_MEMORY;
*eventEndPP = s;
parser->m_elementDeclHandler(
parser->m_handlerArg, parser->m_declElementType->name, model);
}
dtd->in_eldecl = XML_FALSE;
dtd->contentStringLen = 0;
}
}
break;


case XML_ROLE_PI:
if (! reportProcessingInstruction(parser, enc, s, next))
return XML_ERROR_NO_MEMORY;
handleDefault = XML_FALSE;
break;
case XML_ROLE_COMMENT:
if (! reportComment(parser, enc, s, next))
return XML_ERROR_NO_MEMORY;
handleDefault = XML_FALSE;
break;
case XML_ROLE_NONE:
switch (tok) {
case XML_TOK_BOM:
handleDefault = XML_FALSE;
break;
}
break;
case XML_ROLE_DOCTYPE_NONE:
if (parser->m_startDoctypeDeclHandler)
handleDefault = XML_FALSE;
break;
case XML_ROLE_ENTITY_NONE:
if (dtd->keepProcessing && parser->m_entityDeclHandler)
handleDefault = XML_FALSE;
break;
case XML_ROLE_NOTATION_NONE:
if (parser->m_notationDeclHandler)
handleDefault = XML_FALSE;
break;
case XML_ROLE_ATTLIST_NONE:
if (dtd->keepProcessing && parser->m_attlistDeclHandler)
handleDefault = XML_FALSE;
break;
case XML_ROLE_ELEMENT_NONE:
if (parser->m_elementDeclHandler)
handleDefault = XML_FALSE;
break;
}

if (handleDefault && parser->m_defaultHandler)
reportDefault(parser, enc, s, next);

switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:
*nextPtr = next;
return XML_ERROR_NONE;
case XML_FINISHED:
return XML_ERROR_ABORTED;
default:
s = next;
tok = XmlPrologTok(enc, s, end, &next);
}
}

}

static enum XML_Error PTRCALL
epilogProcessor(XML_Parser parser, const char *s, const char *end,
const char **nextPtr) {
parser->m_processor = epilogProcessor;
parser->m_eventPtr = s;
for (;;) {
const char *next = NULL;
int tok = XmlPrologTok(parser->m_encoding, s, end, &next);
parser->m_eventEndPtr = next;
switch (tok) {

case -XML_TOK_PROLOG_S:
if (parser->m_defaultHandler) {
reportDefault(parser, parser->m_encoding, s, next);
if (parser->m_parsingStatus.parsing == XML_FINISHED)
return XML_ERROR_ABORTED;
}
*nextPtr = next;
return XML_ERROR_NONE;
case XML_TOK_NONE:
*nextPtr = s;
return XML_ERROR_NONE;
case XML_TOK_PROLOG_S:
if (parser->m_defaultHandler)
reportDefault(parser, parser->m_encoding, s, next);
break;
case XML_TOK_PI:
if (! reportProcessingInstruction(parser, parser->m_encoding, s, next))
return XML_ERROR_NO_MEMORY;
break;
case XML_TOK_COMMENT:
if (! reportComment(parser, parser->m_encoding, s, next))
return XML_ERROR_NO_MEMORY;
break;
case XML_TOK_INVALID:
parser->m_eventPtr = next;
return XML_ERROR_INVALID_TOKEN;
case XML_TOK_PARTIAL:
if (! parser->m_parsingStatus.finalBuffer) {
*nextPtr = s;
return XML_ERROR_NONE;
}
return XML_ERROR_UNCLOSED_TOKEN;
case XML_TOK_PARTIAL_CHAR:
if (! parser->m_parsingStatus.finalBuffer) {
*nextPtr = s;
return XML_ERROR_NONE;
}
return XML_ERROR_PARTIAL_CHAR;
default:
return XML_ERROR_JUNK_AFTER_DOC_ELEMENT;
}
parser->m_eventPtr = s = next;
switch (parser->m_parsingStatus.parsing) {
case XML_SUSPENDED:
*nextPtr = next;
return XML_ERROR_NONE;
case XML_FINISHED:
return XML_ERROR_ABORTED;
default:;
}
}
}

static enum XML_Error
processInternalEntity(XML_Parser parser, ENTITY *entity, XML_Bool betweenDecl) {
const char *textStart, *textEnd;
const char *next;
enum XML_Error result;
OPEN_INTERNAL_ENTITY *openEntity;

if (parser->m_freeInternalEntities) {
openEntity = parser->m_freeInternalEntities;
parser->m_freeInternalEntities = openEntity->next;
} else {
openEntity
= (OPEN_INTERNAL_ENTITY *)MALLOC(parser, sizeof(OPEN_INTERNAL_ENTITY));
if (! openEntity)
return XML_ERROR_NO_MEMORY;
}
entity->open = XML_TRUE;
entity->processed = 0;
openEntity->next = parser->m_openInternalEntities;
parser->m_openInternalEntities = openEntity;
openEntity->entity = entity;
openEntity->startTagLevel = parser->m_tagLevel;
openEntity->betweenDecl = betweenDecl;
openEntity->internalEventPtr = NULL;
openEntity->internalEventEndPtr = NULL;
textStart = (char *)entity->textPtr;
textEnd = (char *)(entity->textPtr + entity->textLen);

next = textStart;

#if defined(XML_DTD)
if (entity->is_param) {
int tok
= XmlPrologTok(parser->m_internalEncoding, textStart, textEnd, &next);
result = doProlog(parser, parser->m_internalEncoding, textStart, textEnd,
tok, next, &next, XML_FALSE, XML_FALSE);
} else
#endif
result = doContent(parser, parser->m_tagLevel, parser->m_internalEncoding,
textStart, textEnd, &next, XML_FALSE);

if (result == XML_ERROR_NONE) {
if (textEnd != next && parser->m_parsingStatus.parsing == XML_SUSPENDED) {
entity->processed = (int)(next - textStart);
parser->m_processor = internalEntityProcessor;
} else {
entity->open = XML_FALSE;
parser->m_openInternalEntities = openEntity->next;

openEntity->next = parser->m_freeInternalEntities;
parser->m_freeInternalEntities = openEntity;
}
}
return result;
}

static enum XML_Error PTRCALL
internalEntityProcessor(XML_Parser parser, const char *s, const char *end,
const char **nextPtr) {
ENTITY *entity;
const char *textStart, *textEnd;
const char *next;
enum XML_Error result;
OPEN_INTERNAL_ENTITY *openEntity = parser->m_openInternalEntities;
if (! openEntity)
return XML_ERROR_UNEXPECTED_STATE;

entity = openEntity->entity;
textStart = ((char *)entity->textPtr) + entity->processed;
textEnd = (char *)(entity->textPtr + entity->textLen);

next = textStart;

#if defined(XML_DTD)
if (entity->is_param) {
int tok
= XmlPrologTok(parser->m_internalEncoding, textStart, textEnd, &next);
result = doProlog(parser, parser->m_internalEncoding, textStart, textEnd,
tok, next, &next, XML_FALSE, XML_TRUE);
} else
#endif
result = doContent(parser, openEntity->startTagLevel,
parser->m_internalEncoding, textStart, textEnd, &next,
XML_FALSE);

if (result != XML_ERROR_NONE)
return result;
else if (textEnd != next
&& parser->m_parsingStatus.parsing == XML_SUSPENDED) {
entity->processed = (int)(next - (char *)entity->textPtr);
return result;
} else {
entity->open = XML_FALSE;
parser->m_openInternalEntities = openEntity->next;

openEntity->next = parser->m_freeInternalEntities;
parser->m_freeInternalEntities = openEntity;
}

#if defined(XML_DTD)
if (entity->is_param) {
int tok;
parser->m_processor = prologProcessor;
tok = XmlPrologTok(parser->m_encoding, s, end, &next);
return doProlog(parser, parser->m_encoding, s, end, tok, next, nextPtr,
(XML_Bool)! parser->m_parsingStatus.finalBuffer, XML_TRUE);
} else
#endif
{
parser->m_processor = contentProcessor;

return doContent(parser, parser->m_parentParser ? 1 : 0, parser->m_encoding,
s, end, nextPtr,
(XML_Bool)! parser->m_parsingStatus.finalBuffer);
}
}

static enum XML_Error PTRCALL
errorProcessor(XML_Parser parser, const char *s, const char *end,
const char **nextPtr) {
UNUSED_P(s);
UNUSED_P(end);
UNUSED_P(nextPtr);
return parser->m_errorCode;
}

static enum XML_Error
storeAttributeValue(XML_Parser parser, const ENCODING *enc, XML_Bool isCdata,
const char *ptr, const char *end, STRING_POOL *pool) {
enum XML_Error result
= appendAttributeValue(parser, enc, isCdata, ptr, end, pool);
if (result)
return result;
if (! isCdata && poolLength(pool) && poolLastChar(pool) == 0x20)
poolChop(pool);
if (! poolAppendChar(pool, XML_T('\0')))
return XML_ERROR_NO_MEMORY;
return XML_ERROR_NONE;
}

static enum XML_Error
appendAttributeValue(XML_Parser parser, const ENCODING *enc, XML_Bool isCdata,
const char *ptr, const char *end, STRING_POOL *pool) {
DTD *const dtd = parser->m_dtd;
for (;;) {
const char *next;
int tok = XmlAttributeValueTok(enc, ptr, end, &next);
switch (tok) {
case XML_TOK_NONE:
return XML_ERROR_NONE;
case XML_TOK_INVALID:
if (enc == parser->m_encoding)
parser->m_eventPtr = next;
return XML_ERROR_INVALID_TOKEN;
case XML_TOK_PARTIAL:
if (enc == parser->m_encoding)
parser->m_eventPtr = ptr;
return XML_ERROR_INVALID_TOKEN;
case XML_TOK_CHAR_REF: {
XML_Char buf[XML_ENCODE_MAX];
int i;
int n = XmlCharRefNumber(enc, ptr);
if (n < 0) {
if (enc == parser->m_encoding)
parser->m_eventPtr = ptr;
return XML_ERROR_BAD_CHAR_REF;
}
if (! isCdata && n == 0x20
&& (poolLength(pool) == 0 || poolLastChar(pool) == 0x20))
break;
n = XmlEncode(n, (ICHAR *)buf);









for (i = 0; i < n; i++) {
if (! poolAppendChar(pool, buf[i]))
return XML_ERROR_NO_MEMORY;
}
} break;
case XML_TOK_DATA_CHARS:
if (! poolAppend(pool, enc, ptr, next))
return XML_ERROR_NO_MEMORY;
break;
case XML_TOK_TRAILING_CR:
next = ptr + enc->minBytesPerChar;

case XML_TOK_ATTRIBUTE_VALUE_S:
case XML_TOK_DATA_NEWLINE:
if (! isCdata && (poolLength(pool) == 0 || poolLastChar(pool) == 0x20))
break;
if (! poolAppendChar(pool, 0x20))
return XML_ERROR_NO_MEMORY;
break;
case XML_TOK_ENTITY_REF: {
const XML_Char *name;
ENTITY *entity;
char checkEntityDecl;
XML_Char ch = (XML_Char)XmlPredefinedEntityName(
enc, ptr + enc->minBytesPerChar, next - enc->minBytesPerChar);
if (ch) {
if (! poolAppendChar(pool, ch))
return XML_ERROR_NO_MEMORY;
break;
}
name = poolStoreString(&parser->m_temp2Pool, enc,
ptr + enc->minBytesPerChar,
next - enc->minBytesPerChar);
if (! name)
return XML_ERROR_NO_MEMORY;
entity = (ENTITY *)lookup(parser, &dtd->generalEntities, name, 0);
poolDiscard(&parser->m_temp2Pool);



if (pool == &dtd->pool)
checkEntityDecl =
#if defined(XML_DTD)
parser->m_prologState.documentEntity &&
#endif
(dtd->standalone ? ! parser->m_openInternalEntities
: ! dtd->hasParamEntityRefs);
else
checkEntityDecl = ! dtd->hasParamEntityRefs || dtd->standalone;
if (checkEntityDecl) {
if (! entity)
return XML_ERROR_UNDEFINED_ENTITY;
else if (! entity->is_internal)
return XML_ERROR_ENTITY_DECLARED_IN_PE;
} else if (! entity) {










break;
}
if (entity->open) {
if (enc == parser->m_encoding) {

















parser->m_eventPtr = ptr;
}
return XML_ERROR_RECURSIVE_ENTITY_REF;
}
if (entity->notation) {
if (enc == parser->m_encoding)
parser->m_eventPtr = ptr;
return XML_ERROR_BINARY_ENTITY_REF;
}
if (! entity->textPtr) {
if (enc == parser->m_encoding)
parser->m_eventPtr = ptr;
return XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF;
} else {
enum XML_Error result;
const XML_Char *textEnd = entity->textPtr + entity->textLen;
entity->open = XML_TRUE;
result = appendAttributeValue(parser, parser->m_internalEncoding,
isCdata, (char *)entity->textPtr,
(char *)textEnd, pool);
entity->open = XML_FALSE;
if (result)
return result;
}
} break;
default:











if (enc == parser->m_encoding)
parser->m_eventPtr = ptr;
return XML_ERROR_UNEXPECTED_STATE;

}
ptr = next;
}

}

static enum XML_Error
storeEntityValue(XML_Parser parser, const ENCODING *enc,
const char *entityTextPtr, const char *entityTextEnd) {
DTD *const dtd = parser->m_dtd;
STRING_POOL *pool = &(dtd->entityValuePool);
enum XML_Error result = XML_ERROR_NONE;
#if defined(XML_DTD)
int oldInEntityValue = parser->m_prologState.inEntityValue;
parser->m_prologState.inEntityValue = 1;
#endif



if (! pool->blocks) {
if (! poolGrow(pool))
return XML_ERROR_NO_MEMORY;
}

for (;;) {
const char *next;
int tok = XmlEntityValueTok(enc, entityTextPtr, entityTextEnd, &next);
switch (tok) {
case XML_TOK_PARAM_ENTITY_REF:
#if defined(XML_DTD)
if (parser->m_isParamEntity || enc != parser->m_encoding) {
const XML_Char *name;
ENTITY *entity;
name = poolStoreString(&parser->m_tempPool, enc,
entityTextPtr + enc->minBytesPerChar,
next - enc->minBytesPerChar);
if (! name) {
result = XML_ERROR_NO_MEMORY;
goto endEntityValue;
}
entity = (ENTITY *)lookup(parser, &dtd->paramEntities, name, 0);
poolDiscard(&parser->m_tempPool);
if (! entity) {






dtd->keepProcessing = dtd->standalone;
goto endEntityValue;
}
if (entity->open) {
if (enc == parser->m_encoding)
parser->m_eventPtr = entityTextPtr;
result = XML_ERROR_RECURSIVE_ENTITY_REF;
goto endEntityValue;
}
if (entity->systemId) {
if (parser->m_externalEntityRefHandler) {
dtd->paramEntityRead = XML_FALSE;
entity->open = XML_TRUE;
if (! parser->m_externalEntityRefHandler(
parser->m_externalEntityRefHandlerArg, 0, entity->base,
entity->systemId, entity->publicId)) {
entity->open = XML_FALSE;
result = XML_ERROR_EXTERNAL_ENTITY_HANDLING;
goto endEntityValue;
}
entity->open = XML_FALSE;
if (! dtd->paramEntityRead)
dtd->keepProcessing = dtd->standalone;
} else
dtd->keepProcessing = dtd->standalone;
} else {
entity->open = XML_TRUE;
result = storeEntityValue(
parser, parser->m_internalEncoding, (char *)entity->textPtr,
(char *)(entity->textPtr + entity->textLen));
entity->open = XML_FALSE;
if (result)
goto endEntityValue;
}
break;
}
#endif


parser->m_eventPtr = entityTextPtr;
result = XML_ERROR_PARAM_ENTITY_REF;
goto endEntityValue;
case XML_TOK_NONE:
result = XML_ERROR_NONE;
goto endEntityValue;
case XML_TOK_ENTITY_REF:
case XML_TOK_DATA_CHARS:
if (! poolAppend(pool, enc, entityTextPtr, next)) {
result = XML_ERROR_NO_MEMORY;
goto endEntityValue;
}
break;
case XML_TOK_TRAILING_CR:
next = entityTextPtr + enc->minBytesPerChar;

case XML_TOK_DATA_NEWLINE:
if (pool->end == pool->ptr && ! poolGrow(pool)) {
result = XML_ERROR_NO_MEMORY;
goto endEntityValue;
}
*(pool->ptr)++ = 0xA;
break;
case XML_TOK_CHAR_REF: {
XML_Char buf[XML_ENCODE_MAX];
int i;
int n = XmlCharRefNumber(enc, entityTextPtr);
if (n < 0) {
if (enc == parser->m_encoding)
parser->m_eventPtr = entityTextPtr;
result = XML_ERROR_BAD_CHAR_REF;
goto endEntityValue;
}
n = XmlEncode(n, (ICHAR *)buf);









for (i = 0; i < n; i++) {
if (pool->end == pool->ptr && ! poolGrow(pool)) {
result = XML_ERROR_NO_MEMORY;
goto endEntityValue;
}
*(pool->ptr)++ = buf[i];
}
} break;
case XML_TOK_PARTIAL:
if (enc == parser->m_encoding)
parser->m_eventPtr = entityTextPtr;
result = XML_ERROR_INVALID_TOKEN;
goto endEntityValue;
case XML_TOK_INVALID:
if (enc == parser->m_encoding)
parser->m_eventPtr = next;
result = XML_ERROR_INVALID_TOKEN;
goto endEntityValue;
default:







if (enc == parser->m_encoding)
parser->m_eventPtr = entityTextPtr;
result = XML_ERROR_UNEXPECTED_STATE;
goto endEntityValue;

}
entityTextPtr = next;
}
endEntityValue:
#if defined(XML_DTD)
parser->m_prologState.inEntityValue = oldInEntityValue;
#endif
return result;
}

static void FASTCALL
normalizeLines(XML_Char *s) {
XML_Char *p;
for (;; s++) {
if (*s == XML_T('\0'))
return;
if (*s == 0xD)
break;
}
p = s;
do {
if (*s == 0xD) {
*p++ = 0xA;
if (*++s == 0xA)
s++;
} else
*p++ = *s++;
} while (*s);
*p = XML_T('\0');
}

static int
reportProcessingInstruction(XML_Parser parser, const ENCODING *enc,
const char *start, const char *end) {
const XML_Char *target;
XML_Char *data;
const char *tem;
if (! parser->m_processingInstructionHandler) {
if (parser->m_defaultHandler)
reportDefault(parser, enc, start, end);
return 1;
}
start += enc->minBytesPerChar * 2;
tem = start + XmlNameLength(enc, start);
target = poolStoreString(&parser->m_tempPool, enc, start, tem);
if (! target)
return 0;
poolFinish(&parser->m_tempPool);
data = poolStoreString(&parser->m_tempPool, enc, XmlSkipS(enc, tem),
end - enc->minBytesPerChar * 2);
if (! data)
return 0;
normalizeLines(data);
parser->m_processingInstructionHandler(parser->m_handlerArg, target, data);
poolClear(&parser->m_tempPool);
return 1;
}

static int
reportComment(XML_Parser parser, const ENCODING *enc, const char *start,
const char *end) {
XML_Char *data;
if (! parser->m_commentHandler) {
if (parser->m_defaultHandler)
reportDefault(parser, enc, start, end);
return 1;
}
data = poolStoreString(&parser->m_tempPool, enc,
start + enc->minBytesPerChar * 4,
end - enc->minBytesPerChar * 3);
if (! data)
return 0;
normalizeLines(data);
parser->m_commentHandler(parser->m_handlerArg, data);
poolClear(&parser->m_tempPool);
return 1;
}

static void
reportDefault(XML_Parser parser, const ENCODING *enc, const char *s,
const char *end) {
if (MUST_CONVERT(enc, s)) {
enum XML_Convert_Result convert_res;
const char **eventPP;
const char **eventEndPP;
if (enc == parser->m_encoding) {
eventPP = &parser->m_eventPtr;
eventEndPP = &parser->m_eventEndPtr;
} else {
















eventPP = &(parser->m_openInternalEntities->internalEventPtr);
eventEndPP = &(parser->m_openInternalEntities->internalEventEndPtr);

}
do {
ICHAR *dataPtr = (ICHAR *)parser->m_dataBuf;
convert_res
= XmlConvert(enc, &s, end, &dataPtr, (ICHAR *)parser->m_dataBufEnd);
*eventEndPP = s;
parser->m_defaultHandler(parser->m_handlerArg, parser->m_dataBuf,
(int)(dataPtr - (ICHAR *)parser->m_dataBuf));
*eventPP = s;
} while ((convert_res != XML_CONVERT_COMPLETED)
&& (convert_res != XML_CONVERT_INPUT_INCOMPLETE));
} else
parser->m_defaultHandler(parser->m_handlerArg, (XML_Char *)s,
(int)((XML_Char *)end - (XML_Char *)s));
}

static int
defineAttribute(ELEMENT_TYPE *type, ATTRIBUTE_ID *attId, XML_Bool isCdata,
XML_Bool isId, const XML_Char *value, XML_Parser parser) {
DEFAULT_ATTRIBUTE *att;
if (value || isId) {


int i;
for (i = 0; i < type->nDefaultAtts; i++)
if (attId == type->defaultAtts[i].id)
return 1;
if (isId && ! type->idAtt && ! attId->xmlns)
type->idAtt = attId;
}
if (type->nDefaultAtts == type->allocDefaultAtts) {
if (type->allocDefaultAtts == 0) {
type->allocDefaultAtts = 8;
type->defaultAtts = (DEFAULT_ATTRIBUTE *)MALLOC(
parser, type->allocDefaultAtts * sizeof(DEFAULT_ATTRIBUTE));
if (! type->defaultAtts) {
type->allocDefaultAtts = 0;
return 0;
}
} else {
DEFAULT_ATTRIBUTE *temp;
int count = type->allocDefaultAtts * 2;
temp = (DEFAULT_ATTRIBUTE *)REALLOC(parser, type->defaultAtts,
(count * sizeof(DEFAULT_ATTRIBUTE)));
if (temp == NULL)
return 0;
type->allocDefaultAtts = count;
type->defaultAtts = temp;
}
}
att = type->defaultAtts + type->nDefaultAtts;
att->id = attId;
att->value = value;
att->isCdata = isCdata;
if (! isCdata)
attId->maybeTokenized = XML_TRUE;
type->nDefaultAtts += 1;
return 1;
}

static int
setElementTypePrefix(XML_Parser parser, ELEMENT_TYPE *elementType) {
DTD *const dtd = parser->m_dtd;
const XML_Char *name;
for (name = elementType->name; *name; name++) {
if (*name == XML_T(ASCII_COLON)) {
PREFIX *prefix;
const XML_Char *s;
for (s = elementType->name; s != name; s++) {
if (! poolAppendChar(&dtd->pool, *s))
return 0;
}
if (! poolAppendChar(&dtd->pool, XML_T('\0')))
return 0;
prefix = (PREFIX *)lookup(parser, &dtd->prefixes, poolStart(&dtd->pool),
sizeof(PREFIX));
if (! prefix)
return 0;
if (prefix->name == poolStart(&dtd->pool))
poolFinish(&dtd->pool);
else
poolDiscard(&dtd->pool);
elementType->prefix = prefix;
break;
}
}
return 1;
}

static ATTRIBUTE_ID *
getAttributeId(XML_Parser parser, const ENCODING *enc, const char *start,
const char *end) {
DTD *const dtd = parser->m_dtd;
ATTRIBUTE_ID *id;
const XML_Char *name;
if (! poolAppendChar(&dtd->pool, XML_T('\0')))
return NULL;
name = poolStoreString(&dtd->pool, enc, start, end);
if (! name)
return NULL;

++name;
id = (ATTRIBUTE_ID *)lookup(parser, &dtd->attributeIds, name,
sizeof(ATTRIBUTE_ID));
if (! id)
return NULL;
if (id->name != name)
poolDiscard(&dtd->pool);
else {
poolFinish(&dtd->pool);
if (! parser->m_ns)
;
else if (name[0] == XML_T(ASCII_x) && name[1] == XML_T(ASCII_m)
&& name[2] == XML_T(ASCII_l) && name[3] == XML_T(ASCII_n)
&& name[4] == XML_T(ASCII_s)
&& (name[5] == XML_T('\0') || name[5] == XML_T(ASCII_COLON))) {
if (name[5] == XML_T('\0'))
id->prefix = &dtd->defaultPrefix;
else
id->prefix = (PREFIX *)lookup(parser, &dtd->prefixes, name + 6,
sizeof(PREFIX));
id->xmlns = XML_TRUE;
} else {
int i;
for (i = 0; name[i]; i++) {

if (name[i] == XML_T(ASCII_COLON)) {
int j;
for (j = 0; j < i; j++) {
if (! poolAppendChar(&dtd->pool, name[j]))
return NULL;
}
if (! poolAppendChar(&dtd->pool, XML_T('\0')))
return NULL;
id->prefix = (PREFIX *)lookup(parser, &dtd->prefixes,
poolStart(&dtd->pool), sizeof(PREFIX));
if (! id->prefix)
return NULL;
if (id->prefix->name == poolStart(&dtd->pool))
poolFinish(&dtd->pool);
else
poolDiscard(&dtd->pool);
break;
}
}
}
}
return id;
}

#define CONTEXT_SEP XML_T(ASCII_FF)

static const XML_Char *
getContext(XML_Parser parser) {
DTD *const dtd = parser->m_dtd;
HASH_TABLE_ITER iter;
XML_Bool needSep = XML_FALSE;

if (dtd->defaultPrefix.binding) {
int i;
int len;
if (! poolAppendChar(&parser->m_tempPool, XML_T(ASCII_EQUALS)))
return NULL;
len = dtd->defaultPrefix.binding->uriLen;
if (parser->m_namespaceSeparator)
len--;
for (i = 0; i < len; i++) {
if (! poolAppendChar(&parser->m_tempPool,
dtd->defaultPrefix.binding->uri[i])) {



















return NULL;
}
}
needSep = XML_TRUE;
}

hashTableIterInit(&iter, &(dtd->prefixes));
for (;;) {
int i;
int len;
const XML_Char *s;
PREFIX *prefix = (PREFIX *)hashTableIterNext(&iter);
if (! prefix)
break;
if (! prefix->binding) {






continue;
}
if (needSep && ! poolAppendChar(&parser->m_tempPool, CONTEXT_SEP))
return NULL;
for (s = prefix->name; *s; s++)
if (! poolAppendChar(&parser->m_tempPool, *s))
return NULL;
if (! poolAppendChar(&parser->m_tempPool, XML_T(ASCII_EQUALS)))
return NULL;
len = prefix->binding->uriLen;
if (parser->m_namespaceSeparator)
len--;
for (i = 0; i < len; i++)
if (! poolAppendChar(&parser->m_tempPool, prefix->binding->uri[i]))
return NULL;
needSep = XML_TRUE;
}

hashTableIterInit(&iter, &(dtd->generalEntities));
for (;;) {
const XML_Char *s;
ENTITY *e = (ENTITY *)hashTableIterNext(&iter);
if (! e)
break;
if (! e->open)
continue;
if (needSep && ! poolAppendChar(&parser->m_tempPool, CONTEXT_SEP))
return NULL;
for (s = e->name; *s; s++)
if (! poolAppendChar(&parser->m_tempPool, *s))
return 0;
needSep = XML_TRUE;
}

if (! poolAppendChar(&parser->m_tempPool, XML_T('\0')))
return NULL;
return parser->m_tempPool.start;
}

static XML_Bool
setContext(XML_Parser parser, const XML_Char *context) {
DTD *const dtd = parser->m_dtd;
const XML_Char *s = context;

while (*context != XML_T('\0')) {
if (*s == CONTEXT_SEP || *s == XML_T('\0')) {
ENTITY *e;
if (! poolAppendChar(&parser->m_tempPool, XML_T('\0')))
return XML_FALSE;
e = (ENTITY *)lookup(parser, &dtd->generalEntities,
poolStart(&parser->m_tempPool), 0);
if (e)
e->open = XML_TRUE;
if (*s != XML_T('\0'))
s++;
context = s;
poolDiscard(&parser->m_tempPool);
} else if (*s == XML_T(ASCII_EQUALS)) {
PREFIX *prefix;
if (poolLength(&parser->m_tempPool) == 0)
prefix = &dtd->defaultPrefix;
else {
if (! poolAppendChar(&parser->m_tempPool, XML_T('\0')))
return XML_FALSE;
prefix
= (PREFIX *)lookup(parser, &dtd->prefixes,
poolStart(&parser->m_tempPool), sizeof(PREFIX));
if (! prefix)
return XML_FALSE;
if (prefix->name == poolStart(&parser->m_tempPool)) {
prefix->name = poolCopyString(&dtd->pool, prefix->name);
if (! prefix->name)
return XML_FALSE;
}
poolDiscard(&parser->m_tempPool);
}
for (context = s + 1; *context != CONTEXT_SEP && *context != XML_T('\0');
context++)
if (! poolAppendChar(&parser->m_tempPool, *context))
return XML_FALSE;
if (! poolAppendChar(&parser->m_tempPool, XML_T('\0')))
return XML_FALSE;
if (addBinding(parser, prefix, NULL, poolStart(&parser->m_tempPool),
&parser->m_inheritedBindings)
!= XML_ERROR_NONE)
return XML_FALSE;
poolDiscard(&parser->m_tempPool);
if (*context != XML_T('\0'))
++context;
s = context;
} else {
if (! poolAppendChar(&parser->m_tempPool, *s))
return XML_FALSE;
s++;
}
}
return XML_TRUE;
}

static void FASTCALL
normalizePublicId(XML_Char *publicId) {
XML_Char *p = publicId;
XML_Char *s;
for (s = publicId; *s; s++) {
switch (*s) {
case 0x20:
case 0xD:
case 0xA:
if (p != publicId && p[-1] != 0x20)
*p++ = 0x20;
break;
default:
*p++ = *s;
}
}
if (p != publicId && p[-1] == 0x20)
--p;
*p = XML_T('\0');
}

static DTD *
dtdCreate(const XML_Memory_Handling_Suite *ms) {
DTD *p = (DTD *)ms->malloc_fcn(sizeof(DTD));
if (p == NULL)
return p;
poolInit(&(p->pool), ms);
poolInit(&(p->entityValuePool), ms);
hashTableInit(&(p->generalEntities), ms);
hashTableInit(&(p->elementTypes), ms);
hashTableInit(&(p->attributeIds), ms);
hashTableInit(&(p->prefixes), ms);
#if defined(XML_DTD)
p->paramEntityRead = XML_FALSE;
hashTableInit(&(p->paramEntities), ms);
#endif
p->defaultPrefix.name = NULL;
p->defaultPrefix.binding = NULL;

p->in_eldecl = XML_FALSE;
p->scaffIndex = NULL;
p->scaffold = NULL;
p->scaffLevel = 0;
p->scaffSize = 0;
p->scaffCount = 0;
p->contentStringLen = 0;

p->keepProcessing = XML_TRUE;
p->hasParamEntityRefs = XML_FALSE;
p->standalone = XML_FALSE;
return p;
}

static void
dtdReset(DTD *p, const XML_Memory_Handling_Suite *ms) {
HASH_TABLE_ITER iter;
hashTableIterInit(&iter, &(p->elementTypes));
for (;;) {
ELEMENT_TYPE *e = (ELEMENT_TYPE *)hashTableIterNext(&iter);
if (! e)
break;
if (e->allocDefaultAtts != 0)
ms->free_fcn(e->defaultAtts);
}
hashTableClear(&(p->generalEntities));
#if defined(XML_DTD)
p->paramEntityRead = XML_FALSE;
hashTableClear(&(p->paramEntities));
#endif
hashTableClear(&(p->elementTypes));
hashTableClear(&(p->attributeIds));
hashTableClear(&(p->prefixes));
poolClear(&(p->pool));
poolClear(&(p->entityValuePool));
p->defaultPrefix.name = NULL;
p->defaultPrefix.binding = NULL;

p->in_eldecl = XML_FALSE;

ms->free_fcn(p->scaffIndex);
p->scaffIndex = NULL;
ms->free_fcn(p->scaffold);
p->scaffold = NULL;

p->scaffLevel = 0;
p->scaffSize = 0;
p->scaffCount = 0;
p->contentStringLen = 0;

p->keepProcessing = XML_TRUE;
p->hasParamEntityRefs = XML_FALSE;
p->standalone = XML_FALSE;
}

static void
dtdDestroy(DTD *p, XML_Bool isDocEntity, const XML_Memory_Handling_Suite *ms) {
HASH_TABLE_ITER iter;
hashTableIterInit(&iter, &(p->elementTypes));
for (;;) {
ELEMENT_TYPE *e = (ELEMENT_TYPE *)hashTableIterNext(&iter);
if (! e)
break;
if (e->allocDefaultAtts != 0)
ms->free_fcn(e->defaultAtts);
}
hashTableDestroy(&(p->generalEntities));
#if defined(XML_DTD)
hashTableDestroy(&(p->paramEntities));
#endif
hashTableDestroy(&(p->elementTypes));
hashTableDestroy(&(p->attributeIds));
hashTableDestroy(&(p->prefixes));
poolDestroy(&(p->pool));
poolDestroy(&(p->entityValuePool));
if (isDocEntity) {
ms->free_fcn(p->scaffIndex);
ms->free_fcn(p->scaffold);
}
ms->free_fcn(p);
}




static int
dtdCopy(XML_Parser oldParser, DTD *newDtd, const DTD *oldDtd,
const XML_Memory_Handling_Suite *ms) {
HASH_TABLE_ITER iter;



hashTableIterInit(&iter, &(oldDtd->prefixes));
for (;;) {
const XML_Char *name;
const PREFIX *oldP = (PREFIX *)hashTableIterNext(&iter);
if (! oldP)
break;
name = poolCopyString(&(newDtd->pool), oldP->name);
if (! name)
return 0;
if (! lookup(oldParser, &(newDtd->prefixes), name, sizeof(PREFIX)))
return 0;
}

hashTableIterInit(&iter, &(oldDtd->attributeIds));



for (;;) {
ATTRIBUTE_ID *newA;
const XML_Char *name;
const ATTRIBUTE_ID *oldA = (ATTRIBUTE_ID *)hashTableIterNext(&iter);

if (! oldA)
break;

if (! poolAppendChar(&(newDtd->pool), XML_T('\0')))
return 0;
name = poolCopyString(&(newDtd->pool), oldA->name);
if (! name)
return 0;
++name;
newA = (ATTRIBUTE_ID *)lookup(oldParser, &(newDtd->attributeIds), name,
sizeof(ATTRIBUTE_ID));
if (! newA)
return 0;
newA->maybeTokenized = oldA->maybeTokenized;
if (oldA->prefix) {
newA->xmlns = oldA->xmlns;
if (oldA->prefix == &oldDtd->defaultPrefix)
newA->prefix = &newDtd->defaultPrefix;
else
newA->prefix = (PREFIX *)lookup(oldParser, &(newDtd->prefixes),
oldA->prefix->name, 0);
}
}



hashTableIterInit(&iter, &(oldDtd->elementTypes));

for (;;) {
int i;
ELEMENT_TYPE *newE;
const XML_Char *name;
const ELEMENT_TYPE *oldE = (ELEMENT_TYPE *)hashTableIterNext(&iter);
if (! oldE)
break;
name = poolCopyString(&(newDtd->pool), oldE->name);
if (! name)
return 0;
newE = (ELEMENT_TYPE *)lookup(oldParser, &(newDtd->elementTypes), name,
sizeof(ELEMENT_TYPE));
if (! newE)
return 0;
if (oldE->nDefaultAtts) {
newE->defaultAtts = (DEFAULT_ATTRIBUTE *)ms->malloc_fcn(
oldE->nDefaultAtts * sizeof(DEFAULT_ATTRIBUTE));
if (! newE->defaultAtts) {
return 0;
}
}
if (oldE->idAtt)
newE->idAtt = (ATTRIBUTE_ID *)lookup(oldParser, &(newDtd->attributeIds),
oldE->idAtt->name, 0);
newE->allocDefaultAtts = newE->nDefaultAtts = oldE->nDefaultAtts;
if (oldE->prefix)
newE->prefix = (PREFIX *)lookup(oldParser, &(newDtd->prefixes),
oldE->prefix->name, 0);
for (i = 0; i < newE->nDefaultAtts; i++) {
newE->defaultAtts[i].id = (ATTRIBUTE_ID *)lookup(
oldParser, &(newDtd->attributeIds), oldE->defaultAtts[i].id->name, 0);
newE->defaultAtts[i].isCdata = oldE->defaultAtts[i].isCdata;
if (oldE->defaultAtts[i].value) {
newE->defaultAtts[i].value
= poolCopyString(&(newDtd->pool), oldE->defaultAtts[i].value);
if (! newE->defaultAtts[i].value)
return 0;
} else
newE->defaultAtts[i].value = NULL;
}
}


if (! copyEntityTable(oldParser, &(newDtd->generalEntities), &(newDtd->pool),
&(oldDtd->generalEntities)))
return 0;

#if defined(XML_DTD)
if (! copyEntityTable(oldParser, &(newDtd->paramEntities), &(newDtd->pool),
&(oldDtd->paramEntities)))
return 0;
newDtd->paramEntityRead = oldDtd->paramEntityRead;
#endif

newDtd->keepProcessing = oldDtd->keepProcessing;
newDtd->hasParamEntityRefs = oldDtd->hasParamEntityRefs;
newDtd->standalone = oldDtd->standalone;


newDtd->in_eldecl = oldDtd->in_eldecl;
newDtd->scaffold = oldDtd->scaffold;
newDtd->contentStringLen = oldDtd->contentStringLen;
newDtd->scaffSize = oldDtd->scaffSize;
newDtd->scaffLevel = oldDtd->scaffLevel;
newDtd->scaffIndex = oldDtd->scaffIndex;

return 1;
}

static int
copyEntityTable(XML_Parser oldParser, HASH_TABLE *newTable,
STRING_POOL *newPool, const HASH_TABLE *oldTable) {
HASH_TABLE_ITER iter;
const XML_Char *cachedOldBase = NULL;
const XML_Char *cachedNewBase = NULL;

hashTableIterInit(&iter, oldTable);

for (;;) {
ENTITY *newE;
const XML_Char *name;
const ENTITY *oldE = (ENTITY *)hashTableIterNext(&iter);
if (! oldE)
break;
name = poolCopyString(newPool, oldE->name);
if (! name)
return 0;
newE = (ENTITY *)lookup(oldParser, newTable, name, sizeof(ENTITY));
if (! newE)
return 0;
if (oldE->systemId) {
const XML_Char *tem = poolCopyString(newPool, oldE->systemId);
if (! tem)
return 0;
newE->systemId = tem;
if (oldE->base) {
if (oldE->base == cachedOldBase)
newE->base = cachedNewBase;
else {
cachedOldBase = oldE->base;
tem = poolCopyString(newPool, cachedOldBase);
if (! tem)
return 0;
cachedNewBase = newE->base = tem;
}
}
if (oldE->publicId) {
tem = poolCopyString(newPool, oldE->publicId);
if (! tem)
return 0;
newE->publicId = tem;
}
} else {
const XML_Char *tem
= poolCopyStringN(newPool, oldE->textPtr, oldE->textLen);
if (! tem)
return 0;
newE->textPtr = tem;
newE->textLen = oldE->textLen;
}
if (oldE->notation) {
const XML_Char *tem = poolCopyString(newPool, oldE->notation);
if (! tem)
return 0;
newE->notation = tem;
}
newE->is_param = oldE->is_param;
newE->is_internal = oldE->is_internal;
}
return 1;
}

#define INIT_POWER 6

static XML_Bool FASTCALL
keyeq(KEY s1, KEY s2) {
for (; *s1 == *s2; s1++, s2++)
if (*s1 == 0)
return XML_TRUE;
return XML_FALSE;
}

static size_t
keylen(KEY s) {
size_t len = 0;
for (; *s; s++, len++)
;
return len;
}

static void
copy_salt_to_sipkey(XML_Parser parser, struct sipkey *key) {
key->k[0] = 0;
key->k[1] = get_hash_secret_salt(parser);
}

static unsigned long FASTCALL
hash(XML_Parser parser, KEY s) {
struct siphash state;
struct sipkey key;
(void)sip24_valid;
copy_salt_to_sipkey(parser, &key);
sip24_init(&state, &key);
sip24_update(&state, s, keylen(s) * sizeof(XML_Char));
return (unsigned long)sip24_final(&state);
}

static NAMED *
lookup(XML_Parser parser, HASH_TABLE *table, KEY name, size_t createSize) {
size_t i;
if (table->size == 0) {
size_t tsize;
if (! createSize)
return NULL;
table->power = INIT_POWER;

table->size = (size_t)1 << INIT_POWER;
tsize = table->size * sizeof(NAMED *);
table->v = (NAMED **)table->mem->malloc_fcn(tsize);
if (! table->v) {
table->size = 0;
return NULL;
}
memset(table->v, 0, tsize);
i = hash(parser, name) & ((unsigned long)table->size - 1);
} else {
unsigned long h = hash(parser, name);
unsigned long mask = (unsigned long)table->size - 1;
unsigned char step = 0;
i = h & mask;
while (table->v[i]) {
if (keyeq(name, table->v[i]->name))
return table->v[i];
if (! step)
step = PROBE_STEP(h, mask, table->power);
i < step ? (i += table->size - step) : (i -= step);
}
if (! createSize)
return NULL;


if (table->used >> (table->power - 1)) {
unsigned char newPower = table->power + 1;
size_t newSize = (size_t)1 << newPower;
unsigned long newMask = (unsigned long)newSize - 1;
size_t tsize = newSize * sizeof(NAMED *);
NAMED **newV = (NAMED **)table->mem->malloc_fcn(tsize);
if (! newV)
return NULL;
memset(newV, 0, tsize);
for (i = 0; i < table->size; i++)
if (table->v[i]) {
unsigned long newHash = hash(parser, table->v[i]->name);
size_t j = newHash & newMask;
step = 0;
while (newV[j]) {
if (! step)
step = PROBE_STEP(newHash, newMask, newPower);
j < step ? (j += newSize - step) : (j -= step);
}
newV[j] = table->v[i];
}
table->mem->free_fcn(table->v);
table->v = newV;
table->power = newPower;
table->size = newSize;
i = h & newMask;
step = 0;
while (table->v[i]) {
if (! step)
step = PROBE_STEP(h, newMask, newPower);
i < step ? (i += newSize - step) : (i -= step);
}
}
}
table->v[i] = (NAMED *)table->mem->malloc_fcn(createSize);
if (! table->v[i])
return NULL;
memset(table->v[i], 0, createSize);
table->v[i]->name = name;
(table->used)++;
return table->v[i];
}

static void FASTCALL
hashTableClear(HASH_TABLE *table) {
size_t i;
for (i = 0; i < table->size; i++) {
table->mem->free_fcn(table->v[i]);
table->v[i] = NULL;
}
table->used = 0;
}

static void FASTCALL
hashTableDestroy(HASH_TABLE *table) {
size_t i;
for (i = 0; i < table->size; i++)
table->mem->free_fcn(table->v[i]);
table->mem->free_fcn(table->v);
}

static void FASTCALL
hashTableInit(HASH_TABLE *p, const XML_Memory_Handling_Suite *ms) {
p->power = 0;
p->size = 0;
p->used = 0;
p->v = NULL;
p->mem = ms;
}

static void FASTCALL
hashTableIterInit(HASH_TABLE_ITER *iter, const HASH_TABLE *table) {
iter->p = table->v;
iter->end = iter->p + table->size;
}

static NAMED *FASTCALL
hashTableIterNext(HASH_TABLE_ITER *iter) {
while (iter->p != iter->end) {
NAMED *tem = *(iter->p)++;
if (tem)
return tem;
}
return NULL;
}

static void FASTCALL
poolInit(STRING_POOL *pool, const XML_Memory_Handling_Suite *ms) {
pool->blocks = NULL;
pool->freeBlocks = NULL;
pool->start = NULL;
pool->ptr = NULL;
pool->end = NULL;
pool->mem = ms;
}

static void FASTCALL
poolClear(STRING_POOL *pool) {
if (! pool->freeBlocks)
pool->freeBlocks = pool->blocks;
else {
BLOCK *p = pool->blocks;
while (p) {
BLOCK *tem = p->next;
p->next = pool->freeBlocks;
pool->freeBlocks = p;
p = tem;
}
}
pool->blocks = NULL;
pool->start = NULL;
pool->ptr = NULL;
pool->end = NULL;
}

static void FASTCALL
poolDestroy(STRING_POOL *pool) {
BLOCK *p = pool->blocks;
while (p) {
BLOCK *tem = p->next;
pool->mem->free_fcn(p);
p = tem;
}
p = pool->freeBlocks;
while (p) {
BLOCK *tem = p->next;
pool->mem->free_fcn(p);
p = tem;
}
}

static XML_Char *
poolAppend(STRING_POOL *pool, const ENCODING *enc, const char *ptr,
const char *end) {
if (! pool->ptr && ! poolGrow(pool))
return NULL;
for (;;) {
const enum XML_Convert_Result convert_res = XmlConvert(
enc, &ptr, end, (ICHAR **)&(pool->ptr), (ICHAR *)pool->end);
if ((convert_res == XML_CONVERT_COMPLETED)
|| (convert_res == XML_CONVERT_INPUT_INCOMPLETE))
break;
if (! poolGrow(pool))
return NULL;
}
return pool->start;
}

static const XML_Char *FASTCALL
poolCopyString(STRING_POOL *pool, const XML_Char *s) {
do {
if (! poolAppendChar(pool, *s))
return NULL;
} while (*s++);
s = pool->start;
poolFinish(pool);
return s;
}

static const XML_Char *
poolCopyStringN(STRING_POOL *pool, const XML_Char *s, int n) {
if (! pool->ptr && ! poolGrow(pool)) {











return NULL;
}
for (; n > 0; --n, s++) {
if (! poolAppendChar(pool, *s))
return NULL;
}
s = pool->start;
poolFinish(pool);
return s;
}

static const XML_Char *FASTCALL
poolAppendString(STRING_POOL *pool, const XML_Char *s) {
while (*s) {
if (! poolAppendChar(pool, *s))
return NULL;
s++;
}
return pool->start;
}

static XML_Char *
poolStoreString(STRING_POOL *pool, const ENCODING *enc, const char *ptr,
const char *end) {
if (! poolAppend(pool, enc, ptr, end))
return NULL;
if (pool->ptr == pool->end && ! poolGrow(pool))
return NULL;
*(pool->ptr)++ = 0;
return pool->start;
}

static size_t
poolBytesToAllocateFor(int blockSize) {







const size_t stretch = sizeof(XML_Char);

if (blockSize <= 0)
return 0;

if (blockSize > (int)(INT_MAX / stretch))
return 0;

{
const int stretchedBlockSize = blockSize * (int)stretch;
const int bytesToAllocate
= (int)(offsetof(BLOCK, s) + (unsigned)stretchedBlockSize);
if (bytesToAllocate < 0)
return 0;

return (size_t)bytesToAllocate;
}
}

static XML_Bool FASTCALL
poolGrow(STRING_POOL *pool) {
if (pool->freeBlocks) {
if (pool->start == 0) {
pool->blocks = pool->freeBlocks;
pool->freeBlocks = pool->freeBlocks->next;
pool->blocks->next = NULL;
pool->start = pool->blocks->s;
pool->end = pool->start + pool->blocks->size;
pool->ptr = pool->start;
return XML_TRUE;
}
if (pool->end - pool->start < pool->freeBlocks->size) {
BLOCK *tem = pool->freeBlocks->next;
pool->freeBlocks->next = pool->blocks;
pool->blocks = pool->freeBlocks;
pool->freeBlocks = tem;
memcpy(pool->blocks->s, pool->start,
(pool->end - pool->start) * sizeof(XML_Char));
pool->ptr = pool->blocks->s + (pool->ptr - pool->start);
pool->start = pool->blocks->s;
pool->end = pool->start + pool->blocks->size;
return XML_TRUE;
}
}
if (pool->blocks && pool->start == pool->blocks->s) {
BLOCK *temp;
int blockSize = (int)((unsigned)(pool->end - pool->start) * 2U);
size_t bytesToAllocate;



const ptrdiff_t offsetInsideBlock = pool->ptr - pool->start;

if (blockSize < 0) {






return XML_FALSE;
}

bytesToAllocate = poolBytesToAllocateFor(blockSize);
if (bytesToAllocate == 0)
return XML_FALSE;

temp = (BLOCK *)pool->mem->realloc_fcn(pool->blocks,
(unsigned)bytesToAllocate);
if (temp == NULL)
return XML_FALSE;
pool->blocks = temp;
pool->blocks->size = blockSize;
pool->ptr = pool->blocks->s + offsetInsideBlock;
pool->start = pool->blocks->s;
pool->end = pool->start + blockSize;
} else {
BLOCK *tem;
int blockSize = (int)(pool->end - pool->start);
size_t bytesToAllocate;

if (blockSize < 0) {









return XML_FALSE;
}

if (blockSize < INIT_BLOCK_SIZE)
blockSize = INIT_BLOCK_SIZE;
else {

if ((int)((unsigned)blockSize * 2U) < 0) {
return XML_FALSE;
}
blockSize *= 2;
}

bytesToAllocate = poolBytesToAllocateFor(blockSize);
if (bytesToAllocate == 0)
return XML_FALSE;

tem = (BLOCK *)pool->mem->malloc_fcn(bytesToAllocate);
if (! tem)
return XML_FALSE;
tem->size = blockSize;
tem->next = pool->blocks;
pool->blocks = tem;
if (pool->ptr != pool->start)
memcpy(tem->s, pool->start, (pool->ptr - pool->start) * sizeof(XML_Char));
pool->ptr = tem->s + (pool->ptr - pool->start);
pool->start = tem->s;
pool->end = tem->s + blockSize;
}
return XML_TRUE;
}

static int FASTCALL
nextScaffoldPart(XML_Parser parser) {
DTD *const dtd = parser->m_dtd;
CONTENT_SCAFFOLD *me;
int next;

if (! dtd->scaffIndex) {
dtd->scaffIndex = (int *)MALLOC(parser, parser->m_groupSize * sizeof(int));
if (! dtd->scaffIndex)
return -1;
dtd->scaffIndex[0] = 0;
}

if (dtd->scaffCount >= dtd->scaffSize) {
CONTENT_SCAFFOLD *temp;
if (dtd->scaffold) {
temp = (CONTENT_SCAFFOLD *)REALLOC(
parser, dtd->scaffold, dtd->scaffSize * 2 * sizeof(CONTENT_SCAFFOLD));
if (temp == NULL)
return -1;
dtd->scaffSize *= 2;
} else {
temp = (CONTENT_SCAFFOLD *)MALLOC(parser, INIT_SCAFFOLD_ELEMENTS
* sizeof(CONTENT_SCAFFOLD));
if (temp == NULL)
return -1;
dtd->scaffSize = INIT_SCAFFOLD_ELEMENTS;
}
dtd->scaffold = temp;
}
next = dtd->scaffCount++;
me = &dtd->scaffold[next];
if (dtd->scaffLevel) {
CONTENT_SCAFFOLD *parent
= &dtd->scaffold[dtd->scaffIndex[dtd->scaffLevel - 1]];
if (parent->lastchild) {
dtd->scaffold[parent->lastchild].nextsib = next;
}
if (! parent->childcnt)
parent->firstchild = next;
parent->lastchild = next;
parent->childcnt++;
}
me->firstchild = me->lastchild = me->childcnt = me->nextsib = 0;
return next;
}

static void
build_node(XML_Parser parser, int src_node, XML_Content *dest,
XML_Content **contpos, XML_Char **strpos) {
DTD *const dtd = parser->m_dtd;
dest->type = dtd->scaffold[src_node].type;
dest->quant = dtd->scaffold[src_node].quant;
if (dest->type == XML_CTYPE_NAME) {
const XML_Char *src;
dest->name = *strpos;
src = dtd->scaffold[src_node].name;
for (;;) {
*(*strpos)++ = *src;
if (! *src)
break;
src++;
}
dest->numchildren = 0;
dest->children = NULL;
} else {
unsigned int i;
int cn;
dest->numchildren = dtd->scaffold[src_node].childcnt;
dest->children = *contpos;
*contpos += dest->numchildren;
for (i = 0, cn = dtd->scaffold[src_node].firstchild; i < dest->numchildren;
i++, cn = dtd->scaffold[cn].nextsib) {
build_node(parser, cn, &(dest->children[i]), contpos, strpos);
}
dest->name = NULL;
}
}

static XML_Content *
build_model(XML_Parser parser) {
DTD *const dtd = parser->m_dtd;
XML_Content *ret;
XML_Content *cpos;
XML_Char *str;
int allocsize = (dtd->scaffCount * sizeof(XML_Content)
+ (dtd->contentStringLen * sizeof(XML_Char)));

ret = (XML_Content *)MALLOC(parser, allocsize);
if (! ret)
return NULL;

str = (XML_Char *)(&ret[dtd->scaffCount]);
cpos = &ret[1];

build_node(parser, 0, ret, &cpos, &str);
return ret;
}

static ELEMENT_TYPE *
getElementType(XML_Parser parser, const ENCODING *enc, const char *ptr,
const char *end) {
DTD *const dtd = parser->m_dtd;
const XML_Char *name = poolStoreString(&dtd->pool, enc, ptr, end);
ELEMENT_TYPE *ret;

if (! name)
return NULL;
ret = (ELEMENT_TYPE *)lookup(parser, &dtd->elementTypes, name,
sizeof(ELEMENT_TYPE));
if (! ret)
return NULL;
if (ret->name != name)
poolDiscard(&dtd->pool);
else {
poolFinish(&dtd->pool);
if (! setElementTypePrefix(parser, ret))
return NULL;
}
return ret;
}

static XML_Char *
copyString(const XML_Char *s, const XML_Memory_Handling_Suite *memsuite) {
int charsRequired = 0;
XML_Char *result;


while (s[charsRequired] != 0) {
charsRequired++;
}

charsRequired++;


result = memsuite->malloc_fcn(charsRequired * sizeof(XML_Char));
if (result == NULL)
return NULL;

memcpy(result, s, charsRequired * sizeof(XML_Char));
return result;
}
