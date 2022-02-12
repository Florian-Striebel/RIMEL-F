#include "rax.h"
#include "listpack.h"
typedef struct streamID {
uint64_t ms;
uint64_t seq;
} streamID;
typedef struct stream {
rax *rax;
uint64_t length;
streamID last_id;
rax *cgroups;
} stream;
typedef struct streamIterator {
stream *stream;
streamID master_id;
uint64_t master_fields_count;
unsigned char *master_fields_start;
unsigned char *master_fields_ptr;
int entry_flags;
int rev;
uint64_t start_key[2];
uint64_t end_key[2];
raxIterator ri;
unsigned char *lp;
unsigned char *lp_ele;
unsigned char *lp_flags;
unsigned char field_buf[LP_INTBUF_SIZE];
unsigned char value_buf[LP_INTBUF_SIZE];
} streamIterator;
typedef struct streamCG {
streamID last_id;
rax *pel;
rax *consumers;
} streamCG;
typedef struct streamConsumer {
mstime_t seen_time;
sds name;
rax *pel;
} streamConsumer;
typedef struct streamNACK {
mstime_t delivery_time;
uint64_t delivery_count;
streamConsumer *consumer;
} streamNACK;
typedef struct streamPropInfo {
robj *keyname;
robj *groupname;
} streamPropInfo;
struct client;
#define SLC_DEFAULT 0
#define SLC_NO_REFRESH (1<<0)
#define SCC_DEFAULT 0
#define SCC_NO_NOTIFY (1<<0)
#define SCC_NO_DIRTIFY (1<<1)
stream *streamNew(void);
void freeStream(stream *s);
unsigned long streamLength(const robj *subject);
size_t streamReplyWithRange(client *c, stream *s, streamID *start, streamID *end, size_t count, int rev, streamCG *group, streamConsumer *consumer, int flags, streamPropInfo *spi);
void streamIteratorStart(streamIterator *si, stream *s, streamID *start, streamID *end, int rev);
int streamIteratorGetID(streamIterator *si, streamID *id, int64_t *numfields);
void streamIteratorGetField(streamIterator *si, unsigned char **fieldptr, unsigned char **valueptr, int64_t *fieldlen, int64_t *valuelen);
void streamIteratorRemoveEntry(streamIterator *si, streamID *current);
void streamIteratorStop(streamIterator *si);
streamCG *streamLookupCG(stream *s, sds groupname);
streamConsumer *streamLookupConsumer(streamCG *cg, sds name, int flags);
streamConsumer *streamCreateConsumer(streamCG *cg, sds name, robj *key, int dbid, int flags);
streamCG *streamCreateCG(stream *s, char *name, size_t namelen, streamID *id);
streamNACK *streamCreateNACK(streamConsumer *consumer);
void streamDecodeID(void *buf, streamID *id);
int streamCompareID(streamID *a, streamID *b);
void streamFreeNACK(streamNACK *na);
int streamIncrID(streamID *id);
int streamDecrID(streamID *id);
void streamPropagateConsumerCreation(client *c, robj *key, robj *groupname, sds consumername);
robj *streamDup(robj *o);
int streamValidateListpackIntegrity(unsigned char *lp, size_t size, int deep);
int streamParseID(const robj *o, streamID *id);
robj *createObjectFromStreamID(streamID *id);
int streamAppendItem(stream *s, robj **argv, int64_t numfields, streamID *added_id, streamID *use_id, int seq_given);
int streamDeleteItem(stream *s, streamID *id);
int64_t streamTrimByLength(stream *s, long long maxlen, int approx);
int64_t streamTrimByID(stream *s, streamID minid, int approx);
