

























#if !defined(EVENT2_RPC_COMPAT_H_INCLUDED_)
#define EVENT2_RPC_COMPAT_H_INCLUDED_








#if defined(__cplusplus)
extern "C" {
#endif


#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

#undef EVTAG_ASSIGN
#undef EVTAG_GET
#undef EVTAG_ADD

#define EVTAG_ASSIGN(msg, member, args...) (*(msg)->base->member##_assign)(msg, ##args)

#define EVTAG_GET(msg, member, args...) (*(msg)->base->member##_get)(msg, ##args)

#define EVTAG_ADD(msg, member, args...) (*(msg)->base->member##_add)(msg, ##args)

#endif
#define EVTAG_LEN(msg, member) ((msg)->member##_length)

#if defined(__cplusplus)
}
#endif

#endif
