






















#if !defined(UTLIST_H)
#define UTLIST_H

#define UTLIST_VERSION 1.9.8

#include <assert.h>




































#if defined(_MSC_VER)
#if _MSC_VER >= 1600 && defined(__cplusplus)
#define LDECLTYPE(x) decltype(x)
#else
#define NO_DECLTYPE
#define LDECLTYPE(x) char*
#endif
#elif defined(__ICCARM__)
#define NO_DECLTYPE
#define LDECLTYPE(x) char*
#else
#define LDECLTYPE(x) __typeof(x)
#endif




#if defined(NO_DECLTYPE)
#define _SV(elt,list) _tmp = (char*)(list); {char **_alias = (char**)&(list); *_alias = (elt); }
#define _NEXT(elt,list,next) ((char*)((list)->next))
#define _NEXTASGN(elt,list,to,next) { char **_alias = (char**)&((list)->next); *_alias=(char*)(to); }

#define _PREVASGN(elt,list,to,prev) { char **_alias = (char**)&((list)->prev); *_alias=(char*)(to); }
#define _RS(list) { char **_alias = (char**)&(list); *_alias=_tmp; }
#define _CASTASGN(a,b) { char **_alias = (char**)&(a); *_alias=(char*)(b); }
#else
#define _SV(elt,list)
#define _NEXT(elt,list,next) ((elt)->next)
#define _NEXTASGN(elt,list,to,next) ((elt)->next)=(to)

#define _PREVASGN(elt,list,to,prev) ((elt)->prev)=(to)
#define _RS(list)
#define _CASTASGN(a,b) (a)=(b)
#endif





#define LL_SORT(list, cmp) LL_SORT2(list, cmp, next)


#define LL_SORT2(list, cmp, next) do { LDECLTYPE(list) _ls_p; LDECLTYPE(list) _ls_q; LDECLTYPE(list) _ls_e; LDECLTYPE(list) _ls_tail; int _ls_insize, _ls_nmerges, _ls_psize, _ls_qsize, _ls_i, _ls_looping; if (list) { _ls_insize = 1; _ls_looping = 1; while (_ls_looping) { _CASTASGN(_ls_p,list); list = NULL; _ls_tail = NULL; _ls_nmerges = 0; while (_ls_p) { _ls_nmerges++; _ls_q = _ls_p; _ls_psize = 0; for (_ls_i = 0; _ls_i < _ls_insize; _ls_i++) { _ls_psize++; _SV(_ls_q,list); _ls_q = _NEXT(_ls_q,list,next); _RS(list); if (!_ls_q) break; } _ls_qsize = _ls_insize; while (_ls_psize > 0 || (_ls_qsize > 0 && _ls_q)) { if (_ls_psize == 0) { _ls_e = _ls_q; _SV(_ls_q,list); _ls_q = _NEXT(_ls_q,list,next); _RS(list); _ls_qsize--; } else if (_ls_qsize == 0 || !_ls_q) { _ls_e = _ls_p; _SV(_ls_p,list); _ls_p = _NEXT(_ls_p,list,next); _RS(list); _ls_psize--; } else if (cmp(_ls_p,_ls_q) <= 0) { _ls_e = _ls_p; _SV(_ls_p,list); _ls_p = _NEXT(_ls_p,list,next); _RS(list); _ls_psize--; } else { _ls_e = _ls_q; _SV(_ls_q,list); _ls_q = _NEXT(_ls_q,list,next); _RS(list); _ls_qsize--; } if (_ls_tail) { _SV(_ls_tail,list); _NEXTASGN(_ls_tail,list,_ls_e,next); _RS(list); } else { _CASTASGN(list,_ls_e); } _ls_tail = _ls_e; } _ls_p = _ls_q; } if (_ls_tail) { _SV(_ls_tail,list); _NEXTASGN(_ls_tail,list,NULL,next); _RS(list); } if (_ls_nmerges <= 1) { _ls_looping=0; } _ls_insize *= 2; } } } while (0)



























































#define DL_SORT(list, cmp) DL_SORT2(list, cmp, prev, next)


