




























#include "server.h"










void listTypePush(robj *subject, robj *value, int where) {
if (subject->encoding == OBJ_ENCODING_QUICKLIST) {
int pos = (where == LIST_HEAD) ? QUICKLIST_HEAD : QUICKLIST_TAIL;
if (value->encoding == OBJ_ENCODING_INT) {
char buf[32];
ll2string(buf, 32, (long)value->ptr);
quicklistPush(subject->ptr, buf, strlen(buf), pos);
} else {
quicklistPush(subject->ptr, value->ptr, sdslen(value->ptr), pos);
}
} else {
serverPanic("Unknown list encoding");
}
}

void *listPopSaver(unsigned char *data, size_t sz) {
return createStringObject((char*)data,sz);
}

robj *listTypePop(robj *subject, int where) {
long long vlong;
robj *value = NULL;

int ql_where = where == LIST_HEAD ? QUICKLIST_HEAD : QUICKLIST_TAIL;
if (subject->encoding == OBJ_ENCODING_QUICKLIST) {
if (quicklistPopCustom(subject->ptr, ql_where, (unsigned char **)&value,
NULL, &vlong, listPopSaver)) {
if (!value)
value = createStringObjectFromLongLong(vlong);
}
} else {
serverPanic("Unknown list encoding");
}
return value;
}

unsigned long listTypeLength(const robj *subject) {
if (subject->encoding == OBJ_ENCODING_QUICKLIST) {
return quicklistCount(subject->ptr);
} else {
serverPanic("Unknown list encoding");
}
}


listTypeIterator *listTypeInitIterator(robj *subject, long index,
unsigned char direction) {
listTypeIterator *li = zmalloc(sizeof(listTypeIterator));
li->subject = subject;
li->encoding = subject->encoding;
li->direction = direction;
li->iter = NULL;


int iter_direction =
direction == LIST_HEAD ? AL_START_TAIL : AL_START_HEAD;
if (li->encoding == OBJ_ENCODING_QUICKLIST) {
li->iter = quicklistGetIteratorAtIdx(li->subject->ptr,
iter_direction, index);
} else {
serverPanic("Unknown list encoding");
}
return li;
}


void listTypeSetIteratorDirection(listTypeIterator *li, unsigned char direction) {
li->direction = direction;
int dir = direction == LIST_HEAD ? AL_START_TAIL : AL_START_HEAD;
quicklistSetDirection(li->iter, dir);
}


void listTypeReleaseIterator(listTypeIterator *li) {
quicklistReleaseIterator(li->iter);
zfree(li);
}




int listTypeNext(listTypeIterator *li, listTypeEntry *entry) {

serverAssert(li->subject->encoding == li->encoding);

entry->li = li;
if (li->encoding == OBJ_ENCODING_QUICKLIST) {
return quicklistNext(li->iter, &entry->entry);
} else {
serverPanic("Unknown list encoding");
}
return 0;
}


robj *listTypeGet(listTypeEntry *entry) {
robj *value = NULL;
if (entry->li->encoding == OBJ_ENCODING_QUICKLIST) {
if (entry->entry.value) {
value = createStringObject((char *)entry->entry.value,
entry->entry.sz);
} else {
value = createStringObjectFromLongLong(entry->entry.longval);
}
} else {
serverPanic("Unknown list encoding");
}
return value;
}

void listTypeInsert(listTypeEntry *entry, robj *value, int where) {
if (entry->li->encoding == OBJ_ENCODING_QUICKLIST) {
value = getDecodedObject(value);
sds str = value->ptr;
size_t len = sdslen(str);
if (where == LIST_TAIL) {
quicklistInsertAfter(entry->li->iter, &entry->entry, str, len);
} else if (where == LIST_HEAD) {
quicklistInsertBefore(entry->li->iter, &entry->entry, str, len);
}
decrRefCount(value);
} else {
serverPanic("Unknown list encoding");
}
}


