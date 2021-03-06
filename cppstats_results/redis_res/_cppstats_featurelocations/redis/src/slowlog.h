




























#if !defined(__SLOWLOG_H__)
#define __SLOWLOG_H__

#define SLOWLOG_ENTRY_MAX_ARGC 32
#define SLOWLOG_ENTRY_MAX_STRING 128


typedef struct slowlogEntry {
robj **argv;
int argc;
long long id;
long long duration;
time_t time;
sds cname;
sds peerid;
} slowlogEntry;


void slowlogInit(void);
void slowlogPushEntryIfNeeded(client *c, robj **argv, int argc, long long duration);

#endif