#define DL_SORT2(list, cmp, prev, next) do { LDECLTYPE(list) _ls_p; LDECLTYPE(list) _ls_q; LDECLTYPE(list) _ls_e; LDECLTYPE(list) _ls_tail; int _ls_insize, _ls_nmerges, _ls_psize, _ls_qsize, _ls_i, _ls_looping; if (list) { _ls_insize = 1; _ls_looping = 1; while (_ls_looping) { _CASTASGN(_ls_p,list); list = NULL; _ls_tail = NULL; _ls_nmerges = 0; while (_ls_p) { _ls_nmerges++; _ls_q = _ls_p; _ls_psize = 0; for (_ls_i = 0; _ls_i < _ls_insize; _ls_i++) { _ls_psize++; _SV(_ls_q,list); _ls_q = _NEXT(_ls_q,list,next); _RS(list); if (!_ls_q) break; } _ls_qsize = _ls_insize; while (_ls_psize > 0 || (_ls_qsize > 0 && _ls_q)) { if (_ls_psize == 0) { _ls_e = _ls_q; _SV(_ls_q,list); _ls_q = _NEXT(_ls_q,list,next); _RS(list); _ls_qsize--; } else if (_ls_qsize == 0 || !_ls_q) { _ls_e = _ls_p; _SV(_ls_p,list); _ls_p = _NEXT(_ls_p,list,next); _RS(list); _ls_psize--; } else if (cmp(_ls_p,_ls_q) <= 0) { _ls_e = _ls_p; _SV(_ls_p,list); _ls_p = _NEXT(_ls_p,list,next); _RS(list); _ls_psize--; } else { _ls_e = _ls_q; _SV(_ls_q,list); _ls_q = _NEXT(_ls_q,list,next); _RS(list); _ls_qsize--; } if (_ls_tail) { _SV(_ls_tail,list); _NEXTASGN(_ls_tail,list,_ls_e,next); _RS(list); } else { _CASTASGN(list,_ls_e); } _SV(_ls_e,list); _PREVASGN(_ls_e,list,_ls_tail,prev); _RS(list); _ls_tail = _ls_e; } _ls_p = _ls_q; } _CASTASGN(list->prev, _ls_tail); _SV(_ls_tail,list); _NEXTASGN(_ls_tail,list,NULL,next); _RS(list); if (_ls_nmerges <= 1) { _ls_looping=0; } _ls_insize *= 2; } } } while (0)


























































#define CDL_SORT(list, cmp) CDL_SORT2(list, cmp, prev, next)


#define CDL_SORT2(list, cmp, prev, next) do { LDECLTYPE(list) _ls_p; LDECLTYPE(list) _ls_q; LDECLTYPE(list) _ls_e; LDECLTYPE(list) _ls_tail; LDECLTYPE(list) _ls_oldhead; LDECLTYPE(list) _tmp; int _ls_insize, _ls_nmerges, _ls_psize, _ls_qsize, _ls_i, _ls_looping; if (list) { _ls_insize = 1; _ls_looping = 1; while (_ls_looping) { _CASTASGN(_ls_p,list); _CASTASGN(_ls_oldhead,list); list = NULL; _ls_tail = NULL; _ls_nmerges = 0; while (_ls_p) { _ls_nmerges++; _ls_q = _ls_p; _ls_psize = 0; for (_ls_i = 0; _ls_i < _ls_insize; _ls_i++) { _ls_psize++; _SV(_ls_q,list); if (_NEXT(_ls_q,list,next) == _ls_oldhead) { _ls_q = NULL; } else { _ls_q = _NEXT(_ls_q,list,next); } _RS(list); if (!_ls_q) break; } _ls_qsize = _ls_insize; while (_ls_psize > 0 || (_ls_qsize > 0 && _ls_q)) { if (_ls_psize == 0) { _ls_e = _ls_q; _SV(_ls_q,list); _ls_q = _NEXT(_ls_q,list,next); _RS(list); _ls_qsize--; if (_ls_q == _ls_oldhead) { _ls_q = NULL; } } else if (_ls_qsize == 0 || !_ls_q) { _ls_e = _ls_p; _SV(_ls_p,list); _ls_p = _NEXT(_ls_p,list,next); _RS(list); _ls_psize--; if (_ls_p == _ls_oldhead) { _ls_p = NULL; } } else if (cmp(_ls_p,_ls_q) <= 0) { _ls_e = _ls_p; _SV(_ls_p,list); _ls_p = _NEXT(_ls_p,list,next); _RS(list); _ls_psize--; if (_ls_p == _ls_oldhead) { _ls_p = NULL; } } else { _ls_e = _ls_q; _SV(_ls_q,list); _ls_q = _NEXT(_ls_q,list,next); _RS(list); _ls_qsize--; if (_ls_q == _ls_oldhead) { _ls_q = NULL; } } if (_ls_tail) { _SV(_ls_tail,list); _NEXTASGN(_ls_tail,list,_ls_e,next); _RS(list); } else { _CASTASGN(list,_ls_e); } _SV(_ls_e,list); _PREVASGN(_ls_e,list,_ls_tail,prev); _RS(list); _ls_tail = _ls_e; } _ls_p = _ls_q; } _CASTASGN(list->prev,_ls_tail); _CASTASGN(_tmp,list); _SV(_ls_tail,list); _NEXTASGN(_ls_tail,list,_tmp,next); _RS(list); if (_ls_nmerges <= 1) { _ls_looping=0; } _ls_insize *= 2; } } } while (0)











































































