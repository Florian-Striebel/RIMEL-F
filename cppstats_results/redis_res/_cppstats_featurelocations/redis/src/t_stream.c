




























#include "server.h"
#include "endianconv.h"
#include "stream.h"




#define STREAM_ITEM_FLAG_NONE 0
#define STREAM_ITEM_FLAG_DELETED (1<<0)
#define STREAM_ITEM_FLAG_SAMEFIELDS (1<<1)




#define STREAMID_STATIC_VECTOR_LEN 8



#define STREAM_LISTPACK_MAX_PRE_ALLOCATE 4096





#define STREAM_LISTPACK_MAX_SIZE (1<<30)

void streamFreeCG(streamCG *cg);
void streamFreeNACK(streamNACK *na);
size_t streamReplyWithRangeFromConsumerPEL(client *c, stream *s, streamID *start, streamID *end, size_t count, streamConsumer *consumer);
int streamParseStrictIDOrReply(client *c, robj *o, streamID *id, uint64_t missing_seq, int *seq_given);
int streamParseIDOrReply(client *c, robj *o, streamID *id, uint64_t missing_seq);






stream *streamNew(void) {
stream *s = zmalloc(sizeof(*s));
s->rax = raxNew();
s->length = 0;
s->last_id.ms = 0;
s->last_id.seq = 0;
s->cgroups = NULL;
return s;
}


void freeStream(stream *s) {
raxFreeWithCallback(s->rax,(void(*)(void*))lpFree);
if (s->cgroups)
raxFreeWithCallback(s->cgroups,(void(*)(void*))streamFreeCG);
zfree(s);
}


unsigned long streamLength(const robj *subject) {
stream *s = subject->ptr;
return s->length;
}




int streamIncrID(streamID *id) {
int ret = C_OK;
if (id->seq == UINT64_MAX) {
if (id->ms == UINT64_MAX) {

id->ms = id->seq = 0;
ret = C_ERR;
} else {
id->ms++;
id->seq = 0;
}
} else {
id->seq++;
}
return ret;
}




int streamDecrID(streamID *id) {
int ret = C_OK;
if (id->seq == 0) {
if (id->ms == 0) {

id->ms = id->seq = UINT64_MAX;
ret = C_ERR;
} else {
id->ms--;
id->seq = UINT64_MAX;
}
} else {
id->seq--;
}
return ret;
}





void streamNextID(streamID *last_id, streamID *new_id) {
uint64_t ms = mstime();
if (ms > last_id->ms) {
new_id->ms = ms;
new_id->seq = 0;
} else {
*new_id = *last_id;
streamIncrID(new_id);
}
}






robj *streamDup(robj *o) {
robj *sobj;

serverAssert(o->type == OBJ_STREAM);

switch (o->encoding) {
case OBJ_ENCODING_STREAM:
sobj = createStreamObject();
break;
default:
serverPanic("Wrong encoding.");
break;
}

stream *s;
stream *new_s;
s = o->ptr;
new_s = sobj->ptr;

raxIterator ri;
uint64_t rax_key[2];
raxStart(&ri, s->rax);
raxSeek(&ri, "^", NULL, 0);
size_t lp_bytes = 0;
unsigned char *lp = NULL;

while (raxNext(&ri)) {
lp = ri.data;
lp_bytes = lpBytes(lp);
unsigned char *new_lp = zmalloc(lp_bytes);
memcpy(new_lp, lp, lp_bytes);
memcpy(rax_key, ri.key, sizeof(rax_key));
raxInsert(new_s->rax, (unsigned char *)&rax_key, sizeof(rax_key),
new_lp, NULL);
}
new_s->length = s->length;
new_s->last_id = s->last_id;
raxStop(&ri);

if (s->cgroups == NULL) return sobj;


raxIterator ri_cgroups;
raxStart(&ri_cgroups, s->cgroups);
raxSeek(&ri_cgroups, "^", NULL, 0);
while (raxNext(&ri_cgroups)) {
streamCG *cg = ri_cgroups.data;
streamCG *new_cg = streamCreateCG(new_s, (char *)ri_cgroups.key,
ri_cgroups.key_len, &cg->last_id);

serverAssert(new_cg != NULL);


raxIterator ri_cg_pel;
raxStart(&ri_cg_pel,cg->pel);
raxSeek(&ri_cg_pel,"^",NULL,0);
while(raxNext(&ri_cg_pel)){
streamNACK *nack = ri_cg_pel.data;
streamNACK *new_nack = streamCreateNACK(NULL);
new_nack->delivery_time = nack->delivery_time;
new_nack->delivery_count = nack->delivery_count;
raxInsert(new_cg->pel, ri_cg_pel.key, sizeof(streamID), new_nack, NULL);
}
raxStop(&ri_cg_pel);


raxIterator ri_consumers;
raxStart(&ri_consumers, cg->consumers);
raxSeek(&ri_consumers, "^", NULL, 0);
while (raxNext(&ri_consumers)) {
streamConsumer *consumer = ri_consumers.data;
streamConsumer *new_consumer;
new_consumer = zmalloc(sizeof(*new_consumer));
new_consumer->name = sdsdup(consumer->name);
new_consumer->pel = raxNew();
raxInsert(new_cg->consumers,(unsigned char *)new_consumer->name,
sdslen(new_consumer->name), new_consumer, NULL);
new_consumer->seen_time = consumer->seen_time;


raxIterator ri_cpel;
raxStart(&ri_cpel, consumer->pel);
raxSeek(&ri_cpel, "^", NULL, 0);
while (raxNext(&ri_cpel)) {
streamNACK *new_nack = raxFind(new_cg->pel,ri_cpel.key,sizeof(streamID));

serverAssert(new_nack != raxNotFound);

new_nack->consumer = new_consumer;
raxInsert(new_consumer->pel,ri_cpel.key,sizeof(streamID),new_nack,NULL);
}
raxStop(&ri_cpel);
}
raxStop(&ri_consumers);
}
raxStop(&ri_cgroups);
return sobj;
}







static inline int64_t lpGetIntegerIfValid(unsigned char *ele, int *valid) {
int64_t v;
unsigned char *e = lpGet(ele,&v,NULL);
if (e == NULL) {
if (valid)
*valid = 1;
return v;
}



long long ll;
int ret = string2ll((char*)e,v,&ll);
if (valid)
*valid = ret;
else
serverAssert(ret != 0);
v = ll;
return v;
}

#define lpGetInteger(ele) lpGetIntegerIfValid(ele, NULL)



int lpGetEdgeStreamID(unsigned char *lp, int first, streamID *master_id, streamID *edge_id)
{
if (lp == NULL)
return 0;

unsigned char *lp_ele;



if (first) {

lp_ele = lpFirst(lp);
lp_ele = lpNext(lp, lp_ele);
lp_ele = lpNext(lp, lp_ele);
int64_t master_fields_count = lpGetInteger(lp_ele);
lp_ele = lpNext(lp, lp_ele);



for (int64_t i = 0; i < master_fields_count; i++)
lp_ele = lpNext(lp, lp_ele);




lp_ele = lpNext(lp, lp_ele);
if (lp_ele == NULL)
return 0;
} else {



lp_ele = lpLast(lp);




int64_t lp_count = lpGetInteger(lp_ele);
if (lp_count == 0)
return 0;

while (lp_count--)
lp_ele = lpPrev(lp, lp_ele);
}

lp_ele = lpNext(lp, lp_ele);



streamID id = *master_id;
id.ms += lpGetInteger(lp_ele);
lp_ele = lpNext(lp, lp_ele);
id.seq += lpGetInteger(lp_ele);
*edge_id = id;
return 1;
}



void streamLogListpackContent(unsigned char *lp) {
unsigned char *p = lpFirst(lp);
while(p) {
unsigned char buf[LP_INTBUF_SIZE];
int64_t v;
unsigned char *ele = lpGet(p,&v,buf);
serverLog(LL_WARNING,"- [%d] '%.*s'", (int)v, (int)v, ele);
p = lpNext(lp,p);
}
}



void streamEncodeID(void *buf, streamID *id) {
uint64_t e[2];
e[0] = htonu64(id->ms);
e[1] = htonu64(id->seq);
memcpy(buf,e,sizeof(e));
}




void streamDecodeID(void *buf, streamID *id) {
uint64_t e[2];
memcpy(e,buf,sizeof(e));
id->ms = ntohu64(e[0]);
id->seq = ntohu64(e[1]);
}


int streamCompareID(streamID *a, streamID *b) {
if (a->ms > b->ms) return 1;
else if (a->ms < b->ms) return -1;

else if (a->seq > b->seq) return 1;
else if (a->seq < b->seq) return -1;

return 0;
}

void streamGetEdgeID(stream *s, int first, streamID *edge_id)
{
raxIterator ri;
raxStart(&ri, s->rax);
int empty;
if (first) {
raxSeek(&ri, "^", NULL, 0);
empty = !raxNext(&ri);
} else {
raxSeek(&ri, "$", NULL, 0);
empty = !raxPrev(&ri);
}

if (empty) {

edge_id->ms = first ? UINT64_MAX : 0;
edge_id->seq = first ? UINT64_MAX : 0;
raxStop(&ri);
return;
}

unsigned char *lp = ri.data;


streamID master_id;
streamDecodeID(ri.key, &master_id);


lpGetEdgeStreamID(lp, first, &master_id, edge_id);

raxStop(&ri);
}




