void listTypeReplace(listTypeEntry *entry, robj *value) {
if (entry->li->encoding == OBJ_ENCODING_QUICKLIST) {
value = getDecodedObject(value);
sds str = value->ptr;
size_t len = sdslen(str);
quicklistReplaceEntry(entry->li->iter, &entry->entry, str, len);
decrRefCount(value);
} else {
serverPanic("Unknown list encoding");
}
}


int listTypeEqual(listTypeEntry *entry, robj *o) {
if (entry->li->encoding == OBJ_ENCODING_QUICKLIST) {
serverAssertWithInfo(NULL,o,sdsEncodedObject(o));
return quicklistCompare(&entry->entry,o->ptr,sdslen(o->ptr));
} else {
serverPanic("Unknown list encoding");
}
}


void listTypeDelete(listTypeIterator *iter, listTypeEntry *entry) {
if (entry->li->encoding == OBJ_ENCODING_QUICKLIST) {
quicklistDelEntry(iter->iter, &entry->entry);
} else {
serverPanic("Unknown list encoding");
}
}






robj *listTypeDup(robj *o) {
robj *lobj;

serverAssert(o->type == OBJ_LIST);

switch (o->encoding) {
case OBJ_ENCODING_QUICKLIST:
lobj = createObject(OBJ_LIST, quicklistDup(o->ptr));
lobj->encoding = o->encoding;
break;
default:
serverPanic("Unknown list encoding");
break;
}
return lobj;
}


int listTypeDelRange(robj *subject, long start, long count) {
if (subject->encoding == OBJ_ENCODING_QUICKLIST) {
return quicklistDelRange(subject->ptr, start, count);
} else {
serverPanic("Unknown list encoding");
}
}







void pushGenericCommand(client *c, int where, int xx) {
int j;

robj *lobj = lookupKeyWrite(c->db, c->argv[1]);
if (checkType(c,lobj,OBJ_LIST)) return;
if (!lobj) {
if (xx) {
addReply(c, shared.czero);
return;
}

lobj = createQuicklistObject();
quicklistSetOptions(lobj->ptr, server.list_max_listpack_size,
server.list_compress_depth);
dbAdd(c->db,c->argv[1],lobj);
}

for (j = 2; j < c->argc; j++) {
listTypePush(lobj,c->argv[j],where);
server.dirty++;
}

addReplyLongLong(c, listTypeLength(lobj));

char *event = (where == LIST_HEAD) ? "lpush" : "rpush";
signalModifiedKey(c,c->db,c->argv[1]);
notifyKeyspaceEvent(NOTIFY_LIST,event,c->argv[1],c->db->id);
}


void lpushCommand(client *c) {
pushGenericCommand(c,LIST_HEAD,0);
}


void rpushCommand(client *c) {
pushGenericCommand(c,LIST_TAIL,0);
}


void lpushxCommand(client *c) {
pushGenericCommand(c,LIST_HEAD,1);
}


void rpushxCommand(client *c) {
pushGenericCommand(c,LIST_TAIL,1);
}


void linsertCommand(client *c) {
int where;
robj *subject;
listTypeIterator *iter;
listTypeEntry entry;
int inserted = 0;

if (strcasecmp(c->argv[2]->ptr,"after") == 0) {
where = LIST_TAIL;
} else if (strcasecmp(c->argv[2]->ptr,"before") == 0) {
where = LIST_HEAD;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return;
}

if ((subject = lookupKeyWriteOrReply(c,c->argv[1],shared.czero)) == NULL ||
checkType(c,subject,OBJ_LIST)) return;


iter = listTypeInitIterator(subject,0,LIST_TAIL);
while (listTypeNext(iter,&entry)) {
if (listTypeEqual(&entry,c->argv[3])) {
listTypeInsert(&entry,c->argv[4],where);
inserted = 1;
break;
}
}
listTypeReleaseIterator(iter);

if (inserted) {
signalModifiedKey(c,c->db,c->argv[1]);
notifyKeyspaceEvent(NOTIFY_LIST,"linsert",
c->argv[1],c->db->id);
server.dirty++;
} else {

addReplyLongLong(c,-1);
return;
}

addReplyLongLong(c,listTypeLength(subject));
}


