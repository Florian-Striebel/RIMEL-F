#include "server.h"
#include "slowlog.h"
slowlogEntry *slowlogCreateEntry(client *c, robj **argv, int argc, long long duration) {
slowlogEntry *se = zmalloc(sizeof(*se));
int j, slargc = argc;
if (slargc > SLOWLOG_ENTRY_MAX_ARGC) slargc = SLOWLOG_ENTRY_MAX_ARGC;
se->argc = slargc;
se->argv = zmalloc(sizeof(robj*)*slargc);
for (j = 0; j < slargc; j++) {
if (slargc != argc && j == slargc-1) {
se->argv[j] = createObject(OBJ_STRING,
sdscatprintf(sdsempty(),"... (%d more arguments)",
argc-slargc+1));
} else {
if (argv[j]->type == OBJ_STRING &&
sdsEncodedObject(argv[j]) &&
sdslen(argv[j]->ptr) > SLOWLOG_ENTRY_MAX_STRING)
{
sds s = sdsnewlen(argv[j]->ptr, SLOWLOG_ENTRY_MAX_STRING);
s = sdscatprintf(s,"... (%lu more bytes)",
(unsigned long)
sdslen(argv[j]->ptr) - SLOWLOG_ENTRY_MAX_STRING);
se->argv[j] = createObject(OBJ_STRING,s);
} else if (argv[j]->refcount == OBJ_SHARED_REFCOUNT) {
se->argv[j] = argv[j];
} else {
se->argv[j] = dupStringObject(argv[j]);
}
}
}
se->time = time(NULL);
se->duration = duration;
se->id = server.slowlog_entry_id++;
se->peerid = sdsnew(getClientPeerId(c));
se->cname = c->name ? sdsnew(c->name->ptr) : sdsempty();
return se;
}
void slowlogFreeEntry(void *septr) {
slowlogEntry *se = septr;
int j;
for (j = 0; j < se->argc; j++)
decrRefCount(se->argv[j]);
zfree(se->argv);
sdsfree(se->peerid);
sdsfree(se->cname);
zfree(se);
}
void slowlogInit(void) {
server.slowlog = listCreate();
server.slowlog_entry_id = 0;
listSetFreeMethod(server.slowlog,slowlogFreeEntry);
}
void slowlogPushEntryIfNeeded(client *c, robj **argv, int argc, long long duration) {
if (server.slowlog_log_slower_than < 0) return;
if (duration >= server.slowlog_log_slower_than)
listAddNodeHead(server.slowlog,
slowlogCreateEntry(c,argv,argc,duration));
while (listLength(server.slowlog) > server.slowlog_max_len)
listDelNode(server.slowlog,listLast(server.slowlog));
}
void slowlogReset(void) {
while (listLength(server.slowlog) > 0)
listDelNode(server.slowlog,listLast(server.slowlog));
}
void slowlogCommand(client *c) {
if (c->argc == 2 && !strcasecmp(c->argv[1]->ptr,"help")) {
const char *help[] = {
"GET [<count>]",
" Return top <count> entries from the slowlog (default: 10, -1 mean all).",
" Entries are made of:",
" id, timestamp, time in microseconds, arguments array, client IP and port,",
" client name",
"LEN",
" Return the length of the slowlog.",
"RESET",
" Reset the slowlog.",
NULL
};
addReplyHelp(c, help);
} else if (c->argc == 2 && !strcasecmp(c->argv[1]->ptr,"reset")) {
slowlogReset();
addReply(c,shared.ok);
} else if (c->argc == 2 && !strcasecmp(c->argv[1]->ptr,"len")) {
addReplyLongLong(c,listLength(server.slowlog));
} else if ((c->argc == 2 || c->argc == 3) &&
!strcasecmp(c->argv[1]->ptr,"get"))
{
long count = 10, sent = 0;
listIter li;
void *totentries;
listNode *ln;
slowlogEntry *se;
if (c->argc == 3) {
if (getRangeLongFromObjectOrReply(c, c->argv[2], -1,
LONG_MAX, &count, "count should be greater than or equal to -1") != C_OK)
return;
if (count == -1) {
count = listLength(server.slowlog);
}
}
listRewind(server.slowlog,&li);
totentries = addReplyDeferredLen(c);
while(count-- && (ln = listNext(&li))) {
int j;
se = ln->value;
addReplyArrayLen(c,6);
addReplyLongLong(c,se->id);
addReplyLongLong(c,se->time);
addReplyLongLong(c,se->duration);
addReplyArrayLen(c,se->argc);
for (j = 0; j < se->argc; j++)
addReplyBulk(c,se->argv[j]);
addReplyBulkCBuffer(c,se->peerid,sdslen(se->peerid));
addReplyBulkCBuffer(c,se->cname,sdslen(se->cname));
sent++;
}
setDeferredArrayLen(c,totentries,sent);
} else {
addReplySubcommandSyntaxError(c);
}
}
