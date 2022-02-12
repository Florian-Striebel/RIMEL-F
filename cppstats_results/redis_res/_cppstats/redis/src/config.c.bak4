





























#include "server.h"
#include "cluster.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <glob.h>
#include <string.h>





typedef struct configEnum {
const char *name;
const int val;
} configEnum;

typedef struct deprecatedConfig {
const char *name;
const int argc_min;
const int argc_max;
} deprecatedConfig;

configEnum maxmemory_policy_enum[] = {
{"volatile-lru", MAXMEMORY_VOLATILE_LRU},
{"volatile-lfu", MAXMEMORY_VOLATILE_LFU},
{"volatile-random",MAXMEMORY_VOLATILE_RANDOM},
{"volatile-ttl",MAXMEMORY_VOLATILE_TTL},
{"allkeys-lru",MAXMEMORY_ALLKEYS_LRU},
{"allkeys-lfu",MAXMEMORY_ALLKEYS_LFU},
{"allkeys-random",MAXMEMORY_ALLKEYS_RANDOM},
{"noeviction",MAXMEMORY_NO_EVICTION},
{NULL, 0}
};

configEnum syslog_facility_enum[] = {
{"user", LOG_USER},
{"local0", LOG_LOCAL0},
{"local1", LOG_LOCAL1},
{"local2", LOG_LOCAL2},
{"local3", LOG_LOCAL3},
{"local4", LOG_LOCAL4},
{"local5", LOG_LOCAL5},
{"local6", LOG_LOCAL6},
{"local7", LOG_LOCAL7},
{NULL, 0}
};

configEnum loglevel_enum[] = {
{"debug", LL_DEBUG},
{"verbose", LL_VERBOSE},
{"notice", LL_NOTICE},
{"warning", LL_WARNING},
{NULL,0}
};

configEnum supervised_mode_enum[] = {
{"upstart", SUPERVISED_UPSTART},
{"systemd", SUPERVISED_SYSTEMD},
{"auto", SUPERVISED_AUTODETECT},
{"no", SUPERVISED_NONE},
{NULL, 0}
};

configEnum aof_fsync_enum[] = {
{"everysec", AOF_FSYNC_EVERYSEC},
{"always", AOF_FSYNC_ALWAYS},
{"no", AOF_FSYNC_NO},
{NULL, 0}
};

configEnum repl_diskless_load_enum[] = {
{"disabled", REPL_DISKLESS_LOAD_DISABLED},
{"on-empty-db", REPL_DISKLESS_LOAD_WHEN_DB_EMPTY},
{"swapdb", REPL_DISKLESS_LOAD_SWAPDB},
{NULL, 0}
};

configEnum tls_auth_clients_enum[] = {
{"no", TLS_CLIENT_AUTH_NO},
{"yes", TLS_CLIENT_AUTH_YES},
{"optional", TLS_CLIENT_AUTH_OPTIONAL},
{NULL, 0}
};

configEnum oom_score_adj_enum[] = {
{"no", OOM_SCORE_ADJ_NO},
{"yes", OOM_SCORE_RELATIVE},
{"relative", OOM_SCORE_RELATIVE},
{"absolute", OOM_SCORE_ADJ_ABSOLUTE},
{NULL, 0}
};

configEnum acl_pubsub_default_enum[] = {
{"allchannels", SELECTOR_FLAG_ALLCHANNELS},
{"resetchannels", 0},
{NULL, 0}
};

configEnum sanitize_dump_payload_enum[] = {
{"no", SANITIZE_DUMP_NO},
{"yes", SANITIZE_DUMP_YES},
{"clients", SANITIZE_DUMP_CLIENTS},
{NULL, 0}
};

configEnum protected_action_enum[] = {
{"no", PROTECTED_ACTION_ALLOWED_NO},
{"yes", PROTECTED_ACTION_ALLOWED_YES},
{"local", PROTECTED_ACTION_ALLOWED_LOCAL},
{NULL, 0}
};

configEnum cluster_preferred_endpoint_type_enum[] = {
{"ip", CLUSTER_ENDPOINT_TYPE_IP},
{"hostname", CLUSTER_ENDPOINT_TYPE_HOSTNAME},
{"unknown-endpoint", CLUSTER_ENDPOINT_TYPE_UNKNOWN_ENDPOINT},
{NULL, 0}
};


clientBufferLimitsConfig clientBufferLimitsDefaults[CLIENT_TYPE_OBUF_COUNT] = {
{0, 0, 0},
{1024*1024*256, 1024*1024*64, 60},
{1024*1024*32, 1024*1024*8, 60}
};


int configOOMScoreAdjValuesDefaults[CONFIG_OOM_COUNT] = { 0, 200, 800 };









typedef struct boolConfigData {
int *config;
const int default_value;
int (*is_valid_fn)(int val, const char **err);
} boolConfigData;

typedef struct stringConfigData {
char **config;
const char *default_value;
int (*is_valid_fn)(char* val, const char **err);
int convert_empty_to_null;

} stringConfigData;

typedef struct sdsConfigData {
sds *config;
const char *default_value;
int (*is_valid_fn)(sds val, const char **err);
int convert_empty_to_null;

} sdsConfigData;

typedef struct enumConfigData {
int *config;
configEnum *enum_value;
const int default_value;
int (*is_valid_fn)(int val, const char **err);
} enumConfigData;

typedef enum numericType {
NUMERIC_TYPE_INT,
NUMERIC_TYPE_UINT,
NUMERIC_TYPE_LONG,
NUMERIC_TYPE_ULONG,
NUMERIC_TYPE_LONG_LONG,
NUMERIC_TYPE_ULONG_LONG,
NUMERIC_TYPE_SIZE_T,
NUMERIC_TYPE_SSIZE_T,
NUMERIC_TYPE_OFF_T,
NUMERIC_TYPE_TIME_T,
} numericType;

#define INTEGER_CONFIG 0
#define MEMORY_CONFIG (1<<0)
#define PERCENT_CONFIG (1<<1)
#define OCTAL_CONFIG (1<<2)

typedef struct numericConfigData {
union {
int *i;
unsigned int *ui;
long *l;
unsigned long *ul;
long long *ll;
unsigned long long *ull;
size_t *st;
ssize_t *sst;
off_t *ot;
time_t *tt;
} config;
unsigned int flags;
numericType numeric_type;
long long lower_bound;
long long upper_bound;
const long long default_value;
int (*is_valid_fn)(long long val, const char **err);
} numericConfigData;

typedef union typeData {
boolConfigData yesno;
stringConfigData string;
sdsConfigData sds;
enumConfigData enumd;
numericConfigData numeric;
} typeData;

typedef int (*apply_fn)(const char **err);
typedef struct typeInterface {

void (*init)(typeData data);



int (*set)(typeData data, sds *argv, int argc, const char **err);



apply_fn apply;

sds (*get)(typeData data);

void (*rewrite)(typeData data, const char *name, struct rewriteConfigState *state);
} typeInterface;

typedef struct standardConfig {
const char *name;
const char *alias;
const unsigned int flags;
typeInterface interface;
typeData data;
} standardConfig;

#define MODIFIABLE_CONFIG 0

#define IMMUTABLE_CONFIG (1ULL<<0)
#define SENSITIVE_CONFIG (1ULL<<1)
#define DEBUG_CONFIG (1ULL<<2)
#define MULTI_ARG_CONFIG (1ULL<<3)
#define HIDDEN_CONFIG (1ULL<<4)
#define PROTECTED_CONFIG (1ULL<<5)
#define DENY_LOADING_CONFIG (1ULL<<6)

standardConfig configs[];






int configEnumGetValue(configEnum *ce, char *name) {
while(ce->name != NULL) {
if (!strcasecmp(ce->name,name)) return ce->val;
ce++;
}
return INT_MIN;
}


const char *configEnumGetName(configEnum *ce, int val) {
while(ce->name != NULL) {
if (ce->val == val) return ce->name;
ce++;
}
return NULL;
}



const char *configEnumGetNameOrUnknown(configEnum *ce, int val) {
const char *name = configEnumGetName(ce,val);
return name ? name : "unknown";
}


const char *evictPolicyToString(void) {
return configEnumGetNameOrUnknown(maxmemory_policy_enum,server.maxmemory_policy);
}





int yesnotoi(char *s) {
if (!strcasecmp(s,"yes")) return 1;
else if (!strcasecmp(s,"no")) return 0;
else return -1;
}

void appendServerSaveParams(time_t seconds, int changes) {
server.saveparams = zrealloc(server.saveparams,sizeof(struct saveparam)*(server.saveparamslen+1));
server.saveparams[server.saveparamslen].seconds = seconds;
server.saveparams[server.saveparamslen].changes = changes;
server.saveparamslen++;
}

void resetServerSaveParams(void) {
zfree(server.saveparams);
server.saveparams = NULL;
server.saveparamslen = 0;
}

void queueLoadModule(sds path, sds *argv, int argc) {
int i;
struct moduleLoadQueueEntry *loadmod;

loadmod = zmalloc(sizeof(struct moduleLoadQueueEntry));
loadmod->argv = argc ? zmalloc(sizeof(robj*)*argc) : NULL;
loadmod->path = sdsnew(path);
loadmod->argc = argc;
for (i = 0; i < argc; i++) {
loadmod->argv[i] = createRawStringObject(argv[i],sdslen(argv[i]));
}
listAddNodeTail(server.loadmodule_queue,loadmod);
}




static int updateClientOutputBufferLimit(sds *args, int arg_len, const char **err) {
int j;
int class;
unsigned long long hard, soft;
int hard_err, soft_err;
int soft_seconds;
char *soft_seconds_eptr;
clientBufferLimitsConfig values[CLIENT_TYPE_OBUF_COUNT];
int classes[CLIENT_TYPE_OBUF_COUNT] = {0};


if (arg_len % 4) {
if (err) *err = "Wrong number of arguments in "
"buffer limit configuration.";
return 0;
}




for (j = 0; j < arg_len; j += 4) {
class = getClientTypeByName(args[j]);
if (class == -1 || class == CLIENT_TYPE_MASTER) {
if (err) *err = "Invalid client class specified in "
"buffer limit configuration.";
return 0;
}

hard = memtoull(args[j+1], &hard_err);
soft = memtoull(args[j+2], &soft_err);
soft_seconds = strtoll(args[j+3], &soft_seconds_eptr, 10);
if (hard_err || soft_err ||
soft_seconds < 0 || *soft_seconds_eptr != '\0')
{
if (err) *err = "Error in hard, soft or soft_seconds setting in "
"buffer limit configuration.";
return 0;
}

values[class].hard_limit_bytes = hard;
values[class].soft_limit_bytes = soft;
values[class].soft_limit_seconds = soft_seconds;
classes[class] = 1;
}


for (j = 0; j < CLIENT_TYPE_OBUF_COUNT; j++) {
if (classes[j]) server.client_obuf_limits[j] = values[j];
}

return 1;
}

void initConfigValues() {
for (standardConfig *config = configs; config->name != NULL; config++) {
if (config->interface.init) config->interface.init(config->data);
}
}




static int reading_config_file;

void loadServerConfigFromString(char *config) {
deprecatedConfig deprecated_configs[] = {
{"list-max-ziplist-entries", 2, 2},
{"list-max-ziplist-value", 2, 2},
{"lua-replicate-commands", 2, 2},
{NULL, 0},
};
char buf[1024];
const char *err = NULL;
int linenum = 0, totlines, i;
sds *lines;

reading_config_file = 1;
lines = sdssplitlen(config,strlen(config),"\n",1,&totlines);

for (i = 0; i < totlines; i++) {
sds *argv;
int argc;

linenum = i+1;
lines[i] = sdstrim(lines[i]," \t\r\n");


if (lines[i][0] == '#' || lines[i][0] == '\0') continue;


argv = sdssplitargs(lines[i],&argc);
if (argv == NULL) {
err = "Unbalanced quotes in configuration line";
goto loaderr;
}


if (argc == 0) {
sdsfreesplitres(argv,argc);
continue;
}
sdstolower(argv[0]);


int match = 0;
for (standardConfig *config = configs; config->name != NULL; config++) {
if ((!strcasecmp(argv[0],config->name) ||
(config->alias && !strcasecmp(argv[0],config->alias))))
{


if (!(config->flags & MULTI_ARG_CONFIG) && argc != 2) {
err = "wrong number of arguments";
goto loaderr;
}

if (!config->interface.set(config->data, &argv[1], argc-1, &err)) {
goto loaderr;
}

match = 1;
break;
}
}


if (!match) {
for (deprecatedConfig *config = deprecated_configs; config->name != NULL; config++) {
if (!strcasecmp(argv[0], config->name) &&
config->argc_min <= argc &&
argc <= config->argc_max)
{
match = 1;
break;
}
}
}

if (match) {
sdsfreesplitres(argv,argc);
continue;
}


if (!strcasecmp(argv[0],"include") && argc == 2) {
loadServerConfig(argv[1], 0, NULL);
} else if (!strcasecmp(argv[0],"rename-command") && argc == 3) {
struct redisCommand *cmd = lookupCommandBySds(argv[1]);
int retval;

if (!cmd) {
err = "No such command in rename-command";
goto loaderr;
}



retval = dictDelete(server.commands, argv[1]);
serverAssert(retval == DICT_OK);


if (sdslen(argv[2]) != 0) {
sds copy = sdsdup(argv[2]);

retval = dictAdd(server.commands, copy, cmd);
if (retval != DICT_OK) {
sdsfree(copy);
err = "Target command name already exists"; goto loaderr;
}
}
} else if (!strcasecmp(argv[0],"user") && argc >= 2) {
int argc_err;
if (ACLAppendUserForLoading(argv,argc,&argc_err) == C_ERR) {
const char *errmsg = ACLSetUserStringError();
snprintf(buf,sizeof(buf),"Error in user declaration '%s': %s",
argv[argc_err],errmsg);
err = buf;
goto loaderr;
}
} else if (!strcasecmp(argv[0],"loadmodule") && argc >= 2) {
queueLoadModule(argv[1],&argv[2],argc-2);
} else if (!strcasecmp(argv[0],"sentinel")) {


if (argc != 1) {
if (!server.sentinel_mode) {
err = "sentinel directive while not in sentinel mode";
goto loaderr;
}
queueSentinelConfig(argv+1,argc-1,linenum,lines[i]);
}
} else {
err = "Bad directive or wrong number of arguments"; goto loaderr;
}
sdsfreesplitres(argv,argc);
}

if (server.logfile[0] != '\0') {
FILE *logfp;



logfp = fopen(server.logfile,"a");
if (logfp == NULL) {
err = sdscatprintf(sdsempty(),
"Can't open the log file: %s", strerror(errno));
goto loaderr;
}
fclose(logfp);
}


if (server.cluster_enabled && server.masterhost) {
err = "replicaof directive not allowed in cluster mode";
goto loaderr;
}


if (server.config_hz < CONFIG_MIN_HZ) server.config_hz = CONFIG_MIN_HZ;
if (server.config_hz > CONFIG_MAX_HZ) server.config_hz = CONFIG_MAX_HZ;

sdsfreesplitres(lines,totlines);
reading_config_file = 0;
return;

loaderr:
fprintf(stderr, "\n*** FATAL CONFIG FILE ERROR (Redis %s) ***\n",
REDIS_VERSION);
if (i < totlines) {
fprintf(stderr, "Reading the configuration file, at line %d\n", linenum);
fprintf(stderr, ">>> '%s'\n", lines[i]);
}
fprintf(stderr, "%s\n", err);
exit(1);
}