void llenCommand(client *c) {
robj *o = lookupKeyReadOrReply(c,c->argv[1],shared.czero);
if (o == NULL || checkType(c,o,OBJ_LIST)) return;
addReplyLongLong(c,listTypeLength(o));
}


void lindexCommand(client *c) {
robj *o = lookupKeyReadOrReply(c,c->argv[1],shared.null[c->resp]);
if (o == NULL || checkType(c,o,OBJ_LIST)) return;
long index;

if ((getLongFromObjectOrReply(c, c->argv[2], &index, NULL) != C_OK))
return;

if (o->encoding == OBJ_ENCODING_QUICKLIST) {
quicklistEntry entry;
quicklistIter *iter = quicklistGetIteratorEntryAtIdx(o->ptr, index, &entry);
if (iter) {
if (entry.value) {
addReplyBulkCBuffer(c, entry.value, entry.sz);
} else {
addReplyBulkLongLong(c, entry.longval);
}
} else {
addReplyNull(c);
}
quicklistReleaseIterator(iter);
} else {
serverPanic("Unknown list encoding");
}
}


void lsetCommand(client *c) {
robj *o = lookupKeyWriteOrReply(c,c->argv[1],shared.nokeyerr);
if (o == NULL || checkType(c,o,OBJ_LIST)) return;
long index;
robj *value = c->argv[3];

if ((getLongFromObjectOrReply(c, c->argv[2], &index, NULL) != C_OK))
return;

if (o->encoding == OBJ_ENCODING_QUICKLIST) {
quicklist *ql = o->ptr;
int replaced = quicklistReplaceAtIndex(ql, index,
value->ptr, sdslen(value->ptr));
if (!replaced) {
addReplyErrorObject(c,shared.outofrangeerr);
} else {
addReply(c,shared.ok);
signalModifiedKey(c,c->db,c->argv[1]);
notifyKeyspaceEvent(NOTIFY_LIST,"lset",c->argv[1],c->db->id);
server.dirty++;
}
} else {
serverPanic("Unknown list encoding");
}
}












void listPopRangeAndReplyWithKey(client *c, robj *o, robj *key, int where, long count, int *deleted) {
long llen = listTypeLength(o);
long rangelen = (count > llen) ? llen : count;
long rangestart = (where == LIST_HEAD) ? 0 : -rangelen;
long rangeend = (where == LIST_HEAD) ? rangelen - 1 : -1;
int reverse = (where == LIST_HEAD) ? 0 : 1;


addReplyArrayLen(c, 2);
addReplyBulk(c, key);
addListRangeReply(c, o, rangestart, rangeend, reverse);


listTypeDelRange(o, rangestart, rangelen);

listElementsRemoved(c, key, where, o, rangelen, deleted);
}






void addListRangeReply(client *c, robj *o, long start, long end, int reverse) {
long rangelen, llen = listTypeLength(o);


if (start < 0) start = llen+start;
if (end < 0) end = llen+end;
if (start < 0) start = 0;



if (start > end || start >= llen) {
addReply(c,shared.emptyarray);
return;
}
if (end >= llen) end = llen-1;
rangelen = (end-start)+1;


addReplyArrayLen(c,rangelen);
if (o->encoding == OBJ_ENCODING_QUICKLIST) {
int from = reverse ? end : start;
int direction = reverse ? LIST_HEAD : LIST_TAIL;
listTypeIterator *iter = listTypeInitIterator(o,from,direction);

while(rangelen--) {
listTypeEntry entry;
serverAssert(listTypeNext(iter, &entry));
quicklistEntry *qe = &entry.entry;
if (qe->value) {
addReplyBulkCBuffer(c,qe->value,qe->sz);
} else {
addReplyBulkLongLong(c,qe->longval);
}
}
listTypeReleaseIterator(iter);
} else {
serverPanic("Unknown list encoding");
}
}





