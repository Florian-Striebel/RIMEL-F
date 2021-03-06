#include "server.h"
#include "cluster.h"
typedef struct pubsubtype {
int shard;
dict *(*clientPubSubChannels)(client*);
int (*subscriptionCount)(client*);
dict **serverPubSubChannels;
robj **subscribeMsg;
robj **unsubscribeMsg;
}pubsubtype;
int clientSubscriptionsCount(client *c);
int clientShardSubscriptionsCount(client *c);
dict* getClientPubSubChannels(client *c);
dict* getClientPubSubShardChannels(client *c);
void channelList(client *c, sds pat, dict* pubsub_channels);
pubsubtype pubSubType = {
.shard = 0,
.clientPubSubChannels = getClientPubSubChannels,
.subscriptionCount = clientSubscriptionsCount,
.serverPubSubChannels = &server.pubsub_channels,
.subscribeMsg = &shared.subscribebulk,
.unsubscribeMsg = &shared.unsubscribebulk,
};
pubsubtype pubSubShardType = {
.shard = 1,
.clientPubSubChannels = getClientPubSubShardChannels,
.subscriptionCount = clientShardSubscriptionsCount,
.serverPubSubChannels = &server.pubsubshard_channels,
.subscribeMsg = &shared.ssubscribebulk,
.unsubscribeMsg = &shared.sunsubscribebulk
};
void addReplyPubsubMessage(client *c, robj *channel, robj *msg) {
if (c->resp == 2)
addReply(c,shared.mbulkhdr[3]);
else
addReplyPushLen(c,3);
addReply(c,shared.messagebulk);
addReplyBulk(c,channel);
if (msg) addReplyBulk(c,msg);
}
void addReplyPubsubPatMessage(client *c, robj *pat, robj *channel, robj *msg) {
if (c->resp == 2)
addReply(c,shared.mbulkhdr[4]);
else
addReplyPushLen(c,4);
addReply(c,shared.pmessagebulk);
addReplyBulk(c,pat);
addReplyBulk(c,channel);
addReplyBulk(c,msg);
}
void addReplyPubsubSubscribed(client *c, robj *channel, pubsubtype type) {
if (c->resp == 2)
addReply(c,shared.mbulkhdr[3]);
else
addReplyPushLen(c,3);
addReply(c,*type.subscribeMsg);
addReplyBulk(c,channel);
addReplyLongLong(c,type.subscriptionCount(c));
}
void addReplyPubsubUnsubscribed(client *c, robj *channel, pubsubtype type) {
if (c->resp == 2)
addReply(c,shared.mbulkhdr[3]);
else
addReplyPushLen(c,3);
addReply(c, *type.unsubscribeMsg);
if (channel)
addReplyBulk(c,channel);
else
addReplyNull(c);
addReplyLongLong(c,type.subscriptionCount(c));
}
void addReplyPubsubPatSubscribed(client *c, robj *pattern) {
if (c->resp == 2)
addReply(c,shared.mbulkhdr[3]);
else
addReplyPushLen(c,3);
addReply(c,shared.psubscribebulk);
addReplyBulk(c,pattern);
addReplyLongLong(c,clientSubscriptionsCount(c));
}
void addReplyPubsubPatUnsubscribed(client *c, robj *pattern) {
if (c->resp == 2)
addReply(c,shared.mbulkhdr[3]);
else
addReplyPushLen(c,3);
addReply(c,shared.punsubscribebulk);
if (pattern)
addReplyBulk(c,pattern);
else
addReplyNull(c);
addReplyLongLong(c,clientSubscriptionsCount(c));
}
int serverPubsubSubscriptionCount() {
return dictSize(server.pubsub_channels) + dictSize(server.pubsub_patterns);
}
int serverPubsubShardSubscriptionCount() {
return dictSize(server.pubsubshard_channels);
}
int clientSubscriptionsCount(client *c) {
return dictSize(c->pubsub_channels) + listLength(c->pubsub_patterns);
}
int clientShardSubscriptionsCount(client *c) {
return dictSize(c->pubsubshard_channels);
}
dict* getClientPubSubChannels(client *c) {
return c->pubsub_channels;
}
dict* getClientPubSubShardChannels(client *c) {
return c->pubsubshard_channels;
}
int clientTotalPubSubSubscriptionCount(client *c) {
return clientSubscriptionsCount(c) + clientShardSubscriptionsCount(c);
}
int pubsubSubscribeChannel(client *c, robj *channel, pubsubtype type) {
dictEntry *de;
list *clients = NULL;
int retval = 0;
if (dictAdd(type.clientPubSubChannels(c),channel,NULL) == DICT_OK) {
retval = 1;
incrRefCount(channel);
de = dictFind(*type.serverPubSubChannels, channel);
if (de == NULL) {
clients = listCreate();
dictAdd(*type.serverPubSubChannels, channel, clients);
incrRefCount(channel);
} else {
clients = dictGetVal(de);
}
listAddNodeTail(clients,c);
}
addReplyPubsubSubscribed(c,channel,type);
return retval;
}
int pubsubUnsubscribeChannel(client *c, robj *channel, int notify, pubsubtype type) {
dictEntry *de;
list *clients;
listNode *ln;
int retval = 0;
incrRefCount(channel);
if (dictDelete(type.clientPubSubChannels(c),channel) == DICT_OK) {
retval = 1;
de = dictFind(*type.serverPubSubChannels, channel);
serverAssertWithInfo(c,NULL,de != NULL);
clients = dictGetVal(de);
ln = listSearchKey(clients,c);
serverAssertWithInfo(c,NULL,ln != NULL);
listDelNode(clients,ln);
if (listLength(clients) == 0) {
dictDelete(*type.serverPubSubChannels, channel);
if (server.cluster_enabled & type.shard) {
slotToChannelDel(channel->ptr);
}
}
}
if (notify) {
addReplyPubsubUnsubscribed(c,channel,type);
}
decrRefCount(channel);
return retval;
}
void pubsubShardUnsubscribeAllClients(robj *channel) {
int retval;
dictEntry *de = dictFind(server.pubsubshard_channels, channel);
serverAssertWithInfo(NULL,channel,de != NULL);
list *clients = dictGetVal(de);
if (listLength(clients) > 0) {
listIter li;
listNode *ln;
listRewind(clients, &li);
while ((ln = listNext(&li)) != NULL) {
client *c = listNodeValue(ln);
retval = dictDelete(c->pubsubshard_channels, channel);
serverAssertWithInfo(c,channel,retval == DICT_OK);
addReplyPubsubUnsubscribed(c, channel, pubSubShardType);
if (clientTotalPubSubSubscriptionCount(c) == 0) {
c->flags &= ~CLIENT_PUBSUB;
}
}
}
retval = dictDelete(server.pubsubshard_channels, channel);
slotToChannelDel(channel->ptr);
serverAssertWithInfo(NULL,channel,retval == DICT_OK);
decrRefCount(channel);
}
int pubsubSubscribePattern(client *c, robj *pattern) {
dictEntry *de;
list *clients;
int retval = 0;
if (listSearchKey(c->pubsub_patterns,pattern) == NULL) {
retval = 1;
listAddNodeTail(c->pubsub_patterns,pattern);
incrRefCount(pattern);
de = dictFind(server.pubsub_patterns,pattern);
if (de == NULL) {
clients = listCreate();
dictAdd(server.pubsub_patterns,pattern,clients);
incrRefCount(pattern);
} else {
clients = dictGetVal(de);
}
listAddNodeTail(clients,c);
}
addReplyPubsubPatSubscribed(c,pattern);
return retval;
}
int pubsubUnsubscribePattern(client *c, robj *pattern, int notify) {
dictEntry *de;
list *clients;
listNode *ln;
int retval = 0;
incrRefCount(pattern);
if ((ln = listSearchKey(c->pubsub_patterns,pattern)) != NULL) {
retval = 1;
listDelNode(c->pubsub_patterns,ln);
de = dictFind(server.pubsub_patterns,pattern);
serverAssertWithInfo(c,NULL,de != NULL);
clients = dictGetVal(de);
ln = listSearchKey(clients,c);
serverAssertWithInfo(c,NULL,ln != NULL);
listDelNode(clients,ln);
if (listLength(clients) == 0) {
dictDelete(server.pubsub_patterns,pattern);
}
}
if (notify) addReplyPubsubPatUnsubscribed(c,pattern);
decrRefCount(pattern);
return retval;
}
int pubsubUnsubscribeAllChannelsInternal(client *c, int notify, pubsubtype type) {
int count = 0;
if (dictSize(type.clientPubSubChannels(c)) > 0) {
dictIterator *di = dictGetSafeIterator(type.clientPubSubChannels(c));
dictEntry *de;
while((de = dictNext(di)) != NULL) {
robj *channel = dictGetKey(de);
count += pubsubUnsubscribeChannel(c,channel,notify,type);
}
dictReleaseIterator(di);
}
if (notify && count == 0) {
addReplyPubsubUnsubscribed(c,NULL,type);
}
return count;
}
int pubsubUnsubscribeAllChannels(client *c, int notify) {
int count = pubsubUnsubscribeAllChannelsInternal(c,notify,pubSubType);
return count;
}
int pubsubUnsubscribeShardAllChannels(client *c, int notify) {
int count = pubsubUnsubscribeAllChannelsInternal(c, notify, pubSubShardType);
return count;
}
void pubsubUnsubscribeShardChannels(robj **channels, unsigned int count) {
for (unsigned int j = 0; j < count; j++) {
pubsubShardUnsubscribeAllClients(channels[j]);
}
}
int pubsubUnsubscribeAllPatterns(client *c, int notify) {
listNode *ln;
listIter li;
int count = 0;
listRewind(c->pubsub_patterns,&li);
while ((ln = listNext(&li)) != NULL) {
robj *pattern = ln->value;
count += pubsubUnsubscribePattern(c,pattern,notify);
}
if (notify && count == 0) addReplyPubsubPatUnsubscribed(c,NULL);
return count;
}
int pubsubPublishMessageInternal(robj *channel, robj *message, pubsubtype type) {
int receivers = 0;
dictEntry *de;
dictIterator *di;
listNode *ln;
listIter li;
de = dictFind(*type.serverPubSubChannels, channel);
if (de) {
list *list = dictGetVal(de);
listNode *ln;
listIter li;
listRewind(list,&li);
while ((ln = listNext(&li)) != NULL) {
client *c = ln->value;
addReplyPubsubMessage(c,channel,message);
updateClientMemUsage(c);
receivers++;
}
}
if (type.shard) {
return receivers;
}
di = dictGetIterator(server.pubsub_patterns);
if (di) {
channel = getDecodedObject(channel);
while((de = dictNext(di)) != NULL) {
robj *pattern = dictGetKey(de);
list *clients = dictGetVal(de);
if (!stringmatchlen((char*)pattern->ptr,
sdslen(pattern->ptr),
(char*)channel->ptr,
sdslen(channel->ptr),0)) continue;
listRewind(clients,&li);
while ((ln = listNext(&li)) != NULL) {
client *c = listNodeValue(ln);
addReplyPubsubPatMessage(c,pattern,channel,message);
updateClientMemUsage(c);
receivers++;
}
}
decrRefCount(channel);
dictReleaseIterator(di);
}
return receivers;
}
int pubsubPublishMessage(robj *channel, robj *message) {
return pubsubPublishMessageInternal(channel,message,pubSubType);
}
int pubsubPublishMessageShard(robj *channel, robj *message) {
return pubsubPublishMessageInternal(channel, message, pubSubShardType);
}
void subscribeCommand(client *c) {
int j;
if ((c->flags & CLIENT_DENY_BLOCKING) && !(c->flags & CLIENT_MULTI)) {
addReplyError(c, "SUBSCRIBE isn't allowed for a DENY BLOCKING client");
return;
}
for (j = 1; j < c->argc; j++)
pubsubSubscribeChannel(c,c->argv[j],pubSubType);
c->flags |= CLIENT_PUBSUB;
}
void unsubscribeCommand(client *c) {
if (c->argc == 1) {
pubsubUnsubscribeAllChannels(c,1);
} else {
int j;
for (j = 1; j < c->argc; j++)
pubsubUnsubscribeChannel(c,c->argv[j],1,pubSubType);
}
if (clientTotalPubSubSubscriptionCount(c) == 0) c->flags &= ~CLIENT_PUBSUB;
}
void psubscribeCommand(client *c) {
int j;
if ((c->flags & CLIENT_DENY_BLOCKING) && !(c->flags & CLIENT_MULTI)) {
addReplyError(c, "PSUBSCRIBE isn't allowed for a DENY BLOCKING client");
return;
}
for (j = 1; j < c->argc; j++)
pubsubSubscribePattern(c,c->argv[j]);
c->flags |= CLIENT_PUBSUB;
}
void punsubscribeCommand(client *c) {
if (c->argc == 1) {
pubsubUnsubscribeAllPatterns(c,1);
} else {
int j;
for (j = 1; j < c->argc; j++)
pubsubUnsubscribePattern(c,c->argv[j],1);
}
if (clientTotalPubSubSubscriptionCount(c) == 0) c->flags &= ~CLIENT_PUBSUB;
}
void publishCommand(client *c) {
if (server.sentinel_mode) {
sentinelPublishCommand(c);
return;
}
int receivers = pubsubPublishMessage(c->argv[1],c->argv[2]);
if (server.cluster_enabled)
clusterPropagatePublish(c->argv[1],c->argv[2]);
else
forceCommandPropagation(c,PROPAGATE_REPL);
addReplyLongLong(c,receivers);
}
void pubsubCommand(client *c) {
if (c->argc == 2 && !strcasecmp(c->argv[1]->ptr,"help")) {
const char *help[] = {
"CHANNELS [<pattern>]",
" Return the currently active channels matching a <pattern> (default: '*').",
"NUMPAT",
" Return number of subscriptions to patterns.",
"NUMSUB [<channel> ...]",
" Return the number of subscribers for the specified channels, excluding",
" pattern subscriptions(default: no channels)."
"SHARDCHANNELS [<pattern>]",
" Return the currently active shard level channels matching a <pattern> (default: '*').",
"SHARDNUMSUB [<channel> ...]",
" Return the number of subscribers for the specified shard level channel(s)",
NULL
};
addReplyHelp(c, help);
} else if (!strcasecmp(c->argv[1]->ptr,"channels") &&
(c->argc == 2 || c->argc == 3))
{
sds pat = (c->argc == 2) ? NULL : c->argv[2]->ptr;
channelList(c, pat, server.pubsub_channels);
} else if (!strcasecmp(c->argv[1]->ptr,"numsub") && c->argc >= 2) {
int j;
addReplyArrayLen(c,(c->argc-2)*2);
for (j = 2; j < c->argc; j++) {
list *l = dictFetchValue(server.pubsub_channels,c->argv[j]);
addReplyBulk(c,c->argv[j]);
addReplyLongLong(c,l ? listLength(l) : 0);
}
} else if (!strcasecmp(c->argv[1]->ptr,"numpat") && c->argc == 2) {
addReplyLongLong(c,dictSize(server.pubsub_patterns));
} else if (!strcasecmp(c->argv[1]->ptr,"shardchannels") &&
(c->argc == 2 || c->argc == 3))
{
sds pat = (c->argc == 2) ? NULL : c->argv[2]->ptr;
channelList(c,pat,server.pubsubshard_channels);
} else if (!strcasecmp(c->argv[1]->ptr,"shardnumsub") && c->argc >= 2) {
int j;
addReplyArrayLen(c, (c->argc-2)*2);
for (j = 2; j < c->argc; j++) {
list *l = dictFetchValue(server.pubsubshard_channels, c->argv[j]);
addReplyBulk(c,c->argv[j]);
addReplyLongLong(c,l ? listLength(l) : 0);
}
} else {
addReplySubcommandSyntaxError(c);
}
}
void channelList(client *c, sds pat, dict *pubsub_channels) {
dictIterator *di = dictGetIterator(pubsub_channels);
dictEntry *de;
long mblen = 0;
void *replylen;
replylen = addReplyDeferredLen(c);
while((de = dictNext(di)) != NULL) {
robj *cobj = dictGetKey(de);
sds channel = cobj->ptr;
if (!pat || stringmatchlen(pat, sdslen(pat),
channel, sdslen(channel),0))
{
addReplyBulk(c,cobj);
mblen++;
}
}
dictReleaseIterator(di);
setDeferredArrayLen(c,replylen,mblen);
}
void spublishCommand(client *c) {
int receivers = pubsubPublishMessageInternal(c->argv[1], c->argv[2], pubSubShardType);
if (server.cluster_enabled) {
clusterPropagatePublishShard(c->argv[1], c->argv[2]);
} else {
forceCommandPropagation(c,PROPAGATE_REPL);
}
addReplyLongLong(c,receivers);
}
void ssubscribeCommand(client *c) {
if (c->flags & CLIENT_DENY_BLOCKING) {
addReplyError(c, "SSUBSCRIBE isn't allowed for a DENY BLOCKING client");
return;
}
for (int j = 1; j < c->argc; j++) {
if (server.cluster_enabled &
(dictFind(*pubSubShardType.serverPubSubChannels, c->argv[j]) == NULL)) {
slotToChannelAdd(c->argv[j]->ptr);
}
pubsubSubscribeChannel(c, c->argv[j], pubSubShardType);
}
c->flags |= CLIENT_PUBSUB;
}
void sunsubscribeCommand(client *c) {
if (c->argc == 1) {
pubsubUnsubscribeShardAllChannels(c, 1);
} else {
for (int j = 1; j < c->argc; j++) {
pubsubUnsubscribeChannel(c, c->argv[j], 1, pubSubShardType);
}
}
if (clientTotalPubSubSubscriptionCount(c) == 0) c->flags &= ~CLIENT_PUBSUB;
}