void loadServerConfig(char *filename, char config_from_stdin, char *options) {
sds config = sdsempty();
char buf[CONFIG_MAX_LINE+1];
FILE *fp;
glob_t globbuf;


if (filename) {















if (strchr(filename, '*') || strchr(filename, '?') || strchr(filename, '[')) {

if (glob(filename, 0, NULL, &globbuf) == 0) {

for (size_t i = 0; i < globbuf.gl_pathc; i++) {
if ((fp = fopen(globbuf.gl_pathv[i], "r")) == NULL) {
serverLog(LL_WARNING,
"Fatal error, can't open config file '%s': %s",
globbuf.gl_pathv[i], strerror(errno));
exit(1);
}
while(fgets(buf,CONFIG_MAX_LINE+1,fp) != NULL)
config = sdscat(config,buf);
fclose(fp);
}

globfree(&globbuf);
}
} else {


if ((fp = fopen(filename, "r")) == NULL) {
serverLog(LL_WARNING,
"Fatal error, can't open config file '%s': %s",
filename, strerror(errno));
exit(1);
}
while(fgets(buf,CONFIG_MAX_LINE+1,fp) != NULL)
config = sdscat(config,buf);
fclose(fp);
}
}


if (config_from_stdin) {
serverLog(LL_WARNING,"Reading config from stdin");
fp = stdin;
while(fgets(buf,CONFIG_MAX_LINE+1,fp) != NULL)
config = sdscat(config,buf);
}


if (options) {
config = sdscat(config,"\n");
config = sdscat(config,options);
}
loadServerConfigFromString(config);
sdsfree(config);
}

static int performInterfaceSet(standardConfig *config, sds value, const char **errstr) {
sds *argv;
int argc, res;

if (config->flags & MULTI_ARG_CONFIG) {
argv = sdssplitlen(value, sdslen(value), " ", 1, &argc);
} else {
argv = (char**)&value;
argc = 1;
}


res = config->interface.set(config->data, argv, argc, errstr);
if (config->flags & MULTI_ARG_CONFIG) sdsfreesplitres(argv, argc);
return res;
}

static void restoreBackupConfig(standardConfig **set_configs, sds *old_values, int count, apply_fn *apply_fns) {
int i;
const char *errstr = "unknown error";

for (i = 0; i < count; i++) {
if (!performInterfaceSet(set_configs[i], old_values[i], &errstr))
serverLog(LL_WARNING, "Failed restoring failed CONFIG SET command. Error setting %s to '%s': %s",
set_configs[i]->name, old_values[i], errstr);
}

if (apply_fns) {
for (i = 0; i < count && apply_fns[i] != NULL; i++) {
if (!apply_fns[i](&errstr))
serverLog(LL_WARNING, "Failed applying restored failed CONFIG SET command: %s", errstr);
}
}
}





void configSetCommand(client *c) {
const char *errstr = NULL;
const char *invalid_arg_name = NULL;
const char *err_arg_name = NULL;
standardConfig **set_configs;
sds *new_values;
sds *old_values = NULL;
apply_fn *apply_fns;
int config_count, i, j;
int invalid_args = 0, deny_loading_error = 0;
int *config_map_fns;


if (c->argc & 1) {
addReplyErrorObject(c, shared.syntaxerr);
return;
}
config_count = (c->argc - 2) / 2;

set_configs = zcalloc(sizeof(standardConfig*)*config_count);
new_values = zmalloc(sizeof(sds*)*config_count);
old_values = zcalloc(sizeof(sds*)*config_count);
apply_fns = zcalloc(sizeof(apply_fn)*config_count);
config_map_fns = zmalloc(sizeof(int)*config_count);


for (i = 0; i < config_count; i++) {
for (standardConfig *config = configs; config->name != NULL; config++) {
if ((!strcasecmp(c->argv[2+i*2]->ptr,config->name) ||
(config->alias && !strcasecmp(c->argv[2]->ptr,config->alias)))) {




if (config->flags & SENSITIVE_CONFIG) {
redactClientCommandArgument(c,2+i*2+1);
}

if (!invalid_args) {
if (config->flags & IMMUTABLE_CONFIG ||
(config->flags & PROTECTED_CONFIG && !allowProtectedAction(server.enable_protected_configs, c)))
{

errstr = (config->flags & IMMUTABLE_CONFIG) ? "can't set immutable config" : "can't set protected config";
err_arg_name = c->argv[2+i*2]->ptr;
invalid_args = 1;
}

if (server.loading && config->flags & DENY_LOADING_CONFIG) {

deny_loading_error = 1;
invalid_args = 1;
}


for (j = 0; j < i; j++) {
if (set_configs[j] == config) {

errstr = "duplicate parameter";
err_arg_name = c->argv[2+i*2]->ptr;
invalid_args = 1;
break;
}
}
set_configs[i] = config;
new_values[i] = c->argv[2+i*2+1]->ptr;
}
break;
}
}


if (!invalid_args && !set_configs[i]) {
invalid_arg_name = c->argv[2+i*2]->ptr;
invalid_args = 1;
}
}

if (invalid_args) goto err;


for (i = 0; i < config_count; i++)
old_values[i] = set_configs[i]->interface.get(set_configs[i]->data);


for (i = 0; i < config_count; i++) {
int res = performInterfaceSet(set_configs[i], new_values[i], &errstr);
if (!res) {
restoreBackupConfig(set_configs, old_values, i+1, NULL);
err_arg_name = set_configs[i]->name;
goto err;
} else if (res == 1) {

if (set_configs[i]->interface.apply) {

int exists = 0;
for (j = 0; apply_fns[j] != NULL && j <= i; j++) {
if (apply_fns[j] == set_configs[i]->interface.apply) {
exists = 1;
break;
}
}

if (!exists) {
apply_fns[j] = set_configs[i]->interface.apply;
config_map_fns[j] = i;
}
}
}
}


for (i = 0; i < config_count && apply_fns[i] != NULL; i++) {
if (!apply_fns[i](&errstr)) {
serverLog(LL_WARNING, "Failed applying new configuration. Possibly related to new %s setting. Restoring previous settings.", set_configs[config_map_fns[i]]->name);
restoreBackupConfig(set_configs, old_values, config_count, apply_fns);
err_arg_name = set_configs[config_map_fns[i]]->name;
goto err;
}
}
addReply(c,shared.ok);
goto end;

err:
if (deny_loading_error) {

addReplyErrorObject(c,shared.loadingerr);
} else if (invalid_arg_name) {
addReplyErrorFormat(c,"Unknown option or number of arguments for CONFIG SET - '%s'", invalid_arg_name);
} else if (errstr) {
addReplyErrorFormat(c,"CONFIG SET failed (possibly related to argument '%s') - %s", err_arg_name, errstr);
} else {
addReplyErrorFormat(c,"CONFIG SET failed (possibly related to argument '%s')", err_arg_name);
}
end:
zfree(set_configs);
zfree(new_values);
for (i = 0; i < config_count; i++)
sdsfree(old_values[i]);
zfree(old_values);
zfree(apply_fns);
zfree(config_map_fns);
}





void configGetCommand(client *c) {
void *replylen = addReplyDeferredLen(c);
int matches = 0;
int i;

for (standardConfig *config = configs; config->name != NULL; config++) {
int matched_conf = 0;
int matched_alias = 0;
for (i = 0; i < c->argc - 2 && (!matched_conf || !matched_alias); i++) {
robj *o = c->argv[2+i];
char *pattern = o->ptr;


if (!matched_conf &&
(((config->flags & HIDDEN_CONFIG) && !strcasecmp(pattern, config->name)) ||
(!(config->flags & HIDDEN_CONFIG) && stringmatch(pattern, config->name, 1)))) {
addReplyBulkCString(c, config->name);
addReplyBulkSds(c, config->interface.get(config->data));
matches++;
matched_conf = 1;
}
if (!matched_alias && config->alias &&
(((config->flags & HIDDEN_CONFIG) && !strcasecmp(pattern, config->alias)) ||
(!(config->flags & HIDDEN_CONFIG) && stringmatch(pattern, config->alias, 1)))) {
addReplyBulkCString(c, config->alias);
addReplyBulkSds(c, config->interface.get(config->data));
matches++;
matched_alias = 1;
}
}
}

setDeferredMapLen(c,replylen,matches);
}





#define REDIS_CONFIG_REWRITE_SIGNATURE "#Generated by CONFIG REWRITE"




void dictListDestructor(dict *d, void *val);



void rewriteConfigSentinelOption(struct rewriteConfigState *state);

dictType optionToLineDictType = {
dictSdsCaseHash,
NULL,
NULL,
dictSdsKeyCaseCompare,
dictSdsDestructor,
dictListDestructor,
NULL
};

dictType optionSetDictType = {
dictSdsCaseHash,
NULL,
NULL,
dictSdsKeyCaseCompare,
dictSdsDestructor,
NULL,
NULL
};


struct rewriteConfigState {
dict *option_to_line;
dict *rewritten;
int numlines;
sds *lines;
int needs_signature;

int force_write;


};


void rewriteConfigReleaseState(struct rewriteConfigState *state) {
sdsfreesplitres(state->lines,state->numlines);
dictRelease(state->option_to_line);
dictRelease(state->rewritten);
zfree(state);
}


struct rewriteConfigState *rewriteConfigCreateState() {
struct rewriteConfigState *state = zmalloc(sizeof(*state));
state->option_to_line = dictCreate(&optionToLineDictType);
state->rewritten = dictCreate(&optionSetDictType);
state->numlines = 0;
state->lines = NULL;
state->needs_signature = 1;
state->force_write = 0;
return state;
}


void rewriteConfigAppendLine(struct rewriteConfigState *state, sds line) {
state->lines = zrealloc(state->lines, sizeof(char*) * (state->numlines+1));
state->lines[state->numlines++] = line;
}


void rewriteConfigAddLineNumberToOption(struct rewriteConfigState *state, sds option, int linenum) {
list *l = dictFetchValue(state->option_to_line,option);

if (l == NULL) {
l = listCreate();
dictAdd(state->option_to_line,sdsdup(option),l);
}
listAddNodeTail(l,(void*)(long)linenum);
}





void rewriteConfigMarkAsProcessed(struct rewriteConfigState *state, const char *option) {
sds opt = sdsnew(option);

if (dictAdd(state->rewritten,opt,NULL) != DICT_OK) sdsfree(opt);
}






struct rewriteConfigState *rewriteConfigReadOldFile(char *path) {
FILE *fp = fopen(path,"r");
if (fp == NULL && errno != ENOENT) return NULL;

char buf[CONFIG_MAX_LINE+1];
int linenum = -1;
struct rewriteConfigState *state = rewriteConfigCreateState();

if (fp == NULL) return state;


while(fgets(buf,CONFIG_MAX_LINE+1,fp) != NULL) {
int argc;
sds *argv;
sds line = sdstrim(sdsnew(buf),"\r\n\t ");

linenum++;


if (line[0] == '#' || line[0] == '\0') {
if (state->needs_signature && !strcmp(line,REDIS_CONFIG_REWRITE_SIGNATURE))
state->needs_signature = 0;
rewriteConfigAppendLine(state,line);
continue;
}


argv = sdssplitargs(line,&argc);
if (argv == NULL) {



sds aux = sdsnew("#??? ");
aux = sdscatsds(aux,line);
sdsfree(line);
rewriteConfigAppendLine(state,aux);
continue;
}

sdstolower(argv[0]);



rewriteConfigAppendLine(state,line);




char *p = strstr(argv[0],"slave");
if (p) {
sds alt = sdsempty();
alt = sdscatlen(alt,argv[0],p-argv[0]);
alt = sdscatlen(alt,"replica",7);
alt = sdscatlen(alt,p+5,strlen(p+5));
sdsfree(argv[0]);
argv[0] = alt;
}


if (server.sentinel_mode && argc > 1 && !strcasecmp(argv[0],"sentinel")) {
sds sentinelOption = sdsempty();
sentinelOption = sdscatfmt(sentinelOption,"%S %S",argv[0],argv[1]);
rewriteConfigAddLineNumberToOption(state,sentinelOption,linenum);
sdsfree(sentinelOption);
} else {
rewriteConfigAddLineNumberToOption(state,argv[0],linenum);
}
sdsfreesplitres(argv,argc);
}
fclose(fp);
return state;
}

















