

























#if !defined(EVENT2_DNS_STRUCT_H_INCLUDED_)
#define EVENT2_DNS_STRUCT_H_INCLUDED_








#if defined(__cplusplus)
extern "C" {
#endif

#include <event2/event-config.h>
#if defined(EVENT__HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(EVENT__HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif


#include <event2/util.h>





struct evdns_server_request {
int flags;
int nquestions;
struct evdns_server_question **questions;
};
struct evdns_server_question {
int type;
#if defined(__cplusplus)
int dns_question_class;
#else



int class;
#define dns_question_class class
#endif
char name[1];
};

#if defined(__cplusplus)
}
#endif

#endif

