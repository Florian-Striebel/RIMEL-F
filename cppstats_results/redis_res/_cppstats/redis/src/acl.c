#include "server.h"
#include "sha256.h"
#include <fcntl.h>
#include <ctype.h>
rax *Users;
user *DefaultUser;
list *UsersToLoad;
list *ACLLog;
static rax *commandId = NULL;
static unsigned long nextid = 0;
struct ACLCategoryItem {
const char *name;
uint64_t flag;
} ACLCommandCategories[] = {
{"keyspace", ACL_CATEGORY_KEYSPACE},
{"read", ACL_CATEGORY_READ},
{"write", ACL_CATEGORY_WRITE},
{"set", ACL_CATEGORY_SET},
{"sortedset", ACL_CATEGORY_SORTEDSET},
{"list", ACL_CATEGORY_LIST},
{"hash", ACL_CATEGORY_HASH},
{"string", ACL_CATEGORY_STRING},
{"bitmap", ACL_CATEGORY_BITMAP},
{"hyperloglog", ACL_CATEGORY_HYPERLOGLOG},
{"geo", ACL_CATEGORY_GEO},
{"stream", ACL_CATEGORY_STREAM},
{"pubsub", ACL_CATEGORY_PUBSUB},
{"admin", ACL_CATEGORY_ADMIN},
{"fast", ACL_CATEGORY_FAST},
{"slow", ACL_CATEGORY_SLOW},
{"blocking", ACL_CATEGORY_BLOCKING},
{"dangerous", ACL_CATEGORY_DANGEROUS},
{"connection", ACL_CATEGORY_CONNECTION},
{"transaction", ACL_CATEGORY_TRANSACTION},
{"scripting", ACL_CATEGORY_SCRIPTING},
{NULL,0}
};
struct ACLUserFlag {
const char *name;
uint64_t flag;
} ACLUserFlags[] = {
{"on", USER_FLAG_ENABLED},
{"off", USER_FLAG_DISABLED},
{"nopass", USER_FLAG_NOPASS},
{"skip-sanitize-payload", USER_FLAG_SANITIZE_PAYLOAD_SKIP},
{"sanitize-payload", USER_FLAG_SANITIZE_PAYLOAD},
{NULL,0}
};
struct ACLSelectorFlags {
const char *name;
uint64_t flag;
} ACLSelectorFlags[] = {
{"allkeys", SELECTOR_FLAG_ALLKEYS},
{"allchannels", SELECTOR_FLAG_ALLCHANNELS},
{"allcommands", SELECTOR_FLAG_ALLCOMMANDS},
{NULL,0}
};
typedef struct {
uint32_t flags;
uint64_t allowed_commands[USER_COMMAND_BITS_COUNT/64];
sds **allowed_firstargs;
list *patterns;
list *channels;
} aclSelector;
void ACLResetFirstArgsForCommand(aclSelector *selector, unsigned long id);
void ACLResetFirstArgs(aclSelector *selector);
void ACLAddAllowedFirstArg(aclSelector *selector, unsigned long id, const char *sub);
void ACLFreeLogEntry(void *le);
int ACLSetSelector(aclSelector *selector, const char *op, size_t oplen);
#define HASH_PASSWORD_LEN SHA256_BLOCK_SIZE*2
int time_independent_strcmp(char *a, char *b) {
char bufa[CONFIG_AUTHPASS_MAX_LEN], bufb[CONFIG_AUTHPASS_MAX_LEN];
unsigned int alen = strlen(a);
unsigned int blen = strlen(b);
unsigned int j;
int diff = 0;
if (alen > sizeof(bufa) || blen > sizeof(bufb)) return 1;
memset(bufa,0,sizeof(bufa));
memset(bufb,0,sizeof(bufb));
memcpy(bufa,a,alen);
memcpy(bufb,b,blen);
for (j = 0; j < sizeof(bufa); j++) {
diff |= (bufa[j] ^ bufb[j]);
}
diff |= alen ^ blen;
return diff;
}
sds ACLHashPassword(unsigned char *cleartext, size_t len) {
SHA256_CTX ctx;
unsigned char hash[SHA256_BLOCK_SIZE];
char hex[HASH_PASSWORD_LEN];
char *cset = "0123456789abcdef";
sha256_init(&ctx);
sha256_update(&ctx,(unsigned char*)cleartext,len);
sha256_final(&ctx,hash);
for (int j = 0; j < SHA256_BLOCK_SIZE; j++) {
hex[j*2] = cset[((hash[j]&0xF0)>>4)];
hex[j*2+1] = cset[(hash[j]&0xF)];
}
return sdsnewlen(hex,HASH_PASSWORD_LEN);
}
int ACLCheckPasswordHash(unsigned char *hash, int hashlen) {
if (hashlen != HASH_PASSWORD_LEN) {
return C_ERR;
}
for(int i = 0; i < HASH_PASSWORD_LEN; i++) {
char c = hash[i];
if ((c < 'a' || c > 'f') && (c < '0' || c > '9')) {
return C_ERR;
}
}
return C_OK;
}
int ACLStringHasSpaces(const char *s, size_t len) {
for (size_t i = 0; i < len; i++) {
if (isspace(s[i]) || s[i] == 0) return 1;
}
return 0;
}
uint64_t ACLGetCommandCategoryFlagByName(const char *name) {
for (int j = 0; ACLCommandCategories[j].flag != 0; j++) {
if (!strcasecmp(name,ACLCommandCategories[j].name)) {
return ACLCommandCategories[j].flag;
}
}
return 0;
}
int ACLListMatchLoadedUser(void *definition, void *user) {
sds *user_definition = definition;
return sdscmp(user_definition[0], user) == 0;
}
int ACLListMatchSds(void *a, void *b) {
return sdscmp(a,b) == 0;
}
void ACLListFreeSds(void *item) {
sdsfree(item);
}
void *ACLListDupSds(void *item) {
return sdsdup(item);
}
typedef struct {
int flags;
sds pattern;
} keyPattern;
keyPattern *ACLKeyPatternCreate(sds pattern, int flags) {
keyPattern *new = (keyPattern *) zmalloc(sizeof(keyPattern));
new->pattern = pattern;
new->flags = flags;
return new;
}
void ACLKeyPatternFree(keyPattern *pattern) {
sdsfree(pattern->pattern);
zfree(pattern);
}
int ACLListMatchKeyPattern(void *a, void *b) {
return sdscmp(((keyPattern *) a)->pattern,((keyPattern *) b)->pattern) == 0;
}
void ACLListFreeKeyPattern(void *item) {
ACLKeyPatternFree(item);
}
void *ACLListDupKeyPattern(void *item) {
keyPattern *old = (keyPattern *) item;
return ACLKeyPatternCreate(sdsdup(old->pattern), old->flags);
}
sds sdsCatPatternString(sds base, keyPattern *pat) {
if (pat->flags == ACL_ALL_PERMISSION) {
base = sdscatlen(base,"~",1);
} else if (pat->flags == ACL_READ_PERMISSION) {
base = sdscatlen(base,"%R~",3);
} else if (pat->flags == ACL_WRITE_PERMISSION) {
base = sdscatlen(base,"%W~",3);
} else {
serverPanic("Invalid key pattern flag detected");
}
return sdscatsds(base, pat->pattern);
}
aclSelector *ACLCreateSelector(int flags) {
aclSelector *selector = zmalloc(sizeof(aclSelector));
selector->flags = flags | server.acl_pubsub_default;
selector->patterns = listCreate();
selector->channels = listCreate();
selector->allowed_firstargs = NULL;
listSetMatchMethod(selector->patterns,ACLListMatchKeyPattern);
listSetFreeMethod(selector->patterns,ACLListFreeKeyPattern);
listSetDupMethod(selector->patterns,ACLListDupKeyPattern);
listSetMatchMethod(selector->channels,ACLListMatchSds);
listSetFreeMethod(selector->channels,ACLListFreeSds);
listSetDupMethod(selector->channels,ACLListDupSds);
memset(selector->allowed_commands,0,sizeof(selector->allowed_commands));
return selector;
}
void ACLFreeSelector(aclSelector *selector) {
listRelease(selector->patterns);
listRelease(selector->channels);
ACLResetFirstArgs(selector);
zfree(selector);
}
aclSelector *ACLCopySelector(aclSelector *src) {
aclSelector *dst = zmalloc(sizeof(aclSelector));
dst->flags = src->flags;
dst->patterns = listDup(src->patterns);
dst->channels = listDup(src->channels);
memcpy(dst->allowed_commands,src->allowed_commands,
sizeof(dst->allowed_commands));
dst->allowed_firstargs = NULL;
if (src->allowed_firstargs) {
for (int j = 0; j < USER_COMMAND_BITS_COUNT; j++) {
if (!(src->allowed_firstargs[j])) continue;
for (int i = 0; src->allowed_firstargs[j][i]; i++) {
ACLAddAllowedFirstArg(dst, j, src->allowed_firstargs[j][i]);
}
}
}
return dst;
}
void ACLListFreeSelector(void *a) {
ACLFreeSelector((aclSelector *) a);
}
void *ACLListDuplicateSelector(void *src) {
return ACLCopySelector((aclSelector *)src);
}
aclSelector *ACLUserGetRootSelector(user *u) {
serverAssert(listLength(u->selectors));
aclSelector *s = (aclSelector *) listNodeValue(listFirst(u->selectors));
serverAssert(s->flags & SELECTOR_FLAG_ROOT);
return s;
}
user *ACLCreateUser(const char *name, size_t namelen) {
if (raxFind(Users,(unsigned char*)name,namelen) != raxNotFound) return NULL;
user *u = zmalloc(sizeof(*u));
u->name = sdsnewlen(name,namelen);
u->flags = USER_FLAG_DISABLED;
u->passwords = listCreate();
listSetMatchMethod(u->passwords,ACLListMatchSds);
listSetFreeMethod(u->passwords,ACLListFreeSds);
listSetDupMethod(u->passwords,ACLListDupSds);
u->selectors = listCreate();
listSetFreeMethod(u->selectors,ACLListFreeSelector);
listSetDupMethod(u->selectors,ACLListDuplicateSelector);
aclSelector *s = ACLCreateSelector(SELECTOR_FLAG_ROOT);
listAddNodeHead(u->selectors, s);
raxInsert(Users,(unsigned char*)name,namelen,u,NULL);
return u;
}
user *ACLCreateUnlinkedUser(void) {
char username[64];
for (int j = 0; ; j++) {
snprintf(username,sizeof(username),"__fakeuser:%d__",j);
user *fakeuser = ACLCreateUser(username,strlen(username));
if (fakeuser == NULL) continue;
int retval = raxRemove(Users,(unsigned char*) username,
strlen(username),NULL);
serverAssert(retval != 0);
return fakeuser;
}
}
void ACLFreeUser(user *u) {
sdsfree(u->name);
listRelease(u->passwords);
listRelease(u->selectors);
zfree(u);
}
void ACLFreeUserAndKillClients(user *u) {
listIter li;
listNode *ln;
listRewind(server.clients,&li);
while ((ln = listNext(&li)) != NULL) {
client *c = listNodeValue(ln);
if (c->user == u) {
c->user = DefaultUser;
c->authenticated = 0;
if (c == server.current_client) {
c->flags |= CLIENT_CLOSE_AFTER_COMMAND;
} else {
freeClientAsync(c);
}
}
}
ACLFreeUser(u);
}
void ACLCopyUser(user *dst, user *src) {
listRelease(dst->passwords);
listRelease(dst->selectors);
dst->passwords = listDup(src->passwords);
dst->selectors = listDup(src->selectors);
dst->flags = src->flags;
}
void ACLFreeUsersSet(rax *users) {
raxFreeWithCallback(users,(void(*)(void*))ACLFreeUserAndKillClients);
}
int ACLGetCommandBitCoordinates(uint64_t id, uint64_t *word, uint64_t *bit) {
if (id >= USER_COMMAND_BITS_COUNT) return C_ERR;
*word = id / sizeof(uint64_t) / 8;
*bit = 1ULL << (id % (sizeof(uint64_t) * 8));
return C_OK;
}
int ACLGetSelectorCommandBit(const aclSelector *selector, unsigned long id) {
uint64_t word, bit;
if (ACLGetCommandBitCoordinates(id,&word,&bit) == C_ERR) return 0;
return (selector->allowed_commands[word] & bit) != 0;
}
int ACLSelectorCanExecuteFutureCommands(aclSelector *selector) {
return ACLGetSelectorCommandBit(selector,USER_COMMAND_BITS_COUNT-1);
}
void ACLSetSelectorCommandBit(aclSelector *selector, unsigned long id, int value) {
uint64_t word, bit;
if (ACLGetCommandBitCoordinates(id,&word,&bit) == C_ERR) return;
if (value) {
selector->allowed_commands[word] |= bit;
} else {
selector->allowed_commands[word] &= ~bit;
selector->flags &= ~SELECTOR_FLAG_ALLCOMMANDS;
}
}
void ACLChangeSelectorPerm(aclSelector *selector, struct redisCommand *cmd, int allow) {
unsigned long id = cmd->id;
ACLSetSelectorCommandBit(selector,id,allow);
ACLResetFirstArgsForCommand(selector,id);
if (cmd->subcommands_dict) {
dictEntry *de;
dictIterator *di = dictGetSafeIterator(cmd->subcommands_dict);
while((de = dictNext(di)) != NULL) {
struct redisCommand *sub = (struct redisCommand *)dictGetVal(de);
ACLSetSelectorCommandBit(selector,sub->id,allow);
}
dictReleaseIterator(di);
}
}
void ACLSetSelectorCommandBitsForCategoryLogic(dict *commands, aclSelector *selector, uint64_t cflag, int value) {
dictIterator *di = dictGetIterator(commands);
dictEntry *de;
while ((de = dictNext(di)) != NULL) {
struct redisCommand *cmd = dictGetVal(de);
if (cmd->flags & CMD_MODULE) continue;
if (cmd->acl_categories & cflag) {
ACLChangeSelectorPerm(selector,cmd,value);
}
if (cmd->subcommands_dict) {
ACLSetSelectorCommandBitsForCategoryLogic(cmd->subcommands_dict, selector, cflag, value);
}
}
dictReleaseIterator(di);
}
int ACLSetSelectorCommandBitsForCategory(aclSelector *selector, const char *category, int value) {
uint64_t cflag = ACLGetCommandCategoryFlagByName(category);
if (!cflag) return C_ERR;
ACLSetSelectorCommandBitsForCategoryLogic(server.orig_commands, selector, cflag, value);
return C_OK;
}
void ACLCountCategoryBitsForCommands(dict *commands, aclSelector *selector, unsigned long *on, unsigned long *off, uint64_t cflag) {
dictIterator *di = dictGetIterator(commands);
dictEntry *de;
while ((de = dictNext(di)) != NULL) {
struct redisCommand *cmd = dictGetVal(de);
if (cmd->acl_categories & cflag) {
if (ACLGetSelectorCommandBit(selector,cmd->id))
(*on)++;
else
(*off)++;
}
if (cmd->subcommands_dict) {
ACLCountCategoryBitsForCommands(cmd->subcommands_dict, selector, on, off, cflag);
}
}
dictReleaseIterator(di);
}
int ACLCountCategoryBitsForSelector(aclSelector *selector, unsigned long *on, unsigned long *off,
const char *category)
{
uint64_t cflag = ACLGetCommandCategoryFlagByName(category);
if (!cflag) return C_ERR;
*on = *off = 0;
ACLCountCategoryBitsForCommands(server.orig_commands, selector, on, off, cflag);
return C_OK;
}
sds ACLDescribeSelectorCommandRulesSingleCommands(aclSelector *selector, aclSelector *fake_selector,
sds rules, dict *commands) {
dictIterator *di = dictGetIterator(commands);
dictEntry *de;
while ((de = dictNext(di)) != NULL) {
struct redisCommand *cmd = dictGetVal(de);
int userbit = ACLGetSelectorCommandBit(selector,cmd->id);
int fakebit = ACLGetSelectorCommandBit(fake_selector,cmd->id);
if (userbit != fakebit) {
rules = sdscatlen(rules, userbit ? "+" : "-", 1);
rules = sdscatsds(rules,cmd->fullname);
rules = sdscatlen(rules," ",1);
ACLChangeSelectorPerm(fake_selector,cmd,userbit);
}
if (cmd->subcommands_dict)
rules = ACLDescribeSelectorCommandRulesSingleCommands(selector,fake_selector,rules,cmd->subcommands_dict);
if (userbit == 0 && selector->allowed_firstargs &&
selector->allowed_firstargs[cmd->id])
{
for (int j = 0; selector->allowed_firstargs[cmd->id][j]; j++) {
rules = sdscatlen(rules,"+",1);
rules = sdscatsds(rules,cmd->fullname);
rules = sdscatlen(rules,"|",1);
rules = sdscatsds(rules,selector->allowed_firstargs[cmd->id][j]);
rules = sdscatlen(rules," ",1);
}
}
}
dictReleaseIterator(di);
return rules;
}
sds ACLDescribeSelectorCommandRules(aclSelector *selector) {
sds rules = sdsempty();
int additive;
aclSelector fs = {0};
aclSelector *fake_selector = &fs;
if (ACLSelectorCanExecuteFutureCommands(selector)) {
additive = 0;
rules = sdscat(rules,"+@all ");
ACLSetSelector(fake_selector,"+@all",-1);
} else {
additive = 1;
rules = sdscat(rules,"-@all ");
ACLSetSelector(fake_selector,"-@all",-1);
}
aclSelector ts = {0};
aclSelector *temp_selector = &ts;
char applied[sizeof(ACLCommandCategories)/sizeof(ACLCommandCategories[0])];
memset(applied, 0, sizeof(applied));
memcpy(temp_selector->allowed_commands,
selector->allowed_commands,
sizeof(selector->allowed_commands));
while (1) {
int best = -1;
unsigned long mindiff = INT_MAX, maxsame = 0;
for (int j = 0; ACLCommandCategories[j].flag != 0; j++) {
if (applied[j]) continue;
unsigned long on, off, diff, same;
ACLCountCategoryBitsForSelector(temp_selector,&on,&off,ACLCommandCategories[j].name);
diff = additive ? off : on;
same = additive ? on : off;
if (same > diff &&
((diff < mindiff) || (diff == mindiff && same > maxsame)))
{
best = j;
mindiff = diff;
maxsame = same;
}
}
if (best == -1) break;
sds op = sdsnewlen(additive ? "+@" : "-@", 2);
op = sdscat(op,ACLCommandCategories[best].name);
ACLSetSelector(fake_selector,op,-1);
sds invop = sdsnewlen(additive ? "-@" : "+@", 2);
invop = sdscat(invop,ACLCommandCategories[best].name);
ACLSetSelector(temp_selector,invop,-1);
rules = sdscatsds(rules,op);
rules = sdscatlen(rules," ",1);
sdsfree(op);
sdsfree(invop);
applied[best] = 1;
}
rules = ACLDescribeSelectorCommandRulesSingleCommands(selector,fake_selector,rules,server.orig_commands);
sdsrange(rules,0,-2);
if (memcmp(fake_selector->allowed_commands,
selector->allowed_commands,
sizeof(selector->allowed_commands)) != 0)
{
serverLog(LL_WARNING,
"CRITICAL ERROR: User ACLs don't match final bitmap: '%s'",
rules);
serverPanic("No bitmap match in ACLDescribeSelectorCommandRules()");
}
return rules;
}
sds ACLDescribeSelector(aclSelector *selector) {
listIter li;
listNode *ln;
sds res = sdsempty();
if (selector->flags & SELECTOR_FLAG_ALLKEYS) {
res = sdscatlen(res,"~* ",3);
} else {
listRewind(selector->patterns,&li);
while((ln = listNext(&li))) {
keyPattern *thispat = (keyPattern *)listNodeValue(ln);
res = sdsCatPatternString(res, thispat);
res = sdscatlen(res," ",1);
}
}
if (selector->flags & SELECTOR_FLAG_ALLCHANNELS) {
res = sdscatlen(res,"&* ",3);
} else {
res = sdscatlen(res,"resetchannels ",14);
listRewind(selector->channels,&li);
while((ln = listNext(&li))) {
sds thispat = listNodeValue(ln);
res = sdscatlen(res,"&",1);
res = sdscatsds(res,thispat);
res = sdscatlen(res," ",1);
}
}
sds rules = ACLDescribeSelectorCommandRules(selector);
res = sdscatsds(res,rules);
sdsfree(rules);
return res;
}
sds ACLDescribeUser(user *u) {
sds res = sdsempty();
for (int j = 0; ACLUserFlags[j].flag; j++) {
if (u->flags & ACLUserFlags[j].flag) {
res = sdscat(res,ACLUserFlags[j].name);
res = sdscatlen(res," ",1);
}
}
listIter li;
listNode *ln;
listRewind(u->passwords,&li);
while((ln = listNext(&li))) {
sds thispass = listNodeValue(ln);
res = sdscatlen(res,"#",1);
res = sdscatsds(res,thispass);
res = sdscatlen(res," ",1);
}
listRewind(u->selectors,&li);
while((ln = listNext(&li))) {
aclSelector *selector = (aclSelector *) listNodeValue(ln);
sds default_perm = ACLDescribeSelector(selector);
if (selector->flags & SELECTOR_FLAG_ROOT) {
res = sdscatfmt(res, "%s", default_perm);
} else {
res = sdscatfmt(res, " (%s)", default_perm);
}
sdsfree(default_perm);
}
return res;
}
struct redisCommand *ACLLookupCommand(const char *name) {
struct redisCommand *cmd;
sds sdsname = sdsnew(name);
cmd = lookupCommandBySdsLogic(server.orig_commands,sdsname);
sdsfree(sdsname);
return cmd;
}
void ACLResetFirstArgsForCommand(aclSelector *selector, unsigned long id) {
if (selector->allowed_firstargs && selector->allowed_firstargs[id]) {
for (int i = 0; selector->allowed_firstargs[id][i]; i++)
sdsfree(selector->allowed_firstargs[id][i]);
zfree(selector->allowed_firstargs[id]);
selector->allowed_firstargs[id] = NULL;
}
}
void ACLResetFirstArgs(aclSelector *selector) {
if (selector->allowed_firstargs == NULL) return;
for (int j = 0; j < USER_COMMAND_BITS_COUNT; j++) {
if (selector->allowed_firstargs[j]) {
for (int i = 0; selector->allowed_firstargs[j][i]; i++)
sdsfree(selector->allowed_firstargs[j][i]);
zfree(selector->allowed_firstargs[j]);
}
}
zfree(selector->allowed_firstargs);
selector->allowed_firstargs = NULL;
}
void ACLAddAllowedFirstArg(aclSelector *selector, unsigned long id, const char *sub) {
if (selector->allowed_firstargs == NULL) {
selector->allowed_firstargs = zcalloc(USER_COMMAND_BITS_COUNT * sizeof(sds*));
}
long items = 0;
if (selector->allowed_firstargs[id]) {
while(selector->allowed_firstargs[id][items]) {
if (!strcasecmp(selector->allowed_firstargs[id][items],sub))
return;
items++;
}
}
items += 2;
selector->allowed_firstargs[id] = zrealloc(selector->allowed_firstargs[id], sizeof(sds)*items);
selector->allowed_firstargs[id][items-2] = sdsnew(sub);
selector->allowed_firstargs[id][items-1] = NULL;
}
aclSelector *aclCreateSelectorFromOpSet(const char *opset, size_t opsetlen) {
serverAssert(opset[0] == '(' && opset[opsetlen - 1] == ')');
aclSelector *s = ACLCreateSelector(0);
int argc = 0;
sds trimmed = sdsnewlen(opset + 1, opsetlen - 2);
sds *argv = sdssplitargs(trimmed, &argc);
for (int i = 0; i < argc; i++) {
if (ACLSetSelector(s, argv[i], sdslen(argv[i])) == C_ERR) {
ACLFreeSelector(s);
s = NULL;
goto cleanup;
}
}
cleanup:
sdsfreesplitres(argv, argc);
sdsfree(trimmed);
return s;
}
int ACLSetSelector(aclSelector *selector, const char* op, size_t oplen) {
if (!strcasecmp(op,"allkeys") ||
!strcasecmp(op,"~*"))
{
selector->flags |= SELECTOR_FLAG_ALLKEYS;
listEmpty(selector->patterns);
} else if (!strcasecmp(op,"resetkeys")) {
selector->flags &= ~SELECTOR_FLAG_ALLKEYS;
listEmpty(selector->patterns);
} else if (!strcasecmp(op,"allchannels") ||
!strcasecmp(op,"&*"))
{
selector->flags |= SELECTOR_FLAG_ALLCHANNELS;
listEmpty(selector->channels);
} else if (!strcasecmp(op,"resetchannels")) {
selector->flags &= ~SELECTOR_FLAG_ALLCHANNELS;
listEmpty(selector->channels);
} else if (!strcasecmp(op,"allcommands") ||
!strcasecmp(op,"+@all"))
{
memset(selector->allowed_commands,255,sizeof(selector->allowed_commands));
selector->flags |= SELECTOR_FLAG_ALLCOMMANDS;
ACLResetFirstArgs(selector);
} else if (!strcasecmp(op,"nocommands") ||
!strcasecmp(op,"-@all"))
{
memset(selector->allowed_commands,0,sizeof(selector->allowed_commands));
selector->flags &= ~SELECTOR_FLAG_ALLCOMMANDS;
ACLResetFirstArgs(selector);
} else if (op[0] == '~' || op[0] == '%') {
if (selector->flags & SELECTOR_FLAG_ALLKEYS) {
errno = EEXIST;
return C_ERR;
}
int flags = 0;
size_t offset = 1;
if (op[0] == '%') {
for (; offset < oplen; offset++) {
if (toupper(op[offset]) == 'R' && !(flags & ACL_READ_PERMISSION)) {
flags |= ACL_READ_PERMISSION;
} else if (toupper(op[offset]) == 'W' && !(flags & ACL_WRITE_PERMISSION)) {
flags |= ACL_WRITE_PERMISSION;
} else if (op[offset] == '~') {
offset++;
break;
} else {
errno = EINVAL;
return C_ERR;
}
}
} else {
flags = ACL_ALL_PERMISSION;
}
if (ACLStringHasSpaces(op+offset,oplen-offset)) {
errno = EINVAL;
return C_ERR;
}
keyPattern *newpat = ACLKeyPatternCreate(sdsnewlen(op+offset,oplen-offset), flags);
listNode *ln = listSearchKey(selector->patterns,newpat);
if (ln == NULL) {
listAddNodeTail(selector->patterns,newpat);
} else {
((keyPattern *)listNodeValue(ln))->flags |= flags;
ACLKeyPatternFree(newpat);
}
selector->flags &= ~SELECTOR_FLAG_ALLKEYS;
} else if (op[0] == '&') {
if (selector->flags & SELECTOR_FLAG_ALLCHANNELS) {
errno = EISDIR;
return C_ERR;
}
if (ACLStringHasSpaces(op+1,oplen-1)) {
errno = EINVAL;
return C_ERR;
}
sds newpat = sdsnewlen(op+1,oplen-1);
listNode *ln = listSearchKey(selector->channels,newpat);
if (ln == NULL)
listAddNodeTail(selector->channels,newpat);
else
sdsfree(newpat);
selector->flags &= ~SELECTOR_FLAG_ALLCHANNELS;
} else if (op[0] == '+' && op[1] != '@') {
if (strrchr(op,'|') == NULL) {
struct redisCommand *cmd = ACLLookupCommand(op+1);
if (cmd == NULL) {
errno = ENOENT;
return C_ERR;
}
ACLChangeSelectorPerm(selector,cmd,1);
} else {
char *copy = zstrdup(op+1);
char *sub = strrchr(copy,'|');
sub[0] = '\0';
sub++;
struct redisCommand *cmd = ACLLookupCommand(copy);
if (cmd == NULL) {
zfree(copy);
errno = ENOENT;
return C_ERR;
}
if (cmd->parent) {
zfree(copy);
errno = ECHILD;
return C_ERR;
}
if (strlen(sub) == 0) {
zfree(copy);
errno = EINVAL;
return C_ERR;
}
if (cmd->subcommands_dict) {
cmd = ACLLookupCommand(op+1);
if (cmd == NULL) {
zfree(copy);
errno = ENOENT;
return C_ERR;
}
ACLChangeSelectorPerm(selector,cmd,1);
} else {
serverLog(LL_WARNING, "Deprecation warning: Allowing a first arg of an otherwise "
"blocked command is a misuse of ACL and may get disabled "
"in the future (offender: +%s)", op+1);
ACLAddAllowedFirstArg(selector,cmd->id,sub);
}
zfree(copy);
}
} else if (op[0] == '-' && op[1] != '@') {
struct redisCommand *cmd = ACLLookupCommand(op+1);
if (cmd == NULL) {
errno = ENOENT;
return C_ERR;
}
ACLChangeSelectorPerm(selector,cmd,0);
} else if ((op[0] == '+' || op[0] == '-') && op[1] == '@') {
int bitval = op[0] == '+' ? 1 : 0;
if (ACLSetSelectorCommandBitsForCategory(selector,op+2,bitval) == C_ERR) {
errno = ENOENT;
return C_ERR;
}
} else {
errno = EINVAL;
return C_ERR;
}
return C_OK;
}
int ACLSetUser(user *u, const char *op, ssize_t oplen) {
if (oplen == -1) oplen = strlen(op);
if (oplen == 0) return C_OK;
if (!strcasecmp(op,"on")) {
u->flags |= USER_FLAG_ENABLED;
u->flags &= ~USER_FLAG_DISABLED;
} else if (!strcasecmp(op,"off")) {
u->flags |= USER_FLAG_DISABLED;
u->flags &= ~USER_FLAG_ENABLED;
} else if (!strcasecmp(op,"skip-sanitize-payload")) {
u->flags |= USER_FLAG_SANITIZE_PAYLOAD_SKIP;
u->flags &= ~USER_FLAG_SANITIZE_PAYLOAD;
} else if (!strcasecmp(op,"sanitize-payload")) {
u->flags &= ~USER_FLAG_SANITIZE_PAYLOAD_SKIP;
u->flags |= USER_FLAG_SANITIZE_PAYLOAD;
} else if (!strcasecmp(op,"nopass")) {
u->flags |= USER_FLAG_NOPASS;
listEmpty(u->passwords);
} else if (!strcasecmp(op,"resetpass")) {
u->flags &= ~USER_FLAG_NOPASS;
listEmpty(u->passwords);
} else if (op[0] == '>' || op[0] == '#') {
sds newpass;
if (op[0] == '>') {
newpass = ACLHashPassword((unsigned char*)op+1,oplen-1);
} else {
if (ACLCheckPasswordHash((unsigned char*)op+1,oplen-1) == C_ERR) {
errno = EBADMSG;
return C_ERR;
}
newpass = sdsnewlen(op+1,oplen-1);
}
listNode *ln = listSearchKey(u->passwords,newpass);
if (ln == NULL)
listAddNodeTail(u->passwords,newpass);
else
sdsfree(newpass);
u->flags &= ~USER_FLAG_NOPASS;
} else if (op[0] == '<' || op[0] == '!') {
sds delpass;
if (op[0] == '<') {
delpass = ACLHashPassword((unsigned char*)op+1,oplen-1);
} else {
if (ACLCheckPasswordHash((unsigned char*)op+1,oplen-1) == C_ERR) {
errno = EBADMSG;
return C_ERR;
}
delpass = sdsnewlen(op+1,oplen-1);
}
listNode *ln = listSearchKey(u->passwords,delpass);
sdsfree(delpass);
if (ln) {
listDelNode(u->passwords,ln);
} else {
errno = ENODEV;
return C_ERR;
}
} else if (op[0] == '(' && op[oplen - 1] == ')') {
aclSelector *selector = aclCreateSelectorFromOpSet(op, oplen);
if (!selector) {
return C_ERR;
}
listAddNodeTail(u->selectors, selector);
return C_OK;
} else if (!strcasecmp(op,"clearselectors")) {
listIter li;
listNode *ln;
listRewind(u->selectors,&li);
serverAssert(listNext(&li));
while((ln = listNext(&li))) {
listDelNode(u->selectors, ln);
}
return C_OK;
} else if (!strcasecmp(op,"reset")) {
serverAssert(ACLSetUser(u,"resetpass",-1) == C_OK);
serverAssert(ACLSetUser(u,"resetkeys",-1) == C_OK);
serverAssert(ACLSetUser(u,"resetchannels",-1) == C_OK);
if (server.acl_pubsub_default & SELECTOR_FLAG_ALLCHANNELS)
serverAssert(ACLSetUser(u,"allchannels",-1) == C_OK);
serverAssert(ACLSetUser(u,"off",-1) == C_OK);
serverAssert(ACLSetUser(u,"sanitize-payload",-1) == C_OK);
serverAssert(ACLSetUser(u,"clearselectors",-1) == C_OK);
serverAssert(ACLSetUser(u,"-@all",-1) == C_OK);
} else {
aclSelector *selector = ACLUserGetRootSelector(u);
if (ACLSetSelector(selector, op, oplen) == C_ERR) {
return C_ERR;
}
}
return C_OK;
}
const char *ACLSetUserStringError(void) {
const char *errmsg = "Wrong format";
if (errno == ENOENT)
errmsg = "Unknown command or category name in ACL";
else if (errno == EINVAL)
errmsg = "Syntax error";
else if (errno == EEXIST)
errmsg = "Adding a pattern after the * pattern (or the "
"'allkeys' flag) is not valid and does not have any "
"effect. Try 'resetkeys' to start with an empty "
"list of patterns";
else if (errno == EISDIR)
errmsg = "Adding a pattern after the * pattern (or the "
"'allchannels' flag) is not valid and does not have any "
"effect. Try 'resetchannels' to start with an empty "
"list of channels";
else if (errno == ENODEV)
errmsg = "The password you are trying to remove from the user does "
"not exist";
else if (errno == EBADMSG)
errmsg = "The password hash must be exactly 64 characters and contain "
"only lowercase hexadecimal characters";
else if (errno == EALREADY)
errmsg = "Duplicate user found. A user can only be defined once in "
"config files";
else if (errno == ECHILD)
errmsg = "Allowing first-arg of a subcommand is not supported";
return errmsg;
}
user *ACLCreateDefaultUser(void) {
user *new = ACLCreateUser("default",7);
ACLSetUser(new,"+@all",-1);
ACLSetUser(new,"~*",-1);
ACLSetUser(new,"&*",-1);
ACLSetUser(new,"on",-1);
ACLSetUser(new,"nopass",-1);
return new;
}
void ACLInit(void) {
Users = raxNew();
UsersToLoad = listCreate();
listSetMatchMethod(UsersToLoad, ACLListMatchLoadedUser);
ACLLog = listCreate();
DefaultUser = ACLCreateDefaultUser();
}
int ACLCheckUserCredentials(robj *username, robj *password) {
user *u = ACLGetUserByName(username->ptr,sdslen(username->ptr));
if (u == NULL) {
errno = ENOENT;
return C_ERR;
}
if (u->flags & USER_FLAG_DISABLED) {
errno = EINVAL;
return C_ERR;
}
if (u->flags & USER_FLAG_NOPASS) return C_OK;
listIter li;
listNode *ln;
listRewind(u->passwords,&li);
sds hashed = ACLHashPassword(password->ptr,sdslen(password->ptr));
while((ln = listNext(&li))) {
sds thispass = listNodeValue(ln);
if (!time_independent_strcmp(hashed, thispass)) {
sdsfree(hashed);
return C_OK;
}
}
sdsfree(hashed);
errno = EINVAL;
return C_ERR;
}
int ACLAuthenticateUser(client *c, robj *username, robj *password) {
if (ACLCheckUserCredentials(username,password) == C_OK) {
c->authenticated = 1;
c->user = ACLGetUserByName(username->ptr,sdslen(username->ptr));
moduleNotifyUserChanged(c);
return C_OK;
} else {
addACLLogEntry(c,ACL_DENIED_AUTH,(c->flags & CLIENT_MULTI) ? ACL_LOG_CTX_MULTI : ACL_LOG_CTX_TOPLEVEL,0,username->ptr,NULL);
return C_ERR;
}
}
unsigned long ACLGetCommandID(sds cmdname) {
sds lowername = sdsdup(cmdname);
sdstolower(lowername);
if (commandId == NULL) commandId = raxNew();
void *id = raxFind(commandId,(unsigned char*)lowername,sdslen(lowername));
if (id != raxNotFound) {
sdsfree(lowername);
return (unsigned long)id;
}
raxInsert(commandId,(unsigned char*)lowername,strlen(lowername),
(void*)nextid,NULL);
sdsfree(lowername);
unsigned long thisid = nextid;
nextid++;
if (nextid == USER_COMMAND_BITS_COUNT-1) nextid++;
return thisid;
}
void ACLClearCommandID(void) {
if (commandId) raxFree(commandId);
commandId = NULL;
nextid = 0;
}
user *ACLGetUserByName(const char *name, size_t namelen) {
void *myuser = raxFind(Users,(unsigned char*)name,namelen);
if (myuser == raxNotFound) return NULL;
return myuser;
}
static int ACLSelectorCheckKey(aclSelector *selector, const char *key, int keylen, int keyspec_flags) {
if (selector->flags & SELECTOR_FLAG_ALLKEYS) return ACL_OK;
listIter li;
listNode *ln;
listRewind(selector->patterns,&li);
int key_flags = 0;
if (keyspec_flags & CMD_KEY_ACCESS) key_flags |= ACL_READ_PERMISSION;
if (keyspec_flags & CMD_KEY_INSERT) key_flags |= ACL_WRITE_PERMISSION;
if (keyspec_flags & CMD_KEY_DELETE) key_flags |= ACL_WRITE_PERMISSION;
if (keyspec_flags & CMD_KEY_UPDATE) key_flags |= ACL_WRITE_PERMISSION;
while((ln = listNext(&li))) {
keyPattern *pattern = listNodeValue(ln);
if ((pattern->flags & key_flags) != key_flags)
continue;
size_t plen = sdslen(pattern->pattern);
if (stringmatchlen(pattern->pattern,plen,key,keylen,0))
return ACL_OK;
}
return ACL_DENIED_KEY;
}
static int ACLDoesCommandHaveChannels(struct redisCommand *cmd) {
return (cmd->proc == publishCommand
|| cmd->proc == subscribeCommand
|| cmd->proc == psubscribeCommand
|| cmd->proc == spublishCommand
|| cmd->proc == ssubscribeCommand);
}
static int ACLCheckChannelAgainstList(list *reference, const char *channel, int channellen, int literal) {
listIter li;
listNode *ln;
listRewind(reference, &li);
while((ln = listNext(&li))) {
sds pattern = listNodeValue(ln);
size_t plen = sdslen(pattern);
if ((literal && !strcmp(pattern,channel)) ||
(!literal && stringmatchlen(pattern,plen,channel,channellen,0)))
{
return ACL_OK;
}
}
return ACL_DENIED_CHANNEL;
}
static int ACLSelectorCheckPubsubArguments(aclSelector *s, robj **argv, int idx, int count, int literal, int *idxptr) {
for (int j = idx; j < idx+count; j++) {
if (ACLCheckChannelAgainstList(s->channels, argv[j]->ptr, sdslen(argv[j]->ptr), literal != ACL_OK)) {
if (idxptr) *idxptr = j;
return ACL_DENIED_CHANNEL;
}
}
return ACL_OK;
}
typedef struct {
int keys_init;
getKeysResult keys;
} aclKeyResultCache;
void initACLKeyResultCache(aclKeyResultCache *cache) {
cache->keys_init = 0;
}
void cleanupACLKeyResultCache(aclKeyResultCache *cache) {
if (cache->keys_init) getKeysFreeResult(&(cache->keys));
}
static int ACLSelectorCheckCmd(aclSelector *selector, struct redisCommand *cmd, robj **argv, int argc, int *keyidxptr, aclKeyResultCache *cache) {
uint64_t id = cmd->id;
int ret;
if (!(selector->flags & SELECTOR_FLAG_ALLCOMMANDS) && !(cmd->flags & CMD_NO_AUTH)) {
if (ACLGetSelectorCommandBit(selector,id) == 0) {
if (argc < 2 ||
selector->allowed_firstargs == NULL ||
selector->allowed_firstargs[id] == NULL)
{
return ACL_DENIED_CMD;
}
long subid = 0;
while (1) {
if (selector->allowed_firstargs[id][subid] == NULL)
return ACL_DENIED_CMD;
int idx = cmd->parent ? 2 : 1;
if (!strcasecmp(argv[idx]->ptr,selector->allowed_firstargs[id][subid]))
break;
subid++;
}
}
}
if (!(selector->flags & SELECTOR_FLAG_ALLKEYS) && doesCommandHaveKeys(cmd)) {
if (!(cache->keys_init)) {
cache->keys = (getKeysResult) GETKEYS_RESULT_INIT;
getKeysFromCommandWithSpecs(cmd, argv, argc, GET_KEYSPEC_DEFAULT, &(cache->keys));
cache->keys_init = 1;
}
getKeysResult *result = &(cache->keys);
keyReference *resultidx = result->keys;
for (int j = 0; j < result->numkeys; j++) {
int idx = resultidx[j].pos;
ret = ACLSelectorCheckKey(selector, argv[idx]->ptr, sdslen(argv[idx]->ptr), resultidx[j].flags);
if (ret != ACL_OK) {
if (resultidx) *keyidxptr = resultidx[j].pos;
return ret;
}
}
}
if (!(selector->flags & SELECTOR_FLAG_ALLCHANNELS) && ACLDoesCommandHaveChannels(cmd)) {
if (cmd->proc == publishCommand || cmd->proc == spublishCommand) {
ret = ACLSelectorCheckPubsubArguments(selector,argv, 1, 1, 0, keyidxptr);
} else if (cmd->proc == subscribeCommand || cmd->proc == ssubscribeCommand) {
ret = ACLSelectorCheckPubsubArguments(selector, argv, 1, argc-1, 0, keyidxptr);
} else if (cmd->proc == psubscribeCommand) {
ret = ACLSelectorCheckPubsubArguments(selector, argv, 1, argc-1, 1, keyidxptr);
} else {
serverPanic("Encountered a command declared with channels but not handled");
}
if (ret != ACL_OK) {
return ret;
}
}
return ACL_OK;
}
int ACLUserCheckKeyPerm(user *u, const char *key, int keylen, int flags) {
listIter li;
listNode *ln;
if (u == NULL) return ACL_OK;
listRewind(u->selectors,&li);
while((ln = listNext(&li))) {
aclSelector *s = (aclSelector *) listNodeValue(ln);
if (ACLSelectorCheckKey(s, key, keylen, flags) == ACL_OK) {
return ACL_OK;
}
}
return ACL_DENIED_KEY;
}
int ACLUserCheckChannelPerm(user *u, sds channel, int literal) {
listIter li;
listNode *ln;
if (u == NULL) return ACL_OK;
listRewind(u->selectors,&li);
while((ln = listNext(&li))) {
aclSelector *s = (aclSelector *) listNodeValue(ln);
if (s->flags & SELECTOR_FLAG_ALLCHANNELS) return ACL_OK;
if (ACLCheckChannelAgainstList(s->channels, channel, sdslen(channel), literal) == ACL_OK) {
return ACL_OK;
}
}
return ACL_DENIED_CHANNEL;
}
int ACLCheckAllUserCommandPerm(user *u, struct redisCommand *cmd, robj **argv, int argc, int *idxptr) {
listIter li;
listNode *ln;
if (u == NULL) return ACL_OK;
int relevant_error = ACL_DENIED_CMD;
int local_idxptr = 0, last_idx = 0;
aclKeyResultCache cache;
initACLKeyResultCache(&cache);
listRewind(u->selectors,&li);
while((ln = listNext(&li))) {
aclSelector *s = (aclSelector *) listNodeValue(ln);
int acl_retval = ACLSelectorCheckCmd(s, cmd, argv, argc, &local_idxptr, &cache);
if (acl_retval == ACL_OK) {
cleanupACLKeyResultCache(&cache);
return ACL_OK;
}
if (acl_retval > relevant_error ||
(acl_retval == relevant_error && local_idxptr > last_idx))
{
relevant_error = acl_retval;
last_idx = local_idxptr;
}
}
*idxptr = last_idx;
cleanupACLKeyResultCache(&cache);
return relevant_error;
}
int ACLCheckAllPerm(client *c, int *idxptr) {
return ACLCheckAllUserCommandPerm(c->user, c->cmd, c->argv, c->argc, idxptr);
}
void ACLKillPubsubClientsIfNeeded(user *new, user *original) {
listIter li, lpi;
listNode *ln, *lpn;
robj *o;
int kill = 0;
listRewind(new->selectors,&li);
while((ln = listNext(&li))) {
aclSelector *s = (aclSelector *) listNodeValue(ln);
if (s->flags & SELECTOR_FLAG_ALLCHANNELS) return;
}
list *upcoming = listCreate();
listRewind(new->selectors,&li);
while((ln = listNext(&li))) {
aclSelector *s = (aclSelector *) listNodeValue(ln);
listRewind(s->channels, &lpi);
while((lpn = listNext(&lpi))) {
listAddNodeTail(upcoming, listNodeValue(lpn));
}
}
int match = 1;
listRewind(original->selectors,&li);
while((ln = listNext(&li)) && match) {
aclSelector *s = (aclSelector *) listNodeValue(ln);
listRewind(s->channels, &lpi);
while((lpn = listNext(&lpi)) && match) {
if (!listSearchKey(upcoming, listNodeValue(lpn))) {
match = 0;
break;
}
}
}
if (match) {
listRelease(upcoming);
return;
}
listRewind(server.clients,&li);
while ((ln = listNext(&li)) != NULL) {
client *c = listNodeValue(ln);
kill = 0;
if (c->user == original && getClientType(c) == CLIENT_TYPE_PUBSUB) {
listRewind(c->pubsub_patterns,&lpi);
while (!kill && ((lpn = listNext(&lpi)) != NULL)) {
o = lpn->value;
int res = ACLCheckChannelAgainstList(upcoming, o->ptr, sdslen(o->ptr), 1);
kill = (res == ACL_DENIED_CHANNEL);
}
if (!kill) {
dictIterator *di = dictGetIterator(c->pubsub_channels);
dictEntry *de;
while (!kill && ((de = dictNext(di)) != NULL)) {
o = dictGetKey(de);
int res = ACLCheckChannelAgainstList(upcoming, o->ptr, sdslen(o->ptr), 0);
kill = (res == ACL_DENIED_CHANNEL);
}
dictReleaseIterator(di);
di = dictGetIterator(c->pubsubshard_channels);
while (!kill && ((de = dictNext(di)) != NULL)) {
o = dictGetKey(de);
int res = ACLCheckChannelAgainstList(upcoming, o->ptr, sdslen(o->ptr), 0);
kill = (res == ACL_DENIED_CHANNEL);
}
dictReleaseIterator(di);
}
if (kill) {
freeClient(c);
}
}
}
listRelease(upcoming);
}
sds *ACLMergeSelectorArguments(sds *argv, int argc, int *merged_argc, int *invalid_idx) {
*merged_argc = 0;
int open_bracket_start = -1;
sds *acl_args = (sds *) zmalloc(sizeof(sds) * argc);
sds selector = NULL;
for (int j = 0; j < argc; j++) {
char *op = argv[j];
if (op[0] == '(' && op[sdslen(op) - 1] != ')') {
selector = sdsdup(argv[j]);
open_bracket_start = j;
continue;
}
if (open_bracket_start != -1) {
selector = sdscatfmt(selector, " %s", op);
if (op[sdslen(op) - 1] == ')') {
open_bracket_start = -1;
acl_args[*merged_argc] = selector;
(*merged_argc)++;
}
continue;
}
acl_args[*merged_argc] = sdsdup(argv[j]);
(*merged_argc)++;
}
if (open_bracket_start != -1) {
for (int i = 0; i < *merged_argc; i++) sdsfree(acl_args[i]);
zfree(acl_args);
sdsfree(selector);
if (invalid_idx) *invalid_idx = open_bracket_start;
return NULL;
}
return acl_args;
}
int ACLAppendUserForLoading(sds *argv, int argc, int *argc_err) {
if (argc < 2 || strcasecmp(argv[0],"user")) {
if (argc_err) *argc_err = 0;
return C_ERR;
}
if (listSearchKey(UsersToLoad, argv[1])) {
if (argc_err) *argc_err = 1;
errno = EALREADY;
return C_ERR;
}
user *fakeuser = ACLCreateUnlinkedUser();
int merged_argc;
sds *acl_args = ACLMergeSelectorArguments(argv + 2, argc - 2, &merged_argc, argc_err);
if (!acl_args) {
return C_ERR;
}
for (int j = 0; j < merged_argc; j++) {
if (ACLSetUser(fakeuser,acl_args[j],sdslen(acl_args[j])) == C_ERR) {
if (errno != ENOENT) {
ACLFreeUser(fakeuser);
if (argc_err) *argc_err = j;
for (int i = 0; i < merged_argc; i++) sdsfree(acl_args[i]);
zfree(acl_args);
return C_ERR;
}
}
}
sds *copy = zmalloc(sizeof(sds)*(merged_argc + 2));
copy[0] = sdsdup(argv[1]);
for (int j = 0; j < merged_argc; j++) copy[j+1] = sdsdup(acl_args[j]);
copy[merged_argc + 1] = NULL;
listAddNodeTail(UsersToLoad,copy);
ACLFreeUser(fakeuser);
for (int i = 0; i < merged_argc; i++) sdsfree(acl_args[i]);
zfree(acl_args);
return C_OK;
}
int ACLLoadConfiguredUsers(void) {
listIter li;
listNode *ln;
listRewind(UsersToLoad,&li);
while ((ln = listNext(&li)) != NULL) {
sds *aclrules = listNodeValue(ln);
sds username = aclrules[0];
if (ACLStringHasSpaces(aclrules[0],sdslen(aclrules[0]))) {
serverLog(LL_WARNING,"Spaces not allowed in ACL usernames");
return C_ERR;
}
user *u = ACLCreateUser(username,sdslen(username));
if (!u) {
serverAssert(!strcmp(username, "default"));
u = ACLGetUserByName("default",7);
ACLSetUser(u,"reset",-1);
}
for (int j = 1; aclrules[j]; j++) {
if (ACLSetUser(u,aclrules[j],sdslen(aclrules[j])) != C_OK) {
const char *errmsg = ACLSetUserStringError();
serverLog(LL_WARNING,"Error loading ACL rule '%s' for "
"the user named '%s': %s",
aclrules[j],aclrules[0],errmsg);
return C_ERR;
}
}
if (u->flags & USER_FLAG_DISABLED) {
serverLog(LL_NOTICE, "The user '%s' is disabled (there is no "
"'on' modifier in the user description). Make "
"sure this is not a configuration error.",
aclrules[0]);
}
}
return C_OK;
}
sds ACLLoadFromFile(const char *filename) {
FILE *fp;
char buf[1024];
if ((fp = fopen(filename,"r")) == NULL) {
sds errors = sdscatprintf(sdsempty(),
"Error loading ACLs, opening file '%s': %s",
filename, strerror(errno));
return errors;
}
sds acls = sdsempty();
while(fgets(buf,sizeof(buf),fp) != NULL)
acls = sdscat(acls,buf);
fclose(fp);
int totlines;
sds *lines, errors = sdsempty();
lines = sdssplitlen(acls,strlen(acls),"\n",1,&totlines);
sdsfree(acls);
rax *old_users = Users;
Users = raxNew();
for (int i = 0; i < totlines; i++) {
sds *argv;
int argc;
int linenum = i+1;
lines[i] = sdstrim(lines[i]," \t\r\n");
if (lines[i][0] == '\0') continue;
argv = sdssplitlen(lines[i],sdslen(lines[i])," ",1,&argc);
if (argv == NULL) {
errors = sdscatprintf(errors,
"%s:%d: unbalanced quotes in acl line. ",
server.acl_filename, linenum);
continue;
}
if (argc == 0) {
sdsfreesplitres(argv,argc);
continue;
}
if (strcmp(argv[0],"user") || argc < 2) {
errors = sdscatprintf(errors,
"%s:%d should start with user keyword followed "
"by the username. ", server.acl_filename,
linenum);
sdsfreesplitres(argv,argc);
continue;
}
if (ACLStringHasSpaces(argv[1],sdslen(argv[1]))) {
errors = sdscatprintf(errors,
"'%s:%d: username '%s' contains invalid characters. ",
server.acl_filename, linenum, argv[1]);
sdsfreesplitres(argv,argc);
continue;
}
user *u = ACLCreateUser(argv[1],sdslen(argv[1]));
if (!u) {
errors = sdscatprintf(errors,"WARNING: Duplicate user '%s' found on line %d. ", argv[1], linenum);
sdsfreesplitres(argv,argc);
continue;
}
int merged_argc;
sds *acl_args = ACLMergeSelectorArguments(argv + 2, argc - 2, &merged_argc, NULL);
if (!acl_args) {
errors = sdscatprintf(errors,
"%s:%d: Unmatched parenthesis in selector definition.",
server.acl_filename, linenum);
}
int j;
for (j = 0; j < merged_argc; j++) {
acl_args[j] = sdstrim(acl_args[j],"\t\r\n");
if (ACLSetUser(u,acl_args[j],sdslen(acl_args[j])) != C_OK) {
const char *errmsg = ACLSetUserStringError();
errors = sdscatprintf(errors,
"%s:%d: %s. ",
server.acl_filename, linenum, errmsg);
continue;
}
}
for (int i = 0; i < merged_argc; i++) sdsfree(acl_args[i]);
zfree(acl_args);
if (sdslen(errors) != 0) {
sdsfreesplitres(argv,argc);
continue;
}
sdsfreesplitres(argv,argc);
}
sdsfreesplitres(lines,totlines);
if (sdslen(errors) == 0) {
user *new_default = ACLGetUserByName("default",7);
if (!new_default) {
new_default = ACLCreateDefaultUser();
}
ACLCopyUser(DefaultUser,new_default);
ACLFreeUser(new_default);
raxInsert(Users,(unsigned char*)"default",7,DefaultUser,NULL);
raxRemove(old_users,(unsigned char*)"default",7,NULL);
ACLFreeUsersSet(old_users);
sdsfree(errors);
return NULL;
} else {
ACLFreeUsersSet(Users);
Users = old_users;
errors = sdscat(errors,"WARNING: ACL errors detected, no change to the previously active ACL rules was performed");
return errors;
}
}
int ACLSaveToFile(const char *filename) {
sds acl = sdsempty();
int fd = -1;
sds tmpfilename = NULL;
int retval = C_ERR;
raxIterator ri;
raxStart(&ri,Users);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
user *u = ri.data;
sds user = sdsnew("user ");
user = sdscatsds(user,u->name);
user = sdscatlen(user," ",1);
sds descr = ACLDescribeUser(u);
user = sdscatsds(user,descr);
sdsfree(descr);
acl = sdscatsds(acl,user);
acl = sdscatlen(acl,"\n",1);
sdsfree(user);
}
raxStop(&ri);
tmpfilename = sdsnew(filename);
tmpfilename = sdscatfmt(tmpfilename,".tmp-%i-%I",
(int)getpid(),(int)mstime());
if ((fd = open(tmpfilename,O_WRONLY|O_CREAT,0644)) == -1) {
serverLog(LL_WARNING,"Opening temp ACL file for ACL SAVE: %s",
strerror(errno));
goto cleanup;
}
if (write(fd,acl,sdslen(acl)) != (ssize_t)sdslen(acl)) {
serverLog(LL_WARNING,"Writing ACL file for ACL SAVE: %s",
strerror(errno));
goto cleanup;
}
close(fd); fd = -1;
if (rename(tmpfilename,filename) == -1) {
serverLog(LL_WARNING,"Renaming ACL file for ACL SAVE: %s",
strerror(errno));
goto cleanup;
}
sdsfree(tmpfilename); tmpfilename = NULL;
retval = C_OK;
cleanup:
if (fd != -1) close(fd);
if (tmpfilename) unlink(tmpfilename);
sdsfree(tmpfilename);
sdsfree(acl);
return retval;
}
void ACLLoadUsersAtStartup(void) {
if (server.acl_filename[0] != '\0' && listLength(UsersToLoad) != 0) {
serverLog(LL_WARNING,
"Configuring Redis with users defined in redis.conf and at "
"the same setting an ACL file path is invalid. This setup "
"is very likely to lead to configuration errors and security "
"holes, please define either an ACL file or declare users "
"directly in your redis.conf, but not both.");
exit(1);
}
if (ACLLoadConfiguredUsers() == C_ERR) {
serverLog(LL_WARNING,
"Critical error while loading ACLs. Exiting.");
exit(1);
}
if (server.acl_filename[0] != '\0') {
sds errors = ACLLoadFromFile(server.acl_filename);
if (errors) {
serverLog(LL_WARNING,
"Aborting Redis startup because of ACL errors: %s", errors);
sdsfree(errors);
exit(1);
}
}
}
#define ACL_LOG_GROUPING_MAX_TIME_DELTA 60000
typedef struct ACLLogEntry {
uint64_t count;
int reason;
int context;
sds object;
sds username;
mstime_t ctime;
sds cinfo;
} ACLLogEntry;
int ACLLogMatchEntry(ACLLogEntry *a, ACLLogEntry *b) {
if (a->reason != b->reason) return 0;
if (a->context != b->context) return 0;
mstime_t delta = a->ctime - b->ctime;
if (delta < 0) delta = -delta;
if (delta > ACL_LOG_GROUPING_MAX_TIME_DELTA) return 0;
if (sdscmp(a->object,b->object) != 0) return 0;
if (sdscmp(a->username,b->username) != 0) return 0;
return 1;
}
void ACLFreeLogEntry(void *leptr) {
ACLLogEntry *le = leptr;
sdsfree(le->object);
sdsfree(le->username);
sdsfree(le->cinfo);
zfree(le);
}
void addACLLogEntry(client *c, int reason, int context, int argpos, sds username, sds object) {
struct ACLLogEntry *le = zmalloc(sizeof(*le));
le->count = 1;
le->reason = reason;
le->username = sdsdup(username ? username : c->user->name);
le->ctime = mstime();
if (object) {
le->object = object;
} else {
switch(reason) {
case ACL_DENIED_CMD: le->object = sdsdup(c->cmd->fullname); break;
case ACL_DENIED_KEY: le->object = sdsdup(c->argv[argpos]->ptr); break;
case ACL_DENIED_CHANNEL: le->object = sdsdup(c->argv[argpos]->ptr); break;
case ACL_DENIED_AUTH: le->object = sdsdup(c->argv[0]->ptr); break;
default: le->object = sdsempty();
}
}
client *realclient = c;
if (realclient->flags & CLIENT_SCRIPT) realclient = server.script_caller;
le->cinfo = catClientInfoString(sdsempty(),realclient);
le->context = context;
long toscan = 10;
listIter li;
listNode *ln;
listRewind(ACLLog,&li);
ACLLogEntry *match = NULL;
while (toscan-- && (ln = listNext(&li)) != NULL) {
ACLLogEntry *current = listNodeValue(ln);
if (ACLLogMatchEntry(current,le)) {
match = current;
listDelNode(ACLLog,ln);
listAddNodeHead(ACLLog,current);
break;
}
}
if (match) {
sdsfree(match->cinfo);
match->cinfo = le->cinfo;
match->ctime = le->ctime;
match->count++;
le->cinfo = NULL;
ACLFreeLogEntry(le);
} else {
listAddNodeHead(ACLLog, le);
while(listLength(ACLLog) > server.acllog_max_len) {
listNode *ln = listLast(ACLLog);
ACLLogEntry *le = listNodeValue(ln);
ACLFreeLogEntry(le);
listDelNode(ACLLog,ln);
}
}
}
void aclCatWithFlags(client *c, dict *commands, uint64_t cflag, int *arraylen) {
dictEntry *de;
dictIterator *di = dictGetIterator(commands);
while ((de = dictNext(di)) != NULL) {
struct redisCommand *cmd = dictGetVal(de);
if (cmd->flags & CMD_MODULE) continue;
if (cmd->acl_categories & cflag) {
addReplyBulkCBuffer(c, cmd->fullname, sdslen(cmd->fullname));
(*arraylen)++;
}
if (cmd->subcommands_dict) {
aclCatWithFlags(c, cmd->subcommands_dict, cflag, arraylen);
}
}
dictReleaseIterator(di);
}
int aclAddReplySelectorDescription(client *c, aclSelector *s) {
listIter li;
listNode *ln;
addReplyBulkCString(c,"commands");
sds cmddescr = ACLDescribeSelectorCommandRules(s);
addReplyBulkSds(c,cmddescr);
addReplyBulkCString(c,"keys");
if (s->flags & SELECTOR_FLAG_ALLKEYS) {
addReplyBulkCBuffer(c,"~*",2);
} else {
sds dsl = sdsempty();
listRewind(s->patterns,&li);
while((ln = listNext(&li))) {
keyPattern *thispat = (keyPattern *) listNodeValue(ln);
if (ln != listFirst(s->patterns)) dsl = sdscat(dsl, " ");
dsl = sdsCatPatternString(dsl, thispat);
}
addReplyBulkSds(c, dsl);
}
addReplyBulkCString(c,"channels");
if (s->flags & SELECTOR_FLAG_ALLCHANNELS) {
addReplyBulkCBuffer(c,"&*",2);
} else {
sds dsl = sdsempty();
listRewind(s->channels,&li);
while((ln = listNext(&li))) {
sds thispat = listNodeValue(ln);
if (ln != listFirst(s->channels)) dsl = sdscat(dsl, " ");
dsl = sdscatfmt(dsl, "&%S", thispat);
}
addReplyBulkSds(c, dsl);
}
return 3;
}
void aclCommand(client *c) {
char *sub = c->argv[1]->ptr;
if (!strcasecmp(sub,"setuser") && c->argc >= 3) {
for (int j = 2; j < c->argc; j++) {
redactClientCommandArgument(c, j);
}
sds username = c->argv[2]->ptr;
if (ACLStringHasSpaces(username,sdslen(username))) {
addReplyErrorFormat(c,
"Usernames can't contain spaces or null characters");
return;
}
int merged_argc = 0, invalid_idx = 0;
sds *temp_argv = zmalloc(c->argc * sizeof(sds *));
for (int i = 3; i < c->argc; i++) temp_argv[i-3] = c->argv[i]->ptr;
sds *acl_args = ACLMergeSelectorArguments(temp_argv, c->argc - 3, &merged_argc, &invalid_idx);
zfree(temp_argv);
if (!acl_args) {
addReplyErrorFormat(c,
"Unmatched parenthesis in acl selector starting "
"at '%s'.", (char *) c->argv[invalid_idx]->ptr);
return;
}
user *tempu = ACLCreateUnlinkedUser();
user *u = ACLGetUserByName(username,sdslen(username));
if (u) ACLCopyUser(tempu, u);
for (int j = 0; j < merged_argc; j++) {
if (ACLSetUser(tempu,acl_args[j],sdslen(acl_args[j])) != C_OK) {
const char *errmsg = ACLSetUserStringError();
addReplyErrorFormat(c,
"Error in ACL SETUSER modifier '%s': %s",
(char*)acl_args[j], errmsg);
goto setuser_cleanup;
}
}
if (u) ACLKillPubsubClientsIfNeeded(tempu, u);
if (!u) u = ACLCreateUser(username,sdslen(username));
serverAssert(u != NULL);
ACLCopyUser(u, tempu);
addReply(c,shared.ok);
setuser_cleanup:
ACLFreeUser(tempu);
for (int i = 0; i < merged_argc; i++) sdsfree(acl_args[i]);
zfree(acl_args);
return;
} else if (!strcasecmp(sub,"deluser") && c->argc >= 3) {
int deleted = 0;
for (int j = 2; j < c->argc; j++) {
sds username = c->argv[j]->ptr;
if (!strcmp(username,"default")) {
addReplyError(c,"The 'default' user cannot be removed");
return;
}
}
for (int j = 2; j < c->argc; j++) {
sds username = c->argv[j]->ptr;
user *u;
if (raxRemove(Users,(unsigned char*)username,
sdslen(username),
(void**)&u))
{
ACLFreeUserAndKillClients(u);
deleted++;
}
}
addReplyLongLong(c,deleted);
} else if (!strcasecmp(sub,"getuser") && c->argc == 3) {
user *u = ACLGetUserByName(c->argv[2]->ptr,sdslen(c->argv[2]->ptr));
if (u == NULL) {
addReplyNull(c);
return;
}
void *ufields = addReplyDeferredLen(c);
int fields = 3;
addReplyBulkCString(c,"flags");
void *deflen = addReplyDeferredLen(c);
int numflags = 0;
for (int j = 0; ACLUserFlags[j].flag; j++) {
if (u->flags & ACLUserFlags[j].flag) {
addReplyBulkCString(c,ACLUserFlags[j].name);
numflags++;
}
}
setDeferredSetLen(c,deflen,numflags);
addReplyBulkCString(c,"passwords");
addReplyArrayLen(c,listLength(u->passwords));
listIter li;
listNode *ln;
listRewind(u->passwords,&li);
while((ln = listNext(&li))) {
sds thispass = listNodeValue(ln);
addReplyBulkCBuffer(c,thispass,sdslen(thispass));
}
fields += aclAddReplySelectorDescription(c, ACLUserGetRootSelector(u));
addReplyBulkCString(c,"selectors");
addReplyArrayLen(c, listLength(u->selectors) - 1);
listRewind(u->selectors,&li);
serverAssert(listNext(&li));
while((ln = listNext(&li))) {
void *slen = addReplyDeferredLen(c);
int sfields = aclAddReplySelectorDescription(c, (aclSelector *)listNodeValue(ln));
setDeferredMapLen(c, slen, sfields);
}
setDeferredMapLen(c, ufields, fields);
} else if ((!strcasecmp(sub,"list") || !strcasecmp(sub,"users")) &&
c->argc == 2)
{
int justnames = !strcasecmp(sub,"users");
addReplyArrayLen(c,raxSize(Users));
raxIterator ri;
raxStart(&ri,Users);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
user *u = ri.data;
if (justnames) {
addReplyBulkCBuffer(c,u->name,sdslen(u->name));
} else {
sds config = sdsnew("user ");
config = sdscatsds(config,u->name);
config = sdscatlen(config," ",1);
sds descr = ACLDescribeUser(u);
config = sdscatsds(config,descr);
sdsfree(descr);
addReplyBulkSds(c,config);
}
}
raxStop(&ri);
} else if (!strcasecmp(sub,"whoami") && c->argc == 2) {
if (c->user != NULL) {
addReplyBulkCBuffer(c,c->user->name,sdslen(c->user->name));
} else {
addReplyNull(c);
}
} else if (server.acl_filename[0] == '\0' &&
(!strcasecmp(sub,"load") || !strcasecmp(sub,"save")))
{
addReplyError(c,"This Redis instance is not configured to use an ACL file. You may want to specify users via the ACL SETUSER command and then issue a CONFIG REWRITE (assuming you have a Redis configuration file set) in order to store users in the Redis configuration.");
return;
} else if (!strcasecmp(sub,"load") && c->argc == 2) {
sds errors = ACLLoadFromFile(server.acl_filename);
if (errors == NULL) {
addReply(c,shared.ok);
} else {
addReplyError(c,errors);
sdsfree(errors);
}
} else if (!strcasecmp(sub,"save") && c->argc == 2) {
if (ACLSaveToFile(server.acl_filename) == C_OK) {
addReply(c,shared.ok);
} else {
addReplyError(c,"There was an error trying to save the ACLs. "
"Please check the server logs for more "
"information");
}
} else if (!strcasecmp(sub,"cat") && c->argc == 2) {
void *dl = addReplyDeferredLen(c);
int j;
for (j = 0; ACLCommandCategories[j].flag != 0; j++)
addReplyBulkCString(c,ACLCommandCategories[j].name);
setDeferredArrayLen(c,dl,j);
} else if (!strcasecmp(sub,"cat") && c->argc == 3) {
uint64_t cflag = ACLGetCommandCategoryFlagByName(c->argv[2]->ptr);
if (cflag == 0) {
addReplyErrorFormat(c, "Unknown category '%.128s'", (char*)c->argv[2]->ptr);
return;
}
int arraylen = 0;
void *dl = addReplyDeferredLen(c);
aclCatWithFlags(c, server.orig_commands, cflag, &arraylen);
setDeferredArrayLen(c,dl,arraylen);
} else if (!strcasecmp(sub,"genpass") && (c->argc == 2 || c->argc == 3)) {
#define GENPASS_MAX_BITS 4096
char pass[GENPASS_MAX_BITS/8*2];
long bits = 256;
if (c->argc == 3 && getLongFromObjectOrReply(c,c->argv[2],&bits,NULL)
!= C_OK) return;
if (bits <= 0 || bits > GENPASS_MAX_BITS) {
addReplyErrorFormat(c,
"ACL GENPASS argument must be the number of "
"bits for the output password, a positive number "
"up to %d",GENPASS_MAX_BITS);
return;
}
long chars = (bits+3)/4;
getRandomHexChars(pass,chars);
addReplyBulkCBuffer(c,pass,chars);
} else if (!strcasecmp(sub,"log") && (c->argc == 2 || c->argc ==3)) {
long count = 10;
if (c->argc == 3) {
if (!strcasecmp(c->argv[2]->ptr,"reset")) {
listSetFreeMethod(ACLLog,ACLFreeLogEntry);
listEmpty(ACLLog);
listSetFreeMethod(ACLLog,NULL);
addReply(c,shared.ok);
return;
} else if (getLongFromObjectOrReply(c,c->argv[2],&count,NULL)
!= C_OK)
{
return;
}
if (count < 0) count = 0;
}
if ((size_t)count > listLength(ACLLog))
count = listLength(ACLLog);
addReplyArrayLen(c,count);
listIter li;
listNode *ln;
listRewind(ACLLog,&li);
mstime_t now = mstime();
while (count-- && (ln = listNext(&li)) != NULL) {
ACLLogEntry *le = listNodeValue(ln);
addReplyMapLen(c,7);
addReplyBulkCString(c,"count");
addReplyLongLong(c,le->count);
addReplyBulkCString(c,"reason");
char *reasonstr;
switch(le->reason) {
case ACL_DENIED_CMD: reasonstr="command"; break;
case ACL_DENIED_KEY: reasonstr="key"; break;
case ACL_DENIED_CHANNEL: reasonstr="channel"; break;
case ACL_DENIED_AUTH: reasonstr="auth"; break;
default: reasonstr="unknown";
}
addReplyBulkCString(c,reasonstr);
addReplyBulkCString(c,"context");
char *ctxstr;
switch(le->context) {
case ACL_LOG_CTX_TOPLEVEL: ctxstr="toplevel"; break;
case ACL_LOG_CTX_MULTI: ctxstr="multi"; break;
case ACL_LOG_CTX_LUA: ctxstr="lua"; break;
case ACL_LOG_CTX_MODULE: ctxstr="module"; break;
default: ctxstr="unknown";
}
addReplyBulkCString(c,ctxstr);
addReplyBulkCString(c,"object");
addReplyBulkCBuffer(c,le->object,sdslen(le->object));
addReplyBulkCString(c,"username");
addReplyBulkCBuffer(c,le->username,sdslen(le->username));
addReplyBulkCString(c,"age-seconds");
double age = (double)(now - le->ctime)/1000;
addReplyDouble(c,age);
addReplyBulkCString(c,"client-info");
addReplyBulkCBuffer(c,le->cinfo,sdslen(le->cinfo));
}
} else if (!strcasecmp(sub,"dryrun") && c->argc >= 4) {
struct redisCommand *cmd;
user *u = ACLGetUserByName(c->argv[2]->ptr,sdslen(c->argv[2]->ptr));
if (u == NULL) {
addReplyErrorFormat(c, "User '%s' not found", (char *)c->argv[2]->ptr);
return;
}
if ((cmd = lookupCommand(c->argv + 3, c->argc - 3)) == NULL) {
addReplyErrorFormat(c, "Command '%s' not found", (char *)c->argv[3]->ptr);
return;
}
int idx;
int result = ACLCheckAllUserCommandPerm(u, cmd, c->argv + 3, c->argc - 3, &idx);
if (result != ACL_OK) {
sds err = sdsempty();
if (result == ACL_DENIED_CMD) {
err = sdscatfmt(err, "This user has no permissions to run "
"the '%s' command", c->cmd->fullname);
} else if (result == ACL_DENIED_KEY) {
err = sdscatfmt(err, "This user has no permissions to access "
"the '%s' key", c->argv[idx + 3]->ptr);
} else if (result == ACL_DENIED_CHANNEL) {
err = sdscatfmt(err, "This user has no permissions to access "
"the '%s' channel", c->argv[idx + 3]->ptr);
} else {
serverPanic("Invalid permission result");
}
addReplyBulkSds(c, err);
return;
}
addReply(c,shared.ok);
} else if (c->argc == 2 && !strcasecmp(sub,"help")) {
const char *help[] = {
"CAT [<category>]",
" List all commands that belong to <category>, or all command categories",
" when no category is specified.",
"DELUSER <username> [<username> ...]",
" Delete a list of users.",
"DRYRUN <username> <command> [<arg> ...]",
" Returns whether the user can execute the given command without executing the command.",
"GETUSER <username>",
" Get the user's details.",
"GENPASS [<bits>]",
" Generate a secure 256-bit user password. The optional `bits` argument can",
" be used to specify a different size.",
"LIST",
" Show users details in config file format.",
"LOAD",
" Reload users from the ACL file.",
"LOG [<count> | RESET]",
" Show the ACL log entries.",
"SAVE",
" Save the current config to the ACL file.",
"SETUSER <username> <attribute> [<attribute> ...]",
" Create or modify a user with the specified attributes.",
"USERS",
" List all the registered usernames.",
"WHOAMI",
" Return the current connection username.",
NULL
};
addReplyHelp(c,help);
} else {
addReplySubcommandSyntaxError(c);
}
}
void addReplyCommandCategories(client *c, struct redisCommand *cmd) {
int flagcount = 0;
void *flaglen = addReplyDeferredLen(c);
for (int j = 0; ACLCommandCategories[j].flag != 0; j++) {
if (cmd->acl_categories & ACLCommandCategories[j].flag) {
addReplyStatusFormat(c, "@%s", ACLCommandCategories[j].name);
flagcount++;
}
}
setDeferredSetLen(c, flaglen, flagcount);
}
void authCommand(client *c) {
if (c->argc > 3) {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
redactClientCommandArgument(c, 1);
robj *username, *password;
if (c->argc == 2) {
if (DefaultUser->flags & USER_FLAG_NOPASS) {
addReplyError(c,"AUTH <password> called without any password "
"configured for the default user. Are you sure "
"your configuration is correct?");
return;
}
username = shared.default_username;
password = c->argv[1];
} else {
username = c->argv[1];
password = c->argv[2];
redactClientCommandArgument(c, 2);
}
if (ACLAuthenticateUser(c,username,password) == C_OK) {
addReply(c,shared.ok);
} else {
addReplyError(c,"-WRONGPASS invalid username-password pair or user is disabled.");
}
}
void ACLUpdateDefaultUserPassword(sds password) {
ACLSetUser(DefaultUser,"resetpass",-1);
if (password) {
sds aclop = sdscatlen(sdsnew(">"), password, sdslen(password));
ACLSetUser(DefaultUser,aclop,sdslen(aclop));
sdsfree(aclop);
} else {
ACLSetUser(DefaultUser,"nopass",-1);
}
}
