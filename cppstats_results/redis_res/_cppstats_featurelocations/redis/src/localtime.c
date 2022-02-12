




























#include <time.h>





















static int is_leap_year(time_t year) {
if (year % 4) return 0;
else if (year % 100) return 1;
else if (year % 400) return 0;
else return 1;
}

void nolocks_localtime(struct tm *tmp, time_t t, time_t tz, int dst) {
const time_t secs_min = 60;
const time_t secs_hour = 3600;
const time_t secs_day = 3600*24;

t -= tz;
t += 3600*dst;
time_t days = t / secs_day;
time_t seconds = t % secs_day;

tmp->tm_isdst = dst;
tmp->tm_hour = seconds / secs_hour;
tmp->tm_min = (seconds % secs_hour) / secs_min;
tmp->tm_sec = (seconds % secs_hour) % secs_min;




tmp->tm_wday = (days+4)%7;


tmp->tm_year = 1970;
while(1) {

time_t days_this_year = 365 + is_leap_year(tmp->tm_year);
if (days_this_year > days) break;
days -= days_this_year;
tmp->tm_year++;
}
tmp->tm_yday = days;




int mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
mdays[1] += is_leap_year(tmp->tm_year);

tmp->tm_mon = 0;
while(days >= mdays[tmp->tm_mon]) {
days -= mdays[tmp->tm_mon];
tmp->tm_mon++;
}

tmp->tm_mday = days+1;
tmp->tm_year -= 1900;
}

#if defined(LOCALTIME_TEST_MAIN)
#include <stdio.h>

int main(void) {

tzset();
time_t t = time(NULL);
struct tm *aux = localtime(&t);
int daylight_active = aux->tm_isdst;

struct tm tm;
char buf[1024];

nolocks_localtime(&tm,t,timezone,daylight_active);
strftime(buf,sizeof(buf),"%d %b %H:%M:%S",&tm);
printf("[timezone: %d, dl: %d] %s\n", (int)timezone, (int)daylight_active, buf);
}
#endif