void listElementsRemoved(client *c, robj *key, int where, robj *o, long count, int *deleted) {
char *event = (where == LIST_HEAD) ? "lpop" : "rpop";

notifyKeyspaceEvent(NOTIFY_LIST, event, key, c->db->id);
if (listTypeLength(o) == 0) {
if (deleted) *deleted = 1;

dbDelete(c->db, key);
notifyKeyspaceEvent(NOTIFY_GENERIC, "del", key, c->db->id);
} else {
if (deleted) *deleted = 0;
}
signalModifiedKey(c, c->db, key);
server.dirty += count;
}





void popGenericCommand(client *c, int where) {
int hascount = (c->argc == 3);
long count = 0;
robj *value;

if (c->argc > 3) {
addReplyErrorArity(c);
return;
} else if (hascount) {

if (getPositiveLongFromObjectOrReply(c,c->argv[2],&count,NULL) != C_OK)
return;
}

robj *o = lookupKeyWriteOrReply(c, c->argv[1], hascount ? shared.nullarray[c->resp]: shared.null[c->resp]);
if (o == NULL || checkType(c, o, OBJ_LIST))
return;

if (hascount && !count) {

addReply(c,shared.emptyarray);
return;
}

if (!count) {


value = listTypePop(o,where);
serverAssert(value != NULL);
addReplyBulk(c,value);
decrRefCount(value);
listElementsRemoved(c,c->argv[1],where,o,1,NULL);
} else {


long llen = listTypeLength(o);
long rangelen = (count > llen) ? llen : count;
long rangestart = (where == LIST_HEAD) ? 0 : -rangelen;
long rangeend = (where == LIST_HEAD) ? rangelen - 1 : -1;
int reverse = (where == LIST_HEAD) ? 0 : 1;

addListRangeReply(c,o,rangestart,rangeend,reverse);
listTypeDelRange(o,rangestart,rangelen);
listElementsRemoved(c,c->argv[1],where,o,rangelen,NULL);
}
}








void mpopGenericCommand(client *c, robj **keys, int numkeys, int where, long count) {
int j;
robj *o;
robj *key;

for (j = 0; j < numkeys; j++) {
key = keys[j];
o = lookupKeyWrite(c->db, key);


if (o == NULL) continue;

if (checkType(c, o, OBJ_LIST)) return;

long llen = listTypeLength(o);

if (llen == 0) continue;


listPopRangeAndReplyWithKey(c, o, key, where, count, NULL);


robj *count_obj = createStringObjectFromLongLong((count > llen) ? llen : count);
rewriteClientCommandVector(c, 3,
(where == LIST_HEAD) ? shared.lpop : shared.rpop,
key, count_obj);
decrRefCount(count_obj);
return;
}


addReplyNullArray(c);
}


void lpopCommand(client *c) {
popGenericCommand(c,LIST_HEAD);
}


void rpopCommand(client *c) {
popGenericCommand(c,LIST_TAIL);
}


void lrangeCommand(client *c) {
robj *o;
long start, end;

if ((getLongFromObjectOrReply(c, c->argv[2], &start, NULL) != C_OK) ||
(getLongFromObjectOrReply(c, c->argv[3], &end, NULL) != C_OK)) return;

if ((o = lookupKeyReadOrReply(c,c->argv[1],shared.emptyarray)) == NULL
|| checkType(c,o,OBJ_LIST)) return;

addListRangeReply(c,o,start,end,0);
}