void rewriteConfigRewriteLine(struct rewriteConfigState *state, const char *option, sds line, int force) {
sds o = sdsnew(option);
list *l = dictFetchValue(state->option_to_line,o);

rewriteConfigMarkAsProcessed(state,option);

if (!l && !force && !state->force_write) {

sdsfree(line);
sdsfree(o);
return;
}

if (l) {
listNode *ln = listFirst(l);
int linenum = (long) ln->value;



listDelNode(l,ln);
if (listLength(l) == 0) dictDelete(state->option_to_line,o);
sdsfree(state->lines[linenum]);
state->lines[linenum] = line;
} else {

if (state->needs_signature) {
rewriteConfigAppendLine(state,
sdsnew(REDIS_CONFIG_REWRITE_SIGNATURE));
state->needs_signature = 0;
}
rewriteConfigAppendLine(state,line);
}
sdsfree(o);
}



int rewriteConfigFormatMemory(char *buf, size_t len, long long bytes) {
int gb = 1024*1024*1024;
int mb = 1024*1024;
int kb = 1024;

if (bytes && (bytes % gb) == 0) {
return snprintf(buf,len,"%lldgb",bytes/gb);
} else if (bytes && (bytes % mb) == 0) {
return snprintf(buf,len,"%lldmb",bytes/mb);
} else if (bytes && (bytes % kb) == 0) {
return snprintf(buf,len,"%lldkb",bytes/kb);
} else {
return snprintf(buf,len,"%lld",bytes);
}
}


void rewriteConfigBytesOption(struct rewriteConfigState *state, const char *option, long long value, long long defvalue) {
char buf[64];
int force = value != defvalue;
sds line;

rewriteConfigFormatMemory(buf,sizeof(buf),value);
line = sdscatprintf(sdsempty(),"%s %s",option,buf);
rewriteConfigRewriteLine(state,option,line,force);
}


void rewriteConfigPercentOption(struct rewriteConfigState *state, const char *option, long long value, long long defvalue) {
int force = value != defvalue;
sds line = sdscatprintf(sdsempty(),"%s %lld%%",option,value);

rewriteConfigRewriteLine(state,option,line,force);
}


void rewriteConfigYesNoOption(struct rewriteConfigState *state, const char *option, int value, int defvalue) {
int force = value != defvalue;
sds line = sdscatprintf(sdsempty(),"%s %s",option,
value ? "yes" : "no");

rewriteConfigRewriteLine(state,option,line,force);
}


void rewriteConfigStringOption(struct rewriteConfigState *state, const char *option, char *value, const char *defvalue) {
int force = 1;
sds line;



if (value == NULL) {
rewriteConfigMarkAsProcessed(state,option);
return;
}


if (defvalue && strcmp(value,defvalue) == 0) force = 0;

line = sdsnew(option);
line = sdscatlen(line, " ", 1);
line = sdscatrepr(line, value, strlen(value));

rewriteConfigRewriteLine(state,option,line,force);
}


void rewriteConfigSdsOption(struct rewriteConfigState *state, const char *option, sds value, const char *defvalue) {
int force = 1;
sds line;



if (value == NULL) {
rewriteConfigMarkAsProcessed(state, option);
return;
}


if (defvalue && strcmp(value, defvalue) == 0) force = 0;

line = sdsnew(option);
line = sdscatlen(line, " ", 1);
line = sdscatrepr(line, value, sdslen(value));

rewriteConfigRewriteLine(state, option, line, force);
}


void rewriteConfigNumericalOption(struct rewriteConfigState *state, const char *option, long long value, long long defvalue) {
int force = value != defvalue;
sds line = sdscatprintf(sdsempty(),"%s %lld",option,value);

rewriteConfigRewriteLine(state,option,line,force);
}


void rewriteConfigOctalOption(struct rewriteConfigState *state, const char *option, long long value, long long defvalue) {
int force = value != defvalue;
sds line = sdscatprintf(sdsempty(),"%s %llo",option,value);

rewriteConfigRewriteLine(state,option,line,force);
}




void rewriteConfigEnumOption(struct rewriteConfigState *state, const char *option, int value, configEnum *ce, int defval) {
sds line;
const char *name = configEnumGetNameOrUnknown(ce,value);
int force = value != defval;

line = sdscatprintf(sdsempty(),"%s %s",option,name);
rewriteConfigRewriteLine(state,option,line,force);
}


void rewriteConfigSaveOption(typeData data, const char *name, struct rewriteConfigState *state) {
UNUSED(data);
int j;
sds line;


if (server.sentinel_mode) {
rewriteConfigMarkAsProcessed(state,name);
return;
}




if (!server.saveparamslen) {
rewriteConfigRewriteLine(state,name,sdsnew("save \"\""),1);
} else {
for (j = 0; j < server.saveparamslen; j++) {
line = sdscatprintf(sdsempty(),"save %ld %d",
(long) server.saveparams[j].seconds, server.saveparams[j].changes);
rewriteConfigRewriteLine(state,name,line,1);
}
}


rewriteConfigMarkAsProcessed(state,name);
}


void rewriteConfigUserOption(struct rewriteConfigState *state) {



if (server.acl_filename[0] != '\0') {
rewriteConfigMarkAsProcessed(state,"user");
return;
}




raxIterator ri;
raxStart(&ri,Users);
raxSeek(&ri,"^",NULL,0);
while(raxNext(&ri)) {
user *u = ri.data;
sds line = sdsnew("user ");
line = sdscatsds(line,u->name);
line = sdscatlen(line," ",1);
sds descr = ACLDescribeUser(u);
line = sdscatsds(line,descr);
sdsfree(descr);
rewriteConfigRewriteLine(state,"user",line,1);
}
raxStop(&ri);


rewriteConfigMarkAsProcessed(state,"user");
}


void rewriteConfigDirOption(typeData data, const char *name, struct rewriteConfigState *state) {
UNUSED(data);
char cwd[1024];

if (getcwd(cwd,sizeof(cwd)) == NULL) {
rewriteConfigMarkAsProcessed(state,name);
return;
}
rewriteConfigStringOption(state,name,cwd,NULL);
}


void rewriteConfigReplicaOfOption(typeData data, const char *name, struct rewriteConfigState *state) {
UNUSED(data);
sds line;




if (server.cluster_enabled || server.masterhost == NULL) {
rewriteConfigMarkAsProcessed(state, name);
return;
}
line = sdscatprintf(sdsempty(),"%s %s %d", name,
server.masterhost, server.masterport);
rewriteConfigRewriteLine(state,name,line,1);
}


void rewriteConfigNotifyKeyspaceEventsOption(typeData data, const char *name, struct rewriteConfigState *state) {
UNUSED(data);
int force = server.notify_keyspace_events != 0;
sds line, flags;

flags = keyspaceEventsFlagsToString(server.notify_keyspace_events);
line = sdsnew(name);
line = sdscatlen(line, " ", 1);
line = sdscatrepr(line, flags, sdslen(flags));
sdsfree(flags);
rewriteConfigRewriteLine(state,name,line,force);
}


void rewriteConfigClientOutputBufferLimitOption(typeData data, const char *name, struct rewriteConfigState *state) {
UNUSED(data);
int j;
for (j = 0; j < CLIENT_TYPE_OBUF_COUNT; j++) {
int force = (server.client_obuf_limits[j].hard_limit_bytes !=
clientBufferLimitsDefaults[j].hard_limit_bytes) ||
(server.client_obuf_limits[j].soft_limit_bytes !=
clientBufferLimitsDefaults[j].soft_limit_bytes) ||
(server.client_obuf_limits[j].soft_limit_seconds !=
clientBufferLimitsDefaults[j].soft_limit_seconds);
sds line;
char hard[64], soft[64];

rewriteConfigFormatMemory(hard,sizeof(hard),
server.client_obuf_limits[j].hard_limit_bytes);
rewriteConfigFormatMemory(soft,sizeof(soft),
server.client_obuf_limits[j].soft_limit_bytes);

char *typename = getClientTypeName(j);
if (!strcmp(typename,"slave")) typename = "replica";
line = sdscatprintf(sdsempty(),"%s %s %s %s %ld",
name, typename, hard, soft,
(long) server.client_obuf_limits[j].soft_limit_seconds);
rewriteConfigRewriteLine(state,name,line,force);
}
}


void rewriteConfigOOMScoreAdjValuesOption(typeData data, const char *name, struct rewriteConfigState *state) {
UNUSED(data);
int force = 0;
int j;
sds line;

line = sdsnew(name);
line = sdscatlen(line, " ", 1);
for (j = 0; j < CONFIG_OOM_COUNT; j++) {
if (server.oom_score_adj_values[j] != configOOMScoreAdjValuesDefaults[j])
force = 1;

line = sdscatprintf(line, "%d", server.oom_score_adj_values[j]);
if (j+1 != CONFIG_OOM_COUNT)
line = sdscatlen(line, " ", 1);
}
rewriteConfigRewriteLine(state,name,line,force);
}


void rewriteConfigBindOption(typeData data, const char *name, struct rewriteConfigState *state) {
UNUSED(data);
int force = 1;
sds line, addresses;
int is_default = 0;


if (server.bindaddr_count == CONFIG_DEFAULT_BINDADDR_COUNT) {
is_default = 1;
char *default_bindaddr[CONFIG_DEFAULT_BINDADDR_COUNT] = CONFIG_DEFAULT_BINDADDR;
for (int j = 0; j < CONFIG_DEFAULT_BINDADDR_COUNT; j++) {
if (strcmp(server.bindaddr[j], default_bindaddr[j]) != 0) {
is_default = 0;
break;
}
}
}

if (is_default) {
rewriteConfigMarkAsProcessed(state,name);
return;
}


if (server.bindaddr_count > 0)
addresses = sdsjoin(server.bindaddr,server.bindaddr_count," ");
else
addresses = sdsnew("\"\"");
line = sdsnew(name);
line = sdscatlen(line, " ", 1);
line = sdscatsds(line, addresses);
sdsfree(addresses);

rewriteConfigRewriteLine(state,name,line,force);
}


void rewriteConfigLoadmoduleOption(struct rewriteConfigState *state) {
sds line;

dictIterator *di = dictGetIterator(modules);
dictEntry *de;
while ((de = dictNext(di)) != NULL) {
struct RedisModule *module = dictGetVal(de);
line = sdsnew("loadmodule ");
line = sdscatsds(line, module->loadmod->path);
for (int i = 0; i < module->loadmod->argc; i++) {
line = sdscatlen(line, " ", 1);
line = sdscatsds(line, module->loadmod->argv[i]->ptr);
}
rewriteConfigRewriteLine(state,"loadmodule",line,1);
}
dictReleaseIterator(di);

rewriteConfigMarkAsProcessed(state,"loadmodule");
}



sds rewriteConfigGetContentFromState(struct rewriteConfigState *state) {
sds content = sdsempty();
int j, was_empty = 0;

for (j = 0; j < state->numlines; j++) {

if (sdslen(state->lines[j]) == 0) {
if (was_empty) continue;
was_empty = 1;
} else {
was_empty = 0;
}
content = sdscatsds(content,state->lines[j]);
content = sdscatlen(content,"\n",1);
}
return content;
}









void rewriteConfigRemoveOrphaned(struct rewriteConfigState *state) {
dictIterator *di = dictGetIterator(state->option_to_line);
dictEntry *de;

while((de = dictNext(di)) != NULL) {
list *l = dictGetVal(de);
sds option = dictGetKey(de);



if (dictFind(state->rewritten,option) == NULL) {
serverLog(LL_DEBUG,"Not rewritten option: %s", option);
continue;
}

while(listLength(l)) {
listNode *ln = listFirst(l);
int linenum = (long) ln->value;

sdsfree(state->lines[linenum]);
state->lines[linenum] = sdsempty();
listDelNode(l,ln);
}
}
dictReleaseIterator(di);
}



sds getConfigDebugInfo() {
struct rewriteConfigState *state = rewriteConfigCreateState();
state->force_write = 1;
state->needs_signature = 0;



for (standardConfig *config = configs; config->name != NULL; config++) {
if (!(config->flags & DEBUG_CONFIG)) continue;
config->interface.rewrite(config->data, config->name, state);
}
sds info = rewriteConfigGetContentFromState(state);
rewriteConfigReleaseState(state);
return info;
}






int rewriteConfigOverwriteFile(char *configfile, sds content) {
int fd = -1;
int retval = -1;
char tmp_conffile[PATH_MAX];
const char *tmp_suffix = ".XXXXXX";
size_t offset = 0;
ssize_t written_bytes = 0;

int tmp_path_len = snprintf(tmp_conffile, sizeof(tmp_conffile), "%s%s", configfile, tmp_suffix);
if (tmp_path_len <= 0 || (unsigned int)tmp_path_len >= sizeof(tmp_conffile)) {
serverLog(LL_WARNING, "Config file full path is too long");
errno = ENAMETOOLONG;
return retval;
}

#if defined(_GNU_SOURCE)
fd = mkostemp(tmp_conffile, O_CLOEXEC);
#else

fd = mkstemp(tmp_conffile);
#endif

if (fd == -1) {
serverLog(LL_WARNING, "Could not create tmp config file (%s)", strerror(errno));
return retval;
}

while (offset < sdslen(content)) {
written_bytes = write(fd, content + offset, sdslen(content) - offset);
if (written_bytes <= 0) {
if (errno == EINTR) continue;
serverLog(LL_WARNING, "Failed after writing (%zd) bytes to tmp config file (%s)", offset, strerror(errno));
goto cleanup;
}
offset+=written_bytes;
}

if (fsync(fd))
serverLog(LL_WARNING, "Could not sync tmp config file to disk (%s)", strerror(errno));
else if (fchmod(fd, 0644 & ~server.umask) == -1)
serverLog(LL_WARNING, "Could not chmod config file (%s)", strerror(errno));
else if (rename(tmp_conffile, configfile) == -1)
serverLog(LL_WARNING, "Could not rename tmp config file (%s)", strerror(errno));
else {
retval = 0;
serverLog(LL_DEBUG, "Rewritten config file (%s) successfully", configfile);
}

cleanup:
close(fd);
if (retval) unlink(tmp_conffile);
return retval;
}