#define LL_PREPEND(head,add) LL_PREPEND2(head,add,next)


#define LL_PREPEND2(head,add,next) do { (add)->next = head; head = add; } while (0)





#define LL_CONCAT(head1,head2) LL_CONCAT2(head1,head2,next)


#define LL_CONCAT2(head1,head2,next) do { LDECLTYPE(head1) _tmp; if (head1) { _tmp = head1; while (_tmp->next) { _tmp = _tmp->next; } _tmp->next=(head2); } else { (head1)=(head2); } } while (0)











#define LL_APPEND(head,add) LL_APPEND2(head,add,next)


#define LL_APPEND2(head,add,next) do { LDECLTYPE(head) _tmp; (add)->next=NULL; if (head) { _tmp = head; while (_tmp->next) { _tmp = _tmp->next; } _tmp->next=(add); } else { (head)=(add); } } while (0)












#define LL_DELETE(head,del) LL_DELETE2(head,del,next)


#define LL_DELETE2(head,del,next) do { LDECLTYPE(head) _tmp; if ((head) == (del)) { (head)=(head)->next; } else { _tmp = head; while (_tmp->next && (_tmp->next != (del))) { _tmp = _tmp->next; } if (_tmp->next) { _tmp->next = ((del)->next); } } } while (0)
















#define LL_APPEND_VS2008(head,add) LL_APPEND2_VS2008(head,add,next)


#define LL_APPEND2_VS2008(head,add,next) do { if (head) { (add)->next = head; while ((add)->next->next) { (add)->next = (add)->next->next; } (add)->next->next=(add); } else { (head)=(add); } (add)->next=NULL; } while (0)











#define LL_DELETE_VS2008(head,del) LL_DELETE2_VS2008(head,del,next)


#define LL_DELETE2_VS2008(head,del,next) do { if ((head) == (del)) { (head)=(head)->next; } else { char *_tmp = (char*)(head); while ((head)->next && ((head)->next != (del))) { head = (head)->next; } if ((head)->next) { (head)->next = ((del)->next); } { char **_head_alias = (char**)&(head); *_head_alias = _tmp; } } } while (0)

















#if defined(NO_DECLTYPE)
#undef LL_APPEND
#define LL_APPEND LL_APPEND_VS2008
#undef LL_DELETE
#define LL_DELETE LL_DELETE_VS2008
#undef LL_DELETE2
#define LL_DELETE2 LL_DELETE2_VS2008
#undef LL_APPEND2
#define LL_APPEND2 LL_APPEND2_VS2008
#undef LL_CONCAT
#undef DL_CONCAT
#endif


#define LL_COUNT(head,el,counter) LL_COUNT2(head,el,counter,next)


#define LL_COUNT2(head,el,counter,next) { counter = 0; LL_FOREACH2(head,el,next){ ++counter; } }