void ltrimCommand(client *c) {
robj *o;
long start, end, llen, ltrim, rtrim;

if ((getLongFromObjectOrReply(c, c->argv[2], &start, NULL) != C_OK) ||
(getLongFromObjectOrReply(c, c->argv[3], &end, NULL) != C_OK)) return;

if ((o = lookupKeyWriteOrReply(c,c->argv[1],shared.ok)) == NULL ||
checkType(c,o,OBJ_LIST)) return;
llen = listTypeLength(o);


if (start < 0) start = llen+start;
if (end < 0) end = llen+end;
if (start < 0) start = 0;



if (start > end || start >= llen) {

ltrim = llen;
rtrim = 0;
} else {
if (end >= llen) end = llen-1;
ltrim = start;
rtrim = llen-end-1;
}


if (o->encoding == OBJ_ENCODING_QUICKLIST) {
quicklistDelRange(o->ptr,0,ltrim);
quicklistDelRange(o->ptr,-rtrim,rtrim);
} else {
serverPanic("Unknown list encoding");
}

notifyKeyspaceEvent(NOTIFY_LIST,"ltrim",c->argv[1],c->db->id);
if (listTypeLength(o) == 0) {
dbDelete(c->db,c->argv[1]);
notifyKeyspaceEvent(NOTIFY_GENERIC,"del",c->argv[1],c->db->id);
}
signalModifiedKey(c,c->db,c->argv[1]);
server.dirty += (ltrim + rtrim);
addReply(c,shared.ok);
}


