int rewriteConfig(char *path, int force_write) {
struct rewriteConfigState *state;
sds newcontent;
int retval;


if ((state = rewriteConfigReadOldFile(path)) == NULL) return -1;
if (force_write) state->force_write = 1;





for (standardConfig *config = configs; config->name != NULL; config++) {
if (config->interface.rewrite) config->interface.rewrite(config->data, config->name, state);
}

rewriteConfigUserOption(state);
rewriteConfigLoadmoduleOption(state);


if (server.sentinel_mode) rewriteConfigSentinelOption(state);




rewriteConfigRemoveOrphaned(state);



newcontent = rewriteConfigGetContentFromState(state);
retval = rewriteConfigOverwriteFile(server.configfile,newcontent);

sdsfree(newcontent);
rewriteConfigReleaseState(state);
return retval;
}




#define LOADBUF_SIZE 256
static char loadbuf[LOADBUF_SIZE];

#define embedCommonConfig(config_name, config_alias, config_flags) .name = (config_name), .alias = (config_alias), .flags = (config_flags),




#define embedConfigInterface(initfn, setfn, getfn, rewritefn, applyfn) .interface = { .init = (initfn), .set = (setfn), .get = (getfn), .rewrite = (rewritefn), .apply = (applyfn) },




















static void boolConfigInit(typeData data) {
*data.yesno.config = data.yesno.default_value;
}

static int boolConfigSet(typeData data, sds *argv, int argc, const char **err) {
UNUSED(argc);
int yn = yesnotoi(argv[0]);
if (yn == -1) {
*err = "argument must be 'yes' or 'no'";
return 0;
}
if (data.yesno.is_valid_fn && !data.yesno.is_valid_fn(yn, err))
return 0;
int prev = *(data.yesno.config);
if (prev != yn) {
*(data.yesno.config) = yn;
return 1;
}
return 2;
}

static sds boolConfigGet(typeData data) {
return sdsnew(*data.yesno.config ? "yes" : "no");
}

static void boolConfigRewrite(typeData data, const char *name, struct rewriteConfigState *state) {
rewriteConfigYesNoOption(state, name,*(data.yesno.config), data.yesno.default_value);
}

#define createBoolConfig(name, alias, flags, config_addr, default, is_valid, apply) { embedCommonConfig(name, alias, flags) embedConfigInterface(boolConfigInit, boolConfigSet, boolConfigGet, boolConfigRewrite, apply) .data.yesno = { .config = &(config_addr), .default_value = (default), .is_valid_fn = (is_valid), } }










static void stringConfigInit(typeData data) {
*data.string.config = (data.string.convert_empty_to_null && !data.string.default_value) ? NULL : zstrdup(data.string.default_value);
}

static int stringConfigSet(typeData data, sds *argv, int argc, const char **err) {
UNUSED(argc);
if (data.string.is_valid_fn && !data.string.is_valid_fn(argv[0], err))
return 0;
char *prev = *data.string.config;
char *new = (data.string.convert_empty_to_null && !argv[0][0]) ? NULL : argv[0];
if (new != prev && (new == NULL || prev == NULL || strcmp(prev, new))) {
*data.string.config = new != NULL ? zstrdup(new) : NULL;
zfree(prev);
return 1;
}
return 2;
}

static sds stringConfigGet(typeData data) {
return sdsnew(*data.string.config ? *data.string.config : "");
}

static void stringConfigRewrite(typeData data, const char *name, struct rewriteConfigState *state) {
rewriteConfigStringOption(state, name,*(data.string.config), data.string.default_value);
}


static void sdsConfigInit(typeData data) {
*data.sds.config = (data.sds.convert_empty_to_null && !data.sds.default_value) ? NULL: sdsnew(data.sds.default_value);
}

static int sdsConfigSet(typeData data, sds *argv, int argc, const char **err) {
UNUSED(argc);
if (data.sds.is_valid_fn && !data.sds.is_valid_fn(argv[0], err))
return 0;
sds prev = *data.sds.config;
sds new = (data.string.convert_empty_to_null && (sdslen(argv[0]) == 0)) ? NULL : argv[0];
if (new != prev && (new == NULL || prev == NULL || sdscmp(prev, new))) {
*data.sds.config = new != NULL ? sdsdup(new) : NULL;
sdsfree(prev);
return 1;
}
return 2;
}

static sds sdsConfigGet(typeData data) {
if (*data.sds.config) {
return sdsdup(*data.sds.config);
} else {
return sdsnew("");
}
}

static void sdsConfigRewrite(typeData data, const char *name, struct rewriteConfigState *state) {
rewriteConfigSdsOption(state, name, *(data.sds.config), data.sds.default_value);
}


#define ALLOW_EMPTY_STRING 0
#define EMPTY_STRING_IS_NULL 1

#define createStringConfig(name, alias, flags, empty_to_null, config_addr, default, is_valid, apply) { embedCommonConfig(name, alias, flags) embedConfigInterface(stringConfigInit, stringConfigSet, stringConfigGet, stringConfigRewrite, apply) .data.string = { .config = &(config_addr), .default_value = (default), .is_valid_fn = (is_valid), .convert_empty_to_null = (empty_to_null), } }










#define createSDSConfig(name, alias, flags, empty_to_null, config_addr, default, is_valid, apply) { embedCommonConfig(name, alias, flags) embedConfigInterface(sdsConfigInit, sdsConfigSet, sdsConfigGet, sdsConfigRewrite, apply) .data.sds = { .config = &(config_addr), .default_value = (default), .is_valid_fn = (is_valid), .convert_empty_to_null = (empty_to_null), } }











static void enumConfigInit(typeData data) {
*data.enumd.config = data.enumd.default_value;
}

static int enumConfigSet(typeData data, sds *argv, int argc, const char **err) {
UNUSED(argc);
int enumval = configEnumGetValue(data.enumd.enum_value, argv[0]);
if (enumval == INT_MIN) {
sds enumerr = sdsnew("argument must be one of the following: ");
configEnum *enumNode = data.enumd.enum_value;
while(enumNode->name != NULL) {
enumerr = sdscatlen(enumerr, enumNode->name,
strlen(enumNode->name));
enumerr = sdscatlen(enumerr, ", ", 2);
enumNode++;
}
sdsrange(enumerr,0,-3);

strncpy(loadbuf, enumerr, LOADBUF_SIZE);
loadbuf[LOADBUF_SIZE - 1] = '\0';

sdsfree(enumerr);
*err = loadbuf;
return 0;
}
if (data.enumd.is_valid_fn && !data.enumd.is_valid_fn(enumval, err))
return 0;
int prev = *(data.enumd.config);
if (prev != enumval) {
*(data.enumd.config) = enumval;
return 1;
}
return 2;
}

static sds enumConfigGet(typeData data) {
return sdsnew(configEnumGetNameOrUnknown(data.enumd.enum_value,*data.enumd.config));
}

static void enumConfigRewrite(typeData data, const char *name, struct rewriteConfigState *state) {
rewriteConfigEnumOption(state, name,*(data.enumd.config), data.enumd.enum_value, data.enumd.default_value);
}

#define createEnumConfig(name, alias, flags, enum, config_addr, default, is_valid, apply) { embedCommonConfig(name, alias, flags) embedConfigInterface(enumConfigInit, enumConfigSet, enumConfigGet, enumConfigRewrite, apply) .data.enumd = { .config = &(config_addr), .default_value = (default), .is_valid_fn = (is_valid), .enum_value = (enum), } }












#define SET_NUMERIC_TYPE(val) if (data.numeric.numeric_type == NUMERIC_TYPE_INT) { *(data.numeric.config.i) = (int) val; } else if (data.numeric.numeric_type == NUMERIC_TYPE_UINT) { *(data.numeric.config.ui) = (unsigned int) val; } else if (data.numeric.numeric_type == NUMERIC_TYPE_LONG) { *(data.numeric.config.l) = (long) val; } else if (data.numeric.numeric_type == NUMERIC_TYPE_ULONG) { *(data.numeric.config.ul) = (unsigned long) val; } else if (data.numeric.numeric_type == NUMERIC_TYPE_LONG_LONG) { *(data.numeric.config.ll) = (long long) val; } else if (data.numeric.numeric_type == NUMERIC_TYPE_ULONG_LONG) { *(data.numeric.config.ull) = (unsigned long long) val; } else if (data.numeric.numeric_type == NUMERIC_TYPE_SIZE_T) { *(data.numeric.config.st) = (size_t) val; } else if (data.numeric.numeric_type == NUMERIC_TYPE_SSIZE_T) { *(data.numeric.config.sst) = (ssize_t) val; } else if (data.numeric.numeric_type == NUMERIC_TYPE_OFF_T) { *(data.numeric.config.ot) = (off_t) val; } else if (data.numeric.numeric_type == NUMERIC_TYPE_TIME_T) { *(data.numeric.config.tt) = (time_t) val; }
























#define GET_NUMERIC_TYPE(val) if (data.numeric.numeric_type == NUMERIC_TYPE_INT) { val = *(data.numeric.config.i); } else if (data.numeric.numeric_type == NUMERIC_TYPE_UINT) { val = *(data.numeric.config.ui); } else if (data.numeric.numeric_type == NUMERIC_TYPE_LONG) { val = *(data.numeric.config.l); } else if (data.numeric.numeric_type == NUMERIC_TYPE_ULONG) { val = *(data.numeric.config.ul); } else if (data.numeric.numeric_type == NUMERIC_TYPE_LONG_LONG) { val = *(data.numeric.config.ll); } else if (data.numeric.numeric_type == NUMERIC_TYPE_ULONG_LONG) { val = *(data.numeric.config.ull); } else if (data.numeric.numeric_type == NUMERIC_TYPE_SIZE_T) { val = *(data.numeric.config.st); } else if (data.numeric.numeric_type == NUMERIC_TYPE_SSIZE_T) { val = *(data.numeric.config.sst); } else if (data.numeric.numeric_type == NUMERIC_TYPE_OFF_T) { val = *(data.numeric.config.ot); } else if (data.numeric.numeric_type == NUMERIC_TYPE_TIME_T) { val = *(data.numeric.config.tt); }























static void numericConfigInit(typeData data) {
SET_NUMERIC_TYPE(data.numeric.default_value)
}

static int numericBoundaryCheck(typeData data, long long ll, const char **err) {
if (data.numeric.numeric_type == NUMERIC_TYPE_ULONG_LONG ||
data.numeric.numeric_type == NUMERIC_TYPE_UINT ||
data.numeric.numeric_type == NUMERIC_TYPE_SIZE_T) {

unsigned long long ull = ll;
unsigned long long upper_bound = data.numeric.upper_bound;
unsigned long long lower_bound = data.numeric.lower_bound;
if (ull > upper_bound || ull < lower_bound) {
if (data.numeric.flags & OCTAL_CONFIG) {
snprintf(loadbuf, LOADBUF_SIZE,
"argument must be between %llo and %llo inclusive",
lower_bound,
upper_bound);
} else {
snprintf(loadbuf, LOADBUF_SIZE,
"argument must be between %llu and %llu inclusive",
lower_bound,
upper_bound);
}
*err = loadbuf;
return 0;
}
} else {

if (data.numeric.flags & PERCENT_CONFIG && ll < 0) {
if (ll < data.numeric.lower_bound) {
snprintf(loadbuf, LOADBUF_SIZE,
"percentage argument must be less or equal to %lld",
-data.numeric.lower_bound);
*err = loadbuf;
return 0;
}
}

else if (ll > data.numeric.upper_bound || ll < data.numeric.lower_bound) {
snprintf(loadbuf, LOADBUF_SIZE,
"argument must be between %lld and %lld inclusive",
data.numeric.lower_bound,
data.numeric.upper_bound);
*err = loadbuf;
return 0;
}
}
return 1;
}

static int numericParseString(typeData data, sds value, const char **err, long long *res) {

if (data.numeric.flags & MEMORY_CONFIG) {
int memerr;
*res = memtoull(value, &memerr);
if (!memerr)
return 1;
}


if (data.numeric.flags & PERCENT_CONFIG &&
sdslen(value) > 1 && value[sdslen(value)-1] == '%' &&
string2ll(value, sdslen(value)-1, res) &&
*res >= 0) {

*res = -*res;
return 1;
}


if (data.numeric.flags & OCTAL_CONFIG) {
char *endptr;
errno = 0;
*res = strtoll(value, &endptr, 8);
if (errno == 0 && *endptr == '\0')
return 1;
}


if (!data.numeric.flags && string2ll(value, sdslen(value), res))
return 1;


if (data.numeric.flags & MEMORY_CONFIG &&
data.numeric.flags & PERCENT_CONFIG)
*err = "argument must be a memory or percent value" ;
else if (data.numeric.flags & MEMORY_CONFIG)
*err = "argument must be a memory value";
else if (data.numeric.flags & OCTAL_CONFIG)
*err = "argument couldn't be parsed as an octal number";
else
*err = "argument couldn't be parsed into an integer";
return 0;
}

