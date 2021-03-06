






































#if !defined(LDNS_DURATION_H)
#define LDNS_DURATION_H

#include <stdint.h>
#include <time.h>





typedef struct ldns_duration_struct ldns_duration_type;
struct ldns_duration_struct
{
time_t years;
time_t months;
time_t weeks;
time_t days;
time_t hours;
time_t minutes;
time_t seconds;
};






ldns_duration_type* ldns_duration_create(void);








int ldns_duration_compare(const ldns_duration_type* d1, const ldns_duration_type* d2);







ldns_duration_type* ldns_duration_create_from_string(const char* str);







char* ldns_duration2string(const ldns_duration_type* duration);







time_t ldns_duration2time(const ldns_duration_type* duration);






void ldns_duration_cleanup(ldns_duration_type* duration);

#endif
