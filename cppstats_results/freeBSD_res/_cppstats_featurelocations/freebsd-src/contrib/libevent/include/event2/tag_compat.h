

























#if !defined(EVENT2_TAG_COMPAT_H_INCLUDED_)
#define EVENT2_TAG_COMPAT_H_INCLUDED_
















#define encode_int(evbuf, number) evtag_encode_int((evbuf), (number))
#define encode_int64(evbuf, number) evtag_encode_int64((evbuf), (number))


#endif