static int numericConfigSet(typeData data, sds *argv, int argc, const char **err) {
UNUSED(argc);
long long ll, prev = 0;

if (!numericParseString(data, argv[0], err, &ll))
return 0;

if (!numericBoundaryCheck(data, ll, err))
return 0;

if (data.numeric.is_valid_fn && !data.numeric.is_valid_fn(ll, err))
return 0;

GET_NUMERIC_TYPE(prev)
if (prev != ll) {
SET_NUMERIC_TYPE(ll)
return 1;
}

return 2;
}

static sds numericConfigGet(typeData data) {
char buf[128];

long long value = 0;
GET_NUMERIC_TYPE(value)

if (data.numeric.flags & PERCENT_CONFIG && value < 0) {
int len = ll2string(buf, sizeof(buf), -value);
buf[len] = '%';
buf[len+1] = '\0';
}
else if (data.numeric.flags & MEMORY_CONFIG) {
ull2string(buf, sizeof(buf), value);
} else if (data.numeric.flags & OCTAL_CONFIG) {
snprintf(buf, sizeof(buf), "%llo", value);
} else {
ll2string(buf, sizeof(buf), value);
}
return sdsnew(buf);
}

static void numericConfigRewrite(typeData data, const char *name, struct rewriteConfigState *state) {
long long value = 0;

GET_NUMERIC_TYPE(value)

if (data.numeric.flags & PERCENT_CONFIG && value < 0) {
rewriteConfigPercentOption(state, name, -value, data.numeric.default_value);
} else if (data.numeric.flags & MEMORY_CONFIG) {
rewriteConfigBytesOption(state, name, value, data.numeric.default_value);
} else if (data.numeric.flags & OCTAL_CONFIG) {
rewriteConfigOctalOption(state, name, value, data.numeric.default_value);
} else {
rewriteConfigNumericalOption(state, name, value, data.numeric.default_value);
}
}

#define embedCommonNumericalConfig(name, alias, _flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) { embedCommonConfig(name, alias, _flags) embedConfigInterface(numericConfigInit, numericConfigSet, numericConfigGet, numericConfigRewrite, apply) .data.numeric = { .lower_bound = (lower), .upper_bound = (upper), .default_value = (default), .is_valid_fn = (is_valid), .flags = (num_conf_flags),









#define createIntConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) embedCommonNumericalConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) .numeric_type = NUMERIC_TYPE_INT, .config.i = &(config_addr) } }






#define createUIntConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) embedCommonNumericalConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) .numeric_type = NUMERIC_TYPE_UINT, .config.ui = &(config_addr) } }






#define createLongConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) embedCommonNumericalConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) .numeric_type = NUMERIC_TYPE_LONG, .config.l = &(config_addr) } }






#define createULongConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) embedCommonNumericalConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) .numeric_type = NUMERIC_TYPE_ULONG, .config.ul = &(config_addr) } }






#define createLongLongConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) embedCommonNumericalConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) .numeric_type = NUMERIC_TYPE_LONG_LONG, .config.ll = &(config_addr) } }






#define createULongLongConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) embedCommonNumericalConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) .numeric_type = NUMERIC_TYPE_ULONG_LONG, .config.ull = &(config_addr) } }






#define createSizeTConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) embedCommonNumericalConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) .numeric_type = NUMERIC_TYPE_SIZE_T, .config.st = &(config_addr) } }






#define createSSizeTConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) embedCommonNumericalConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) .numeric_type = NUMERIC_TYPE_SSIZE_T, .config.sst = &(config_addr) } }






#define createTimeTConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) embedCommonNumericalConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) .numeric_type = NUMERIC_TYPE_TIME_T, .config.tt = &(config_addr) } }






#define createOffTConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) embedCommonNumericalConfig(name, alias, flags, lower, upper, config_addr, default, num_conf_flags, is_valid, apply) .numeric_type = NUMERIC_TYPE_OFF_T, .config.ot = &(config_addr) } }






#define createSpecialConfig(name, alias, modifiable, setfn, getfn, rewritefn, applyfn) { embedCommonConfig(name, alias, modifiable) embedConfigInterface(NULL, setfn, getfn, rewritefn, applyfn) }




static int isValidActiveDefrag(int val, const char **err) {
#if !defined(HAVE_DEFRAG)
if (val) {
*err = "Active defragmentation cannot be enabled: it "
"requires a Redis server compiled with a modified Jemalloc "
"like the one shipped by default with the Redis source "
"distribution";
return 0;
}
#else
UNUSED(val);
UNUSED(err);
#endif
return 1;
}

static int isValidDBfilename(char *val, const char **err) {
if (!pathIsBaseName(val)) {
*err = "dbfilename can't be a path, just a filename";
return 0;
}
return 1;
}

static int isValidAOFfilename(char *val, const char **err) {
if (!strcmp(val, "")) {
*err = "appendfilename can't be empty";
return 0;
}
if (!pathIsBaseName(val)) {
*err = "appendfilename can't be a path, just a filename";
return 0;
}
return 1;
}

static int isValidAOFdirname(char *val, const char **err) {
if (!strcmp(val, "")) {
*err = "appenddirname can't be empty";
return 0;
}
if (!pathIsBaseName(val)) {
*err = "appenddirname can't be a path, just a dirname";
return 0;
}
return 1;
}

static int isValidAnnouncedHostname(char *val, const char **err) {
if (strlen(val) >= NET_HOST_STR_LEN) {
*err = "Hostnames must be less than "
STRINGIFY(NET_HOST_STR_LEN) " characters";
return 0;
}

int i = 0;
char c;
while ((c = val[i])) {


if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
|| (c >= '0' && c <= '9') || (c == '-') || (c == '.')))
{
*err = "Hostnames may only contain alphanumeric characters, "
"hyphens or dots";
return 0;
}
c = val[i++];
}
return 1;
}


static int isValidProcTitleTemplate(char *val, const char **err) {
if (!validateProcTitleTemplate(val)) {
*err = "template format is invalid or contains unknown variables";
return 0;
}
return 1;
}

static int updateProcTitleTemplate(const char **err) {
if (redisSetProcTitle(NULL) == C_ERR) {
*err = "failed to set process title";
return 0;
}
return 1;
}

static int updateHZ(const char **err) {
UNUSED(err);


if (server.config_hz < CONFIG_MIN_HZ) server.config_hz = CONFIG_MIN_HZ;
if (server.config_hz > CONFIG_MAX_HZ) server.config_hz = CONFIG_MAX_HZ;
server.hz = server.config_hz;
return 1;
}

static int updatePort(const char **err) {
if (changeListenPort(server.port, &server.ipfd, acceptTcpHandler) == C_ERR) {
*err = "Unable to listen on this port. Check server logs.";
return 0;
}

return 1;
}

static int updateJemallocBgThread(const char **err) {
UNUSED(err);
set_jemalloc_bg_thread(server.jemalloc_bg_thread);
return 1;
}

static int updateReplBacklogSize(const char **err) {
UNUSED(err);
resizeReplicationBacklog();
return 1;
}

static int updateMaxmemory(const char **err) {
UNUSED(err);
if (server.maxmemory) {
size_t used = zmalloc_used_memory()-freeMemoryGetNotCountedMemory();
if (server.maxmemory < used) {
serverLog(LL_WARNING,"WARNING: the new maxmemory value set via CONFIG SET (%llu) is smaller than the current memory usage (%zu). This will result in key eviction and/or the inability to accept new write commands depending on the maxmemory-policy.", server.maxmemory, used);
}
startEvictionTimeProc();
}
return 1;
}

static int updateGoodSlaves(const char **err) {
UNUSED(err);
refreshGoodSlavesCount();
return 1;
}

static int updateWatchdogPeriod(const char **err) {
UNUSED(err);
applyWatchdogPeriod();
return 1;
}

static int updateAppendonly(const char **err) {
if (!server.aof_enabled && server.aof_state != AOF_OFF) {
stopAppendOnly();
} else if (server.aof_enabled && server.aof_state == AOF_OFF) {
if (startAppendOnly() == C_ERR) {
*err = "Unable to turn on AOF. Check server logs.";
return 0;
}
}
return 1;
}

static int updateAofAutoGCEnabled(const char **err) {
UNUSED(err);
if (!server.aof_disable_auto_gc) {
aofDelHistoryFiles();
}

return 1;
}

static int updateSighandlerEnabled(const char **err) {
UNUSED(err);
if (server.crashlog_enabled)
setupSignalHandlers();
else
removeSignalHandlers();
return 1;
}

static int updateMaxclients(const char **err) {
unsigned int new_maxclients = server.maxclients;
adjustOpenFilesLimit();
if (server.maxclients != new_maxclients) {
static char msg[128];
sprintf(msg, "The operating system is not able to handle the specified number of clients, try with %d", server.maxclients);
*err = msg;
return 0;
}
if ((unsigned int) aeGetSetSize(server.el) <
server.maxclients + CONFIG_FDSET_INCR)
{
if (aeResizeSetSize(server.el,
server.maxclients + CONFIG_FDSET_INCR) == AE_ERR)
{
*err = "The event loop API used by Redis is not able to handle the specified number of clients";
return 0;
}
}
return 1;
}

static int updateOOMScoreAdj(const char **err) {
if (setOOMScoreAdj(-1) == C_ERR) {
*err = "Failed to set current oom_score_adj. Check server logs.";
return 0;
}

return 1;
}

int updateRequirePass(const char **err) {
UNUSED(err);




ACLUpdateDefaultUserPassword(server.requirepass);
return 1;
}

static int applyBind(const char **err) {
if (changeBindAddr() == C_ERR) {
*err = "Failed to bind to specified addresses.";
return 0;
}

return 1;
}

int updateClusterFlags(const char **err) {
UNUSED(err);
clusterUpdateMyselfFlags();
return 1;
}

static int updateClusterIp(const char **err) {
UNUSED(err);
clusterUpdateMyselfIp();
return 1;
}

int updateClusterHostname(const char **err) {
UNUSED(err);
clusterUpdateMyselfHostname();
return 1;
}

#if defined(USE_OPENSSL)
static int applyTlsCfg(const char **err) {
UNUSED(err);


if ((server.tls_port || server.tls_replication || server.tls_cluster)
&& tlsConfigure(&server.tls_ctx_config) == C_ERR) {
*err = "Unable to update TLS configuration. Check server logs.";
return 0;
}
return 1;
}

static int applyTLSPort(const char **err) {

if (!isTlsConfigured() && tlsConfigure(&server.tls_ctx_config) == C_ERR) {
*err = "Unable to update TLS configuration. Check server logs.";
return 0;
}

if (changeListenPort(server.tls_port, &server.tlsfd, acceptTLSHandler) == C_ERR) {
*err = "Unable to listen on this port. Check server logs.";
return 0;
}

return 1;
}

#endif

static int setConfigDirOption(typeData data, sds *argv, int argc, const char **err) {
UNUSED(data);
if (argc != 1) {
*err = "wrong number of arguments";
return 0;
}
if (chdir(argv[0]) == -1) {
*err = strerror(errno);
return 0;
}
return 1;
}

static sds getConfigDirOption(typeData data) {
UNUSED(data);
char buf[1024];

if (getcwd(buf,sizeof(buf)) == NULL)
buf[0] = '\0';

return sdsnew(buf);
}

static int setConfigSaveOption(typeData data, sds *argv, int argc, const char **err) {
UNUSED(data);
int j;


if (argc == 1 && !strcasecmp(argv[0],""))
argc = 0;




if (argc & 1) {
*err = "Invalid save parameters";
return 0;
}
for (j = 0; j < argc; j++) {
char *eptr;
long val;

val = strtoll(argv[j], &eptr, 10);
if (eptr[0] != '\0' ||
((j & 1) == 0 && val < 1) ||
((j & 1) == 1 && val < 0)) {
*err = "Invalid save parameters";
return 0;
}
}

if (!reading_config_file) {
resetServerSaveParams();
} else {



static int save_loaded = 0;
if (!save_loaded) {
save_loaded = 1;
resetServerSaveParams();
}
}

for (j = 0; j < argc; j += 2) {
time_t seconds;
int changes;

seconds = strtoll(argv[j],NULL,10);
changes = strtoll(argv[j+1],NULL,10);
appendServerSaveParams(seconds, changes);
}

return 1;
}

static sds getConfigSaveOption(typeData data) {
UNUSED(data);
sds buf = sdsempty();
int j;

for (j = 0; j < server.saveparamslen; j++) {
buf = sdscatprintf(buf,"%jd %d",
(intmax_t)server.saveparams[j].seconds,
server.saveparams[j].changes);
if (j != server.saveparamslen-1)
buf = sdscatlen(buf," ",1);
}

return buf;
}

static int setConfigClientOutputBufferLimitOption(typeData data, sds *argv, int argc, const char **err) {
UNUSED(data);
return updateClientOutputBufferLimit(argv, argc, err);
}

static sds getConfigClientOutputBufferLimitOption(typeData data) {
UNUSED(data);
sds buf = sdsempty();
int j;
for (j = 0; j < CLIENT_TYPE_OBUF_COUNT; j++) {
buf = sdscatprintf(buf,"%s %llu %llu %ld",
getClientTypeName(j),
server.client_obuf_limits[j].hard_limit_bytes,
server.client_obuf_limits[j].soft_limit_bytes,
(long) server.client_obuf_limits[j].soft_limit_seconds);
if (j != CLIENT_TYPE_OBUF_COUNT-1)
buf = sdscatlen(buf," ",1);
}
return buf;
}