int streamAppendItem(stream *s, robj **argv, int64_t numfields, streamID *added_id, streamID *use_id, int seq_given) {


streamID id;
if (use_id) {
if (seq_given) {
id = *use_id;
} else {




if (s->last_id.ms == use_id->ms) {
if (s->last_id.seq == UINT64_MAX) {
return C_ERR;
}
id = s->last_id;
id.seq++;
} else {
id = *use_id;
}
}
} else {
streamNextID(&s->last_id,&id);
}





if (streamCompareID(&id,&s->last_id) <= 0) {
errno = EDOM;
return C_ERR;
}




size_t totelelen = 0;
for (int64_t i = 0; i < numfields*2; i++) {
sds ele = argv[i]->ptr;
totelelen += sdslen(ele);
}
if (totelelen > STREAM_LISTPACK_MAX_SIZE) {
errno = ERANGE;
return C_ERR;
}


raxIterator ri;
raxStart(&ri,s->rax);
raxSeek(&ri,"$",NULL,0);

size_t lp_bytes = 0;
unsigned char *lp = NULL;

if (!raxEOF(&ri)) {

lp = ri.data;
lp_bytes = lpBytes(lp);
}
raxStop(&ri);




uint64_t rax_key[2];
streamID master_id;




































if (lp != NULL) {
size_t node_max_bytes = server.stream_node_max_bytes;
if (node_max_bytes == 0 || node_max_bytes > STREAM_LISTPACK_MAX_SIZE)
node_max_bytes = STREAM_LISTPACK_MAX_SIZE;
if (lp_bytes + totelelen >= node_max_bytes) {
lp = NULL;
} else if (server.stream_node_max_entries) {
unsigned char *lp_ele = lpFirst(lp);

int64_t count = lpGetInteger(lp_ele) + lpGetInteger(lpNext(lp,lp_ele));
if (count >= server.stream_node_max_entries) {

lp = lpShrinkToFit(lp);
if (ri.data != lp)
raxInsert(s->rax,ri.key,ri.key_len,lp,NULL);
lp = NULL;
}
}
}

int flags = STREAM_ITEM_FLAG_NONE;
if (lp == NULL) {
master_id = id;
streamEncodeID(rax_key,&id);






size_t prealloc = STREAM_LISTPACK_MAX_PRE_ALLOCATE;
if (server.stream_node_max_bytes > 0 && server.stream_node_max_bytes < prealloc) {
prealloc = server.stream_node_max_bytes;
}
lp = lpNew(prealloc);
lp = lpAppendInteger(lp,1);
lp = lpAppendInteger(lp,0);
lp = lpAppendInteger(lp,numfields);
for (int64_t i = 0; i < numfields; i++) {
sds field = argv[i*2]->ptr;
lp = lpAppend(lp,(unsigned char*)field,sdslen(field));
}
lp = lpAppendInteger(lp,0);
raxInsert(s->rax,(unsigned char*)&rax_key,sizeof(rax_key),lp,NULL);


flags |= STREAM_ITEM_FLAG_SAMEFIELDS;
} else {
serverAssert(ri.key_len == sizeof(rax_key));
memcpy(rax_key,ri.key,sizeof(rax_key));


streamDecodeID(rax_key,&master_id);
unsigned char *lp_ele = lpFirst(lp);


int64_t count = lpGetInteger(lp_ele);
lp = lpReplaceInteger(lp,&lp_ele,count+1);
lp_ele = lpNext(lp,lp_ele);
lp_ele = lpNext(lp,lp_ele);



int64_t master_fields_count = lpGetInteger(lp_ele);
lp_ele = lpNext(lp,lp_ele);
if (numfields == master_fields_count) {
int64_t i;
for (i = 0; i < master_fields_count; i++) {
sds field = argv[i*2]->ptr;
int64_t e_len;
unsigned char buf[LP_INTBUF_SIZE];
unsigned char *e = lpGet(lp_ele,&e_len,buf);

if (sdslen(field) != (size_t)e_len ||
memcmp(e,field,e_len) != 0) break;
lp_ele = lpNext(lp,lp_ele);
}


if (i == master_fields_count) flags |= STREAM_ITEM_FLAG_SAMEFIELDS;
}
}























lp = lpAppendInteger(lp,flags);
lp = lpAppendInteger(lp,id.ms - master_id.ms);
lp = lpAppendInteger(lp,id.seq - master_id.seq);
if (!(flags & STREAM_ITEM_FLAG_SAMEFIELDS))
lp = lpAppendInteger(lp,numfields);
for (int64_t i = 0; i < numfields; i++) {
sds field = argv[i*2]->ptr, value = argv[i*2+1]->ptr;
if (!(flags & STREAM_ITEM_FLAG_SAMEFIELDS))
lp = lpAppend(lp,(unsigned char*)field,sdslen(field));
lp = lpAppend(lp,(unsigned char*)value,sdslen(value));
}

int64_t lp_count = numfields;
lp_count += 3;
if (!(flags & STREAM_ITEM_FLAG_SAMEFIELDS)) {


lp_count += numfields+1;
}
lp = lpAppendInteger(lp,lp_count);


if (ri.data != lp)
raxInsert(s->rax,(unsigned char*)&rax_key,sizeof(rax_key),lp,NULL);
s->length++;
s->last_id = id;
if (added_id) *added_id = id;
return C_OK;
}

typedef struct {

streamID id;
int id_given;
int seq_given;
int no_mkstream;


int trim_strategy;
int trim_strategy_arg_idx;
int approx_trim;

long long limit;


long long maxlen;

streamID minid;
} streamAddTrimArgs;

#define TRIM_STRATEGY_NONE 0
#define TRIM_STRATEGY_MAXLEN 1
#define TRIM_STRATEGY_MINID 2
























int64_t streamTrim(stream *s, streamAddTrimArgs *args) {
size_t maxlen = args->maxlen;
streamID *id = &args->minid;
int approx = args->approx_trim;
int64_t limit = args->limit;
int trim_strategy = args->trim_strategy;

if (trim_strategy == TRIM_STRATEGY_NONE)
return 0;

raxIterator ri;
raxStart(&ri,s->rax);
raxSeek(&ri,"^",NULL,0);

int64_t deleted = 0;
while (raxNext(&ri)) {
if (trim_strategy == TRIM_STRATEGY_MAXLEN && s->length <= maxlen)
break;

unsigned char *lp = ri.data, *p = lpFirst(lp);
int64_t entries = lpGetInteger(p);


if (limit && (deleted + entries) > limit)
break;


int remove_node;
streamID master_id = {0};
if (trim_strategy == TRIM_STRATEGY_MAXLEN) {
remove_node = s->length - entries >= maxlen;
} else {

streamDecodeID(ri.key, &master_id);


streamID last_id;
lpGetEdgeStreamID(lp, 0, &master_id, &last_id);


remove_node = streamCompareID(&last_id, id) < 0;
}

if (remove_node) {
lpFree(lp);
raxRemove(s->rax,ri.key,ri.key_len,NULL);
raxSeek(&ri,">=",ri.key,ri.key_len);
s->length -= entries;
deleted += entries;
continue;
}



if (approx) break;


int64_t deleted_from_lp = 0;

p = lpNext(lp, p);
p = lpNext(lp, p);


int64_t master_fields_count = lpGetInteger(p);
p = lpNext(lp,p);
for (int64_t j = 0; j < master_fields_count; j++)
p = lpNext(lp,p);
p = lpNext(lp,p);




while (p) {


unsigned char *pcopy = p;

int64_t flags = lpGetInteger(p);
p = lpNext(lp, p);
int64_t to_skip;

int64_t ms_delta = lpGetInteger(p);
p = lpNext(lp, p);
int64_t seq_delta = lpGetInteger(p);
p = lpNext(lp, p);

streamID currid = {0};
if (trim_strategy == TRIM_STRATEGY_MINID) {
currid.ms = master_id.ms + ms_delta;
currid.seq = master_id.seq + seq_delta;
}

int stop;
if (trim_strategy == TRIM_STRATEGY_MAXLEN) {
stop = s->length <= maxlen;
} else {


stop = streamCompareID(&currid, id) >= 0;
}
if (stop)
break;

if (flags & STREAM_ITEM_FLAG_SAMEFIELDS) {
to_skip = master_fields_count;
} else {
to_skip = lpGetInteger(p);
p = lpNext(lp,p);
to_skip *= 2;
}

while(to_skip--) p = lpNext(lp,p);
p = lpNext(lp,p);


if (!(flags & STREAM_ITEM_FLAG_DELETED)) {
intptr_t delta = p - lp;
flags |= STREAM_ITEM_FLAG_DELETED;
lp = lpReplaceInteger(lp, &pcopy, flags);
deleted_from_lp++;
s->length--;
p = lp + delta;
}
}
deleted += deleted_from_lp;


p = lpFirst(lp);
lp = lpReplaceInteger(lp,&p,entries-deleted_from_lp);
p = lpNext(lp,p);
int64_t marked_deleted = lpGetInteger(p);
lp = lpReplaceInteger(lp,&p,marked_deleted+deleted_from_lp);
p = lpNext(lp,p);



entries -= deleted_from_lp;
marked_deleted += deleted_from_lp;
if (entries + marked_deleted > 10 && marked_deleted > entries/2) {

}


raxInsert(s->rax,ri.key,ri.key_len,lp,NULL);

break;

}

raxStop(&ri);
return deleted;
}


int64_t streamTrimByLength(stream *s, long long maxlen, int approx) {
streamAddTrimArgs args = {
.trim_strategy = TRIM_STRATEGY_MAXLEN,
.approx_trim = approx,
.limit = approx ? 100 * server.stream_node_max_entries : 0,
.maxlen = maxlen
};
return streamTrim(s, &args);
}


int64_t streamTrimByID(stream *s, streamID minid, int approx) {
streamAddTrimArgs args = {
.trim_strategy = TRIM_STRATEGY_MINID,
.approx_trim = approx,
.limit = approx ? 100 * server.stream_node_max_entries : 0,
.minid = minid
};
return streamTrim(s, &args);
}







static int streamParseAddOrTrimArgsOrReply(client *c, streamAddTrimArgs *args, int xadd) {

memset(args, 0, sizeof(*args));


int i = 2;

int limit_given = 0;
for (; i < c->argc; i++) {
int moreargs = (c->argc-1) - i;
char *opt = c->argv[i]->ptr;
if (xadd && opt[0] == '*' && opt[1] == '\0') {


break;
} else if (!strcasecmp(opt,"maxlen") && moreargs) {
if (args->trim_strategy != TRIM_STRATEGY_NONE) {
addReplyError(c,"syntax error, MAXLEN and MINID options at the same time are not compatible");
return -1;
}
args->approx_trim = 0;
char *next = c->argv[i+1]->ptr;

if (moreargs >= 2 && next[0] == '~' && next[1] == '\0') {
args->approx_trim = 1;
i++;
} else if (moreargs >= 2 && next[0] == '=' && next[1] == '\0') {
i++;
}
if (getLongLongFromObjectOrReply(c,c->argv[i+1],&args->maxlen,NULL)
!= C_OK) return -1;

if (args->maxlen < 0) {
addReplyError(c,"The MAXLEN argument must be >= 0.");
return -1;
}
i++;
args->trim_strategy = TRIM_STRATEGY_MAXLEN;
args->trim_strategy_arg_idx = i;
} else if (!strcasecmp(opt,"minid") && moreargs) {
if (args->trim_strategy != TRIM_STRATEGY_NONE) {
addReplyError(c,"syntax error, MAXLEN and MINID options at the same time are not compatible");
return -1;
}
args->approx_trim = 0;
char *next = c->argv[i+1]->ptr;

if (moreargs >= 2 && next[0] == '~' && next[1] == '\0') {
args->approx_trim = 1;
i++;
} else if (moreargs >= 2 && next[0] == '=' && next[1] == '\0') {
i++;
}

if (streamParseStrictIDOrReply(c,c->argv[i+1],&args->minid,0,NULL) != C_OK)
return -1;

i++;
args->trim_strategy = TRIM_STRATEGY_MINID;
args->trim_strategy_arg_idx = i;
} else if (!strcasecmp(opt,"limit") && moreargs) {






if (getLongLongFromObjectOrReply(c,c->argv[i+1],&args->limit,NULL) != C_OK)
return -1;

if (args->limit < 0) {
addReplyError(c,"The LIMIT argument must be >= 0.");
return -1;
}
limit_given = 1;
i++;
} else if (xadd && !strcasecmp(opt,"nomkstream")) {
args->no_mkstream = 1;
} else if (xadd) {

if (streamParseStrictIDOrReply(c,c->argv[i],&args->id,0,&args->seq_given) != C_OK)
return -1;
args->id_given = 1;
break;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return -1;
}
}

if (args->limit && args->trim_strategy == TRIM_STRATEGY_NONE) {
addReplyError(c,"syntax error, LIMIT cannot be used without specifying a trimming strategy");
return -1;
}

if (!xadd && args->trim_strategy == TRIM_STRATEGY_NONE) {
addReplyError(c,"syntax error, XTRIM must be called with a trimming strategy");
return -1;
}

if (c == server.master || c->id == CLIENT_ID_AOF) {



args->limit = 0;
} else {

if (limit_given) {
if (!args->approx_trim) {

addReplyError(c,"syntax error, LIMIT cannot be used without the special ~ option");
return -1;
}
} else {

if (args->approx_trim) {





args->limit = 100 * server.stream_node_max_entries;
if (args->limit <= 0) args->limit = 10000;
if (args->limit > 1000000) args->limit = 1000000;
} else {

args->limit = 0;
}
}
}

return i;
}






















