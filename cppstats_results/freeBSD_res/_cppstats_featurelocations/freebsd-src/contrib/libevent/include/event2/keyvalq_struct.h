

























#if !defined(EVENT2_KEYVALQ_STRUCT_H_INCLUDED_)
#define EVENT2_KEYVALQ_STRUCT_H_INCLUDED_

#if defined(__cplusplus)
extern "C" {
#endif



#if !defined(TAILQ_ENTRY)
#define EVENT_DEFINED_TQENTRY_
#define TAILQ_ENTRY(type) struct { struct type *tqe_next; struct type **tqe_prev; }




#endif

#if !defined(TAILQ_HEAD)
#define EVENT_DEFINED_TQHEAD_
#define TAILQ_HEAD(name, type) struct name { struct type *tqh_first; struct type **tqh_last; }




#endif





struct evkeyval {
TAILQ_ENTRY(evkeyval) next;

char *key;
char *value;
};

TAILQ_HEAD (evkeyvalq, evkeyval);


#if defined(EVENT_DEFINED_TQENTRY_)
#undef TAILQ_ENTRY
#endif

#if defined(EVENT_DEFINED_TQHEAD_)
#undef TAILQ_HEAD
#endif

#if defined(__cplusplus)
}
#endif

#endif