static int setConfigOOMScoreAdjValuesOption(typeData data, sds *argv, int argc, const char **err) {
int i;
int values[CONFIG_OOM_COUNT];
int change = 0;
UNUSED(data);

if (argc != CONFIG_OOM_COUNT) {
*err = "wrong number of arguments";
return 0;
}

for (i = 0; i < CONFIG_OOM_COUNT; i++) {
char *eptr;
long long val = strtoll(argv[i], &eptr, 10);

if (*eptr != '\0' || val < -2000 || val > 2000) {
if (err) *err = "Invalid oom-score-adj-values, elements must be between -2000 and 2000.";
return -1;
}

values[i] = val;
}





if (values[CONFIG_OOM_REPLICA] < values[CONFIG_OOM_MASTER] ||
values[CONFIG_OOM_BGCHILD] < values[CONFIG_OOM_REPLICA])
{
serverLog(LL_WARNING,
"The oom-score-adj-values configuration may not work for non-privileged processes! "
"Please consult the documentation.");
}

for (i = 0; i < CONFIG_OOM_COUNT; i++) {
if (server.oom_score_adj_values[i] != values[i]) {
server.oom_score_adj_values[i] = values[i];
change = 1;
}
}

return change ? 1 : 2;
}

static sds getConfigOOMScoreAdjValuesOption(typeData data) {
UNUSED(data);
sds buf = sdsempty();
int j;

for (j = 0; j < CONFIG_OOM_COUNT; j++) {
buf = sdscatprintf(buf,"%d", server.oom_score_adj_values[j]);
if (j != CONFIG_OOM_COUNT-1)
buf = sdscatlen(buf," ",1);
}

return buf;
}

static int setConfigNotifyKeyspaceEventsOption(typeData data, sds *argv, int argc, const char **err) {
UNUSED(data);
if (argc != 1) {
*err = "wrong number of arguments";
return 0;
}
int flags = keyspaceEventsStringToFlags(argv[0]);
if (flags == -1) {
*err = "Invalid event class character. Use 'Ag$lshzxeKEtmd'.";
return 0;
}
server.notify_keyspace_events = flags;
return 1;
}

static sds getConfigNotifyKeyspaceEventsOption(typeData data) {
UNUSED(data);
return keyspaceEventsFlagsToString(server.notify_keyspace_events);
}

static int setConfigBindOption(typeData data, sds* argv, int argc, const char **err) {
UNUSED(data);
int j;

if (argc > CONFIG_BINDADDR_MAX) {
*err = "Too many bind addresses specified.";
return 0;
}


if (argc == 1 && sdslen(argv[0]) == 0) argc = 0;


for (j = 0; j < server.bindaddr_count; j++) {
zfree(server.bindaddr[j]);
}
for (j = 0; j < argc; j++)
server.bindaddr[j] = zstrdup(argv[j]);
server.bindaddr_count = argc;

return 1;
}

static int setConfigReplicaOfOption(typeData data, sds* argv, int argc, const char **err) {
UNUSED(data);

if (argc != 2) {
*err = "wrong number of arguments";
return 0;
}

sdsfree(server.masterhost);
server.masterhost = NULL;
if (!strcasecmp(argv[0], "no") && !strcasecmp(argv[1], "one")) {
return 1;
}
char *ptr;
server.masterport = strtol(argv[1], &ptr, 10);
if (server.masterport < 0 || server.masterport > 65535 || *ptr != '\0') {
*err = "Invalid master port";
return 0;
}
server.masterhost = sdsnew(argv[0]);
server.repl_state = REPL_STATE_CONNECT;
return 1;
}

static sds getConfigBindOption(typeData data) {
UNUSED(data);
return sdsjoin(server.bindaddr,server.bindaddr_count," ");
}

static sds getConfigReplicaOfOption(typeData data) {
UNUSED(data);
char buf[256];
if (server.masterhost)
snprintf(buf,sizeof(buf),"%s %d",
server.masterhost, server.masterport);
else
buf[0] = '\0';
return sdsnew(buf);
}

int allowProtectedAction(int config, client *c) {
return (config == PROTECTED_ACTION_ALLOWED_YES) ||
(config == PROTECTED_ACTION_ALLOWED_LOCAL && islocalClient(c));
}


static int setConfigLatencyTrackingInfoPercentilesOutputOption(typeData data, sds *argv, int argc, const char **err) {
UNUSED(data);
zfree(server.latency_tracking_info_percentiles);
server.latency_tracking_info_percentiles = NULL;
server.latency_tracking_info_percentiles_len = argc;


if (argc == 1 && sdslen(argv[0]) == 0)
server.latency_tracking_info_percentiles_len = 0;
else
server.latency_tracking_info_percentiles = zmalloc(sizeof(double)*argc);

for (int j = 0; j < server.latency_tracking_info_percentiles_len; j++) {
double percentile;
if (!string2d(argv[j], sdslen(argv[j]), &percentile)) {
*err = "Invalid latency-tracking-info-percentiles parameters";
goto configerr;
}
if (percentile > 100.0 || percentile < 0.0) {
*err = "latency-tracking-info-percentiles parameters should sit between [0.0,100.0]";
goto configerr;
}
server.latency_tracking_info_percentiles[j] = percentile;
}

return 1;
configerr:
zfree(server.latency_tracking_info_percentiles);
server.latency_tracking_info_percentiles = NULL;
server.latency_tracking_info_percentiles_len = 0;
return 0;
}

static sds getConfigLatencyTrackingInfoPercentilesOutputOption(typeData data) {
UNUSED(data);
sds buf = sdsempty();
for (int j = 0; j < server.latency_tracking_info_percentiles_len; j++) {
char fbuf[128];
size_t len = sprintf(fbuf, "%f", server.latency_tracking_info_percentiles[j]);
len = trimDoubleString(fbuf, len);
buf = sdscatlen(buf, fbuf, len);
if (j != server.latency_tracking_info_percentiles_len-1)
buf = sdscatlen(buf," ",1);
}
return buf;
}


void rewriteConfigLatencyTrackingInfoPercentilesOutputOption(typeData data, const char *name, struct rewriteConfigState *state) {
UNUSED(data);
sds line = sdsnew(name);




if (!server.latency_tracking_info_percentiles_len) {
line = sdscat(line," \"\"");
} else {
for (int j = 0; j < server.latency_tracking_info_percentiles_len; j++) {
char fbuf[128];
size_t len = sprintf(fbuf, " %f", server.latency_tracking_info_percentiles[j]);
len = trimDoubleString(fbuf, len);
line = sdscatlen(line, fbuf, len);
}
}
rewriteConfigRewriteLine(state,name,line,1);
}