#define LL_FOREACH(head,el) LL_FOREACH2(head,el,next)


#define LL_FOREACH2(head,el,next) for(el=head;el;el=(el)->next)


#define LL_FOREACH_SAFE(head,el,tmp) LL_FOREACH_SAFE2(head,el,tmp,next)


#define LL_FOREACH_SAFE2(head,el,tmp,next) for((el)=(head);(el) && (tmp = (el)->next, 1); (el) = tmp)


#define LL_SEARCH_SCALAR(head,out,field,val) LL_SEARCH_SCALAR2(head,out,field,val,next)


#define LL_SEARCH_SCALAR2(head,out,field,val,next) do { LL_FOREACH2(head,out,next) { if ((out)->field == (val)) break; } } while(0)






#define LL_SEARCH(head,out,elt,cmp) LL_SEARCH2(head,out,elt,cmp,next)


#define LL_SEARCH2(head,out,elt,cmp,next) do { LL_FOREACH2(head,out,next) { if ((cmp(out,elt))==0) break; } } while(0)






#define LL_REPLACE_ELEM(head, el, add) do { LDECLTYPE(head) _tmp; assert(head != NULL); assert(el != NULL); assert(add != NULL); (add)->next = (el)->next; if ((head) == (el)) { (head) = (add); } else { _tmp = head; while (_tmp->next && (_tmp->next != (el))) { _tmp = _tmp->next; } if (_tmp->next) { _tmp->next = (add); } } } while (0)



















#define LL_PREPEND_ELEM(head, el, add) do { LDECLTYPE(head) _tmp; assert(head != NULL); assert(el != NULL); assert(add != NULL); (add)->next = (el); if ((head) == (el)) { (head) = (add); } else { _tmp = head; while (_tmp->next && (_tmp->next != (el))) { _tmp = _tmp->next; } if (_tmp->next) { _tmp->next = (add); } } } while (0)























#define DL_PREPEND(head,add) DL_PREPEND2(head,add,prev,next)


#define DL_PREPEND2(head,add,prev,next) do { (add)->next = head; if (head) { (add)->prev = (head)->prev; (head)->prev = (add); } else { (add)->prev = (add); } (head) = (add); } while (0)











#define DL_APPEND(head,add) DL_APPEND2(head,add,prev,next)


#define DL_APPEND2(head,add,prev,next) do { if (head) { (add)->prev = (head)->prev; (head)->prev->next = (add); (head)->prev = (add); (add)->next = NULL; } else { (head)=(add); (head)->prev = (head); (head)->next = NULL; } } while (0)













#define DL_CONCAT(head1,head2) DL_CONCAT2(head1,head2,prev,next)


#define DL_CONCAT2(head1,head2,prev,next) do { LDECLTYPE(head1) _tmp; if (head2) { if (head1) { _tmp = (head2)->prev; (head2)->prev = (head1)->prev; (head1)->prev->next = (head2); (head1)->prev = _tmp; } else { (head1)=(head2); } } } while (0)














#define DL_DELETE(head,del) DL_DELETE2(head,del,prev,next)


#define DL_DELETE2(head,del,prev,next) do { assert((del)->prev != NULL); if ((del)->prev == (del)) { (head)=NULL; } else if ((del)==(head)) { (del)->next->prev = (del)->prev; (head) = (del)->next; } else { (del)->prev->next = (del)->next; if ((del)->next) { (del)->next->prev = (del)->prev; } else { (head)->prev = (del)->prev; } } } while (0)

















#define DL_COUNT(head,el,counter) DL_COUNT2(head,el,counter,next)


#define DL_COUNT2(head,el,counter,next) { counter = 0; DL_FOREACH2(head,el,next){ ++counter; } }





#define DL_FOREACH(head,el) DL_FOREACH2(head,el,next)


#define DL_FOREACH2(head,el,next) for(el=head;el;el=(el)->next)



#define DL_FOREACH_SAFE(head,el,tmp) DL_FOREACH_SAFE2(head,el,tmp,next)


#define DL_FOREACH_SAFE2(head,el,tmp,next) for((el)=(head);(el) && (tmp = (el)->next, 1); (el) = tmp)