void streamIteratorStart(streamIterator *si, stream *s, streamID *start, streamID *end, int rev) {


if (start) {
streamEncodeID(si->start_key,start);
} else {
si->start_key[0] = 0;
si->start_key[1] = 0;
}

if (end) {
streamEncodeID(si->end_key,end);
} else {
si->end_key[0] = UINT64_MAX;
si->end_key[1] = UINT64_MAX;
}


raxStart(&si->ri,s->rax);
if (!rev) {
if (start && (start->ms || start->seq)) {
raxSeek(&si->ri,"<=",(unsigned char*)si->start_key,
sizeof(si->start_key));
if (raxEOF(&si->ri)) raxSeek(&si->ri,"^",NULL,0);
} else {
raxSeek(&si->ri,"^",NULL,0);
}
} else {
if (end && (end->ms || end->seq)) {
raxSeek(&si->ri,"<=",(unsigned char*)si->end_key,
sizeof(si->end_key));
if (raxEOF(&si->ri)) raxSeek(&si->ri,"$",NULL,0);
} else {
raxSeek(&si->ri,"$",NULL,0);
}
}
si->stream = s;
si->lp = NULL;
si->lp_ele = NULL;
si->rev = rev;
}




int streamIteratorGetID(streamIterator *si, streamID *id, int64_t *numfields) {
while(1) {



if (si->lp == NULL || si->lp_ele == NULL) {
if (!si->rev && !raxNext(&si->ri)) return 0;
else if (si->rev && !raxPrev(&si->ri)) return 0;
serverAssert(si->ri.key_len == sizeof(streamID));

streamDecodeID(si->ri.key,&si->master_id);

si->lp = si->ri.data;
si->lp_ele = lpFirst(si->lp);
si->lp_ele = lpNext(si->lp,si->lp_ele);
si->lp_ele = lpNext(si->lp,si->lp_ele);
si->master_fields_count = lpGetInteger(si->lp_ele);
si->lp_ele = lpNext(si->lp,si->lp_ele);
si->master_fields_start = si->lp_ele;



if (!si->rev) {


for (uint64_t i = 0; i < si->master_fields_count; i++)
si->lp_ele = lpNext(si->lp,si->lp_ele);
} else {



si->lp_ele = lpLast(si->lp);
}
} else if (si->rev) {




int64_t lp_count = lpGetInteger(si->lp_ele);
while(lp_count--) si->lp_ele = lpPrev(si->lp,si->lp_ele);

si->lp_ele = lpPrev(si->lp,si->lp_ele);
}



while(1) {
if (!si->rev) {



si->lp_ele = lpNext(si->lp,si->lp_ele);
if (si->lp_ele == NULL) break;
} else {



int64_t lp_count = lpGetInteger(si->lp_ele);
if (lp_count == 0) {
si->lp = NULL;
si->lp_ele = NULL;
break;
}
while(lp_count--) si->lp_ele = lpPrev(si->lp,si->lp_ele);
}


si->lp_flags = si->lp_ele;
int64_t flags = lpGetInteger(si->lp_ele);
si->lp_ele = lpNext(si->lp,si->lp_ele);



*id = si->master_id;
id->ms += lpGetInteger(si->lp_ele);
si->lp_ele = lpNext(si->lp,si->lp_ele);
id->seq += lpGetInteger(si->lp_ele);
si->lp_ele = lpNext(si->lp,si->lp_ele);
unsigned char buf[sizeof(streamID)];
streamEncodeID(buf,id);



if (flags & STREAM_ITEM_FLAG_SAMEFIELDS) {
*numfields = si->master_fields_count;
} else {
*numfields = lpGetInteger(si->lp_ele);
si->lp_ele = lpNext(si->lp,si->lp_ele);
}
serverAssert(*numfields>=0);



if (!si->rev) {
if (memcmp(buf,si->start_key,sizeof(streamID)) >= 0 &&
!(flags & STREAM_ITEM_FLAG_DELETED))
{
if (memcmp(buf,si->end_key,sizeof(streamID)) > 0)
return 0;
si->entry_flags = flags;
if (flags & STREAM_ITEM_FLAG_SAMEFIELDS)
si->master_fields_ptr = si->master_fields_start;
return 1;
}
} else {
if (memcmp(buf,si->end_key,sizeof(streamID)) <= 0 &&
!(flags & STREAM_ITEM_FLAG_DELETED))
{
if (memcmp(buf,si->start_key,sizeof(streamID)) < 0)
return 0;
si->entry_flags = flags;
if (flags & STREAM_ITEM_FLAG_SAMEFIELDS)
si->master_fields_ptr = si->master_fields_start;
return 1;
}
}




if (!si->rev) {
int64_t to_discard = (flags & STREAM_ITEM_FLAG_SAMEFIELDS) ?
*numfields : *numfields*2;
for (int64_t i = 0; i < to_discard; i++)
si->lp_ele = lpNext(si->lp,si->lp_ele);
} else {
int64_t prev_times = 4;




if (!(flags & STREAM_ITEM_FLAG_SAMEFIELDS)) prev_times++;
while(prev_times--) si->lp_ele = lpPrev(si->lp,si->lp_ele);
}
}


}
}







void streamIteratorGetField(streamIterator *si, unsigned char **fieldptr, unsigned char **valueptr, int64_t *fieldlen, int64_t *valuelen) {
if (si->entry_flags & STREAM_ITEM_FLAG_SAMEFIELDS) {
*fieldptr = lpGet(si->master_fields_ptr,fieldlen,si->field_buf);
si->master_fields_ptr = lpNext(si->lp,si->master_fields_ptr);
} else {
*fieldptr = lpGet(si->lp_ele,fieldlen,si->field_buf);
si->lp_ele = lpNext(si->lp,si->lp_ele);
}
*valueptr = lpGet(si->lp_ele,valuelen,si->value_buf);
si->lp_ele = lpNext(si->lp,si->lp_ele);
}











void streamIteratorRemoveEntry(streamIterator *si, streamID *current) {
unsigned char *lp = si->lp;
int64_t aux;






int64_t flags = lpGetInteger(si->lp_flags);
flags |= STREAM_ITEM_FLAG_DELETED;
lp = lpReplaceInteger(lp,&si->lp_flags,flags);


unsigned char *p = lpFirst(lp);
aux = lpGetInteger(p);

if (aux == 1) {


lpFree(lp);
raxRemove(si->stream->rax,si->ri.key,si->ri.key_len,NULL);
} else {

lp = lpReplaceInteger(lp,&p,aux-1);
p = lpNext(lp,p);
aux = lpGetInteger(p);
lp = lpReplaceInteger(lp,&p,aux+1);


if (si->lp != lp)
raxInsert(si->stream->rax,si->ri.key,si->ri.key_len,lp,NULL);
}


si->stream->length--;


streamID start, end;
if (si->rev) {
streamDecodeID(si->start_key,&start);
end = *current;
} else {
start = *current;
streamDecodeID(si->end_key,&end);
}
streamIteratorStop(si);
streamIteratorStart(si,si->stream,&start,&end,si->rev);



}




void streamIteratorStop(streamIterator *si) {
raxStop(&si->ri);
}



int streamDeleteItem(stream *s, streamID *id) {
int deleted = 0;
streamIterator si;
streamIteratorStart(&si,s,id,id,0);
streamID myid;
int64_t numfields;
if (streamIteratorGetID(&si,&myid,&numfields)) {
streamIteratorRemoveEntry(&si,&myid);
deleted = 1;
}
streamIteratorStop(&si);
return deleted;
}


void streamLastValidID(stream *s, streamID *maxid)
{
streamIterator si;
streamIteratorStart(&si,s,NULL,NULL,1);
int64_t numfields;
if (!streamIteratorGetID(&si,maxid,&numfields) && s->length)
serverPanic("Corrupt stream, length is %llu, but no max id", (unsigned long long)s->length);
streamIteratorStop(&si);
}




void addReplyStreamID(client *c, streamID *id) {
sds replyid = sdscatfmt(sdsempty(),"%U-%U",id->ms,id->seq);
addReplyBulkSds(c,replyid);
}

void setDeferredReplyStreamID(client *c, void *dr, streamID *id) {
sds replyid = sdscatfmt(sdsempty(),"%U-%U",id->ms,id->seq);
setDeferredReplyBulkSds(c, dr, replyid);
}



robj *createObjectFromStreamID(streamID *id) {
return createObject(OBJ_STRING, sdscatfmt(sdsempty(),"%U-%U",
id->ms,id->seq));
}




void streamPropagateXCLAIM(client *c, robj *key, streamCG *group, robj *groupname, robj *id, streamNACK *nack) {







robj *argv[14];
argv[0] = shared.xclaim;
argv[1] = key;
argv[2] = groupname;
argv[3] = createStringObject(nack->consumer->name,sdslen(nack->consumer->name));
argv[4] = shared.integers[0];
argv[5] = id;
argv[6] = shared.time;
argv[7] = createStringObjectFromLongLong(nack->delivery_time);
argv[8] = shared.retrycount;
argv[9] = createStringObjectFromLongLong(nack->delivery_count);
argv[10] = shared.force;
argv[11] = shared.justid;
argv[12] = shared.lastid;
argv[13] = createObjectFromStreamID(&group->last_id);

alsoPropagate(c->db->id,argv,14,PROPAGATE_AOF|PROPAGATE_REPL);

decrRefCount(argv[3]);
decrRefCount(argv[7]);
decrRefCount(argv[9]);
decrRefCount(argv[13]);
}







void streamPropagateGroupID(client *c, robj *key, streamCG *group, robj *groupname) {
robj *argv[5];
argv[0] = shared.xgroup;
argv[1] = shared.setid;
argv[2] = key;
argv[3] = groupname;
argv[4] = createObjectFromStreamID(&group->last_id);

alsoPropagate(c->db->id,argv,5,PROPAGATE_AOF|PROPAGATE_REPL);

decrRefCount(argv[4]);
}







void streamPropagateConsumerCreation(client *c, robj *key, robj *groupname, sds consumername) {
robj *argv[5];
argv[0] = shared.xgroup;
argv[1] = shared.createconsumer;
argv[2] = key;
argv[3] = groupname;
argv[4] = createObject(OBJ_STRING,sdsdup(consumername));

alsoPropagate(c->db->id,argv,5,PROPAGATE_AOF|PROPAGATE_REPL);

decrRefCount(argv[4]);
}













































#define STREAM_RWR_NOACK (1<<0)
#define STREAM_RWR_RAWENTRIES (1<<1)

