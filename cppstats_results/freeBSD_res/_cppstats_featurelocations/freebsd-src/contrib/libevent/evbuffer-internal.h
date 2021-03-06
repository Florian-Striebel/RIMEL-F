

























#if !defined(EVBUFFER_INTERNAL_H_INCLUDED_)
#define EVBUFFER_INTERNAL_H_INCLUDED_

#if defined(__cplusplus)
extern "C" {
#endif

#include "event2/event-config.h"
#include "evconfig-private.h"
#include "event2/util.h"
#include "event2/event_struct.h"
#include "util-internal.h"
#include "defer-internal.h"




#define EVBUFFER_CB_NODEFER 2

#if defined(_WIN32)
#include <winsock2.h>
#endif
#include <sys/queue.h>




#if EVENT__SIZEOF_VOID_P < 8
#define MIN_BUFFER_SIZE 512
#else
#define MIN_BUFFER_SIZE 1024
#endif



struct evbuffer_cb_entry {

LIST_ENTRY(evbuffer_cb_entry) next;



union {
evbuffer_cb_func cb_func;
evbuffer_cb cb_obsolete;
} cb;

void *cbarg;

ev_uint32_t flags;
};

struct bufferevent;
struct evbuffer_chain;
struct evbuffer {

struct evbuffer_chain *first;

struct evbuffer_chain *last;














struct evbuffer_chain **last_with_datap;


size_t total_len;



size_t n_add_for_cb;


size_t n_del_for_cb;

#if !defined(EVENT__DISABLE_THREAD_SUPPORT)

void *lock;
#endif


unsigned own_lock : 1;


unsigned freeze_start : 1;


unsigned freeze_end : 1;





unsigned deferred_cbs : 1;
#if defined(_WIN32)

unsigned is_overlapped : 1;
#endif

ev_uint32_t flags;


struct event_base *cb_queue;





int refcnt;



struct event_callback deferred;


LIST_HEAD(evbuffer_cb_queue, evbuffer_cb_entry) callbacks;



struct bufferevent *parent;
};

#if EVENT__SIZEOF_OFF_T < EVENT__SIZEOF_SIZE_T
typedef ev_ssize_t ev_misalign_t;
#define EVBUFFER_CHAIN_MAX ((size_t)EV_SSIZE_MAX)
#else
typedef ev_off_t ev_misalign_t;
#if EVENT__SIZEOF_OFF_T > EVENT__SIZEOF_SIZE_T
#define EVBUFFER_CHAIN_MAX EV_SIZE_MAX
#else
#define EVBUFFER_CHAIN_MAX ((size_t)EV_SSIZE_MAX)
#endif
#endif


struct evbuffer_chain {

struct evbuffer_chain *next;


size_t buffer_len;



ev_misalign_t misalign;




size_t off;


unsigned flags;
#define EVBUFFER_FILESEGMENT 0x0001
#define EVBUFFER_SENDFILE 0x0002
#define EVBUFFER_REFERENCE 0x0004
#define EVBUFFER_IMMUTABLE 0x0008


#define EVBUFFER_MEM_PINNED_R 0x0010
#define EVBUFFER_MEM_PINNED_W 0x0020
#define EVBUFFER_MEM_PINNED_ANY (EVBUFFER_MEM_PINNED_R|EVBUFFER_MEM_PINNED_W)


#define EVBUFFER_DANGLING 0x0040

#define EVBUFFER_MULTICAST 0x0080


int refcnt;







unsigned char *buffer;
};




struct evbuffer_chain_reference {
evbuffer_ref_cleanup_cb cleanupfn;
void *extra;
};



struct evbuffer_chain_file_segment {
struct evbuffer_file_segment *segment;
#if defined(_WIN32)

HANDLE view_handle;
#endif
};


struct evbuffer_file_segment {
void *lock;
int refcnt;
unsigned flags;


unsigned can_sendfile : 1;
unsigned is_mapping : 1;


int fd;

void *mapping;
#if defined(_WIN32)

HANDLE mapping_handle;
#endif


char *contents;

ev_off_t file_offset;


ev_off_t mmap_offset;

ev_off_t length;

evbuffer_file_segment_cleanup_cb cleanup_cb;

void *cleanup_cb_arg;
};



struct evbuffer_multicast_parent {

struct evbuffer *source;

struct evbuffer_chain *parent;
};

#define EVBUFFER_CHAIN_SIZE sizeof(struct evbuffer_chain)

#define EVBUFFER_CHAIN_EXTRA(t, c) (t *)((struct evbuffer_chain *)(c) + 1)


#define ASSERT_EVBUFFER_LOCKED(buffer) EVLOCK_ASSERT_LOCKED((buffer)->lock)


#define EVBUFFER_LOCK(buffer) do { EVLOCK_LOCK((buffer)->lock, 0); } while (0)



#define EVBUFFER_UNLOCK(buffer) do { EVLOCK_UNLOCK((buffer)->lock, 0); } while (0)



#define EVBUFFER_LOCK2(buffer1, buffer2) do { EVLOCK_LOCK2((buffer1)->lock, (buffer2)->lock, 0, 0); } while (0)



#define EVBUFFER_UNLOCK2(buffer1, buffer2) do { EVLOCK_UNLOCK2((buffer1)->lock, (buffer2)->lock, 0, 0); } while (0)





void evbuffer_incref_(struct evbuffer *buf);

void evbuffer_incref_and_lock_(struct evbuffer *buf);


void evbuffer_chain_pin_(struct evbuffer_chain *chain, unsigned flag);

void evbuffer_chain_unpin_(struct evbuffer_chain *chain, unsigned flag);


void evbuffer_decref_and_unlock_(struct evbuffer *buffer);



int evbuffer_expand_fast_(struct evbuffer *, size_t, int);







int evbuffer_read_setup_vecs_(struct evbuffer *buf, ev_ssize_t howmuch,
struct evbuffer_iovec *vecs, int n_vecs, struct evbuffer_chain ***chainp,
int exact);


#define WSABUF_FROM_EVBUFFER_IOV(i,ei) do { (i)->buf = (ei)->iov_base; (i)->len = (unsigned long)(ei)->iov_len; } while (0)







void evbuffer_set_parent_(struct evbuffer *buf, struct bufferevent *bev);

void evbuffer_invoke_callbacks_(struct evbuffer *buf);


int evbuffer_get_callbacks_(struct evbuffer *buffer,
struct event_callback **cbs,
int max_cbs);

#if defined(__cplusplus)
}
#endif

#endif
