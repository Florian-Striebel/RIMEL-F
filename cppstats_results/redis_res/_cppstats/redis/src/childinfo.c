#include "server.h"
#include <unistd.h>
#include <fcntl.h>
typedef struct {
size_t keys;
size_t cow;
monotime cow_updated;
double progress;
childInfoType information_type;
} child_info_data;
void openChildInfoPipe(void) {
if (anetPipe(server.child_info_pipe, O_NONBLOCK, 0) == -1) {
closeChildInfoPipe();
} else {
server.child_info_nread = 0;
}
}
void closeChildInfoPipe(void) {
if (server.child_info_pipe[0] != -1 ||
server.child_info_pipe[1] != -1)
{
close(server.child_info_pipe[0]);
close(server.child_info_pipe[1]);
server.child_info_pipe[0] = -1;
server.child_info_pipe[1] = -1;
server.child_info_nread = 0;
}
}
void sendChildInfoGeneric(childInfoType info_type, size_t keys, double progress, char *pname) {
if (server.child_info_pipe[1] == -1) return;
static monotime cow_updated = 0;
static uint64_t cow_update_cost = 0;
static size_t cow = 0;
static size_t peak_cow = 0;
static size_t update_count = 0;
static unsigned long long sum_cow = 0;
child_info_data data = {0};
monotime now = getMonotonicUs();
if (info_type != CHILD_INFO_TYPE_CURRENT_INFO ||
!cow_updated ||
now - cow_updated > cow_update_cost * CHILD_COW_DUTY_CYCLE)
{
cow = zmalloc_get_private_dirty(-1);
cow_updated = getMonotonicUs();
cow_update_cost = cow_updated - now;
if (cow > peak_cow) peak_cow = cow;
sum_cow += cow;
update_count++;
int cow_info = (info_type != CHILD_INFO_TYPE_CURRENT_INFO);
if (cow || cow_info) {
serverLog(cow_info ? LL_NOTICE : LL_VERBOSE,
"Fork CoW for %s: current %zu MB, peak %zu MB, average %llu MB",
pname, cow>>20, peak_cow>>20, (sum_cow/update_count)>>20);
}
}
data.information_type = info_type;
data.keys = keys;
data.cow = cow;
data.cow_updated = cow_updated;
data.progress = progress;
ssize_t wlen = sizeof(data);
if (write(server.child_info_pipe[1], &data, wlen) != wlen) {
}
}
void updateChildInfo(childInfoType information_type, size_t cow, monotime cow_updated, size_t keys, double progress) {
if (cow > server.stat_current_cow_peak) server.stat_current_cow_peak = cow;
if (information_type == CHILD_INFO_TYPE_CURRENT_INFO) {
server.stat_current_cow_bytes = cow;
server.stat_current_cow_updated = cow_updated;
server.stat_current_save_keys_processed = keys;
if (progress != -1) server.stat_module_progress = progress;
} else if (information_type == CHILD_INFO_TYPE_AOF_COW_SIZE) {
server.stat_aof_cow_bytes = server.stat_current_cow_peak;
} else if (information_type == CHILD_INFO_TYPE_RDB_COW_SIZE) {
server.stat_rdb_cow_bytes = server.stat_current_cow_peak;
} else if (information_type == CHILD_INFO_TYPE_MODULE_COW_SIZE) {
server.stat_module_cow_bytes = server.stat_current_cow_peak;
}
}
int readChildInfo(childInfoType *information_type, size_t *cow, monotime *cow_updated, size_t *keys, double* progress) {
static child_info_data buffer;
ssize_t wlen = sizeof(buffer);
if (server.child_info_nread == wlen) server.child_info_nread = 0;
int nread = read(server.child_info_pipe[0], (char *)&buffer + server.child_info_nread, wlen - server.child_info_nread);
if (nread > 0) {
server.child_info_nread += nread;
}
if (server.child_info_nread == wlen) {
*information_type = buffer.information_type;
*cow = buffer.cow;
*cow_updated = buffer.cow_updated;
*keys = buffer.keys;
*progress = buffer.progress;
return 1;
} else {
return 0;
}
}
void receiveChildInfo(void) {
if (server.child_info_pipe[0] == -1) return;
size_t cow;
monotime cow_updated;
size_t keys;
double progress;
childInfoType information_type;
while (readChildInfo(&information_type, &cow, &cow_updated, &keys, &progress)) {
updateChildInfo(information_type, cow, cow_updated, keys, progress);
}
}