#define STREAM_RWR_HISTORY (1<<2)
size_t streamReplyWithRange(client *c, stream *s, streamID *start, streamID *end, size_t count, int rev, streamCG *group, streamConsumer *consumer, int flags, streamPropInfo *spi) {
void *arraylen_ptr = NULL;
size_t arraylen = 0;
streamIterator si;
int64_t numfields;
streamID id;
int propagate_last_id = 0;
int noack = flags & STREAM_RWR_NOACK;






if (group && (flags & STREAM_RWR_HISTORY)) {
return streamReplyWithRangeFromConsumerPEL(c,s,start,end,count,
consumer);
}

if (!(flags & STREAM_RWR_RAWENTRIES))
arraylen_ptr = addReplyDeferredLen(c);
streamIteratorStart(&si,s,start,end,rev);
while(streamIteratorGetID(&si,&id,&numfields)) {

if (group && streamCompareID(&id,&group->last_id) > 0) {
group->last_id = id;



if (noack) propagate_last_id = 1;
}



addReplyArrayLen(c,2);
addReplyStreamID(c,&id);

addReplyArrayLen(c,numfields*2);


while(numfields--) {
unsigned char *key, *value;
int64_t key_len, value_len;
streamIteratorGetField(&si,&key,&value,&key_len,&value_len);
addReplyBulkCBuffer(c,key,key_len);
addReplyBulkCBuffer(c,value,value_len);
}










if (group && !noack) {
unsigned char buf[sizeof(streamID)];
streamEncodeID(buf,&id);




streamNACK *nack = streamCreateNACK(consumer);
int group_inserted =
raxTryInsert(group->pel,buf,sizeof(buf),nack,NULL);
int consumer_inserted =
raxTryInsert(consumer->pel,buf,sizeof(buf),nack,NULL);




if (group_inserted == 0) {
streamFreeNACK(nack);
nack = raxFind(group->pel,buf,sizeof(buf));
serverAssert(nack != raxNotFound);
raxRemove(nack->consumer->pel,buf,sizeof(buf),NULL);

nack->consumer = consumer;
nack->delivery_time = mstime();
nack->delivery_count = 1;

raxInsert(consumer->pel,buf,sizeof(buf),nack,NULL);
} else if (group_inserted == 1 && consumer_inserted == 0) {
serverPanic("NACK half-created. Should not be possible.");
}


if (spi) {
robj *idarg = createObjectFromStreamID(&id);
streamPropagateXCLAIM(c,spi->keyname,group,spi->groupname,idarg,nack);
decrRefCount(idarg);
}
}

arraylen++;
if (count && count == arraylen) break;
}

if (spi && propagate_last_id)
streamPropagateGroupID(c,spi->keyname,group,spi->groupname);

streamIteratorStop(&si);
if (arraylen_ptr) setDeferredArrayLen(c,arraylen_ptr,arraylen);
return arraylen;
}














size_t streamReplyWithRangeFromConsumerPEL(client *c, stream *s, streamID *start, streamID *end, size_t count, streamConsumer *consumer) {
raxIterator ri;
unsigned char startkey[sizeof(streamID)];
unsigned char endkey[sizeof(streamID)];
streamEncodeID(startkey,start);
if (end) streamEncodeID(endkey,end);

size_t arraylen = 0;
void *arraylen_ptr = addReplyDeferredLen(c);
raxStart(&ri,consumer->pel);
raxSeek(&ri,">=",startkey,sizeof(startkey));
while(raxNext(&ri) && (!count || arraylen < count)) {
if (end && memcmp(ri.key,end,ri.key_len) > 0) break;
streamID thisid;
streamDecodeID(ri.key,&thisid);
if (streamReplyWithRange(c,s,&thisid,&thisid,1,0,NULL,NULL,
STREAM_RWR_RAWENTRIES,NULL) == 0)
{




addReplyArrayLen(c,2);
addReplyStreamID(c,&thisid);
addReplyNullArray(c);
} else {
streamNACK *nack = ri.data;
nack->delivery_time = mstime();
nack->delivery_count++;
}
arraylen++;
}
raxStop(&ri);
setDeferredArrayLen(c,arraylen_ptr,arraylen);
return arraylen;
}







robj *streamTypeLookupWriteOrCreate(client *c, robj *key, int no_create) {
robj *o = lookupKeyWrite(c->db,key);
if (checkType(c,o,OBJ_STREAM)) return NULL;
if (o == NULL) {
if (no_create) {
addReplyNull(c);
return NULL;
}
o = createStreamObject();
dbAdd(c->db,key,o);
}
return o;
}


















int streamGenericParseIDOrReply(client *c, const robj *o, streamID *id, uint64_t missing_seq, int strict, int *seq_given) {
char buf[128];
if (sdslen(o->ptr) > sizeof(buf)-1) goto invalid;
memcpy(buf,o->ptr,sdslen(o->ptr)+1);

if (strict && (buf[0] == '-' || buf[0] == '+') && buf[1] == '\0')
goto invalid;

if (seq_given != NULL) {
*seq_given = 1;
}


if (buf[0] == '-' && buf[1] == '\0') {
id->ms = 0;
id->seq = 0;
return C_OK;
} else if (buf[0] == '+' && buf[1] == '\0') {
id->ms = UINT64_MAX;
id->seq = UINT64_MAX;
return C_OK;
}


unsigned long long ms, seq;
char *dot = strchr(buf,'-');
if (dot) *dot = '\0';
if (string2ull(buf,&ms) == 0) goto invalid;
if (dot) {
size_t seqlen = strlen(dot+1);
if (seq_given != NULL && seqlen == 1 && *(dot + 1) == '*') {

seq = 0;
*seq_given = 0;
} else if (string2ull(dot+1,&seq) == 0) {
goto invalid;
}
} else {
seq = missing_seq;
}
id->ms = ms;
id->seq = seq;
return C_OK;

invalid:
if (c) addReplyError(c,"Invalid stream ID specified as stream "
"command argument");
return C_ERR;
}


int streamParseID(const robj *o, streamID *id) {
return streamGenericParseIDOrReply(NULL,o,id,0,0,NULL);
}



int streamParseIDOrReply(client *c, robj *o, streamID *id, uint64_t missing_seq) {
return streamGenericParseIDOrReply(c,o,id,missing_seq,0,NULL);
}




int streamParseStrictIDOrReply(client *c, robj *o, streamID *id, uint64_t missing_seq, int *seq_given) {
return streamGenericParseIDOrReply(c,o,id,missing_seq,1,seq_given);
}







int streamParseIntervalIDOrReply(client *c, robj *o, streamID *id, int *exclude, uint64_t missing_seq) {
char *p = o->ptr;
size_t len = sdslen(p);
int invalid = 0;

if (exclude != NULL) *exclude = (len > 1 && p[0] == '(');
if (exclude != NULL && *exclude) {
robj *t = createStringObject(p+1,len-1);
invalid = (streamParseStrictIDOrReply(c,t,id,missing_seq,NULL) == C_ERR);
decrRefCount(t);
} else
invalid = (streamParseIDOrReply(c,o,id,missing_seq) == C_ERR);
if (invalid)
return C_ERR;
return C_OK;
}

void streamRewriteApproxSpecifier(client *c, int idx) {
rewriteClientCommandArgument(c,idx,shared.special_equals);
}



void streamRewriteTrimArgument(client *c, stream *s, int trim_strategy, int idx) {
robj *arg;
if (trim_strategy == TRIM_STRATEGY_MAXLEN) {
arg = createStringObjectFromLongLong(s->length);
} else {
streamID first_id;
streamGetEdgeID(s, 1, &first_id);
arg = createObjectFromStreamID(&first_id);
}

rewriteClientCommandArgument(c,idx,arg);
decrRefCount(arg);
}


void xaddCommand(client *c) {

streamAddTrimArgs parsed_args;
int idpos = streamParseAddOrTrimArgsOrReply(c, &parsed_args, 1);
if (idpos < 0)
return;
int field_pos = idpos+1;


if ((c->argc - field_pos) < 2 || ((c->argc-field_pos) % 2) == 1) {
addReplyErrorArity(c);
return;
}




if (parsed_args.id_given && parsed_args.seq_given &&
parsed_args.id.ms == 0 && parsed_args.id.seq == 0)
{
addReplyError(c,"The ID specified in XADD must be greater than 0-0");
return;
}


robj *o;
stream *s;
if ((o = streamTypeLookupWriteOrCreate(c,c->argv[1],parsed_args.no_mkstream)) == NULL) return;
s = o->ptr;


if (s->last_id.ms == UINT64_MAX && s->last_id.seq == UINT64_MAX) {
addReplyError(c,"The stream has exhausted the last possible ID, "
"unable to add more items");
return;
}


streamID id;
if (streamAppendItem(s,c->argv+field_pos,(c->argc-field_pos)/2,
&id,parsed_args.id_given ? &parsed_args.id : NULL,parsed_args.seq_given) == C_ERR)
{
if (errno == EDOM)
addReplyError(c,"The ID specified in XADD is equal or smaller than "
"the target stream top item");
else
addReplyError(c,"Elements are too large to be stored");
return;
}
addReplyStreamID(c,&id);

signalModifiedKey(c,c->db,c->argv[1]);
notifyKeyspaceEvent(NOTIFY_STREAM,"xadd",c->argv[1],c->db->id);
server.dirty++;


if (parsed_args.trim_strategy != TRIM_STRATEGY_NONE) {
if (streamTrim(s, &parsed_args)) {
notifyKeyspaceEvent(NOTIFY_STREAM,"xtrim",c->argv[1],c->db->id);
}
if (parsed_args.approx_trim) {





streamRewriteApproxSpecifier(c,parsed_args.trim_strategy_arg_idx-1);
streamRewriteTrimArgument(c,s,parsed_args.trim_strategy,parsed_args.trim_strategy_arg_idx);
}
}



if (!parsed_args.id_given || !parsed_args.seq_given) {
robj *idarg = createObjectFromStreamID(&id);
rewriteClientCommandArgument(c, idpos, idarg);
decrRefCount(idarg);
}



signalKeyAsReady(c->db, c->argv[1], OBJ_STREAM);
}








