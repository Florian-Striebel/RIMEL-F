#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
int decr_every = 1;
int keyspace_size = 1000000;
time_t switch_after = 30;
struct entry {
uint8_t counter;
uint16_t decrtime;
uint64_t hits;
time_t ctime;
};
#define to_16bit_minutes(x) ((x/60) & 65535)
#define COUNTER_INIT_VAL 5
uint16_t minutes_diff(uint16_t now, uint16_t prev) {
if (now >= prev) return now-prev;
return 65535-prev+now;
}
uint8_t log_incr(uint8_t counter) {
if (counter == 255) return counter;
double r = (double)rand()/RAND_MAX;
double baseval = counter-COUNTER_INIT_VAL;
if (baseval < 0) baseval = 0;
double limit = 1.0/(baseval*10+1);
if (r < limit) counter++;
return counter;
}
void access_entry(struct entry *e) {
e->counter = log_incr(e->counter);
e->hits++;
}
uint8_t scan_entry(struct entry *e) {
if (minutes_diff(to_16bit_minutes(time(NULL)),e->decrtime)
>= decr_every)
{
if (e->counter) {
if (e->counter > COUNTER_INIT_VAL*2) {
e->counter /= 2;
} else {
e->counter--;
}
}
e->decrtime = to_16bit_minutes(time(NULL));
}
return e->counter;
}
void show_entry(long pos, struct entry *e) {
char *tag = "normal ";
if (pos >= 10 && pos <= 14) tag = "new no access";
if (pos >= 15 && pos <= 19) tag = "new accessed ";
if (pos >= keyspace_size -5) tag= "old no access";
printf("%ld] <%s> frequency:%d decrtime:%d [%lu hits | age:%ld sec]\n",
pos, tag, e->counter, e->decrtime, (unsigned long)e->hits,
time(NULL) - e->ctime);
}
int main(void) {
time_t start = time(NULL);
time_t new_entry_time = start;
time_t display_time = start;
struct entry *entries = malloc(sizeof(*entries)*keyspace_size);
long j;
for (j = 0; j < keyspace_size; j++) {
entries[j].counter = COUNTER_INIT_VAL;
entries[j].decrtime = to_16bit_minutes(start);
entries[j].hits = 0;
entries[j].ctime = time(NULL);
}
while(1) {
time_t now = time(NULL);
long idx;
for (j = 0; j < 3; j++) {
scan_entry(entries+(rand()%keyspace_size));
}
if (now-start < switch_after) {
idx = 1;
while((rand() % 21) != 0 && idx < keyspace_size) idx *= 2;
if (idx > keyspace_size) idx = keyspace_size;
idx = rand() % idx;
} else {
idx = rand() % keyspace_size;
}
if ((idx < 10 || idx > 14) && (idx < keyspace_size-5))
access_entry(entries+idx);
if (new_entry_time <= now) {
idx = 10+(rand()%10);
entries[idx].counter = COUNTER_INIT_VAL;
entries[idx].decrtime = to_16bit_minutes(time(NULL));
entries[idx].hits = 0;
entries[idx].ctime = time(NULL);
new_entry_time = now+10;
}
if (display_time != now) {
printf("=============================\n");
printf("Current minutes time: %d\n", (int)to_16bit_minutes(now));
printf("Access method: %s\n",
(now-start < switch_after) ? "power-law" : "flat");
for (j = 0; j < 20; j++)
show_entry(j,entries+j);
for (j = keyspace_size-20; j < keyspace_size; j++)
show_entry(j,entries+j);
display_time = now;
}
}
return 0;
}
