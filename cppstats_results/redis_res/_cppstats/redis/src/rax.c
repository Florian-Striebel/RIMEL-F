#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "rax.h"
#if !defined(RAX_MALLOC_INCLUDE)
#define RAX_MALLOC_INCLUDE "rax_malloc.h"
#endif
#include RAX_MALLOC_INCLUDE
void *raxNotFound = (void*)"rax-not-found-pointer";
void raxDebugShowNode(const char *msg, raxNode *n);
#if defined(RAX_DEBUG_MSG)
#define debugf(...) if (raxDebugMsg) { printf("%s:%s:%d:\t", __FILE__, __func__, __LINE__); printf(__VA_ARGS__); fflush(stdout); }
#define debugnode(msg,n) raxDebugShowNode(msg,n)
#else
#define debugf(...)
#define debugnode(msg,n)
#endif
static int raxDebugMsg = 1;
void raxSetDebugMsg(int onoff) {
raxDebugMsg = onoff;
}
static inline void raxStackInit(raxStack *ts) {
ts->stack = ts->static_items;
ts->items = 0;
ts->maxitems = RAX_STACK_STATIC_ITEMS;
ts->oom = 0;
}
static inline int raxStackPush(raxStack *ts, void *ptr) {
if (ts->items == ts->maxitems) {
if (ts->stack == ts->static_items) {
ts->stack = rax_malloc(sizeof(void*)*ts->maxitems*2);
if (ts->stack == NULL) {
ts->stack = ts->static_items;
ts->oom = 1;
errno = ENOMEM;
return 0;
}
memcpy(ts->stack,ts->static_items,sizeof(void*)*ts->maxitems);
} else {
void **newalloc = rax_realloc(ts->stack,sizeof(void*)*ts->maxitems*2);
if (newalloc == NULL) {
ts->oom = 1;
errno = ENOMEM;
return 0;
}
ts->stack = newalloc;
}
ts->maxitems *= 2;
}
ts->stack[ts->items] = ptr;
ts->items++;
return 1;
}
static inline void *raxStackPop(raxStack *ts) {
if (ts->items == 0) return NULL;
ts->items--;
return ts->stack[ts->items];
}
static inline void *raxStackPeek(raxStack *ts) {
if (ts->items == 0) return NULL;
return ts->stack[ts->items-1];
}
static inline void raxStackFree(raxStack *ts) {
if (ts->stack != ts->static_items) rax_free(ts->stack);
}
#define raxPadding(nodesize) ((sizeof(void*)-((nodesize+4) % sizeof(void*))) & (sizeof(void*)-1))
#define raxNodeLastChildPtr(n) ((raxNode**) ( ((char*)(n)) + raxNodeCurrentLength(n) - sizeof(raxNode*) - (((n)->iskey && !(n)->isnull) ? sizeof(void*) : 0) ))
#define raxNodeFirstChildPtr(n) ((raxNode**) ( (n)->data + (n)->size + raxPadding((n)->size)))
#define raxNodeCurrentLength(n) ( sizeof(raxNode)+(n)->size+ raxPadding((n)->size)+ ((n)->iscompr ? sizeof(raxNode*) : sizeof(raxNode*)*(n)->size)+ (((n)->iskey && !(n)->isnull)*sizeof(void*)) )
raxNode *raxNewNode(size_t children, int datafield) {
size_t nodesize = sizeof(raxNode)+children+raxPadding(children)+
sizeof(raxNode*)*children;
if (datafield) nodesize += sizeof(void*);
raxNode *node = rax_malloc(nodesize);
if (node == NULL) return NULL;
node->iskey = 0;
node->isnull = 0;
node->iscompr = 0;
node->size = children;
return node;
}
rax *raxNew(void) {
rax *rax = rax_malloc(sizeof(*rax));
if (rax == NULL) return NULL;
rax->numele = 0;
rax->numnodes = 1;
rax->head = raxNewNode(0,0);
if (rax->head == NULL) {
rax_free(rax);
return NULL;
} else {
return rax;
}
}
raxNode *raxReallocForData(raxNode *n, void *data) {
if (data == NULL) return n;
size_t curlen = raxNodeCurrentLength(n);
return rax_realloc(n,curlen+sizeof(void*));
}
void raxSetData(raxNode *n, void *data) {
n->iskey = 1;
if (data != NULL) {
n->isnull = 0;
void **ndata = (void**)
((char*)n+raxNodeCurrentLength(n)-sizeof(void*));
memcpy(ndata,&data,sizeof(data));
} else {
n->isnull = 1;
}
}
void *raxGetData(raxNode *n) {
if (n->isnull) return NULL;
void **ndata =(void**)((char*)n+raxNodeCurrentLength(n)-sizeof(void*));
void *data;
memcpy(&data,ndata,sizeof(data));
return data;
}
raxNode *raxAddChild(raxNode *n, unsigned char c, raxNode **childptr, raxNode ***parentlink) {
assert(n->iscompr == 0);
size_t curlen = raxNodeCurrentLength(n);
n->size++;
size_t newlen = raxNodeCurrentLength(n);
n->size--;
raxNode *child = raxNewNode(0,0);
if (child == NULL) return NULL;
raxNode *newn = rax_realloc(n,newlen);
if (newn == NULL) {
rax_free(child);
return NULL;
}
n = newn;
int pos;
for (pos = 0; pos < n->size; pos++) {
if (n->data[pos] > c) break;
}
unsigned char *src, *dst;
if (n->iskey && !n->isnull) {
src = ((unsigned char*)n+curlen-sizeof(void*));
dst = ((unsigned char*)n+newlen-sizeof(void*));
memmove(dst,src,sizeof(void*));
}
size_t shift = newlen - curlen - sizeof(void*);
src = n->data+n->size+
raxPadding(n->size)+
sizeof(raxNode*)*pos;
memmove(src+shift+sizeof(raxNode*),src,sizeof(raxNode*)*(n->size-pos));
if (shift) {
src = (unsigned char*) raxNodeFirstChildPtr(n);
memmove(src+shift,src,sizeof(raxNode*)*pos);
}
src = n->data+pos;
memmove(src+1,src,n->size-pos);
n->data[pos] = c;
n->size++;
src = (unsigned char*) raxNodeFirstChildPtr(n);
raxNode **childfield = (raxNode**)(src+sizeof(raxNode*)*pos);
memcpy(childfield,&child,sizeof(child));
*childptr = child;
*parentlink = childfield;
return n;
}
raxNode *raxCompressNode(raxNode *n, unsigned char *s, size_t len, raxNode **child) {
assert(n->size == 0 && n->iscompr == 0);
void *data = NULL;
size_t newsize;
debugf("Compress node: %.*s\n", (int)len,s);
*child = raxNewNode(0,0);
if (*child == NULL) return NULL;
newsize = sizeof(raxNode)+len+raxPadding(len)+sizeof(raxNode*);
if (n->iskey) {
data = raxGetData(n);
if (!n->isnull) newsize += sizeof(void*);
}
raxNode *newn = rax_realloc(n,newsize);
if (newn == NULL) {
rax_free(*child);
return NULL;
}
n = newn;
n->iscompr = 1;
n->size = len;
memcpy(n->data,s,len);
if (n->iskey) raxSetData(n,data);
raxNode **childfield = raxNodeLastChildPtr(n);
memcpy(childfield,child,sizeof(*child));
return n;
}
static inline size_t raxLowWalk(rax *rax, unsigned char *s, size_t len, raxNode **stopnode, raxNode ***plink, int *splitpos, raxStack *ts) {
raxNode *h = rax->head;
raxNode **parentlink = &rax->head;
size_t i = 0;
size_t j = 0;
while(h->size && i < len) {
debugnode("Lookup current node",h);
unsigned char *v = h->data;
if (h->iscompr) {
for (j = 0; j < h->size && i < len; j++, i++) {
if (v[j] != s[i]) break;
}
if (j != h->size) break;
} else {
for (j = 0; j < h->size; j++) {
if (v[j] == s[i]) break;
}
if (j == h->size) break;
i++;
}
if (ts) raxStackPush(ts,h);
raxNode **children = raxNodeFirstChildPtr(h);
if (h->iscompr) j = 0;
memcpy(&h,children+j,sizeof(h));
parentlink = children+j;
j = 0;
}
debugnode("Lookup stop node is",h);
if (stopnode) *stopnode = h;
if (plink) *plink = parentlink;
if (splitpos && h->iscompr) *splitpos = j;
return i;
}
int raxGenericInsert(rax *rax, unsigned char *s, size_t len, void *data, void **old, int overwrite) {
size_t i;
int j = 0;
raxNode *h, **parentlink;
debugf("###Insert %.*s with value %p\n", (int)len, s, data);
i = raxLowWalk(rax,s,len,&h,&parentlink,&j,NULL);
if (i == len && (!h->iscompr || j == 0 )) {
debugf("###Insert: node representing key exists\n");
if (!h->iskey || (h->isnull && overwrite)) {
h = raxReallocForData(h,data);
if (h) memcpy(parentlink,&h,sizeof(h));
}
if (h == NULL) {
errno = ENOMEM;
return 0;
}
if (h->iskey) {
if (old) *old = raxGetData(h);
if (overwrite) raxSetData(h,data);
errno = 0;
return 0;
}
raxSetData(h,data);
rax->numele++;
return 1;
}
if (h->iscompr && i != len) {
debugf("ALGO 1: Stopped at compressed node %.*s (%p)\n",
h->size, h->data, (void*)h);
debugf("Still to insert: %.*s\n", (int)(len-i), s+i);
debugf("Splitting at %d: '%c'\n", j, ((char*)h->data)[j]);
debugf("Other (key) letter is '%c'\n", s[i]);
raxNode **childfield = raxNodeLastChildPtr(h);
raxNode *next;
memcpy(&next,childfield,sizeof(next));
debugf("Next is %p\n", (void*)next);
debugf("iskey %d\n", h->iskey);
if (h->iskey) {
debugf("key value is %p\n", raxGetData(h));
}
size_t trimmedlen = j;
size_t postfixlen = h->size - j - 1;
int split_node_is_key = !trimmedlen && h->iskey && !h->isnull;
size_t nodesize;
raxNode *splitnode = raxNewNode(1, split_node_is_key);
raxNode *trimmed = NULL;
raxNode *postfix = NULL;
if (trimmedlen) {
nodesize = sizeof(raxNode)+trimmedlen+raxPadding(trimmedlen)+
sizeof(raxNode*);
if (h->iskey && !h->isnull) nodesize += sizeof(void*);
trimmed = rax_malloc(nodesize);
}
if (postfixlen) {
nodesize = sizeof(raxNode)+postfixlen+raxPadding(postfixlen)+
sizeof(raxNode*);
postfix = rax_malloc(nodesize);
}
if (splitnode == NULL ||
(trimmedlen && trimmed == NULL) ||
(postfixlen && postfix == NULL))
{
rax_free(splitnode);
rax_free(trimmed);
rax_free(postfix);
errno = ENOMEM;
return 0;
}
splitnode->data[0] = h->data[j];
if (j == 0) {
if (h->iskey) {
void *ndata = raxGetData(h);
raxSetData(splitnode,ndata);
}
memcpy(parentlink,&splitnode,sizeof(splitnode));
} else {
trimmed->size = j;
memcpy(trimmed->data,h->data,j);
trimmed->iscompr = j > 1 ? 1 : 0;
trimmed->iskey = h->iskey;
trimmed->isnull = h->isnull;
if (h->iskey && !h->isnull) {
void *ndata = raxGetData(h);
raxSetData(trimmed,ndata);
}
raxNode **cp = raxNodeLastChildPtr(trimmed);
memcpy(cp,&splitnode,sizeof(splitnode));
memcpy(parentlink,&trimmed,sizeof(trimmed));
parentlink = cp;
rax->numnodes++;
}
if (postfixlen) {
postfix->iskey = 0;
postfix->isnull = 0;
postfix->size = postfixlen;
postfix->iscompr = postfixlen > 1;
memcpy(postfix->data,h->data+j+1,postfixlen);
raxNode **cp = raxNodeLastChildPtr(postfix);
memcpy(cp,&next,sizeof(next));
rax->numnodes++;
} else {
postfix = next;
}
raxNode **splitchild = raxNodeLastChildPtr(splitnode);
memcpy(splitchild,&postfix,sizeof(postfix));
rax_free(h);
h = splitnode;
} else if (h->iscompr && i == len) {
debugf("ALGO 2: Stopped at compressed node %.*s (%p) j = %d\n",
h->size, h->data, (void*)h, j);
size_t postfixlen = h->size - j;
size_t nodesize = sizeof(raxNode)+postfixlen+raxPadding(postfixlen)+
sizeof(raxNode*);
if (data != NULL) nodesize += sizeof(void*);
raxNode *postfix = rax_malloc(nodesize);
nodesize = sizeof(raxNode)+j+raxPadding(j)+sizeof(raxNode*);
if (h->iskey && !h->isnull) nodesize += sizeof(void*);
raxNode *trimmed = rax_malloc(nodesize);
if (postfix == NULL || trimmed == NULL) {
rax_free(postfix);
rax_free(trimmed);
errno = ENOMEM;
return 0;
}
raxNode **childfield = raxNodeLastChildPtr(h);
raxNode *next;
memcpy(&next,childfield,sizeof(next));
postfix->size = postfixlen;
postfix->iscompr = postfixlen > 1;
postfix->iskey = 1;
postfix->isnull = 0;
memcpy(postfix->data,h->data+j,postfixlen);
raxSetData(postfix,data);
raxNode **cp = raxNodeLastChildPtr(postfix);
memcpy(cp,&next,sizeof(next));
rax->numnodes++;
trimmed->size = j;
trimmed->iscompr = j > 1;
trimmed->iskey = 0;
trimmed->isnull = 0;
memcpy(trimmed->data,h->data,j);
memcpy(parentlink,&trimmed,sizeof(trimmed));
if (h->iskey) {
void *aux = raxGetData(h);
raxSetData(trimmed,aux);
}
cp = raxNodeLastChildPtr(trimmed);
memcpy(cp,&postfix,sizeof(postfix));
rax->numele++;
rax_free(h);
return 1;
}
while(i < len) {
raxNode *child;
if (h->size == 0 && len-i > 1) {
debugf("Inserting compressed node\n");
size_t comprsize = len-i;
if (comprsize > RAX_NODE_MAX_SIZE)
comprsize = RAX_NODE_MAX_SIZE;
raxNode *newh = raxCompressNode(h,s+i,comprsize,&child);
if (newh == NULL) goto oom;
h = newh;
memcpy(parentlink,&h,sizeof(h));
parentlink = raxNodeLastChildPtr(h);
i += comprsize;
} else {
debugf("Inserting normal node\n");
raxNode **new_parentlink;
raxNode *newh = raxAddChild(h,s[i],&child,&new_parentlink);
if (newh == NULL) goto oom;
h = newh;
memcpy(parentlink,&h,sizeof(h));
parentlink = new_parentlink;
i++;
}
rax->numnodes++;
h = child;
}
raxNode *newh = raxReallocForData(h,data);
if (newh == NULL) goto oom;
h = newh;
if (!h->iskey) rax->numele++;
raxSetData(h,data);
memcpy(parentlink,&h,sizeof(h));
return 1;
oom:
if (h->size == 0) {
h->isnull = 1;
h->iskey = 1;
rax->numele++;
assert(raxRemove(rax,s,i,NULL) != 0);
}
errno = ENOMEM;
return 0;
}
int raxInsert(rax *rax, unsigned char *s, size_t len, void *data, void **old) {
return raxGenericInsert(rax,s,len,data,old,1);
}
int raxTryInsert(rax *rax, unsigned char *s, size_t len, void *data, void **old) {
return raxGenericInsert(rax,s,len,data,old,0);
}
void *raxFind(rax *rax, unsigned char *s, size_t len) {
raxNode *h;
debugf("###Lookup: %.*s\n", (int)len, s);
int splitpos = 0;
size_t i = raxLowWalk(rax,s,len,&h,NULL,&splitpos,NULL);
if (i != len || (h->iscompr && splitpos != 0) || !h->iskey)
return raxNotFound;
return raxGetData(h);
}
raxNode **raxFindParentLink(raxNode *parent, raxNode *child) {
raxNode **cp = raxNodeFirstChildPtr(parent);
raxNode *c;
while(1) {
memcpy(&c,cp,sizeof(c));
if (c == child) break;
cp++;
}
return cp;
}
raxNode *raxRemoveChild(raxNode *parent, raxNode *child) {
debugnode("raxRemoveChild before", parent);
if (parent->iscompr) {
void *data = NULL;
if (parent->iskey) data = raxGetData(parent);
parent->isnull = 0;
parent->iscompr = 0;
parent->size = 0;
if (parent->iskey) raxSetData(parent,data);
debugnode("raxRemoveChild after", parent);
return parent;
}
raxNode **cp = raxNodeFirstChildPtr(parent);
raxNode **c = cp;
unsigned char *e = parent->data;
while(1) {
raxNode *aux;
memcpy(&aux,c,sizeof(aux));
if (aux == child) break;
c++;
e++;
}
int taillen = parent->size - (e - parent->data) - 1;
debugf("raxRemoveChild tail len: %d\n", taillen);
memmove(e,e+1,taillen);
size_t shift = ((parent->size+4) % sizeof(void*)) == 1 ? sizeof(void*) : 0;
if (shift)
memmove(((char*)cp)-shift,cp,(parent->size-taillen-1)*sizeof(raxNode**));
size_t valuelen = (parent->iskey && !parent->isnull) ? sizeof(void*) : 0;
memmove(((char*)c)-shift,c+1,taillen*sizeof(raxNode**)+valuelen);
parent->size--;
raxNode *newnode = rax_realloc(parent,raxNodeCurrentLength(parent));
if (newnode) {
debugnode("raxRemoveChild after", newnode);
}
return newnode ? newnode : parent;
}
int raxRemove(rax *rax, unsigned char *s, size_t len, void **old) {
raxNode *h;
raxStack ts;
debugf("###Delete: %.*s\n", (int)len, s);
raxStackInit(&ts);
int splitpos = 0;
size_t i = raxLowWalk(rax,s,len,&h,NULL,&splitpos,&ts);
if (i != len || (h->iscompr && splitpos != 0) || !h->iskey) {
raxStackFree(&ts);
return 0;
}
if (old) *old = raxGetData(h);
h->iskey = 0;
rax->numele--;
int trycompress = 0;
if (h->size == 0) {
debugf("Key deleted in node without children. Cleanup needed.\n");
raxNode *child = NULL;
while(h != rax->head) {
child = h;
debugf("Freeing child %p [%.*s] key:%d\n", (void*)child,
(int)child->size, (char*)child->data, child->iskey);
rax_free(child);
rax->numnodes--;
h = raxStackPop(&ts);
if (h->iskey || (!h->iscompr && h->size != 1)) break;
}
if (child) {
debugf("Unlinking child %p from parent %p\n",
(void*)child, (void*)h);
raxNode *new = raxRemoveChild(h,child);
if (new != h) {
raxNode *parent = raxStackPeek(&ts);
raxNode **parentlink;
if (parent == NULL) {
parentlink = &rax->head;
} else {
parentlink = raxFindParentLink(parent,h);
}
memcpy(parentlink,&new,sizeof(new));
}
if (new->size == 1 && new->iskey == 0) {
trycompress = 1;
h = new;
}
}
} else if (h->size == 1) {
trycompress = 1;
}
if (trycompress && ts.oom) trycompress = 0;
if (trycompress) {
debugf("After removing %.*s:\n", (int)len, s);
debugnode("Compression may be needed",h);
debugf("Seek start node\n");
raxNode *parent;
while(1) {
parent = raxStackPop(&ts);
if (!parent || parent->iskey ||
(!parent->iscompr && parent->size != 1)) break;
h = parent;
debugnode("Going up to",h);
}
raxNode *start = h;
size_t comprsize = h->size;
int nodes = 1;
while(h->size != 0) {
raxNode **cp = raxNodeLastChildPtr(h);
memcpy(&h,cp,sizeof(h));
if (h->iskey || (!h->iscompr && h->size != 1)) break;
if (comprsize + h->size > RAX_NODE_MAX_SIZE) break;
nodes++;
comprsize += h->size;
}
if (nodes > 1) {
size_t nodesize =
sizeof(raxNode)+comprsize+raxPadding(comprsize)+sizeof(raxNode*);
raxNode *new = rax_malloc(nodesize);
if (new == NULL) {
raxStackFree(&ts);
return 1;
}
new->iskey = 0;
new->isnull = 0;
new->iscompr = 1;
new->size = comprsize;
rax->numnodes++;
comprsize = 0;
h = start;
while(h->size != 0) {
memcpy(new->data+comprsize,h->data,h->size);
comprsize += h->size;
raxNode **cp = raxNodeLastChildPtr(h);
raxNode *tofree = h;
memcpy(&h,cp,sizeof(h));
rax_free(tofree); rax->numnodes--;
if (h->iskey || (!h->iscompr && h->size != 1)) break;
}
debugnode("New node",new);
raxNode **cp = raxNodeLastChildPtr(new);
memcpy(cp,&h,sizeof(h));
if (parent) {
raxNode **parentlink = raxFindParentLink(parent,start);
memcpy(parentlink,&new,sizeof(new));
} else {
rax->head = new;
}
debugf("Compressed %d nodes, %d total bytes\n",
nodes, (int)comprsize);
}
}
raxStackFree(&ts);
return 1;
}
void raxRecursiveFree(rax *rax, raxNode *n, void (*free_callback)(void*)) {
debugnode("free traversing",n);
int numchildren = n->iscompr ? 1 : n->size;
raxNode **cp = raxNodeLastChildPtr(n);
while(numchildren--) {
raxNode *child;
memcpy(&child,cp,sizeof(child));
raxRecursiveFree(rax,child,free_callback);
cp--;
}
debugnode("free depth-first",n);
if (free_callback && n->iskey && !n->isnull)
free_callback(raxGetData(n));
rax_free(n);
rax->numnodes--;
}
void raxFreeWithCallback(rax *rax, void (*free_callback)(void*)) {
raxRecursiveFree(rax,rax->head,free_callback);
assert(rax->numnodes == 0);
rax_free(rax);
}
void raxFree(rax *rax) {
raxFreeWithCallback(rax,NULL);
}
void raxStart(raxIterator *it, rax *rt) {
it->flags = RAX_ITER_EOF;
it->rt = rt;
it->key_len = 0;
it->key = it->key_static_string;
it->key_max = RAX_ITER_STATIC_LEN;
it->data = NULL;
it->node_cb = NULL;
raxStackInit(&it->stack);
}
int raxIteratorAddChars(raxIterator *it, unsigned char *s, size_t len) {
if (len == 0) return 1;
if (it->key_max < it->key_len+len) {
unsigned char *old = (it->key == it->key_static_string) ? NULL :
it->key;
size_t new_max = (it->key_len+len)*2;
it->key = rax_realloc(old,new_max);
if (it->key == NULL) {
it->key = (!old) ? it->key_static_string : old;
errno = ENOMEM;
return 0;
}
if (old == NULL) memcpy(it->key,it->key_static_string,it->key_len);
it->key_max = new_max;
}
memmove(it->key+it->key_len,s,len);
it->key_len += len;
return 1;
}
void raxIteratorDelChars(raxIterator *it, size_t count) {
it->key_len -= count;
}
int raxIteratorNextStep(raxIterator *it, int noup) {
if (it->flags & RAX_ITER_EOF) {
return 1;
} else if (it->flags & RAX_ITER_JUST_SEEKED) {
it->flags &= ~RAX_ITER_JUST_SEEKED;
return 1;
}
size_t orig_key_len = it->key_len;
size_t orig_stack_items = it->stack.items;
raxNode *orig_node = it->node;
while(1) {
int children = it->node->iscompr ? 1 : it->node->size;
if (!noup && children) {
debugf("GO DEEPER\n");
if (!raxStackPush(&it->stack,it->node)) return 0;
raxNode **cp = raxNodeFirstChildPtr(it->node);
if (!raxIteratorAddChars(it,it->node->data,
it->node->iscompr ? it->node->size : 1)) return 0;
memcpy(&it->node,cp,sizeof(it->node));
if (it->node_cb && it->node_cb(&it->node))
memcpy(cp,&it->node,sizeof(it->node));
if (it->node->iskey) {
it->data = raxGetData(it->node);
return 1;
}
} else {
while(1) {
int old_noup = noup;
if (!noup && it->node == it->rt->head) {
it->flags |= RAX_ITER_EOF;
it->stack.items = orig_stack_items;
it->key_len = orig_key_len;
it->node = orig_node;
return 1;
}
unsigned char prevchild = it->key[it->key_len-1];
if (!noup) {
it->node = raxStackPop(&it->stack);
} else {
noup = 0;
}
int todel = it->node->iscompr ? it->node->size : 1;
raxIteratorDelChars(it,todel);
if (!it->node->iscompr && it->node->size > (old_noup ? 0 : 1)) {
raxNode **cp = raxNodeFirstChildPtr(it->node);
int i = 0;
while (i < it->node->size) {
debugf("SCAN NEXT %c\n", it->node->data[i]);
if (it->node->data[i] > prevchild) break;
i++;
cp++;
}
if (i != it->node->size) {
debugf("SCAN found a new node\n");
raxIteratorAddChars(it,it->node->data+i,1);
if (!raxStackPush(&it->stack,it->node)) return 0;
memcpy(&it->node,cp,sizeof(it->node));
if (it->node_cb && it->node_cb(&it->node))
memcpy(cp,&it->node,sizeof(it->node));
if (it->node->iskey) {
it->data = raxGetData(it->node);
return 1;
}
break;
}
}
}
}
}
}
int raxSeekGreatest(raxIterator *it) {
while(it->node->size) {
if (it->node->iscompr) {
if (!raxIteratorAddChars(it,it->node->data,
it->node->size)) return 0;
} else {
if (!raxIteratorAddChars(it,it->node->data+it->node->size-1,1))
return 0;
}
raxNode **cp = raxNodeLastChildPtr(it->node);
if (!raxStackPush(&it->stack,it->node)) return 0;
memcpy(&it->node,cp,sizeof(it->node));
}
return 1;
}
int raxIteratorPrevStep(raxIterator *it, int noup) {
if (it->flags & RAX_ITER_EOF) {
return 1;
} else if (it->flags & RAX_ITER_JUST_SEEKED) {
it->flags &= ~RAX_ITER_JUST_SEEKED;
return 1;
}
size_t orig_key_len = it->key_len;
size_t orig_stack_items = it->stack.items;
raxNode *orig_node = it->node;
while(1) {
int old_noup = noup;
if (!noup && it->node == it->rt->head) {
it->flags |= RAX_ITER_EOF;
it->stack.items = orig_stack_items;
it->key_len = orig_key_len;
it->node = orig_node;
return 1;
}
unsigned char prevchild = it->key[it->key_len-1];
if (!noup) {
it->node = raxStackPop(&it->stack);
} else {
noup = 0;
}
int todel = it->node->iscompr ? it->node->size : 1;
raxIteratorDelChars(it,todel);
if (!it->node->iscompr && it->node->size > (old_noup ? 0 : 1)) {
raxNode **cp = raxNodeLastChildPtr(it->node);
int i = it->node->size-1;
while (i >= 0) {
debugf("SCAN PREV %c\n", it->node->data[i]);
if (it->node->data[i] < prevchild) break;
i--;
cp--;
}
if (i != -1) {
debugf("SCAN found a new node\n");
if (!raxIteratorAddChars(it,it->node->data+i,1)) return 0;
if (!raxStackPush(&it->stack,it->node)) return 0;
memcpy(&it->node,cp,sizeof(it->node));
if (!raxSeekGreatest(it)) return 0;
}
}
if (it->node->iskey) {
it->data = raxGetData(it->node);
return 1;
}
}
}
int raxSeek(raxIterator *it, const char *op, unsigned char *ele, size_t len) {
int eq = 0, lt = 0, gt = 0, first = 0, last = 0;
it->stack.items = 0;
it->flags |= RAX_ITER_JUST_SEEKED;
it->flags &= ~RAX_ITER_EOF;
it->key_len = 0;
it->node = NULL;
if (op[0] == '>') {
gt = 1;
if (op[1] == '=') eq = 1;
} else if (op[0] == '<') {
lt = 1;
if (op[1] == '=') eq = 1;
} else if (op[0] == '=') {
eq = 1;
} else if (op[0] == '^') {
first = 1;
} else if (op[0] == '$') {
last = 1;
} else {
errno = 0;
return 0;
}
if (it->rt->numele == 0) {
it->flags |= RAX_ITER_EOF;
return 1;
}
if (first) {
return raxSeek(it,">=",NULL,0);
}
if (last) {
it->node = it->rt->head;
if (!raxSeekGreatest(it)) return 0;
assert(it->node->iskey);
it->data = raxGetData(it->node);
return 1;
}
int splitpos = 0;
size_t i = raxLowWalk(it->rt,ele,len,&it->node,NULL,&splitpos,&it->stack);
if (it->stack.oom) return 0;
if (eq && i == len && (!it->node->iscompr || splitpos == 0) &&
it->node->iskey)
{
if (!raxIteratorAddChars(it,ele,len)) return 0;
it->data = raxGetData(it->node);
} else if (lt || gt) {
raxIteratorAddChars(it, ele, i-splitpos);
debugf("After initial seek: i=%d len=%d key=%.*s\n",
(int)i, (int)len, (int)it->key_len, it->key);
if (i != len && !it->node->iscompr) {
if (!raxIteratorAddChars(it,ele+i,1)) return 0;
debugf("Seek normal node on mismatch: %.*s\n",
(int)it->key_len, (char*)it->key);
it->flags &= ~RAX_ITER_JUST_SEEKED;
if (lt && !raxIteratorPrevStep(it,1)) return 0;
if (gt && !raxIteratorNextStep(it,1)) return 0;
it->flags |= RAX_ITER_JUST_SEEKED;
} else if (i != len && it->node->iscompr) {
debugf("Compressed mismatch: %.*s\n",
(int)it->key_len, (char*)it->key);
int nodechar = it->node->data[splitpos];
int keychar = ele[i];
it->flags &= ~RAX_ITER_JUST_SEEKED;
if (gt) {
if (nodechar > keychar) {
if (!raxIteratorNextStep(it,0)) return 0;
} else {
if (!raxIteratorAddChars(it,it->node->data,it->node->size))
return 0;
if (!raxIteratorNextStep(it,1)) return 0;
}
}
if (lt) {
if (nodechar < keychar) {
if (!raxSeekGreatest(it)) return 0;
it->data = raxGetData(it->node);
} else {
if (!raxIteratorAddChars(it,it->node->data,it->node->size))
return 0;
if (!raxIteratorPrevStep(it,1)) return 0;
}
}
it->flags |= RAX_ITER_JUST_SEEKED;
} else {
debugf("No mismatch: %.*s\n",
(int)it->key_len, (char*)it->key);
it->flags &= ~RAX_ITER_JUST_SEEKED;
if (it->node->iscompr && it->node->iskey && splitpos && lt) {
it->data = raxGetData(it->node);
} else {
if (gt && !raxIteratorNextStep(it,0)) return 0;
if (lt && !raxIteratorPrevStep(it,0)) return 0;
}
it->flags |= RAX_ITER_JUST_SEEKED;
}
} else {
it->flags |= RAX_ITER_EOF;
return 1;
}
return 1;
}
int raxNext(raxIterator *it) {
if (!raxIteratorNextStep(it,0)) {
errno = ENOMEM;
return 0;
}
if (it->flags & RAX_ITER_EOF) {
errno = 0;
return 0;
}
return 1;
}
int raxPrev(raxIterator *it) {
if (!raxIteratorPrevStep(it,0)) {
errno = ENOMEM;
return 0;
}
if (it->flags & RAX_ITER_EOF) {
errno = 0;
return 0;
}
return 1;
}
int raxRandomWalk(raxIterator *it, size_t steps) {
if (it->rt->numele == 0) {
it->flags |= RAX_ITER_EOF;
return 0;
}
if (steps == 0) {
size_t fle = 1+floor(log(it->rt->numele));
fle *= 2;
steps = 1 + rand() % fle;
}
raxNode *n = it->node;
while(steps > 0 || !n->iskey) {
int numchildren = n->iscompr ? 1 : n->size;
int r = rand() % (numchildren+(n != it->rt->head));
if (r == numchildren) {
n = raxStackPop(&it->stack);
int todel = n->iscompr ? n->size : 1;
raxIteratorDelChars(it,todel);
} else {
if (n->iscompr) {
if (!raxIteratorAddChars(it,n->data,n->size)) return 0;
} else {
if (!raxIteratorAddChars(it,n->data+r,1)) return 0;
}
raxNode **cp = raxNodeFirstChildPtr(n)+r;
if (!raxStackPush(&it->stack,n)) return 0;
memcpy(&n,cp,sizeof(n));
}
if (n->iskey) steps--;
}
it->node = n;
it->data = raxGetData(it->node);
return 1;
}
int raxCompare(raxIterator *iter, const char *op, unsigned char *key, size_t key_len) {
int eq = 0, lt = 0, gt = 0;
if (op[0] == '=' || op[1] == '=') eq = 1;
if (op[0] == '>') gt = 1;
else if (op[0] == '<') lt = 1;
else if (op[1] != '=') return 0;
size_t minlen = key_len < iter->key_len ? key_len : iter->key_len;
int cmp = memcmp(iter->key,key,minlen);
if (lt == 0 && gt == 0) return cmp == 0 && key_len == iter->key_len;
if (cmp == 0) {
if (eq && key_len == iter->key_len) return 1;
else if (lt) return iter->key_len < key_len;
else if (gt) return iter->key_len > key_len;
else return 0;
} else if (cmp > 0) {
return gt ? 1 : 0;
} else {
return lt ? 1 : 0;
}
}
void raxStop(raxIterator *it) {
if (it->key != it->key_static_string) rax_free(it->key);
raxStackFree(&it->stack);
}
int raxEOF(raxIterator *it) {
return it->flags & RAX_ITER_EOF;
}
uint64_t raxSize(rax *rax) {
return rax->numele;
}
void raxRecursiveShow(int level, int lpad, raxNode *n) {
char s = n->iscompr ? '"' : '[';
char e = n->iscompr ? '"' : ']';
int numchars = printf("%c%.*s%c", s, n->size, n->data, e);
if (n->iskey) {
numchars += printf("=%p",raxGetData(n));
}
int numchildren = n->iscompr ? 1 : n->size;
if (level) {
lpad += (numchildren > 1) ? 7 : 4;
if (numchildren == 1) lpad += numchars;
}
raxNode **cp = raxNodeFirstChildPtr(n);
for (int i = 0; i < numchildren; i++) {
char *branch = " `-(%c) ";
if (numchildren > 1) {
printf("\n");
for (int j = 0; j < lpad; j++) putchar(' ');
printf(branch,n->data[i]);
} else {
printf(" -> ");
}
raxNode *child;
memcpy(&child,cp,sizeof(child));
raxRecursiveShow(level+1,lpad,child);
cp++;
}
}
void raxShow(rax *rax) {
raxRecursiveShow(0,0,rax->head);
putchar('\n');
}
void raxDebugShowNode(const char *msg, raxNode *n) {
if (raxDebugMsg == 0) return;
printf("%s: %p [%.*s] key:%u size:%u children:",
msg, (void*)n, (int)n->size, (char*)n->data, n->iskey, n->size);
int numcld = n->iscompr ? 1 : n->size;
raxNode **cldptr = raxNodeLastChildPtr(n) - (numcld-1);
while(numcld--) {
raxNode *child;
memcpy(&child,cldptr,sizeof(child));
cldptr++;
printf("%p ", (void*)child);
}
printf("\n");
fflush(stdout);
}
unsigned long raxTouch(raxNode *n) {
debugf("Touching %p\n", (void*)n);
unsigned long sum = 0;
if (n->iskey) {
sum += (unsigned long)raxGetData(n);
}
int numchildren = n->iscompr ? 1 : n->size;
raxNode **cp = raxNodeFirstChildPtr(n);
int count = 0;
for (int i = 0; i < numchildren; i++) {
if (numchildren > 1) {
sum += (long)n->data[i];
}
raxNode *child;
memcpy(&child,cp,sizeof(child));
if (child == (void*)0x65d1760) count++;
if (count > 1) exit(1);
sum += raxTouch(child);
cp++;
}
return sum;
}