void xrangeGenericCommand(client *c, int rev) {
robj *o;
stream *s;
streamID startid, endid;
long long count = -1;
robj *startarg = rev ? c->argv[3] : c->argv[2];
robj *endarg = rev ? c->argv[2] : c->argv[3];
int startex = 0, endex = 0;


if (streamParseIntervalIDOrReply(c,startarg,&startid,&startex,0) != C_OK)
return;
if (startex && streamIncrID(&startid) != C_OK) {
addReplyError(c,"invalid start ID for the interval");
return;
}
if (streamParseIntervalIDOrReply(c,endarg,&endid,&endex,UINT64_MAX) != C_OK)
return;
if (endex && streamDecrID(&endid) != C_OK) {
addReplyError(c,"invalid end ID for the interval");
return;
}


if (c->argc > 4) {
for (int j = 4; j < c->argc; j++) {
int additional = c->argc-j-1;
if (strcasecmp(c->argv[j]->ptr,"COUNT") == 0 && additional >= 1) {
if (getLongLongFromObjectOrReply(c,c->argv[j+1],&count,NULL)
!= C_OK) return;
if (count < 0) count = 0;
j++;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
}
}


if ((o = lookupKeyReadOrReply(c,c->argv[1],shared.emptyarray)) == NULL ||
checkType(c,o,OBJ_STREAM)) return;

s = o->ptr;

if (count == 0) {
addReplyNullArray(c);
} else {
if (count == -1) count = 0;
streamReplyWithRange(c,s,&startid,&endid,count,rev,NULL,NULL,0,NULL);
}
}


void xrangeCommand(client *c) {
xrangeGenericCommand(c,0);
}


void xrevrangeCommand(client *c) {
xrangeGenericCommand(c,1);
}


void xlenCommand(client *c) {
robj *o;
if ((o = lookupKeyReadOrReply(c,c->argv[1],shared.czero)) == NULL
|| checkType(c,o,OBJ_STREAM)) return;
stream *s = o->ptr;
addReplyLongLong(c,s->length);
}








#define XREAD_BLOCKED_DEFAULT_COUNT 1000
void xreadCommand(client *c) {
long long timeout = -1;
long long count = 0;
int streams_count = 0;
int streams_arg = 0;
int noack = 0;
streamID static_ids[STREAMID_STATIC_VECTOR_LEN];
streamID *ids = static_ids;
streamCG **groups = NULL;
int xreadgroup = sdslen(c->argv[0]->ptr) == 10;
robj *groupname = NULL;
robj *consumername = NULL;


for (int i = 1; i < c->argc; i++) {
int moreargs = c->argc-i-1;
char *o = c->argv[i]->ptr;
if (!strcasecmp(o,"BLOCK") && moreargs) {
if (c->flags & CLIENT_SCRIPT) {




addReplyErrorFormat(c, "%s command is not allowed with BLOCK option from scripts", (char *)c->argv[0]->ptr);
return;
}
i++;
if (getTimeoutFromObjectOrReply(c,c->argv[i],&timeout,
UNIT_MILLISECONDS) != C_OK) return;
} else if (!strcasecmp(o,"COUNT") && moreargs) {
i++;
if (getLongLongFromObjectOrReply(c,c->argv[i],&count,NULL) != C_OK)
return;
if (count < 0) count = 0;
} else if (!strcasecmp(o,"STREAMS") && moreargs) {
streams_arg = i+1;
streams_count = (c->argc-streams_arg);
if ((streams_count % 2) != 0) {
addReplyError(c,"Unbalanced XREAD list of streams: "
"for each stream key an ID or '$' must be "
"specified.");
return;
}
streams_count /= 2;
break;
} else if (!strcasecmp(o,"GROUP") && moreargs >= 2) {
if (!xreadgroup) {
addReplyError(c,"The GROUP option is only supported by "
"XREADGROUP. You called XREAD instead.");
return;
}
groupname = c->argv[i+1];
consumername = c->argv[i+2];
i += 2;
} else if (!strcasecmp(o,"NOACK")) {
if (!xreadgroup) {
addReplyError(c,"The NOACK option is only supported by "
"XREADGROUP. You called XREAD instead.");
return;
}
noack = 1;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
}


if (streams_arg == 0) {
addReplyErrorObject(c,shared.syntaxerr);
return;
}



if (xreadgroup && groupname == NULL) {
addReplyError(c,"Missing GROUP option for XREADGROUP");
return;
}


if (streams_count > STREAMID_STATIC_VECTOR_LEN)
ids = zmalloc(sizeof(streamID)*streams_count);
if (groupname) groups = zmalloc(sizeof(streamCG*)*streams_count);

for (int i = streams_arg + streams_count; i < c->argc; i++) {



int id_idx = i - streams_arg - streams_count;
robj *key = c->argv[i-streams_count];
robj *o = lookupKeyRead(c->db,key);
if (checkType(c,o,OBJ_STREAM)) goto cleanup;
streamCG *group = NULL;



if (groupname) {
if (o == NULL ||
(group = streamLookupCG(o->ptr,groupname->ptr)) == NULL)
{
addReplyErrorFormat(c, "-NOGROUP No such key '%s' or consumer "
"group '%s' in XREADGROUP with GROUP "
"option",
(char*)key->ptr,(char*)groupname->ptr);
goto cleanup;
}
groups[id_idx] = group;
}

if (strcmp(c->argv[i]->ptr,"$") == 0) {
if (xreadgroup) {
addReplyError(c,"The $ ID is meaningless in the context of "
"XREADGROUP: you want to read the history of "
"this consumer by specifying a proper ID, or "
"use the > ID to get new messages. The $ ID would "
"just return an empty result set.");
goto cleanup;
}
if (o) {
stream *s = o->ptr;
ids[id_idx] = s->last_id;
} else {
ids[id_idx].ms = 0;
ids[id_idx].seq = 0;
}
continue;
} else if (strcmp(c->argv[i]->ptr,">") == 0) {
if (!xreadgroup) {
addReplyError(c,"The > ID can be specified only when calling "
"XREADGROUP using the GROUP <group> "
"<consumer> option.");
goto cleanup;
}



ids[id_idx].ms = UINT64_MAX;
ids[id_idx].seq = UINT64_MAX;
continue;
}
if (streamParseStrictIDOrReply(c,c->argv[i],ids+id_idx,0,NULL) != C_OK)
goto cleanup;
}


size_t arraylen = 0;
void *arraylen_ptr = NULL;
for (int i = 0; i < streams_count; i++) {
robj *o = lookupKeyRead(c->db,c->argv[streams_arg+i]);
if (o == NULL) continue;
stream *s = o->ptr;
streamID *gt = ids+i;
int serve_synchronously = 0;
int serve_history = 0;



if (groups) {



if (gt->ms != UINT64_MAX ||
gt->seq != UINT64_MAX)
{
serve_synchronously = 1;
serve_history = 1;
} else if (s->length) {



streamID maxid, *last = &groups[i]->last_id;
streamLastValidID(s, &maxid);
if (streamCompareID(&maxid, last) > 0) {
serve_synchronously = 1;
*gt = *last;
}
}
} else if (s->length) {


streamID maxid;
streamLastValidID(s, &maxid);
if (streamCompareID(&maxid, gt) > 0) {
serve_synchronously = 1;
}
}

if (serve_synchronously) {
arraylen++;
if (arraylen == 1) arraylen_ptr = addReplyDeferredLen(c);



streamID start = *gt;
streamIncrID(&start);



if (c->resp == 2) addReplyArrayLen(c,2);
addReplyBulk(c,c->argv[streams_arg+i]);
streamConsumer *consumer = NULL;
streamPropInfo spi = {c->argv[i+streams_arg],groupname};
if (groups) {
consumer = streamLookupConsumer(groups[i],consumername->ptr,SLC_DEFAULT);
if (consumer == NULL) {
consumer = streamCreateConsumer(groups[i],consumername->ptr,
c->argv[streams_arg+i],
c->db->id,SCC_DEFAULT);
if (noack)
streamPropagateConsumerCreation(c,spi.keyname,
spi.groupname,
consumer->name);
}
}
int flags = 0;
if (noack) flags |= STREAM_RWR_NOACK;
if (serve_history) flags |= STREAM_RWR_HISTORY;
streamReplyWithRange(c,s,&start,NULL,count,0,
groups ? groups[i] : NULL,
consumer, flags, &spi);
if (groups) server.dirty++;
}
}


if (arraylen) {
if (c->resp == 2)
setDeferredArrayLen(c,arraylen_ptr,arraylen);
else
setDeferredMapLen(c,arraylen_ptr,arraylen);
goto cleanup;
}


if (timeout != -1) {


if (c->flags & CLIENT_DENY_BLOCKING) {
addReplyNullArray(c);
goto cleanup;
}
blockForKeys(c, BLOCKED_STREAM, c->argv+streams_arg, streams_count,
-1, timeout, NULL, NULL, ids);



c->bpop.xread_count = count ? count : XREAD_BLOCKED_DEFAULT_COUNT;





if (groupname) {
incrRefCount(groupname);
incrRefCount(consumername);
c->bpop.xread_group = groupname;
c->bpop.xread_consumer = consumername;
c->bpop.xread_group_noack = noack;
} else {
c->bpop.xread_group = NULL;
c->bpop.xread_consumer = NULL;
}
goto cleanup;
}



addReplyNullArray(c);


cleanup:



preventCommandPropagation(c);
if (ids != static_ids) zfree(ids);
zfree(groups);
}








streamNACK *streamCreateNACK(streamConsumer *consumer) {
streamNACK *nack = zmalloc(sizeof(*nack));
nack->delivery_time = mstime();
nack->delivery_count = 1;
nack->consumer = consumer;
return nack;
}


void streamFreeNACK(streamNACK *na) {
zfree(na);
}






void streamFreeConsumer(streamConsumer *sc) {
raxFree(sc->pel);

sdsfree(sc->name);
zfree(sc);
}





streamCG *streamCreateCG(stream *s, char *name, size_t namelen, streamID *id) {
if (s->cgroups == NULL) s->cgroups = raxNew();
if (raxFind(s->cgroups,(unsigned char*)name,namelen) != raxNotFound)
return NULL;

streamCG *cg = zmalloc(sizeof(*cg));
cg->pel = raxNew();
cg->consumers = raxNew();
cg->last_id = *id;
raxInsert(s->cgroups,(unsigned char*)name,namelen,cg,NULL);
return cg;
}


void streamFreeCG(streamCG *cg) {
raxFreeWithCallback(cg->pel,(void(*)(void*))streamFreeNACK);
raxFreeWithCallback(cg->consumers,(void(*)(void*))streamFreeConsumer);
zfree(cg);
}



streamCG *streamLookupCG(stream *s, sds groupname) {
if (s->cgroups == NULL) return NULL;
streamCG *cg = raxFind(s->cgroups,(unsigned char*)groupname,
sdslen(groupname));
return (cg == raxNotFound) ? NULL : cg;
}





streamConsumer *streamCreateConsumer(streamCG *cg, sds name, robj *key, int dbid, int flags) {
if (cg == NULL) return NULL;
int notify = !(flags & SCC_NO_NOTIFY);
int dirty = !(flags & SCC_NO_DIRTIFY);
streamConsumer *consumer = zmalloc(sizeof(*consumer));
int success = raxTryInsert(cg->consumers,(unsigned char*)name,
sdslen(name),consumer,NULL);
if (!success) {
zfree(consumer);
return NULL;
}
consumer->name = sdsdup(name);
consumer->pel = raxNew();
consumer->seen_time = mstime();
if (dirty) server.dirty++;
if (notify) notifyKeyspaceEvent(NOTIFY_STREAM,"xgroup-createconsumer",key,dbid);
return consumer;
}



streamConsumer *streamLookupConsumer(streamCG *cg, sds name, int flags) {
if (cg == NULL) return NULL;
int refresh = !(flags & SLC_NO_REFRESH);
streamConsumer *consumer = raxFind(cg->consumers,(unsigned char*)name,
sdslen(name));
if (consumer == raxNotFound) return NULL;
if (refresh) consumer->seen_time = mstime();
return consumer;
}


void streamDelConsumer(streamCG *cg, streamConsumer *consumer) {


raxIterator ri;
raxStart(&ri,consumer->pel);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
streamNACK *nack = ri.data;
raxRemove(cg->pel,ri.key,ri.key_len,NULL);
streamFreeNACK(nack);
}
raxStop(&ri);


raxRemove(cg->consumers,(unsigned char*)consumer->name,
sdslen(consumer->name),NULL);
streamFreeConsumer(consumer);
}










void xgroupCommand(client *c) {
stream *s = NULL;
sds grpname = NULL;
streamCG *cg = NULL;
char *opt = c->argv[1]->ptr;
int mkstream = 0;
robj *o;



if (c->argc == 6 && !strcasecmp(opt,"CREATE")) {
if (strcasecmp(c->argv[5]->ptr,"MKSTREAM")) {
addReplySubcommandSyntaxError(c);
return;
}
mkstream = 1;
grpname = c->argv[3]->ptr;
}


if (c->argc >= 4) {
o = lookupKeyWrite(c->db,c->argv[2]);
if (o) {
if (checkType(c,o,OBJ_STREAM)) return;
s = o->ptr;
}
grpname = c->argv[3]->ptr;
}


if (c->argc >= 4 && !mkstream) {

if (s == NULL) {
addReplyError(c,
"The XGROUP subcommand requires the key to exist. "
"Note that for CREATE you may want to use the MKSTREAM "
"option to create an empty stream automatically.");
return;
}


if ((cg = streamLookupCG(s,grpname)) == NULL &&
(!strcasecmp(opt,"SETID") ||
!strcasecmp(opt,"CREATECONSUMER") ||
!strcasecmp(opt,"DELCONSUMER")))
{
addReplyErrorFormat(c, "-NOGROUP No such consumer group '%s' "
"for key name '%s'",
(char*)grpname, (char*)c->argv[2]->ptr);
return;
}
}


if (c->argc == 2 && !strcasecmp(opt,"HELP")) {
const char *help[] = {
"CREATE <key> <groupname> <id|$> [option]",
" Create a new consumer group. Options are:",
" * MKSTREAM",
" Create the empty stream if it does not exist.",
"CREATECONSUMER <key> <groupname> <consumer>",
" Create a new consumer in the specified group.",
"DELCONSUMER <key> <groupname> <consumer>",
" Remove the specified consumer.",
"DESTROY <key> <groupname>",
" Remove the specified group.",
"SETID <key> <groupname> <id|$>",
" Set the current group ID.",
NULL
};
addReplyHelp(c, help);
} else if (!strcasecmp(opt,"CREATE") && (c->argc == 5 || c->argc == 6)) {
streamID id;
if (!strcmp(c->argv[4]->ptr,"$")) {
if (s) {
id = s->last_id;
} else {
id.ms = 0;
id.seq = 0;
}
} else if (streamParseStrictIDOrReply(c,c->argv[4],&id,0,NULL) != C_OK) {
return;
}


if (s == NULL) {
serverAssert(mkstream);
o = createStreamObject();
dbAdd(c->db,c->argv[2],o);
s = o->ptr;
signalModifiedKey(c,c->db,c->argv[2]);
}

streamCG *cg = streamCreateCG(s,grpname,sdslen(grpname),&id);
if (cg) {
addReply(c,shared.ok);
server.dirty++;
notifyKeyspaceEvent(NOTIFY_STREAM,"xgroup-create",
c->argv[2],c->db->id);
} else {
addReplyError(c,"-BUSYGROUP Consumer Group name already exists");
}
} else if (!strcasecmp(opt,"SETID") && c->argc == 5) {
streamID id;
if (!strcmp(c->argv[4]->ptr,"$")) {
id = s->last_id;
} else if (streamParseIDOrReply(c,c->argv[4],&id,0) != C_OK) {
return;
}
cg->last_id = id;
addReply(c,shared.ok);
server.dirty++;
notifyKeyspaceEvent(NOTIFY_STREAM,"xgroup-setid",c->argv[2],c->db->id);
} else if (!strcasecmp(opt,"DESTROY") && c->argc == 4) {
if (cg) {
raxRemove(s->cgroups,(unsigned char*)grpname,sdslen(grpname),NULL);
streamFreeCG(cg);
addReply(c,shared.cone);
server.dirty++;
notifyKeyspaceEvent(NOTIFY_STREAM,"xgroup-destroy",
c->argv[2],c->db->id);

signalKeyAsReady(c->db,c->argv[2],OBJ_STREAM);
} else {
addReply(c,shared.czero);
}
} else if (!strcasecmp(opt,"CREATECONSUMER") && c->argc == 5) {
streamConsumer *created = streamCreateConsumer(cg,c->argv[4]->ptr,c->argv[2],
c->db->id,SCC_DEFAULT);
addReplyLongLong(c,created ? 1 : 0);
} else if (!strcasecmp(opt,"DELCONSUMER") && c->argc == 5) {
long long pending = 0;
streamConsumer *consumer = streamLookupConsumer(cg,c->argv[4]->ptr,SLC_NO_REFRESH);
if (consumer) {


pending = raxSize(consumer->pel);
streamDelConsumer(cg,consumer);
server.dirty++;
notifyKeyspaceEvent(NOTIFY_STREAM,"xgroup-delconsumer",
c->argv[2],c->db->id);
}
addReplyLongLong(c,pending);
} else {
addReplySubcommandSyntaxError(c);
}
}




void xsetidCommand(client *c) {
robj *o = lookupKeyWriteOrReply(c,c->argv[1],shared.nokeyerr);
if (o == NULL || checkType(c,o,OBJ_STREAM)) return;

stream *s = o->ptr;
streamID id;
if (streamParseStrictIDOrReply(c,c->argv[2],&id,0,NULL) != C_OK) return;




if (s->length > 0) {
streamID maxid;
streamLastValidID(s,&maxid);

if (streamCompareID(&id,&maxid) < 0) {
addReplyError(c,"The ID specified in XSETID is smaller than the "
"target stream top item");
return;
}
}
s->last_id = id;
addReply(c,shared.ok);
server.dirty++;
notifyKeyspaceEvent(NOTIFY_STREAM,"xsetid",c->argv[1],c->db->id);
}










void xackCommand(client *c) {
streamCG *group = NULL;
robj *o = lookupKeyRead(c->db,c->argv[1]);
if (o) {
if (checkType(c,o,OBJ_STREAM)) return;
group = streamLookupCG(o->ptr,c->argv[2]->ptr);
}


if (o == NULL || group == NULL) {
addReply(c,shared.czero);
return;
}





streamID static_ids[STREAMID_STATIC_VECTOR_LEN];
streamID *ids = static_ids;
int id_count = c->argc-3;
if (id_count > STREAMID_STATIC_VECTOR_LEN)
ids = zmalloc(sizeof(streamID)*id_count);
for (int j = 3; j < c->argc; j++) {
if (streamParseStrictIDOrReply(c,c->argv[j],&ids[j-3],0,NULL) != C_OK) goto cleanup;
}

int acknowledged = 0;
for (int j = 3; j < c->argc; j++) {
unsigned char buf[sizeof(streamID)];
streamEncodeID(buf,&ids[j-3]);




streamNACK *nack = raxFind(group->pel,buf,sizeof(buf));
if (nack != raxNotFound) {
raxRemove(group->pel,buf,sizeof(buf),NULL);
raxRemove(nack->consumer->pel,buf,sizeof(buf),NULL);
streamFreeNACK(nack);
acknowledged++;
server.dirty++;
}
}
addReplyLongLong(c,acknowledged);
cleanup:
if (ids != static_ids) zfree(ids);
}










void xpendingCommand(client *c) {
int justinfo = c->argc == 3;

robj *key = c->argv[1];
robj *groupname = c->argv[2];
robj *consumername = NULL;
streamID startid, endid;
long long count = 0;
long long minidle = 0;
int startex = 0, endex = 0;


if (c->argc != 3 && (c->argc < 6 || c->argc > 9)) {
addReplyErrorObject(c,shared.syntaxerr);
return;
}



if (c->argc >= 6) {
int startidx = 3;

if (!strcasecmp(c->argv[3]->ptr, "IDLE")) {
if (getLongLongFromObjectOrReply(c, c->argv[4], &minidle, NULL) == C_ERR)
return;
if (c->argc < 8) {

addReplyErrorObject(c,shared.syntaxerr);
return;
}

startidx += 2;
}


if (getLongLongFromObjectOrReply(c,c->argv[startidx+2],&count,NULL) == C_ERR)
return;
if (count < 0) count = 0;


if (streamParseIntervalIDOrReply(c,c->argv[startidx],&startid,&startex,0) != C_OK)
return;
if (startex && streamIncrID(&startid) != C_OK) {
addReplyError(c,"invalid start ID for the interval");
return;
}
if (streamParseIntervalIDOrReply(c,c->argv[startidx+1],&endid,&endex,UINT64_MAX) != C_OK)
return;
if (endex && streamDecrID(&endid) != C_OK) {
addReplyError(c,"invalid end ID for the interval");
return;
}

if (startidx+3 < c->argc) {

consumername = c->argv[startidx+3];
}
}


robj *o = lookupKeyRead(c->db,c->argv[1]);
streamCG *group;

if (checkType(c,o,OBJ_STREAM)) return;
if (o == NULL ||
(group = streamLookupCG(o->ptr,groupname->ptr)) == NULL)
{
addReplyErrorFormat(c, "-NOGROUP No such key '%s' or consumer "
"group '%s'",
(char*)key->ptr,(char*)groupname->ptr);
return;
}


if (justinfo) {
addReplyArrayLen(c,4);

addReplyLongLong(c,raxSize(group->pel));

if (raxSize(group->pel) == 0) {
addReplyNull(c);
addReplyNull(c);
addReplyNullArray(c);
} else {

raxIterator ri;
raxStart(&ri,group->pel);
raxSeek(&ri,"^",NULL,0);
raxNext(&ri);
streamDecodeID(ri.key,&startid);
addReplyStreamID(c,&startid);


raxSeek(&ri,"$",NULL,0);
raxNext(&ri);
streamDecodeID(ri.key,&endid);
addReplyStreamID(c,&endid);
raxStop(&ri);


raxStart(&ri,group->consumers);
raxSeek(&ri,"^",NULL,0);
void *arraylen_ptr = addReplyDeferredLen(c);
size_t arraylen = 0;
while(raxNext(&ri)) {
streamConsumer *consumer = ri.data;
if (raxSize(consumer->pel) == 0) continue;
addReplyArrayLen(c,2);
addReplyBulkCBuffer(c,ri.key,ri.key_len);
addReplyBulkLongLong(c,raxSize(consumer->pel));
arraylen++;
}
setDeferredArrayLen(c,arraylen_ptr,arraylen);
raxStop(&ri);
}
} else {
streamConsumer *consumer = NULL;
if (consumername) {
consumer = streamLookupConsumer(group,consumername->ptr,SLC_NO_REFRESH);



if (consumer == NULL) {
addReplyArrayLen(c,0);
return;
}
}

rax *pel = consumer ? consumer->pel : group->pel;
unsigned char startkey[sizeof(streamID)];
unsigned char endkey[sizeof(streamID)];
raxIterator ri;
mstime_t now = mstime();

streamEncodeID(startkey,&startid);
streamEncodeID(endkey,&endid);
raxStart(&ri,pel);
raxSeek(&ri,">=",startkey,sizeof(startkey));
void *arraylen_ptr = addReplyDeferredLen(c);
size_t arraylen = 0;

while(count && raxNext(&ri) && memcmp(ri.key,endkey,ri.key_len) <= 0) {
streamNACK *nack = ri.data;

if (minidle) {
mstime_t this_idle = now - nack->delivery_time;
if (this_idle < minidle) continue;
}

arraylen++;
count--;
addReplyArrayLen(c,4);


streamID id;
streamDecodeID(ri.key,&id);
addReplyStreamID(c,&id);


addReplyBulkCBuffer(c,nack->consumer->name,
sdslen(nack->consumer->name));


mstime_t elapsed = now - nack->delivery_time;
if (elapsed < 0) elapsed = 0;
addReplyLongLong(c,elapsed);


addReplyLongLong(c,nack->delivery_count);
}
raxStop(&ri);
setDeferredArrayLen(c,arraylen_ptr,arraylen);
}
}



































































void xclaimCommand(client *c) {
streamCG *group = NULL;
robj *o = lookupKeyRead(c->db,c->argv[1]);
long long minidle;
long long retrycount = -1;
mstime_t deliverytime = -1;
int force = 0;
int justid = 0;

if (o) {
if (checkType(c,o,OBJ_STREAM)) return;
group = streamLookupCG(o->ptr,c->argv[2]->ptr);
}



if (o == NULL || group == NULL) {
addReplyErrorFormat(c,"-NOGROUP No such key '%s' or "
"consumer group '%s'", (char*)c->argv[1]->ptr,
(char*)c->argv[2]->ptr);
return;
}

if (getLongLongFromObjectOrReply(c,c->argv[4],&minidle,
"Invalid min-idle-time argument for XCLAIM")
!= C_OK) return;
if (minidle < 0) minidle = 0;





int j;
streamID static_ids[STREAMID_STATIC_VECTOR_LEN];
streamID *ids = static_ids;
int id_count = c->argc-5;
if (id_count > STREAMID_STATIC_VECTOR_LEN)
ids = zmalloc(sizeof(streamID)*id_count);
for (j = 5; j < c->argc; j++) {
if (streamParseStrictIDOrReply(NULL,c->argv[j],&ids[j-5],0,NULL) != C_OK) break;
}
int last_id_arg = j-1;



mstime_t now = mstime();
streamID last_id = {0,0};
int propagate_last_id = 0;
for (; j < c->argc; j++) {
int moreargs = (c->argc-1) - j;
char *opt = c->argv[j]->ptr;
if (!strcasecmp(opt,"FORCE")) {
force = 1;
} else if (!strcasecmp(opt,"JUSTID")) {
justid = 1;
} else if (!strcasecmp(opt,"IDLE") && moreargs) {
j++;
if (getLongLongFromObjectOrReply(c,c->argv[j],&deliverytime,
"Invalid IDLE option argument for XCLAIM")
!= C_OK) goto cleanup;
deliverytime = now - deliverytime;
} else if (!strcasecmp(opt,"TIME") && moreargs) {
j++;
if (getLongLongFromObjectOrReply(c,c->argv[j],&deliverytime,
"Invalid TIME option argument for XCLAIM")
!= C_OK) goto cleanup;
} else if (!strcasecmp(opt,"RETRYCOUNT") && moreargs) {
j++;
if (getLongLongFromObjectOrReply(c,c->argv[j],&retrycount,
"Invalid RETRYCOUNT option argument for XCLAIM")
!= C_OK) goto cleanup;
} else if (!strcasecmp(opt,"LASTID") && moreargs) {
j++;
if (streamParseStrictIDOrReply(c,c->argv[j],&last_id,0,NULL) != C_OK) goto cleanup;
} else {
addReplyErrorFormat(c,"Unrecognized XCLAIM option '%s'",opt);
goto cleanup;
}
}

if (streamCompareID(&last_id,&group->last_id) > 0) {
group->last_id = last_id;
propagate_last_id = 1;
}

if (deliverytime != -1) {







if (deliverytime < 0 || deliverytime > now) deliverytime = now;
} else {



deliverytime = now;
}


streamConsumer *consumer = NULL;
void *arraylenptr = addReplyDeferredLen(c);
size_t arraylen = 0;
sds name = c->argv[3]->ptr;
for (int j = 5; j <= last_id_arg; j++) {
streamID id = ids[j-5];
unsigned char buf[sizeof(streamID)];
streamEncodeID(buf,&id);


streamNACK *nack = raxFind(group->pel,buf,sizeof(buf));






if (force && nack == raxNotFound) {
streamIterator myiterator;
streamIteratorStart(&myiterator,o->ptr,&id,&id,0);
int64_t numfields;
int found = 0;
streamID item_id;
if (streamIteratorGetID(&myiterator,&item_id,&numfields)) found = 1;
streamIteratorStop(&myiterator);


if (!found) continue;


nack = streamCreateNACK(NULL);
raxInsert(group->pel,buf,sizeof(buf),nack,NULL);
}

if (nack != raxNotFound) {






if (nack->consumer && minidle) {
mstime_t this_idle = now - nack->delivery_time;
if (this_idle < minidle) continue;
}
if (consumer == NULL &&
(consumer = streamLookupConsumer(group,name,SLC_DEFAULT)) == NULL)
{
consumer = streamCreateConsumer(group,name,c->argv[1],c->db->id,SCC_DEFAULT);
}
if (nack->consumer != consumer) {



if (nack->consumer)
raxRemove(nack->consumer->pel,buf,sizeof(buf),NULL);
}
nack->delivery_time = deliverytime;


if (retrycount >= 0) {
nack->delivery_count = retrycount;
} else if (!justid) {
nack->delivery_count++;
}
if (nack->consumer != consumer) {

raxInsert(consumer->pel,buf,sizeof(buf),nack,NULL);
nack->consumer = consumer;
}

if (justid) {
addReplyStreamID(c,&id);
} else {
size_t emitted = streamReplyWithRange(c,o->ptr,&id,&id,1,0,
NULL,NULL,STREAM_RWR_RAWENTRIES,NULL);
if (!emitted) addReplyNull(c);
}
arraylen++;


streamPropagateXCLAIM(c,c->argv[1],group,c->argv[2],c->argv[j],nack);
propagate_last_id = 0;
server.dirty++;
}
}
if (propagate_last_id) {
streamPropagateGroupID(c,c->argv[1],group,c->argv[2]);
server.dirty++;
}
setDeferredArrayLen(c,arraylenptr,arraylen);
preventCommandPropagation(c);
cleanup:
if (ids != static_ids) zfree(ids);
}

















void xautoclaimCommand(client *c) {
streamCG *group = NULL;
robj *o = lookupKeyRead(c->db,c->argv[1]);
long long minidle;
long count = 100;
streamID startid;
int startex;
int justid = 0;



if (getLongLongFromObjectOrReply(c,c->argv[4],&minidle,"Invalid min-idle-time argument for XAUTOCLAIM") != C_OK)
return;
if (minidle < 0) minidle = 0;

if (streamParseIntervalIDOrReply(c,c->argv[5],&startid,&startex,0) != C_OK)
return;
if (startex && streamIncrID(&startid) != C_OK) {
addReplyError(c,"invalid start ID for the interval");
return;
}

int j = 6;
while(j < c->argc) {
int moreargs = (c->argc-1) - j;
char *opt = c->argv[j]->ptr;
if (!strcasecmp(opt,"COUNT") && moreargs) {
if (getRangeLongFromObjectOrReply(c,c->argv[j+1],1,LONG_MAX,&count,"COUNT must be > 0") != C_OK)
return;
j++;
} else if (!strcasecmp(opt,"JUSTID")) {
justid = 1;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
j++;
}

if (o) {
if (checkType(c,o,OBJ_STREAM))
return;
group = streamLookupCG(o->ptr,c->argv[2]->ptr);
}



if (o == NULL || group == NULL) {
addReplyErrorFormat(c,"-NOGROUP No such key '%s' or consumer group '%s'",
(char*)c->argv[1]->ptr,
(char*)c->argv[2]->ptr);
return;
}


streamConsumer *consumer = NULL;
long long attempts = count*10;

addReplyArrayLen(c, 2);
void *endidptr = addReplyDeferredLen(c);
void *arraylenptr = addReplyDeferredLen(c);

unsigned char startkey[sizeof(streamID)];
streamEncodeID(startkey,&startid);
raxIterator ri;
raxStart(&ri,group->pel);
raxSeek(&ri,">=",startkey,sizeof(startkey));
size_t arraylen = 0;
mstime_t now = mstime();
sds name = c->argv[3]->ptr;
while (attempts-- && count && raxNext(&ri)) {
streamNACK *nack = ri.data;

if (minidle) {
mstime_t this_idle = now - nack->delivery_time;
if (this_idle < minidle)
continue;
}

streamID id;
streamDecodeID(ri.key, &id);

if (consumer == NULL &&
(consumer = streamLookupConsumer(group,name,SLC_DEFAULT)) == NULL)
{
consumer = streamCreateConsumer(group,name,c->argv[1],c->db->id,SCC_DEFAULT);
}
if (nack->consumer != consumer) {



if (nack->consumer)
raxRemove(nack->consumer->pel,ri.key,ri.key_len,NULL);
}


nack->delivery_time = now;

if (!justid)
nack->delivery_count++;

if (nack->consumer != consumer) {

raxInsert(consumer->pel,ri.key,ri.key_len,nack,NULL);
nack->consumer = consumer;
}


if (justid) {
addReplyStreamID(c,&id);
} else {
size_t emitted =
streamReplyWithRange(c,o->ptr,&id,&id,1,0,NULL,NULL,
STREAM_RWR_RAWENTRIES,NULL);
if (!emitted)
addReplyNull(c);
}
arraylen++;
count--;


robj *idstr = createObjectFromStreamID(&id);
streamPropagateXCLAIM(c,c->argv[1],group,c->argv[2],idstr,nack);
decrRefCount(idstr);
server.dirty++;
}


raxNext(&ri);

streamID endid;
if (raxEOF(&ri)) {
endid.ms = endid.seq = 0;
} else {
streamDecodeID(ri.key, &endid);
}
raxStop(&ri);

setDeferredArrayLen(c,arraylenptr,arraylen);
setDeferredReplyStreamID(c,endidptr,&endid);

preventCommandPropagation(c);
}






void xdelCommand(client *c) {
robj *o;

if ((o = lookupKeyWriteOrReply(c,c->argv[1],shared.czero)) == NULL
|| checkType(c,o,OBJ_STREAM)) return;
stream *s = o->ptr;




streamID static_ids[STREAMID_STATIC_VECTOR_LEN];
streamID *ids = static_ids;
int id_count = c->argc-2;
if (id_count > STREAMID_STATIC_VECTOR_LEN)
ids = zmalloc(sizeof(streamID)*id_count);
for (int j = 2; j < c->argc; j++) {
if (streamParseStrictIDOrReply(c,c->argv[j],&ids[j-2],0,NULL) != C_OK) goto cleanup;
}


int deleted = 0;
for (int j = 2; j < c->argc; j++) {
deleted += streamDeleteItem(s,&ids[j-2]);
}


if (deleted) {
signalModifiedKey(c,c->db,c->argv[1]);
notifyKeyspaceEvent(NOTIFY_STREAM,"xdel",c->argv[1],c->db->id);
server.dirty += deleted;
}
addReplyLongLong(c,deleted);
cleanup:
if (ids != static_ids) zfree(ids);
}
























void xtrimCommand(client *c) {
robj *o;


streamAddTrimArgs parsed_args;
if (streamParseAddOrTrimArgsOrReply(c, &parsed_args, 0) < 0)
return;



if ((o = lookupKeyWriteOrReply(c,c->argv[1],shared.czero)) == NULL
|| checkType(c,o,OBJ_STREAM)) return;
stream *s = o->ptr;


int64_t deleted = streamTrim(s, &parsed_args);
if (deleted) {
notifyKeyspaceEvent(NOTIFY_STREAM,"xtrim",c->argv[1],c->db->id);
if (parsed_args.approx_trim) {





streamRewriteApproxSpecifier(c,parsed_args.trim_strategy_arg_idx-1);
streamRewriteTrimArgument(c,s,parsed_args.trim_strategy,parsed_args.trim_strategy_arg_idx);
}


signalModifiedKey(c, c->db,c->argv[1]);
server.dirty += deleted;
}
addReplyLongLong(c,deleted);
}



void xinfoReplyWithStreamInfo(client *c, stream *s) {
int full = 1;
long long count = 10;
robj **optv = c->argv + 3;
int optc = c->argc - 3;


if (optc == 0) {
full = 0;
} else {

if (optc != 1 && optc != 3) {
addReplySubcommandSyntaxError(c);
return;
}


if (strcasecmp(optv[0]->ptr,"full")) {
addReplySubcommandSyntaxError(c);
return;
}

if (optc == 3) {

if (strcasecmp(optv[1]->ptr,"count")) {
addReplySubcommandSyntaxError(c);
return;
}
if (getLongLongFromObjectOrReply(c,optv[2],&count,NULL) == C_ERR)
return;
if (count < 0) count = 10;
}
}

addReplyMapLen(c,full ? 6 : 7);
addReplyBulkCString(c,"length");
addReplyLongLong(c,s->length);
addReplyBulkCString(c,"radix-tree-keys");
addReplyLongLong(c,raxSize(s->rax));
addReplyBulkCString(c,"radix-tree-nodes");
addReplyLongLong(c,s->rax->numnodes);
addReplyBulkCString(c,"last-generated-id");
addReplyStreamID(c,&s->last_id);

if (!full) {


addReplyBulkCString(c,"groups");
addReplyLongLong(c,s->cgroups ? raxSize(s->cgroups) : 0);


int emitted;
streamID start, end;
start.ms = start.seq = 0;
end.ms = end.seq = UINT64_MAX;
addReplyBulkCString(c,"first-entry");
emitted = streamReplyWithRange(c,s,&start,&end,1,0,NULL,NULL,
STREAM_RWR_RAWENTRIES,NULL);
if (!emitted) addReplyNull(c);
addReplyBulkCString(c,"last-entry");
emitted = streamReplyWithRange(c,s,&start,&end,1,1,NULL,NULL,
STREAM_RWR_RAWENTRIES,NULL);
if (!emitted) addReplyNull(c);
} else {



addReplyBulkCString(c,"entries");
streamReplyWithRange(c,s,NULL,NULL,count,0,NULL,NULL,0,NULL);


addReplyBulkCString(c,"groups");
if (s->cgroups == NULL) {
addReplyArrayLen(c,0);
} else {
addReplyArrayLen(c,raxSize(s->cgroups));
raxIterator ri_cgroups;
raxStart(&ri_cgroups,s->cgroups);
raxSeek(&ri_cgroups,"^",NULL,0);
while(raxNext(&ri_cgroups)) {
streamCG *cg = ri_cgroups.data;
addReplyMapLen(c,5);


addReplyBulkCString(c,"name");
addReplyBulkCBuffer(c,ri_cgroups.key,ri_cgroups.key_len);


addReplyBulkCString(c,"last-delivered-id");
addReplyStreamID(c,&cg->last_id);


addReplyBulkCString(c,"pel-count");
addReplyLongLong(c,raxSize(cg->pel));


addReplyBulkCString(c,"pending");
long long arraylen_cg_pel = 0;
void *arrayptr_cg_pel = addReplyDeferredLen(c);
raxIterator ri_cg_pel;
raxStart(&ri_cg_pel,cg->pel);
raxSeek(&ri_cg_pel,"^",NULL,0);
while(raxNext(&ri_cg_pel) && (!count || arraylen_cg_pel < count)) {
streamNACK *nack = ri_cg_pel.data;
addReplyArrayLen(c,4);


streamID id;
streamDecodeID(ri_cg_pel.key,&id);
addReplyStreamID(c,&id);


serverAssert(nack->consumer);
addReplyBulkCBuffer(c,nack->consumer->name,
sdslen(nack->consumer->name));


addReplyLongLong(c,nack->delivery_time);


addReplyLongLong(c,nack->delivery_count);

arraylen_cg_pel++;
}
setDeferredArrayLen(c,arrayptr_cg_pel,arraylen_cg_pel);
raxStop(&ri_cg_pel);


addReplyBulkCString(c,"consumers");
addReplyArrayLen(c,raxSize(cg->consumers));
raxIterator ri_consumers;
raxStart(&ri_consumers,cg->consumers);
raxSeek(&ri_consumers,"^",NULL,0);
while(raxNext(&ri_consumers)) {
streamConsumer *consumer = ri_consumers.data;
addReplyMapLen(c,4);


addReplyBulkCString(c,"name");
addReplyBulkCBuffer(c,consumer->name,sdslen(consumer->name));


addReplyBulkCString(c,"seen-time");
addReplyLongLong(c,consumer->seen_time);


addReplyBulkCString(c,"pel-count");
addReplyLongLong(c,raxSize(consumer->pel));


addReplyBulkCString(c,"pending");
long long arraylen_cpel = 0;
void *arrayptr_cpel = addReplyDeferredLen(c);
raxIterator ri_cpel;
raxStart(&ri_cpel,consumer->pel);
raxSeek(&ri_cpel,"^",NULL,0);
while(raxNext(&ri_cpel) && (!count || arraylen_cpel < count)) {
streamNACK *nack = ri_cpel.data;
addReplyArrayLen(c,3);


streamID id;
streamDecodeID(ri_cpel.key,&id);
addReplyStreamID(c,&id);


addReplyLongLong(c,nack->delivery_time);


addReplyLongLong(c,nack->delivery_count);

arraylen_cpel++;
}
setDeferredArrayLen(c,arrayptr_cpel,arraylen_cpel);
raxStop(&ri_cpel);
}
raxStop(&ri_consumers);
}
raxStop(&ri_cgroups);
}
}
}





void xinfoCommand(client *c) {
stream *s = NULL;
char *opt;
robj *key;


if (!strcasecmp(c->argv[1]->ptr,"HELP")) {
if (c->argc != 2) {
addReplySubcommandSyntaxError(c);
return;
}

const char *help[] = {
"CONSUMERS <key> <groupname>",
" Show consumers of <groupname>.",
"GROUPS <key>",
" Show the stream consumer groups.",
"STREAM <key> [FULL [COUNT <count>]",
" Show information about the stream.",
NULL
};
addReplyHelp(c, help);
return;
} else if (c->argc < 3) {
addReplySubcommandSyntaxError(c);
return;
}



opt = c->argv[1]->ptr;
key = c->argv[2];


robj *o = lookupKeyReadOrReply(c,key,shared.nokeyerr);
if (o == NULL || checkType(c,o,OBJ_STREAM)) return;
s = o->ptr;


if (!strcasecmp(opt,"CONSUMERS") && c->argc == 4) {

streamCG *cg = streamLookupCG(s,c->argv[3]->ptr);
if (cg == NULL) {
addReplyErrorFormat(c, "-NOGROUP No such consumer group '%s' "
"for key name '%s'",
(char*)c->argv[3]->ptr, (char*)key->ptr);
return;
}

addReplyArrayLen(c,raxSize(cg->consumers));
raxIterator ri;
raxStart(&ri,cg->consumers);
raxSeek(&ri,"^",NULL,0);
mstime_t now = mstime();
while(raxNext(&ri)) {
streamConsumer *consumer = ri.data;
mstime_t idle = now - consumer->seen_time;
if (idle < 0) idle = 0;

addReplyMapLen(c,3);
addReplyBulkCString(c,"name");
addReplyBulkCBuffer(c,consumer->name,sdslen(consumer->name));
addReplyBulkCString(c,"pending");
addReplyLongLong(c,raxSize(consumer->pel));
addReplyBulkCString(c,"idle");
addReplyLongLong(c,idle);
}
raxStop(&ri);
} else if (!strcasecmp(opt,"GROUPS") && c->argc == 3) {

if (s->cgroups == NULL) {
addReplyArrayLen(c,0);
return;
}

addReplyArrayLen(c,raxSize(s->cgroups));
raxIterator ri;
raxStart(&ri,s->cgroups);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
streamCG *cg = ri.data;
addReplyMapLen(c,4);
addReplyBulkCString(c,"name");
addReplyBulkCBuffer(c,ri.key,ri.key_len);
addReplyBulkCString(c,"consumers");
addReplyLongLong(c,raxSize(cg->consumers));
addReplyBulkCString(c,"pending");
addReplyLongLong(c,raxSize(cg->pel));
addReplyBulkCString(c,"last-delivered-id");
addReplyStreamID(c,&cg->last_id);
}
raxStop(&ri);
} else if (!strcasecmp(opt,"STREAM")) {

xinfoReplyWithStreamInfo(c,s);
} else {
addReplySubcommandSyntaxError(c);
}
}




int streamValidateListpackIntegrity(unsigned char *lp, size_t size, int deep) {
int valid_record;
unsigned char *p, *next;



if (!lpValidateIntegrity(lp, size, 0, NULL, NULL))
return 0;


if (!deep) return 1;

next = p = lpValidateFirst(lp);
if (!lpValidateNext(lp, &next, size)) return 0;
if (!p) return 0;


int64_t entry_count = lpGetIntegerIfValid(p, &valid_record);
if (!valid_record) return 0;
p = next; if (!lpValidateNext(lp, &next, size)) return 0;


int64_t deleted_count = lpGetIntegerIfValid(p, &valid_record);
if (!valid_record) return 0;
p = next; if (!lpValidateNext(lp, &next, size)) return 0;


int64_t master_fields = lpGetIntegerIfValid(p, &valid_record);
if (!valid_record) return 0;
p = next; if (!lpValidateNext(lp, &next, size)) return 0;


for (int64_t j = 0; j < master_fields; j++) {
p = next; if (!lpValidateNext(lp, &next, size)) return 0;
}


int64_t zero = lpGetIntegerIfValid(p, &valid_record);
if (!valid_record || zero != 0) return 0;
p = next; if (!lpValidateNext(lp, &next, size)) return 0;

entry_count += deleted_count;
while (entry_count--) {
if (!p) return 0;
int64_t fields = master_fields, extra_fields = 3;
int64_t flags = lpGetIntegerIfValid(p, &valid_record);
if (!valid_record) return 0;
p = next; if (!lpValidateNext(lp, &next, size)) return 0;


lpGetIntegerIfValid(p, &valid_record);
if (!valid_record) return 0;
p = next; if (!lpValidateNext(lp, &next, size)) return 0;
lpGetIntegerIfValid(p, &valid_record);
if (!valid_record) return 0;
p = next; if (!lpValidateNext(lp, &next, size)) return 0;

if (!(flags & STREAM_ITEM_FLAG_SAMEFIELDS)) {

fields = lpGetIntegerIfValid(p, &valid_record);
if (!valid_record) return 0;
p = next; if (!lpValidateNext(lp, &next, size)) return 0;


for (int64_t j = 0; j < fields; j++) {
p = next; if (!lpValidateNext(lp, &next, size)) return 0;
}

extra_fields += fields + 1;
}


for (int64_t j = 0; j < fields; j++) {
p = next; if (!lpValidateNext(lp, &next, size)) return 0;
}


int64_t lp_count = lpGetIntegerIfValid(p, &valid_record);
if (!valid_record) return 0;
if (lp_count != fields + extra_fields) return 0;
p = next; if (!lpValidateNext(lp, &next, size)) return 0;
}

if (next)
return 0;

return 1;
}