void lposCommand(client *c) {
robj *o, *ele;
ele = c->argv[2];
int direction = LIST_TAIL;
long rank = 1, count = -1, maxlen = 0;


for (int j = 3; j < c->argc; j++) {
char *opt = c->argv[j]->ptr;
int moreargs = (c->argc-1)-j;

if (!strcasecmp(opt,"RANK") && moreargs) {
j++;
if (getLongFromObjectOrReply(c, c->argv[j], &rank, NULL) != C_OK)
return;
if (rank == 0) {
addReplyError(c,"RANK can't be zero: use 1 to start from "
"the first match, 2 from the second, ...");
return;
}
} else if (!strcasecmp(opt,"COUNT") && moreargs) {
j++;
if (getPositiveLongFromObjectOrReply(c, c->argv[j], &count,
"COUNT can't be negative") != C_OK)
return;
} else if (!strcasecmp(opt,"MAXLEN") && moreargs) {
j++;
if (getPositiveLongFromObjectOrReply(c, c->argv[j], &maxlen,
"MAXLEN can't be negative") != C_OK)
return;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
}


if (rank < 0) {
rank = -rank;
direction = LIST_HEAD;
}



if ((o = lookupKeyRead(c->db,c->argv[1])) == NULL) {
if (count != -1)
addReply(c,shared.emptyarray);
else
addReply(c,shared.null[c->resp]);
return;
}
if (checkType(c,o,OBJ_LIST)) return;


void *arraylenptr = NULL;
if (count != -1) arraylenptr = addReplyDeferredLen(c);


listTypeIterator *li;
li = listTypeInitIterator(o,direction == LIST_HEAD ? -1 : 0,direction);
listTypeEntry entry;
long llen = listTypeLength(o);
long index = 0, matches = 0, matchindex = -1, arraylen = 0;
while (listTypeNext(li,&entry) && (maxlen == 0 || index < maxlen)) {
if (listTypeEqual(&entry,ele)) {
matches++;
matchindex = (direction == LIST_TAIL) ? index : llen - index - 1;
if (matches >= rank) {
if (arraylenptr) {
arraylen++;
addReplyLongLong(c,matchindex);
if (count && matches-rank+1 >= count) break;
} else {
break;
}
}
}
index++;
matchindex = -1;
}
listTypeReleaseIterator(li);



if (arraylenptr != NULL) {
setDeferredArrayLen(c,arraylenptr,arraylen);
} else {
if (matchindex != -1)
addReplyLongLong(c,matchindex);
else
addReply(c,shared.null[c->resp]);
}
}


void lremCommand(client *c) {
robj *subject, *obj;
obj = c->argv[3];
long toremove;
long removed = 0;

if ((getLongFromObjectOrReply(c, c->argv[2], &toremove, NULL) != C_OK))
return;

subject = lookupKeyWriteOrReply(c,c->argv[1],shared.czero);
if (subject == NULL || checkType(c,subject,OBJ_LIST)) return;

listTypeIterator *li;
if (toremove < 0) {
toremove = -toremove;
li = listTypeInitIterator(subject,-1,LIST_HEAD);
} else {
li = listTypeInitIterator(subject,0,LIST_TAIL);
}

listTypeEntry entry;
while (listTypeNext(li,&entry)) {
if (listTypeEqual(&entry,obj)) {
listTypeDelete(li, &entry);
server.dirty++;
removed++;
if (toremove && removed == toremove) break;
}
}
listTypeReleaseIterator(li);

if (removed) {
signalModifiedKey(c,c->db,c->argv[1]);
notifyKeyspaceEvent(NOTIFY_LIST,"lrem",c->argv[1],c->db->id);
}

if (listTypeLength(subject) == 0) {
dbDelete(c->db,c->argv[1]);
notifyKeyspaceEvent(NOTIFY_GENERIC,"del",c->argv[1],c->db->id);
}

addReplyLongLong(c,removed);
}

void lmoveHandlePush(client *c, robj *dstkey, robj *dstobj, robj *value,
int where) {

if (!dstobj) {
dstobj = createQuicklistObject();
quicklistSetOptions(dstobj->ptr, server.list_max_listpack_size,
server.list_compress_depth);
dbAdd(c->db,dstkey,dstobj);
}
signalModifiedKey(c,c->db,dstkey);
listTypePush(dstobj,value,where);
notifyKeyspaceEvent(NOTIFY_LIST,
where == LIST_HEAD ? "lpush" : "rpush",
dstkey,
c->db->id);

addReplyBulk(c,value);
}

int getListPositionFromObjectOrReply(client *c, robj *arg, int *position) {
if (strcasecmp(arg->ptr,"right") == 0) {
*position = LIST_TAIL;
} else if (strcasecmp(arg->ptr,"left") == 0) {
*position = LIST_HEAD;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return C_ERR;
}
return C_OK;
}

robj *getStringObjectFromListPosition(int position) {
if (position == LIST_HEAD) {
return shared.left;
} else {

return shared.right;
}
}

void lmoveGenericCommand(client *c, int wherefrom, int whereto) {
robj *sobj, *value;
if ((sobj = lookupKeyWriteOrReply(c,c->argv[1],shared.null[c->resp]))
== NULL || checkType(c,sobj,OBJ_LIST)) return;

if (listTypeLength(sobj) == 0) {


addReplyNull(c);
} else {
robj *dobj = lookupKeyWrite(c->db,c->argv[2]);
robj *touchedkey = c->argv[1];

if (checkType(c,dobj,OBJ_LIST)) return;
value = listTypePop(sobj,wherefrom);
serverAssert(value);
lmoveHandlePush(c,c->argv[2],dobj,value,whereto);


decrRefCount(value);


notifyKeyspaceEvent(NOTIFY_LIST,
wherefrom == LIST_HEAD ? "lpop" : "rpop",
touchedkey,
c->db->id);
if (listTypeLength(sobj) == 0) {
dbDelete(c->db,touchedkey);
notifyKeyspaceEvent(NOTIFY_GENERIC,"del",
touchedkey,c->db->id);
}
signalModifiedKey(c,c->db,touchedkey);
server.dirty++;
if (c->cmd->proc == blmoveCommand) {
rewriteClientCommandVector(c,5,shared.lmove,
c->argv[1],c->argv[2],c->argv[3],c->argv[4]);
} else if (c->cmd->proc == brpoplpushCommand) {
rewriteClientCommandVector(c,3,shared.rpoplpush,
c->argv[1],c->argv[2]);
}
}
}


void lmoveCommand(client *c) {
int wherefrom, whereto;
if (getListPositionFromObjectOrReply(c,c->argv[3],&wherefrom)
!= C_OK) return;
if (getListPositionFromObjectOrReply(c,c->argv[4],&whereto)
!= C_OK) return;
lmoveGenericCommand(c, wherefrom, whereto);
}
















void rpoplpushCommand(client *c) {
lmoveGenericCommand(c, LIST_TAIL, LIST_HEAD);
}


























void serveClientBlockedOnList(client *receiver, robj *o, robj *key, robj *dstkey, redisDb *db, int wherefrom, int whereto, int *deleted)
{
robj *argv[5];
robj *value = NULL;

if (deleted) *deleted = 0;

if (dstkey == NULL) {

argv[0] = (wherefrom == LIST_HEAD) ? shared.lpop :
shared.rpop;
argv[1] = key;

if (receiver->lastcmd->proc == blmpopCommand) {

long count = receiver->bpop.count;
serverAssert(count > 0);
long llen = listTypeLength(o);
serverAssert(llen > 0);

argv[2] = createStringObjectFromLongLong((count > llen) ? llen : count);
alsoPropagate(db->id, argv, 3, PROPAGATE_AOF|PROPAGATE_REPL);
decrRefCount(argv[2]);


listPopRangeAndReplyWithKey(receiver, o, key, wherefrom, count, deleted);
return;
}

alsoPropagate(db->id, argv, 2, PROPAGATE_AOF|PROPAGATE_REPL);


value = listTypePop(o, wherefrom);
serverAssert(value != NULL);

addReplyArrayLen(receiver,2);
addReplyBulk(receiver,key);
addReplyBulk(receiver,value);


char *event = (wherefrom == LIST_HEAD) ? "lpop" : "rpop";
notifyKeyspaceEvent(NOTIFY_LIST,event,key,receiver->db->id);
} else {

robj *dstobj =
lookupKeyWrite(receiver->db,dstkey);
if (!(dstobj &&
checkType(receiver,dstobj,OBJ_LIST)))
{
value = listTypePop(o, wherefrom);
serverAssert(value != NULL);

lmoveHandlePush(receiver,dstkey,dstobj,value,whereto);

int isbrpoplpush = (receiver->lastcmd->proc == brpoplpushCommand);
argv[0] = isbrpoplpush ? shared.rpoplpush : shared.lmove;
argv[1] = key;
argv[2] = dstkey;
argv[3] = getStringObjectFromListPosition(wherefrom);
argv[4] = getStringObjectFromListPosition(whereto);
alsoPropagate(db->id,argv,(isbrpoplpush ? 3 : 5),PROPAGATE_AOF|PROPAGATE_REPL);


notifyKeyspaceEvent(NOTIFY_LIST,wherefrom == LIST_TAIL ? "rpop" : "lpop",
key,receiver->db->id);
}
}

if (value) decrRefCount(value);

if (listTypeLength(o) == 0) {
if (deleted) *deleted = 1;

dbDelete(receiver->db, key);
notifyKeyspaceEvent(NOTIFY_GENERIC, "del", key, receiver->db->id);
}


}










void blockingPopGenericCommand(client *c, robj **keys, int numkeys, int where, int timeout_idx, long count) {
robj *o;
robj *key;
mstime_t timeout;
int j;

if (getTimeoutFromObjectOrReply(c,c->argv[timeout_idx],&timeout,UNIT_SECONDS)
!= C_OK) return;


for (j = 0; j < numkeys; j++) {
key = keys[j];
o = lookupKeyWrite(c->db, key);


if (o == NULL) continue;

if (checkType(c, o, OBJ_LIST)) return;

long llen = listTypeLength(o);

if (llen == 0) continue;

if (count != -1) {


listPopRangeAndReplyWithKey(c, o, key, where, count, NULL);


robj *count_obj = createStringObjectFromLongLong((count > llen) ? llen : count);
rewriteClientCommandVector(c, 3,
(where == LIST_HEAD) ? shared.lpop : shared.rpop,
key, count_obj);
decrRefCount(count_obj);
return;
}


robj *value = listTypePop(o,where);
serverAssert(value != NULL);

addReplyArrayLen(c,2);
addReplyBulk(c,key);
addReplyBulk(c,value);
decrRefCount(value);
listElementsRemoved(c,key,where,o,1,NULL);


rewriteClientCommandVector(c,2,
(where == LIST_HEAD) ? shared.lpop : shared.rpop,
key);
return;
}



if (c->flags & CLIENT_DENY_BLOCKING) {
addReplyNullArray(c);
return;
}


struct blockPos pos = {where};
blockForKeys(c,BLOCKED_LIST,keys,numkeys,count,timeout,NULL,&pos,NULL);
}


void blpopCommand(client *c) {
blockingPopGenericCommand(c,c->argv+1,c->argc-2,LIST_HEAD,c->argc-1,-1);
}


void brpopCommand(client *c) {
blockingPopGenericCommand(c,c->argv+1,c->argc-2,LIST_TAIL,c->argc-1,-1);
}

void blmoveGenericCommand(client *c, int wherefrom, int whereto, mstime_t timeout) {
robj *key = lookupKeyWrite(c->db, c->argv[1]);
if (checkType(c,key,OBJ_LIST)) return;

if (key == NULL) {
if (c->flags & CLIENT_DENY_BLOCKING) {


addReplyNull(c);
} else {

struct blockPos pos = {wherefrom, whereto};
blockForKeys(c,BLOCKED_LIST,c->argv + 1,1,-1,timeout,c->argv[2],&pos,NULL);
}
} else {


serverAssertWithInfo(c,key,listTypeLength(key) > 0);
lmoveGenericCommand(c,wherefrom,whereto);
}
}


void blmoveCommand(client *c) {
mstime_t timeout;
int wherefrom, whereto;
if (getListPositionFromObjectOrReply(c,c->argv[3],&wherefrom)
!= C_OK) return;
if (getListPositionFromObjectOrReply(c,c->argv[4],&whereto)
!= C_OK) return;
if (getTimeoutFromObjectOrReply(c,c->argv[5],&timeout,UNIT_SECONDS)
!= C_OK) return;
blmoveGenericCommand(c,wherefrom,whereto,timeout);
}


void brpoplpushCommand(client *c) {
mstime_t timeout;
if (getTimeoutFromObjectOrReply(c,c->argv[3],&timeout,UNIT_SECONDS)
!= C_OK) return;
blmoveGenericCommand(c, LIST_TAIL, LIST_HEAD, timeout);
}





void lmpopGenericCommand(client *c, int numkeys_idx, int is_block) {
long j;
long numkeys = 0;
int where = 0;
long count = -1;


if (getRangeLongFromObjectOrReply(c, c->argv[numkeys_idx], 1, LONG_MAX,
&numkeys, "numkeys should be greater than 0") != C_OK)
return;


long where_idx = numkeys_idx + numkeys + 1;
if (where_idx >= c->argc) {
addReplyErrorObject(c, shared.syntaxerr);
return;
}
if (getListPositionFromObjectOrReply(c, c->argv[where_idx], &where) != C_OK)
return;


for (j = where_idx + 1; j < c->argc; j++) {
char *opt = c->argv[j]->ptr;
int moreargs = (c->argc - 1) - j;

if (count == -1 && !strcasecmp(opt, "COUNT") && moreargs) {
j++;
if (getRangeLongFromObjectOrReply(c, c->argv[j], 1, LONG_MAX,
&count,"count should be greater than 0") != C_OK)
return;
} else {
addReplyErrorObject(c, shared.syntaxerr);
return;
}
}

if (count == -1) count = 1;

if (is_block) {

blockingPopGenericCommand(c, c->argv+numkeys_idx+1, numkeys, where, 1, count);
} else {

mpopGenericCommand(c, c->argv+numkeys_idx+1, numkeys, where, count);
}
}


void lmpopCommand(client *c) {
lmpopGenericCommand(c, 1, 0);
}


void blmpopCommand(client *c) {
lmpopGenericCommand(c, 2, 1);
}
