

























#if !defined(EVENT2_RPC_STRUCT_H_INCLUDED_)
#define EVENT2_RPC_STRUCT_H_INCLUDED_

#if defined(__cplusplus)
extern "C" {
#endif











struct evrpc_status {
#define EVRPC_STATUS_ERR_NONE 0
#define EVRPC_STATUS_ERR_TIMEOUT 1
#define EVRPC_STATUS_ERR_BADPAYLOAD 2
#define EVRPC_STATUS_ERR_UNSTARTED 3
#define EVRPC_STATUS_ERR_HOOKABORTED 4
int error;


struct evhttp_request *http_req;
};




struct evrpc {
TAILQ_ENTRY(evrpc) next;


const char* uri;


void *(*request_new)(void *);
void *request_new_arg;


void (*request_free)(void *);


int (*request_unmarshal)(void *, struct evbuffer *);


void *(*reply_new)(void *);
void *reply_new_arg;


void (*reply_free)(void *);


int (*reply_complete)(void *);


void (*reply_marshal)(struct evbuffer*, void *);


void (*cb)(struct evrpc_req_generic *, void *);
void *cb_arg;


struct evrpc_base *base;
};

#if defined(__cplusplus)
}
#endif

#endif