#define DL_SEARCH_SCALAR LL_SEARCH_SCALAR
#define DL_SEARCH LL_SEARCH
#define DL_SEARCH_SCALAR2 LL_SEARCH_SCALAR2
#define DL_SEARCH2 LL_SEARCH2

#define DL_REPLACE_ELEM(head, el, add) do { assert(head != NULL); assert(el != NULL); assert(add != NULL); if ((head) == (el)) { (head) = (add); (add)->next = (el)->next; if ((el)->next == NULL) { (add)->prev = (add); } else { (add)->prev = (el)->prev; (add)->next->prev = (add); } } else { (add)->next = (el)->next; (add)->prev = (el)->prev; (add)->prev->next = (add); if ((el)->next == NULL) { (head)->prev = (add); } else { (add)->next->prev = (add); } } } while (0)

























#define DL_PREPEND_ELEM(head, el, add) do { assert(head != NULL); assert(el != NULL); assert(add != NULL); (add)->next = (el); (add)->prev = (el)->prev; (el)->prev = (add); if ((head) == (el)) { (head) = (add); } else { (add)->prev->next = (add); } } while (0)


















#define CDL_PREPEND(head,add) CDL_PREPEND2(head,add,prev,next)


#define CDL_PREPEND2(head,add,prev,next) do { if (head) { (add)->prev = (head)->prev; (add)->next = (head); (head)->prev = (add); (add)->prev->next = (add); } else { (add)->prev = (add); (add)->next = (add); } (head)=(add); } while (0)













#define CDL_DELETE(head,del) CDL_DELETE2(head,del,prev,next)


#define CDL_DELETE2(head,del,prev,next) do { if ( ((head)==(del)) && ((head)->next == (head))) { (head) = 0L; } else { (del)->next->prev = (del)->prev; (del)->prev->next = (del)->next; if ((del) == (head)) (head)=(del)->next; } } while (0)










#define CDL_COUNT(head,el,counter) CDL_COUNT2(head,el,counter,next)


#define CDL_COUNT2(head, el, counter,next) { counter = 0; CDL_FOREACH2(head,el,next){ ++counter; } }





#define CDL_FOREACH(head,el) CDL_FOREACH2(head,el,next)


#define CDL_FOREACH2(head,el,next) for(el=head;el;el=((el)->next==head ? 0L : (el)->next))


#define CDL_FOREACH_SAFE(head,el,tmp1,tmp2) CDL_FOREACH_SAFE2(head,el,tmp1,tmp2,prev,next)


#define CDL_FOREACH_SAFE2(head,el,tmp1,tmp2,prev,next) for((el)=(head), ((tmp1)=(head)?((head)->prev):NULL); (el) && ((tmp2)=(el)->next, 1); ((el) = (((el)==(tmp1)) ? 0L : (tmp2))))




#define CDL_SEARCH_SCALAR(head,out,field,val) CDL_SEARCH_SCALAR2(head,out,field,val,next)


#define CDL_SEARCH_SCALAR2(head,out,field,val,next) do { CDL_FOREACH2(head,out,next) { if ((out)->field == (val)) break; } } while(0)






#define CDL_SEARCH(head,out,elt,cmp) CDL_SEARCH2(head,out,elt,cmp,next)


#define CDL_SEARCH2(head,out,elt,cmp,next) do { CDL_FOREACH2(head,out,next) { if ((cmp(out,elt))==0) break; } } while(0)






#define CDL_REPLACE_ELEM(head, el, add) do { assert(head != NULL); assert(el != NULL); assert(add != NULL); if ((el)->next == (el)) { (add)->next = (add); (add)->prev = (add); (head) = (add); } else { (add)->next = (el)->next; (add)->prev = (el)->prev; (add)->next->prev = (add); (add)->prev->next = (add); if ((head) == (el)) { (head) = (add); } } } while (0)



















#define CDL_PREPEND_ELEM(head, el, add) do { assert(head != NULL); assert(el != NULL); assert(add != NULL); (add)->next = (el); (add)->prev = (el)->prev; (el)->prev = (add); (add)->prev->next = (add); if ((head) == (el)) { (head) = (add); } } while (0)













#endif

