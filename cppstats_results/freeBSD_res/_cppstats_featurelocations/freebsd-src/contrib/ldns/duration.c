






































#include <ldns/config.h>
#include <ldns/duration.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>






ldns_duration_type*
ldns_duration_create(void)
{
ldns_duration_type* duration;

duration = malloc(sizeof(ldns_duration_type));
if (!duration) {
return NULL;
}
duration->years = 0;
duration->months = 0;
duration->weeks = 0;
duration->days = 0;
duration->hours = 0;
duration->minutes = 0;
duration->seconds = 0;
return duration;
}






int
ldns_duration_compare(const ldns_duration_type* d1, const ldns_duration_type* d2)
{
if (!d1 && !d2) {
return 0;
}
if (!d1 || !d2) {
return d1?-1:1;
}

if (d1->years != d2->years) {
return (int) (d1->years - d2->years);
}
if (d1->months != d2->months) {
return (int) (d1->months - d2->months);
}
if (d1->weeks != d2->weeks) {
return (int) (d1->weeks - d2->weeks);
}
if (d1->days != d2->days) {
return (int) (d1->days - d2->days);
}
if (d1->hours != d2->hours) {
return (int) (d1->hours - d2->hours);
}
if (d1->minutes != d2->minutes) {
return (int) (d1->minutes - d2->minutes);
}
if (d1->seconds != d2->seconds) {
return (int) (d1->seconds - d2->seconds);
}

return 0;
}






ldns_duration_type*
ldns_duration_create_from_string(const char* str)
{
ldns_duration_type* duration = ldns_duration_create();
char* P, *X, *T, *W;
int not_weeks = 0;

if (!duration) {
return NULL;
}
if (!str) {
return duration;
}

P = strchr(str, 'P');
if (!P) {
ldns_duration_cleanup(duration);
return NULL;
}

T = strchr(str, 'T');
X = strchr(str, 'Y');
if (X) {
duration->years = (time_t) atoi(str+1);
str = X;
not_weeks = 1;
}
X = strchr(str, 'M');
if (X && (!T || (size_t) (X-P) < (size_t) (T-P))) {
duration->months = (time_t) atoi(str+1);
str = X;
not_weeks = 1;
}
X = strchr(str, 'D');
if (X) {
duration->days = (time_t) atoi(str+1);
str = X;
not_weeks = 1;
}
if (T) {
str = T;
not_weeks = 1;
}
X = strchr(str, 'H');
if (X && T) {
duration->hours = (time_t) atoi(str+1);
str = X;
not_weeks = 1;
}
X = strrchr(str, 'M');
if (X && T && (size_t) (X-P) > (size_t) (T-P)) {
duration->minutes = (time_t) atoi(str+1);
str = X;
not_weeks = 1;
}
X = strchr(str, 'S');
if (X && T) {
duration->seconds = (time_t) atoi(str+1);
str = X;
not_weeks = 1;
}

W = strchr(str, 'W');
if (W) {
if (not_weeks) {
ldns_duration_cleanup(duration);
return NULL;
} else {
duration->weeks = (time_t) atoi(str+1);
str = W;
}
}
return duration;
}






static size_t
digits_in_number(time_t duration)
{
uint32_t period = (uint32_t) duration;
size_t count = 0;

while (period > 0) {
count++;
period /= 10;
}
return count;
}






char*
ldns_duration2string(const ldns_duration_type* duration)
{
char* str = NULL, *num = NULL;
size_t count = 2;
int T = 0;

if (!duration) {
return NULL;
}

if (duration->years > 0) {
count = count + 1 + digits_in_number(duration->years);
}
if (duration->months > 0) {
count = count + 1 + digits_in_number(duration->months);
}
if (duration->weeks > 0) {
count = count + 1 + digits_in_number(duration->weeks);
}
if (duration->days > 0) {
count = count + 1 + digits_in_number(duration->days);
}
if (duration->hours > 0) {
count = count + 1 + digits_in_number(duration->hours);
T = 1;
}
if (duration->minutes > 0) {
count = count + 1 + digits_in_number(duration->minutes);
T = 1;
}
if (duration->seconds > 0) {
count = count + 1 + digits_in_number(duration->seconds);
T = 1;
}
if (T) {
count++;
}

str = (char*) calloc(count, sizeof(char));
str[0] = 'P';
str[1] = '\0';

if (duration->years > 0) {
count = digits_in_number(duration->years);
num = (char*) calloc(count+2, sizeof(char));
snprintf(num, count+2, "%uY", (unsigned int) duration->years);
str = strncat(str, num, count+2);
free((void*) num);
}
if (duration->months > 0) {
count = digits_in_number(duration->months);
num = (char*) calloc(count+2, sizeof(char));
snprintf(num, count+2, "%uM", (unsigned int) duration->months);
str = strncat(str, num, count+2);
free((void*) num);
}
if (duration->weeks > 0) {
count = digits_in_number(duration->weeks);
num = (char*) calloc(count+2, sizeof(char));
snprintf(num, count+2, "%uW", (unsigned int) duration->weeks);
str = strncat(str, num, count+2);
free((void*) num);
}
if (duration->days > 0) {
count = digits_in_number(duration->days);
num = (char*) calloc(count+2, sizeof(char));
snprintf(num, count+2, "%uD", (unsigned int) duration->days);
str = strncat(str, num, count+2);
free((void*) num);
}
if (T) {
str = strncat(str, "T", 1);
}
if (duration->hours > 0) {
count = digits_in_number(duration->hours);
num = (char*) calloc(count+2, sizeof(char));
snprintf(num, count+2, "%uH", (unsigned int) duration->hours);
str = strncat(str, num, count+2);
free((void*) num);
}
if (duration->minutes > 0) {
count = digits_in_number(duration->minutes);
num = (char*) calloc(count+2, sizeof(char));
snprintf(num, count+2, "%uM", (unsigned int) duration->minutes);
str = strncat(str, num, count+2);
free((void*) num);
}
if (duration->seconds > 0) {
count = digits_in_number(duration->seconds);
num = (char*) calloc(count+2, sizeof(char));
snprintf(num, count+2, "%uS", (unsigned int) duration->seconds);
str = strncat(str, num, count+2);
free((void*) num);
}
return str;
}






time_t
ldns_duration2time(const ldns_duration_type* duration)
{
time_t period = 0;

if (duration) {
period += (duration->seconds);
period += (duration->minutes)*60;
period += (duration->hours)*3600;
period += (duration->days)*86400;
period += (duration->weeks)*86400*7;
period += (duration->months)*86400*31;
period += (duration->years)*86400*365;






}
return period;
}






void
ldns_duration_cleanup(ldns_duration_type* duration)
{
if (!duration) {
return;
}
free(duration);
return;
}