standardConfig configs[] = {

createBoolConfig("rdbchecksum", NULL, IMMUTABLE_CONFIG, server.rdb_checksum, 1, NULL, NULL),
createBoolConfig("daemonize", NULL, IMMUTABLE_CONFIG, server.daemonize, 0, NULL, NULL),
createBoolConfig("io-threads-do-reads", NULL, DEBUG_CONFIG | IMMUTABLE_CONFIG, server.io_threads_do_reads, 0,NULL, NULL),
createBoolConfig("always-show-logo", NULL, IMMUTABLE_CONFIG, server.always_show_logo, 0, NULL, NULL),
createBoolConfig("protected-mode", NULL, MODIFIABLE_CONFIG, server.protected_mode, 1, NULL, NULL),
createBoolConfig("rdbcompression", NULL, MODIFIABLE_CONFIG, server.rdb_compression, 1, NULL, NULL),
createBoolConfig("rdb-del-sync-files", NULL, MODIFIABLE_CONFIG, server.rdb_del_sync_files, 0, NULL, NULL),
createBoolConfig("activerehashing", NULL, MODIFIABLE_CONFIG, server.activerehashing, 1, NULL, NULL),
createBoolConfig("stop-writes-on-bgsave-error", NULL, MODIFIABLE_CONFIG, server.stop_writes_on_bgsave_err, 1, NULL, NULL),
createBoolConfig("set-proc-title", NULL, IMMUTABLE_CONFIG, server.set_proc_title, 1, NULL, NULL),
createBoolConfig("dynamic-hz", NULL, MODIFIABLE_CONFIG, server.dynamic_hz, 1, NULL, NULL),
createBoolConfig("lazyfree-lazy-eviction", NULL, DEBUG_CONFIG | MODIFIABLE_CONFIG, server.lazyfree_lazy_eviction, 0, NULL, NULL),
createBoolConfig("lazyfree-lazy-expire", NULL, DEBUG_CONFIG | MODIFIABLE_CONFIG, server.lazyfree_lazy_expire, 0, NULL, NULL),
createBoolConfig("lazyfree-lazy-server-del", NULL, DEBUG_CONFIG | MODIFIABLE_CONFIG, server.lazyfree_lazy_server_del, 0, NULL, NULL),
createBoolConfig("lazyfree-lazy-user-del", NULL, DEBUG_CONFIG | MODIFIABLE_CONFIG, server.lazyfree_lazy_user_del , 0, NULL, NULL),
createBoolConfig("lazyfree-lazy-user-flush", NULL, DEBUG_CONFIG | MODIFIABLE_CONFIG, server.lazyfree_lazy_user_flush , 0, NULL, NULL),
createBoolConfig("repl-disable-tcp-nodelay", NULL, MODIFIABLE_CONFIG, server.repl_disable_tcp_nodelay, 0, NULL, NULL),
createBoolConfig("repl-diskless-sync", NULL, DEBUG_CONFIG | MODIFIABLE_CONFIG, server.repl_diskless_sync, 1, NULL, NULL),
createBoolConfig("aof-rewrite-incremental-fsync", NULL, MODIFIABLE_CONFIG, server.aof_rewrite_incremental_fsync, 1, NULL, NULL),
createBoolConfig("no-appendfsync-on-rewrite", NULL, MODIFIABLE_CONFIG, server.aof_no_fsync_on_rewrite, 0, NULL, NULL),
createBoolConfig("cluster-require-full-coverage", NULL, MODIFIABLE_CONFIG, server.cluster_require_full_coverage, 1, NULL, NULL),
createBoolConfig("rdb-save-incremental-fsync", NULL, MODIFIABLE_CONFIG, server.rdb_save_incremental_fsync, 1, NULL, NULL),
createBoolConfig("aof-load-truncated", NULL, MODIFIABLE_CONFIG, server.aof_load_truncated, 1, NULL, NULL),
createBoolConfig("aof-use-rdb-preamble", NULL, MODIFIABLE_CONFIG, server.aof_use_rdb_preamble, 1, NULL, NULL),
createBoolConfig("aof-timestamp-enabled", NULL, MODIFIABLE_CONFIG, server.aof_timestamp_enabled, 0, NULL, NULL),
createBoolConfig("cluster-replica-no-failover", "cluster-slave-no-failover", MODIFIABLE_CONFIG, server.cluster_slave_no_failover, 0, NULL, updateClusterFlags),
createBoolConfig("replica-lazy-flush", "slave-lazy-flush", MODIFIABLE_CONFIG, server.repl_slave_lazy_flush, 0, NULL, NULL),
createBoolConfig("replica-serve-stale-data", "slave-serve-stale-data", MODIFIABLE_CONFIG, server.repl_serve_stale_data, 1, NULL, NULL),
createBoolConfig("replica-read-only", "slave-read-only", DEBUG_CONFIG | MODIFIABLE_CONFIG, server.repl_slave_ro, 1, NULL, NULL),
createBoolConfig("replica-ignore-maxmemory", "slave-ignore-maxmemory", MODIFIABLE_CONFIG, server.repl_slave_ignore_maxmemory, 1, NULL, NULL),
createBoolConfig("jemalloc-bg-thread", NULL, MODIFIABLE_CONFIG, server.jemalloc_bg_thread, 1, NULL, updateJemallocBgThread),
createBoolConfig("activedefrag", NULL, DEBUG_CONFIG | MODIFIABLE_CONFIG, server.active_defrag_enabled, 0, isValidActiveDefrag, NULL),
createBoolConfig("syslog-enabled", NULL, IMMUTABLE_CONFIG, server.syslog_enabled, 0, NULL, NULL),
createBoolConfig("cluster-enabled", NULL, IMMUTABLE_CONFIG, server.cluster_enabled, 0, NULL, NULL),
createBoolConfig("appendonly", NULL, MODIFIABLE_CONFIG | DENY_LOADING_CONFIG, server.aof_enabled, 0, NULL, updateAppendonly),
createBoolConfig("cluster-allow-reads-when-down", NULL, MODIFIABLE_CONFIG, server.cluster_allow_reads_when_down, 0, NULL, NULL),
createBoolConfig("cluster-allow-pubsubshard-when-down", NULL, MODIFIABLE_CONFIG, server.cluster_allow_pubsubshard_when_down, 1, NULL, NULL),
createBoolConfig("crash-log-enabled", NULL, MODIFIABLE_CONFIG, server.crashlog_enabled, 1, NULL, updateSighandlerEnabled),
createBoolConfig("crash-memcheck-enabled", NULL, MODIFIABLE_CONFIG, server.memcheck_enabled, 1, NULL, NULL),
createBoolConfig("use-exit-on-panic", NULL, MODIFIABLE_CONFIG | HIDDEN_CONFIG, server.use_exit_on_panic, 0, NULL, NULL),
createBoolConfig("disable-thp", NULL, MODIFIABLE_CONFIG, server.disable_thp, 1, NULL, NULL),
createBoolConfig("cluster-allow-replica-migration", NULL, MODIFIABLE_CONFIG, server.cluster_allow_replica_migration, 1, NULL, NULL),
createBoolConfig("replica-announced", NULL, MODIFIABLE_CONFIG, server.replica_announced, 1, NULL, NULL),
createBoolConfig("latency-tracking", NULL, MODIFIABLE_CONFIG, server.latency_tracking_enabled, 1, NULL, NULL),
createBoolConfig("aof-disable-auto-gc", NULL, MODIFIABLE_CONFIG, server.aof_disable_auto_gc, 0, NULL, updateAofAutoGCEnabled),


createStringConfig("aclfile", NULL, IMMUTABLE_CONFIG, ALLOW_EMPTY_STRING, server.acl_filename, "", NULL, NULL),
createStringConfig("unixsocket", NULL, IMMUTABLE_CONFIG, EMPTY_STRING_IS_NULL, server.unixsocket, NULL, NULL, NULL),
createStringConfig("pidfile", NULL, IMMUTABLE_CONFIG, EMPTY_STRING_IS_NULL, server.pidfile, NULL, NULL, NULL),
createStringConfig("replica-announce-ip", "slave-announce-ip", MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.slave_announce_ip, NULL, NULL, NULL),
createStringConfig("masteruser", NULL, MODIFIABLE_CONFIG | SENSITIVE_CONFIG, EMPTY_STRING_IS_NULL, server.masteruser, NULL, NULL, NULL),
createStringConfig("cluster-announce-ip", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.cluster_announce_ip, NULL, NULL, updateClusterIp),
createStringConfig("cluster-config-file", NULL, IMMUTABLE_CONFIG, ALLOW_EMPTY_STRING, server.cluster_configfile, "nodes.conf", NULL, NULL),
createStringConfig("cluster-announce-hostname", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.cluster_announce_hostname, NULL, isValidAnnouncedHostname, updateClusterHostname),
createStringConfig("syslog-ident", NULL, IMMUTABLE_CONFIG, ALLOW_EMPTY_STRING, server.syslog_ident, "redis", NULL, NULL),
createStringConfig("dbfilename", NULL, MODIFIABLE_CONFIG | PROTECTED_CONFIG, ALLOW_EMPTY_STRING, server.rdb_filename, "dump.rdb", isValidDBfilename, NULL),
createStringConfig("appendfilename", NULL, IMMUTABLE_CONFIG, ALLOW_EMPTY_STRING, server.aof_filename, "appendonly.aof", isValidAOFfilename, NULL),
createStringConfig("appenddirname", NULL, IMMUTABLE_CONFIG, ALLOW_EMPTY_STRING, server.aof_dirname, "appendonlydir", isValidAOFdirname, NULL),
createStringConfig("server_cpulist", NULL, IMMUTABLE_CONFIG, EMPTY_STRING_IS_NULL, server.server_cpulist, NULL, NULL, NULL),
createStringConfig("bio_cpulist", NULL, IMMUTABLE_CONFIG, EMPTY_STRING_IS_NULL, server.bio_cpulist, NULL, NULL, NULL),
createStringConfig("aof_rewrite_cpulist", NULL, IMMUTABLE_CONFIG, EMPTY_STRING_IS_NULL, server.aof_rewrite_cpulist, NULL, NULL, NULL),
createStringConfig("bgsave_cpulist", NULL, IMMUTABLE_CONFIG, EMPTY_STRING_IS_NULL, server.bgsave_cpulist, NULL, NULL, NULL),
createStringConfig("ignore-warnings", NULL, MODIFIABLE_CONFIG, ALLOW_EMPTY_STRING, server.ignore_warnings, "", NULL, NULL),
createStringConfig("proc-title-template", NULL, MODIFIABLE_CONFIG, ALLOW_EMPTY_STRING, server.proc_title_template, CONFIG_DEFAULT_PROC_TITLE_TEMPLATE, isValidProcTitleTemplate, updateProcTitleTemplate),
createStringConfig("bind-source-addr", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.bind_source_addr, NULL, NULL, NULL),
createStringConfig("logfile", NULL, IMMUTABLE_CONFIG, ALLOW_EMPTY_STRING, server.logfile, "", NULL, NULL),


createSDSConfig("masterauth", NULL, MODIFIABLE_CONFIG | SENSITIVE_CONFIG, EMPTY_STRING_IS_NULL, server.masterauth, NULL, NULL, NULL),
createSDSConfig("requirepass", NULL, MODIFIABLE_CONFIG | SENSITIVE_CONFIG, EMPTY_STRING_IS_NULL, server.requirepass, NULL, NULL, updateRequirePass),


createEnumConfig("supervised", NULL, IMMUTABLE_CONFIG, supervised_mode_enum, server.supervised_mode, SUPERVISED_NONE, NULL, NULL),
createEnumConfig("syslog-facility", NULL, IMMUTABLE_CONFIG, syslog_facility_enum, server.syslog_facility, LOG_LOCAL0, NULL, NULL),
createEnumConfig("repl-diskless-load", NULL, DEBUG_CONFIG | MODIFIABLE_CONFIG | DENY_LOADING_CONFIG, repl_diskless_load_enum, server.repl_diskless_load, REPL_DISKLESS_LOAD_DISABLED, NULL, NULL),
createEnumConfig("loglevel", NULL, MODIFIABLE_CONFIG, loglevel_enum, server.verbosity, LL_NOTICE, NULL, NULL),
createEnumConfig("maxmemory-policy", NULL, MODIFIABLE_CONFIG, maxmemory_policy_enum, server.maxmemory_policy, MAXMEMORY_NO_EVICTION, NULL, NULL),
createEnumConfig("appendfsync", NULL, MODIFIABLE_CONFIG, aof_fsync_enum, server.aof_fsync, AOF_FSYNC_EVERYSEC, NULL, NULL),
createEnumConfig("oom-score-adj", NULL, MODIFIABLE_CONFIG, oom_score_adj_enum, server.oom_score_adj, OOM_SCORE_ADJ_NO, NULL, updateOOMScoreAdj),
createEnumConfig("acl-pubsub-default", NULL, MODIFIABLE_CONFIG, acl_pubsub_default_enum, server.acl_pubsub_default, SELECTOR_FLAG_ALLCHANNELS, NULL, NULL),
createEnumConfig("sanitize-dump-payload", NULL, DEBUG_CONFIG | MODIFIABLE_CONFIG, sanitize_dump_payload_enum, server.sanitize_dump_payload, SANITIZE_DUMP_NO, NULL, NULL),
createEnumConfig("enable-protected-configs", NULL, IMMUTABLE_CONFIG, protected_action_enum, server.enable_protected_configs, PROTECTED_ACTION_ALLOWED_NO, NULL, NULL),
createEnumConfig("enable-debug-command", NULL, IMMUTABLE_CONFIG, protected_action_enum, server.enable_debug_cmd, PROTECTED_ACTION_ALLOWED_NO, NULL, NULL),
createEnumConfig("enable-module-command", NULL, IMMUTABLE_CONFIG, protected_action_enum, server.enable_module_cmd, PROTECTED_ACTION_ALLOWED_NO, NULL, NULL),
createEnumConfig("cluster-preferred-endpoint-type", NULL, MODIFIABLE_CONFIG, cluster_preferred_endpoint_type_enum, server.cluster_preferred_endpoint_type, CLUSTER_ENDPOINT_TYPE_IP, NULL, NULL),


createIntConfig("databases", NULL, IMMUTABLE_CONFIG, 1, INT_MAX, server.dbnum, 16, INTEGER_CONFIG, NULL, NULL),
createIntConfig("port", NULL, MODIFIABLE_CONFIG, 0, 65535, server.port, 6379, INTEGER_CONFIG, NULL, updatePort),
createIntConfig("io-threads", NULL, DEBUG_CONFIG | IMMUTABLE_CONFIG, 1, 128, server.io_threads_num, 1, INTEGER_CONFIG, NULL, NULL),
createIntConfig("auto-aof-rewrite-percentage", NULL, MODIFIABLE_CONFIG, 0, INT_MAX, server.aof_rewrite_perc, 100, INTEGER_CONFIG, NULL, NULL),
createIntConfig("cluster-replica-validity-factor", "cluster-slave-validity-factor", MODIFIABLE_CONFIG, 0, INT_MAX, server.cluster_slave_validity_factor, 10, INTEGER_CONFIG, NULL, NULL),
createIntConfig("list-max-listpack-size", "list-max-ziplist-size", MODIFIABLE_CONFIG, INT_MIN, INT_MAX, server.list_max_listpack_size, -2, INTEGER_CONFIG, NULL, NULL),
createIntConfig("tcp-keepalive", NULL, MODIFIABLE_CONFIG, 0, INT_MAX, server.tcpkeepalive, 300, INTEGER_CONFIG, NULL, NULL),
createIntConfig("cluster-migration-barrier", NULL, MODIFIABLE_CONFIG, 0, INT_MAX, server.cluster_migration_barrier, 1, INTEGER_CONFIG, NULL, NULL),
createIntConfig("active-defrag-cycle-min", NULL, MODIFIABLE_CONFIG, 1, 99, server.active_defrag_cycle_min, 1, INTEGER_CONFIG, NULL, NULL),
createIntConfig("active-defrag-cycle-max", NULL, MODIFIABLE_CONFIG, 1, 99, server.active_defrag_cycle_max, 25, INTEGER_CONFIG, NULL, NULL),
createIntConfig("active-defrag-threshold-lower", NULL, MODIFIABLE_CONFIG, 0, 1000, server.active_defrag_threshold_lower, 10, INTEGER_CONFIG, NULL, NULL),
createIntConfig("active-defrag-threshold-upper", NULL, MODIFIABLE_CONFIG, 0, 1000, server.active_defrag_threshold_upper, 100, INTEGER_CONFIG, NULL, NULL),
createIntConfig("lfu-log-factor", NULL, MODIFIABLE_CONFIG, 0, INT_MAX, server.lfu_log_factor, 10, INTEGER_CONFIG, NULL, NULL),
createIntConfig("lfu-decay-time", NULL, MODIFIABLE_CONFIG, 0, INT_MAX, server.lfu_decay_time, 1, INTEGER_CONFIG, NULL, NULL),
createIntConfig("replica-priority", "slave-priority", MODIFIABLE_CONFIG, 0, INT_MAX, server.slave_priority, 100, INTEGER_CONFIG, NULL, NULL),
createIntConfig("repl-diskless-sync-delay", NULL, MODIFIABLE_CONFIG, 0, INT_MAX, server.repl_diskless_sync_delay, 5, INTEGER_CONFIG, NULL, NULL),
createIntConfig("maxmemory-samples", NULL, MODIFIABLE_CONFIG, 1, INT_MAX, server.maxmemory_samples, 5, INTEGER_CONFIG, NULL, NULL),
createIntConfig("maxmemory-eviction-tenacity", NULL, MODIFIABLE_CONFIG, 0, 100, server.maxmemory_eviction_tenacity, 10, INTEGER_CONFIG, NULL, NULL),
createIntConfig("timeout", NULL, MODIFIABLE_CONFIG, 0, INT_MAX, server.maxidletime, 0, INTEGER_CONFIG, NULL, NULL),
createIntConfig("replica-announce-port", "slave-announce-port", MODIFIABLE_CONFIG, 0, 65535, server.slave_announce_port, 0, INTEGER_CONFIG, NULL, NULL),
createIntConfig("tcp-backlog", NULL, IMMUTABLE_CONFIG, 0, INT_MAX, server.tcp_backlog, 511, INTEGER_CONFIG, NULL, NULL),
createIntConfig("cluster-port", NULL, IMMUTABLE_CONFIG, 0, 65535, server.cluster_port, 0, INTEGER_CONFIG, NULL, NULL),
createIntConfig("cluster-announce-bus-port", NULL, MODIFIABLE_CONFIG, 0, 65535, server.cluster_announce_bus_port, 0, INTEGER_CONFIG, NULL, NULL),
createIntConfig("cluster-announce-port", NULL, MODIFIABLE_CONFIG, 0, 65535, server.cluster_announce_port, 0, INTEGER_CONFIG, NULL, NULL),
createIntConfig("cluster-announce-tls-port", NULL, MODIFIABLE_CONFIG, 0, 65535, server.cluster_announce_tls_port, 0, INTEGER_CONFIG, NULL, NULL),
createIntConfig("repl-timeout", NULL, MODIFIABLE_CONFIG, 1, INT_MAX, server.repl_timeout, 60, INTEGER_CONFIG, NULL, NULL),
createIntConfig("repl-ping-replica-period", "repl-ping-slave-period", MODIFIABLE_CONFIG, 1, INT_MAX, server.repl_ping_slave_period, 10, INTEGER_CONFIG, NULL, NULL),
createIntConfig("list-compress-depth", NULL, DEBUG_CONFIG | MODIFIABLE_CONFIG, 0, INT_MAX, server.list_compress_depth, 0, INTEGER_CONFIG, NULL, NULL),
createIntConfig("rdb-key-save-delay", NULL, MODIFIABLE_CONFIG | HIDDEN_CONFIG, INT_MIN, INT_MAX, server.rdb_key_save_delay, 0, INTEGER_CONFIG, NULL, NULL),
createIntConfig("key-load-delay", NULL, MODIFIABLE_CONFIG | HIDDEN_CONFIG, INT_MIN, INT_MAX, server.key_load_delay, 0, INTEGER_CONFIG, NULL, NULL),
createIntConfig("active-expire-effort", NULL, MODIFIABLE_CONFIG, 1, 10, server.active_expire_effort, 1, INTEGER_CONFIG, NULL, NULL),
createIntConfig("hz", NULL, MODIFIABLE_CONFIG, 0, INT_MAX, server.config_hz, CONFIG_DEFAULT_HZ, INTEGER_CONFIG, NULL, updateHZ),
createIntConfig("min-replicas-to-write", "min-slaves-to-write", MODIFIABLE_CONFIG, 0, INT_MAX, server.repl_min_slaves_to_write, 0, INTEGER_CONFIG, NULL, updateGoodSlaves),
createIntConfig("min-replicas-max-lag", "min-slaves-max-lag", MODIFIABLE_CONFIG, 0, INT_MAX, server.repl_min_slaves_max_lag, 10, INTEGER_CONFIG, NULL, updateGoodSlaves),
createIntConfig("watchdog-period", NULL, MODIFIABLE_CONFIG | HIDDEN_CONFIG, 0, INT_MAX, server.watchdog_period, 0, INTEGER_CONFIG, NULL, updateWatchdogPeriod),
createIntConfig("shutdown-timeout", NULL, MODIFIABLE_CONFIG, 0, INT_MAX, server.shutdown_timeout, 10, INTEGER_CONFIG, NULL, NULL),
createIntConfig("repl-diskless-sync-max-replicas", NULL, MODIFIABLE_CONFIG, 0, INT_MAX, server.repl_diskless_sync_max_replicas, 0, INTEGER_CONFIG, NULL, NULL),


createUIntConfig("maxclients", NULL, MODIFIABLE_CONFIG, 1, UINT_MAX, server.maxclients, 10000, INTEGER_CONFIG, NULL, updateMaxclients),
createUIntConfig("unixsocketperm", NULL, IMMUTABLE_CONFIG, 0, 0777, server.unixsocketperm, 0, OCTAL_CONFIG, NULL, NULL),


createULongConfig("active-defrag-max-scan-fields", NULL, MODIFIABLE_CONFIG, 1, LONG_MAX, server.active_defrag_max_scan_fields, 1000, INTEGER_CONFIG, NULL, NULL),
createULongConfig("slowlog-max-len", NULL, MODIFIABLE_CONFIG, 0, LONG_MAX, server.slowlog_max_len, 128, INTEGER_CONFIG, NULL, NULL),
createULongConfig("acllog-max-len", NULL, MODIFIABLE_CONFIG, 0, LONG_MAX, server.acllog_max_len, 128, INTEGER_CONFIG, NULL, NULL),


createLongLongConfig("busy-reply-threshold", "lua-time-limit", MODIFIABLE_CONFIG, 0, LONG_MAX, server.busy_reply_threshold, 5000, INTEGER_CONFIG, NULL, NULL),
createLongLongConfig("cluster-node-timeout", NULL, MODIFIABLE_CONFIG, 0, LLONG_MAX, server.cluster_node_timeout, 15000, INTEGER_CONFIG, NULL, NULL),
createLongLongConfig("slowlog-log-slower-than", NULL, MODIFIABLE_CONFIG, -1, LLONG_MAX, server.slowlog_log_slower_than, 10000, INTEGER_CONFIG, NULL, NULL),
createLongLongConfig("latency-monitor-threshold", NULL, MODIFIABLE_CONFIG, 0, LLONG_MAX, server.latency_monitor_threshold, 0, INTEGER_CONFIG, NULL, NULL),
createLongLongConfig("proto-max-bulk-len", NULL, DEBUG_CONFIG | MODIFIABLE_CONFIG, 1024*1024, LONG_MAX, server.proto_max_bulk_len, 512ll*1024*1024, MEMORY_CONFIG, NULL, NULL),
createLongLongConfig("stream-node-max-entries", NULL, MODIFIABLE_CONFIG, 0, LLONG_MAX, server.stream_node_max_entries, 100, INTEGER_CONFIG, NULL, NULL),
createLongLongConfig("repl-backlog-size", NULL, MODIFIABLE_CONFIG, 1, LLONG_MAX, server.repl_backlog_size, 1024*1024, MEMORY_CONFIG, NULL, updateReplBacklogSize),


createULongLongConfig("maxmemory", NULL, MODIFIABLE_CONFIG, 0, ULLONG_MAX, server.maxmemory, 0, MEMORY_CONFIG, NULL, updateMaxmemory),
createULongLongConfig("cluster-link-sendbuf-limit", NULL, MODIFIABLE_CONFIG, 0, ULLONG_MAX, server.cluster_link_sendbuf_limit_bytes, 0, MEMORY_CONFIG, NULL, NULL),


createSizeTConfig("hash-max-listpack-entries", "hash-max-ziplist-entries", MODIFIABLE_CONFIG, 0, LONG_MAX, server.hash_max_listpack_entries, 512, INTEGER_CONFIG, NULL, NULL),
createSizeTConfig("set-max-intset-entries", NULL, MODIFIABLE_CONFIG, 0, LONG_MAX, server.set_max_intset_entries, 512, INTEGER_CONFIG, NULL, NULL),
createSizeTConfig("zset-max-listpack-entries", "zset-max-ziplist-entries", MODIFIABLE_CONFIG, 0, LONG_MAX, server.zset_max_listpack_entries, 128, INTEGER_CONFIG, NULL, NULL),
createSizeTConfig("active-defrag-ignore-bytes", NULL, MODIFIABLE_CONFIG, 1, LLONG_MAX, server.active_defrag_ignore_bytes, 100<<20, MEMORY_CONFIG, NULL, NULL),
createSizeTConfig("hash-max-listpack-value", "hash-max-ziplist-value", MODIFIABLE_CONFIG, 0, LONG_MAX, server.hash_max_listpack_value, 64, MEMORY_CONFIG, NULL, NULL),
createSizeTConfig("stream-node-max-bytes", NULL, MODIFIABLE_CONFIG, 0, LONG_MAX, server.stream_node_max_bytes, 4096, MEMORY_CONFIG, NULL, NULL),
createSizeTConfig("zset-max-listpack-value", "zset-max-ziplist-value", MODIFIABLE_CONFIG, 0, LONG_MAX, server.zset_max_listpack_value, 64, MEMORY_CONFIG, NULL, NULL),
createSizeTConfig("hll-sparse-max-bytes", NULL, MODIFIABLE_CONFIG, 0, LONG_MAX, server.hll_sparse_max_bytes, 3000, MEMORY_CONFIG, NULL, NULL),
createSizeTConfig("tracking-table-max-keys", NULL, MODIFIABLE_CONFIG, 0, LONG_MAX, server.tracking_table_max_keys, 1000000, INTEGER_CONFIG, NULL, NULL),
createSizeTConfig("client-query-buffer-limit", NULL, DEBUG_CONFIG | MODIFIABLE_CONFIG, 1024*1024, LONG_MAX, server.client_max_querybuf_len, 1024*1024*1024, MEMORY_CONFIG, NULL, NULL),
createSSizeTConfig("maxmemory-clients", NULL, MODIFIABLE_CONFIG, -100, SSIZE_MAX, server.maxmemory_clients, 0, MEMORY_CONFIG | PERCENT_CONFIG, NULL, NULL),


createTimeTConfig("repl-backlog-ttl", NULL, MODIFIABLE_CONFIG, 0, LONG_MAX, server.repl_backlog_time_limit, 60*60, INTEGER_CONFIG, NULL, NULL),
createOffTConfig("auto-aof-rewrite-min-size", NULL, MODIFIABLE_CONFIG, 0, LLONG_MAX, server.aof_rewrite_min_size, 64*1024*1024, MEMORY_CONFIG, NULL, NULL),
createOffTConfig("loading-process-events-interval-bytes", NULL, MODIFIABLE_CONFIG | HIDDEN_CONFIG, 1024, INT_MAX, server.loading_process_events_interval_bytes, 1024*1024*2, INTEGER_CONFIG, NULL, NULL),

#if defined(USE_OPENSSL)
createIntConfig("tls-port", NULL, MODIFIABLE_CONFIG, 0, 65535, server.tls_port, 0, INTEGER_CONFIG, NULL, applyTLSPort),
createIntConfig("tls-session-cache-size", NULL, MODIFIABLE_CONFIG, 0, INT_MAX, server.tls_ctx_config.session_cache_size, 20*1024, INTEGER_CONFIG, NULL, applyTlsCfg),
createIntConfig("tls-session-cache-timeout", NULL, MODIFIABLE_CONFIG, 0, INT_MAX, server.tls_ctx_config.session_cache_timeout, 300, INTEGER_CONFIG, NULL, applyTlsCfg),
createBoolConfig("tls-cluster", NULL, MODIFIABLE_CONFIG, server.tls_cluster, 0, NULL, applyTlsCfg),
createBoolConfig("tls-replication", NULL, MODIFIABLE_CONFIG, server.tls_replication, 0, NULL, applyTlsCfg),
createEnumConfig("tls-auth-clients", NULL, MODIFIABLE_CONFIG, tls_auth_clients_enum, server.tls_auth_clients, TLS_CLIENT_AUTH_YES, NULL, NULL),
createBoolConfig("tls-prefer-server-ciphers", NULL, MODIFIABLE_CONFIG, server.tls_ctx_config.prefer_server_ciphers, 0, NULL, applyTlsCfg),
createBoolConfig("tls-session-caching", NULL, MODIFIABLE_CONFIG, server.tls_ctx_config.session_caching, 1, NULL, applyTlsCfg),
createStringConfig("tls-cert-file", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.tls_ctx_config.cert_file, NULL, NULL, applyTlsCfg),
createStringConfig("tls-key-file", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.tls_ctx_config.key_file, NULL, NULL, applyTlsCfg),
createStringConfig("tls-key-file-pass", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.tls_ctx_config.key_file_pass, NULL, NULL, applyTlsCfg),
createStringConfig("tls-client-cert-file", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.tls_ctx_config.client_cert_file, NULL, NULL, applyTlsCfg),
createStringConfig("tls-client-key-file", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.tls_ctx_config.client_key_file, NULL, NULL, applyTlsCfg),
createStringConfig("tls-client-key-file-pass", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.tls_ctx_config.client_key_file_pass, NULL, NULL, applyTlsCfg),
createStringConfig("tls-dh-params-file", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.tls_ctx_config.dh_params_file, NULL, NULL, applyTlsCfg),
createStringConfig("tls-ca-cert-file", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.tls_ctx_config.ca_cert_file, NULL, NULL, applyTlsCfg),
createStringConfig("tls-ca-cert-dir", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.tls_ctx_config.ca_cert_dir, NULL, NULL, applyTlsCfg),
createStringConfig("tls-protocols", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.tls_ctx_config.protocols, NULL, NULL, applyTlsCfg),
createStringConfig("tls-ciphers", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.tls_ctx_config.ciphers, NULL, NULL, applyTlsCfg),
createStringConfig("tls-ciphersuites", NULL, MODIFIABLE_CONFIG, EMPTY_STRING_IS_NULL, server.tls_ctx_config.ciphersuites, NULL, NULL, applyTlsCfg),
#endif


createSpecialConfig("dir", NULL, MODIFIABLE_CONFIG | PROTECTED_CONFIG | DENY_LOADING_CONFIG, setConfigDirOption, getConfigDirOption, rewriteConfigDirOption, NULL),
createSpecialConfig("save", NULL, MODIFIABLE_CONFIG | MULTI_ARG_CONFIG, setConfigSaveOption, getConfigSaveOption, rewriteConfigSaveOption, NULL),
createSpecialConfig("client-output-buffer-limit", NULL, MODIFIABLE_CONFIG | MULTI_ARG_CONFIG, setConfigClientOutputBufferLimitOption, getConfigClientOutputBufferLimitOption, rewriteConfigClientOutputBufferLimitOption, NULL),
createSpecialConfig("oom-score-adj-values", NULL, MODIFIABLE_CONFIG | MULTI_ARG_CONFIG, setConfigOOMScoreAdjValuesOption, getConfigOOMScoreAdjValuesOption, rewriteConfigOOMScoreAdjValuesOption, updateOOMScoreAdj),
createSpecialConfig("notify-keyspace-events", NULL, MODIFIABLE_CONFIG, setConfigNotifyKeyspaceEventsOption, getConfigNotifyKeyspaceEventsOption, rewriteConfigNotifyKeyspaceEventsOption, NULL),
createSpecialConfig("bind", NULL, MODIFIABLE_CONFIG | MULTI_ARG_CONFIG, setConfigBindOption, getConfigBindOption, rewriteConfigBindOption, applyBind),
createSpecialConfig("replicaof", "slaveof", IMMUTABLE_CONFIG | MULTI_ARG_CONFIG, setConfigReplicaOfOption, getConfigReplicaOfOption, rewriteConfigReplicaOfOption, NULL),
createSpecialConfig("latency-tracking-info-percentiles", NULL, MODIFIABLE_CONFIG | MULTI_ARG_CONFIG, setConfigLatencyTrackingInfoPercentilesOutputOption, getConfigLatencyTrackingInfoPercentilesOutputOption, rewriteConfigLatencyTrackingInfoPercentilesOutputOption, NULL),


{NULL}
};





void configHelpCommand(client *c) {
const char *help[] = {
"GET <pattern>",
" Return parameters matching the glob-like <pattern> and their values.",
"SET <directive> <value>",
" Set the configuration <directive> to <value>.",
"RESETSTAT",
" Reset statistics reported by the INFO command.",
"REWRITE",
" Rewrite the configuration file.",
NULL
};

addReplyHelp(c, help);
}





void configResetStatCommand(client *c) {
resetServerStats();
resetCommandTableStats(server.commands);
resetErrorTableStats();
addReply(c,shared.ok);
}





void configRewriteCommand(client *c) {
if (server.configfile == NULL) {
addReplyError(c,"The server is running without a config file");
return;
}
if (rewriteConfig(server.configfile, 0) == -1) {
serverLog(LL_WARNING,"CONFIG REWRITE failed: %s", strerror(errno));
addReplyErrorFormat(c,"Rewriting config file: %s", strerror(errno));
} else {
serverLog(LL_WARNING,"CONFIG REWRITE executed with success.");
addReply(c,shared.ok);
}
}
