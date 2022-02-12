










































#include <ldns/config.h>
#include <ldns/radix.h>
#include <ldns/util.h>
#include <stdlib.h>


static ldns_radix_node_t* ldns_radix_new_node(void* data, uint8_t* key,
radix_strlen_t len);
static int ldns_radix_find_prefix(ldns_radix_t* tree, uint8_t* key,
radix_strlen_t len, ldns_radix_node_t** result, radix_strlen_t* pos);
static int ldns_radix_array_space(ldns_radix_node_t* node, uint8_t byte);
static int ldns_radix_array_grow(ldns_radix_node_t* node, unsigned need);
static int ldns_radix_str_create(ldns_radix_array_t* array, uint8_t* key,
radix_strlen_t pos, radix_strlen_t len);
static int ldns_radix_prefix_remainder(radix_strlen_t prefix_len,
uint8_t* longer_str, radix_strlen_t longer_len, uint8_t** split_str,
radix_strlen_t* split_len);
static int ldns_radix_array_split(ldns_radix_array_t* array, uint8_t* key,
radix_strlen_t pos, radix_strlen_t len, ldns_radix_node_t* add);
static int ldns_radix_str_is_prefix(uint8_t* str1, radix_strlen_t len1,
uint8_t* str2, radix_strlen_t len2);
static radix_strlen_t ldns_radix_str_common(uint8_t* str1, radix_strlen_t len1,
uint8_t* str2, radix_strlen_t len2);
static ldns_radix_node_t* ldns_radix_next_in_subtree(ldns_radix_node_t* node);
static ldns_radix_node_t* ldns_radix_prev_from_index(ldns_radix_node_t* node,
uint8_t index);
static ldns_radix_node_t* ldns_radix_last_in_subtree_incl_self(
ldns_radix_node_t* node);
static ldns_radix_node_t* ldns_radix_last_in_subtree(ldns_radix_node_t* node);
static void ldns_radix_del_fix(ldns_radix_t* tree, ldns_radix_node_t* node);
static void ldns_radix_cleanup_onechild(ldns_radix_node_t* node);
static void ldns_radix_cleanup_leaf(ldns_radix_node_t* node);
static void ldns_radix_node_free(ldns_radix_node_t* node, void* arg);
static void ldns_radix_node_array_free(ldns_radix_node_t* node);
static void ldns_radix_node_array_free_front(ldns_radix_node_t* node);
static void ldns_radix_node_array_free_end(ldns_radix_node_t* node);
static void ldns_radix_array_reduce(ldns_radix_node_t* node);
static void ldns_radix_self_or_prev(ldns_radix_node_t* node,
ldns_radix_node_t** result);






static ldns_radix_node_t*
ldns_radix_new_node(void* data, uint8_t* key, radix_strlen_t len)
{
ldns_radix_node_t* node = LDNS_MALLOC(ldns_radix_node_t);
if (!node) {
return NULL;
}
node->data = data;
node->key = key;
node->klen = len;
node->parent = NULL;
node->parent_index = 0;
node->len = 0;
node->offset = 0;
node->capacity = 0;
node->array = NULL;
return node;
}






ldns_radix_t *
ldns_radix_create(void)
{
ldns_radix_t* tree;


tree = (ldns_radix_t *) LDNS_MALLOC(ldns_radix_t);
if (!tree) {
return NULL;
}

ldns_radix_init(tree);
return tree;
}






void
ldns_radix_init(ldns_radix_t* tree)
{

if (tree) {
tree->root = NULL;
tree->count = 0;
}
return;
}






void
ldns_radix_free(ldns_radix_t* tree)
{
if (tree) {
if (tree->root) {
ldns_radix_traverse_postorder(tree->root,
ldns_radix_node_free, NULL);
}
LDNS_FREE(tree);
}
return;
}






ldns_status
ldns_radix_insert(ldns_radix_t* tree, uint8_t* key, radix_strlen_t len,
void* data)
{
radix_strlen_t pos = 0;
ldns_radix_node_t* add = NULL;
ldns_radix_node_t* prefix = NULL;

if (!tree || !key || !data) {
return LDNS_STATUS_NULL;
}
add = ldns_radix_new_node(data, key, len);
if (!add) {
return LDNS_STATUS_MEM_ERR;
}

if (!ldns_radix_find_prefix(tree, key, len, &prefix, &pos)) {

assert(tree->root == NULL);
if (len == 0) {




tree->root = add;
} else {




prefix = ldns_radix_new_node(NULL, (uint8_t*)"", 0);
if (!prefix) {
LDNS_FREE(add);
return LDNS_STATUS_MEM_ERR;
}

if (!ldns_radix_array_space(prefix, key[0])) {
LDNS_FREE(add);
LDNS_FREE(prefix->array);
LDNS_FREE(prefix);
return LDNS_STATUS_MEM_ERR;
}

add->parent = prefix;
add->parent_index = 0;
prefix->array[0].edge = add;
if (len > 1) {

if (!ldns_radix_prefix_remainder(1, key,
len, &prefix->array[0].str,
&prefix->array[0].len)) {
LDNS_FREE(add);
LDNS_FREE(prefix->array);
LDNS_FREE(prefix);
return LDNS_STATUS_MEM_ERR;
}
}
tree->root = prefix;
}
} else if (pos == len) {

if (prefix->data) {

LDNS_FREE(add);
return LDNS_STATUS_EXISTS_ERR;
}
prefix->data = data;
prefix->key = key;
prefix->klen = len;
} else {

uint8_t byte = key[pos];
assert(pos < len);
if (byte < prefix->offset ||
(byte - prefix->offset) >= prefix->len) {







if (!ldns_radix_array_space(prefix, byte)) {
LDNS_FREE(add);
return LDNS_STATUS_MEM_ERR;
}
assert(byte >= prefix->offset);
assert((byte - prefix->offset) <= prefix->len);
byte -= prefix->offset;
if (pos+1 < len) {

if (!ldns_radix_str_create(
&prefix->array[byte], key, pos+1,
len)) {
LDNS_FREE(add);
return LDNS_STATUS_MEM_ERR;
}
}

add->parent = prefix;
add->parent_index = byte;
prefix->array[byte].edge = add;
} else if (prefix->array[byte-prefix->offset].edge == NULL) {








byte -= prefix->offset;
if (pos+1 < len) {

if (!ldns_radix_str_create(
&prefix->array[byte], key, pos+1,
len)) {
LDNS_FREE(add);
return LDNS_STATUS_MEM_ERR;
}
}

add->parent = prefix;
add->parent_index = byte;
prefix->array[byte].edge = add;
} else {




if (!ldns_radix_array_split(&prefix->array[byte-(prefix->offset)],
key, pos+1, len, add)) {
LDNS_FREE(add);
return LDNS_STATUS_MEM_ERR;
}
}
}

tree->count ++;
return LDNS_STATUS_OK;
}






void* ldns_radix_delete(ldns_radix_t* tree, const uint8_t* key, radix_strlen_t len)
{
ldns_radix_node_t* del = ldns_radix_search(tree, key, len);
void* data = NULL;
if (del) {
tree->count--;
data = del->data;
del->data = NULL;
ldns_radix_del_fix(tree, del);
return data;
}
return NULL;
}






ldns_radix_node_t*
ldns_radix_search(ldns_radix_t* tree, const uint8_t* key, radix_strlen_t len)
{
ldns_radix_node_t* node = NULL;
radix_strlen_t pos = 0;
uint8_t byte = 0;

if (!tree || !key) {
return NULL;
}
node = tree->root;
while (node) {
if (pos == len) {
return node->data?node:NULL;
}
byte = key[pos];
if (byte < node->offset) {
return NULL;
}
byte -= node->offset;
if (byte >= node->len) {
return NULL;
}
pos++;
if (node->array[byte].len > 0) {

if (pos + node->array[byte].len > len) {
return NULL;
}
if (memcmp(&key[pos], node->array[byte].str,
node->array[byte].len) != 0) {
return NULL;
}
pos += node->array[byte].len;
}
node = node->array[byte].edge;
}
return NULL;
}







int
ldns_radix_find_less_equal(ldns_radix_t* tree, const uint8_t* key,
radix_strlen_t len, ldns_radix_node_t** result)
{
ldns_radix_node_t* node = NULL;
radix_strlen_t pos = 0;
uint8_t byte;
int memcmp_res = 0;

if (!tree || !tree->root || !key) {
*result = NULL;
return 0;
}

node = tree->root;
while (pos < len) {
byte = key[pos];
if (byte < node->offset) {




ldns_radix_self_or_prev(node, result);
return 0;
}
byte -= node->offset;
if (byte >= node->len) {




*result = ldns_radix_last_in_subtree_incl_self(node);
if (*result == NULL) {
*result = ldns_radix_prev(node);
}
return 0;
}
pos++;
if (!node->array[byte].edge) {




*result = ldns_radix_prev_from_index(node, byte);
if (*result == NULL) {
ldns_radix_self_or_prev(node, result);
}
return 0;
}
if (node->array[byte].len != 0) {

if (pos + node->array[byte].len > len) {

if (memcmp(&key[pos], node->array[byte].str,
len-pos) <= 0) {

*result = ldns_radix_prev(
node->array[byte].edge);
} else {

*result = ldns_radix_last_in_subtree_incl_self(node->array[byte].edge);
if (*result == NULL) {
*result = ldns_radix_prev(node->array[byte].edge);
}
}
return 0;
}
memcmp_res = memcmp(&key[pos], node->array[byte].str,
node->array[byte].len);
if (memcmp_res < 0) {
*result = ldns_radix_prev(
node->array[byte].edge);
return 0;
} else if (memcmp_res > 0) {
*result = ldns_radix_last_in_subtree_incl_self(node->array[byte].edge);
if (*result == NULL) {
*result = ldns_radix_prev(node->array[byte].edge);
}
return 0;
}

pos += node->array[byte].len;
}
node = node->array[byte].edge;
}
if (node->data) {

*result = node;
return 1;
}

*result = ldns_radix_prev(node);
return 0;
}






ldns_radix_node_t*
ldns_radix_first(const ldns_radix_t* tree)
{
ldns_radix_node_t* first = NULL;
if (!tree || !tree->root) {
return NULL;
}
first = tree->root;
if (first->data) {
return first;
}
return ldns_radix_next(first);
}






ldns_radix_node_t*
ldns_radix_last(const ldns_radix_t* tree)
{
if (!tree || !tree->root) {
return NULL;
}
return ldns_radix_last_in_subtree_incl_self(tree->root);
}






ldns_radix_node_t*
ldns_radix_next(ldns_radix_node_t* node)
{
if (!node) {
return NULL;
}
if (node->len) {

ldns_radix_node_t* next = ldns_radix_next_in_subtree(node);
if (next) {
return next;
}
}

while (node->parent) {
uint8_t index = node->parent_index;
node = node->parent;
index++;
for (; index < node->len; index++) {
if (node->array[index].edge) {
ldns_radix_node_t* next;

if (node->array[index].edge->data) {
return node->array[index].edge;
}

next = ldns_radix_next_in_subtree(node);
if (next) {
return next;
}
}
}
}
return NULL;
}






ldns_radix_node_t*
ldns_radix_prev(ldns_radix_node_t* node)
{
if (!node) {
return NULL;
}


while (node->parent) {
uint8_t index = node->parent_index;
ldns_radix_node_t* prev;
node = node->parent;
assert(node->len > 0);
prev = ldns_radix_prev_from_index(node, index);
if (prev) {
return prev;
}
if (node->data) {
return node;
}
}
return NULL;
}






static void
ldns_radix_node_print(FILE* fd, ldns_radix_node_t* node,
uint8_t i, uint8_t* str, radix_strlen_t len, unsigned d)
{
uint8_t j;
if (!node) {
return;
}
for (j = 0; j < d; j++) {
fprintf(fd, "--");
}
if (str) {
radix_strlen_t l;
fprintf(fd, "| [%u+", (unsigned) i);
for (l=0; l < len; l++) {
fprintf(fd, "%c", (char) str[l]);
}
fprintf(fd, "]%u", (unsigned) len);
} else {
fprintf(fd, "| [%u]", (unsigned) i);
}

if (node->data) {
fprintf(fd, " %s", (char*) node->data);
}
fprintf(fd, "\n");

for (j = 0; j < node->len; j++) {
if (node->array[j].edge) {
ldns_radix_node_print(fd, node->array[j].edge, j,
node->array[j].str, node->array[j].len, d+1);
}
}
return;
}






void
ldns_radix_printf(FILE* fd, const ldns_radix_t* tree)
{
if (!fd || !tree) {
return;
}
if (!tree->root) {
fprintf(fd, "; empty radix tree\n");
return;
}
ldns_radix_node_print(fd, tree->root, 0, NULL, 0, 0);
return;
}






ldns_status
ldns_radix_join(ldns_radix_t* tree1, ldns_radix_t* tree2)
{
ldns_radix_node_t* cur_node, *next_node;
ldns_status status;
if (!tree2 || !tree2->root) {
return LDNS_STATUS_OK;
}


cur_node = ldns_radix_first(tree2);
while (cur_node) {
status = LDNS_STATUS_NO_DATA;

if (cur_node->data) {
status = ldns_radix_insert(tree1, cur_node->key,
cur_node->klen, cur_node->data);

if (status != LDNS_STATUS_OK &&
status != LDNS_STATUS_EXISTS_ERR) {
return status;
}
}
next_node = ldns_radix_next(cur_node);
if (status == LDNS_STATUS_OK) {
(void) ldns_radix_delete(tree2, cur_node->key,
cur_node->klen);
}
cur_node = next_node;
}

return LDNS_STATUS_OK;
}






ldns_status
ldns_radix_split(ldns_radix_t* tree1, size_t num, ldns_radix_t** tree2)
{
size_t count = 0;
ldns_radix_node_t* cur_node;
ldns_status status = LDNS_STATUS_OK;
if (!tree1 || !tree1->root || num == 0) {
return LDNS_STATUS_OK;
}
if (!tree2) {
return LDNS_STATUS_NULL;
}
if (!*tree2) {
*tree2 = ldns_radix_create();
if (!*tree2) {
return LDNS_STATUS_MEM_ERR;
}
}
cur_node = ldns_radix_first(tree1);
while (count < num && cur_node) {
if (cur_node->data) {

uint8_t* cur_key = cur_node->key;
radix_strlen_t cur_len = cur_node->klen;
void* cur_data = ldns_radix_delete(tree1, cur_key,
cur_len);

if (!cur_data) {
return LDNS_STATUS_NO_DATA;
}
status = ldns_radix_insert(*tree2, cur_key, cur_len,
cur_data);
if (status != LDNS_STATUS_OK &&
status != LDNS_STATUS_EXISTS_ERR) {
return status;
}







count++;
cur_node = ldns_radix_first(tree1);
} else {
cur_node = ldns_radix_next(cur_node);
}
}
return LDNS_STATUS_OK;
}







void
ldns_radix_traverse_postorder(ldns_radix_node_t* node,
void (*func)(ldns_radix_node_t*, void*), void* arg)
{
uint8_t i;
if (!node) {
return;
}
for (i=0; i < node->len; i++) {
ldns_radix_traverse_postorder(node->array[i].edge,
func, arg);
}

(*func)(node, arg);
return;
}

















static int
ldns_radix_find_prefix(ldns_radix_t* tree, uint8_t* key,
radix_strlen_t len, ldns_radix_node_t** result, radix_strlen_t* respos)
{

ldns_radix_node_t* n = tree->root;
radix_strlen_t pos = 0;
uint8_t byte;
*respos = 0;
*result = n;
if (!n) {

return 0;
}

while (n) {
if (pos == len) {

return 1;
}
byte = key[pos];
if (byte < n->offset) {

return 1;
}
byte -= n->offset;
if (byte >= n->len) {

return 1;
}

pos++;
if (n->array[byte].len != 0) {

if (pos + n->array[byte].len > len) {
return 1;
}
if (memcmp(&key[pos], n->array[byte].str,
n->array[byte].len) != 0) {
return 1;
}
pos += n->array[byte].len;
}

n = n->array[byte].edge;
if (!n) {
return 1;
}

*respos = pos;
*result = n;
}

return 1;
}









static int
ldns_radix_array_space(ldns_radix_node_t* node, uint8_t byte)
{

if (!node->array) {
assert(node->capacity == 0);

node->array = LDNS_MALLOC(ldns_radix_array_t);
if (!node->array) {
return 0;
}
memset(&node->array[0], 0, sizeof(ldns_radix_array_t));
node->len = 1;
node->capacity = 1;
node->offset = byte;
return 1;
}

assert(node->array != NULL);
assert(node->capacity > 0);

if (node->len == 0) {

node->len = 1;
node->offset = byte;
} else if (byte < node->offset) {

uint8_t index;
uint16_t need = node->offset - byte;

if (node->len + need > node->capacity) {

if (!ldns_radix_array_grow(node,
(unsigned) (node->len + need))) {
return 0;
}
}

memmove(&node->array[need], &node->array[0],
node->len*sizeof(ldns_radix_array_t));

for (index = 0; index < node->len; index++) {
if (node->array[index+need].edge) {
node->array[index+need].edge->parent_index =
index + need;
}
}

memset(&node->array[0], 0, need*sizeof(ldns_radix_array_t));
node->len += need;
node->offset = byte;
} else if (byte - node->offset >= node->len) {

uint16_t need = (byte - node->offset) - node->len + 1;

if (node->len + need > node->capacity) {

if (!ldns_radix_array_grow(node,
(unsigned) (node->len + need))) {
return 0;
}
}

memset(&node->array[node->len], 0,
need*sizeof(ldns_radix_array_t));
node->len += need;
}
return 1;
}










static int
ldns_radix_array_grow(ldns_radix_node_t* node, unsigned need)
{
unsigned size = ((unsigned)node->capacity)*2;
ldns_radix_array_t* a = NULL;
if (need > size) {
size = need;
}
if (size > 256) {
size = 256;
}
a = LDNS_XMALLOC(ldns_radix_array_t, size);
if (!a) {
return 0;
}
assert(node->len <= node->capacity);
assert(node->capacity < size);
memcpy(&a[0], &node->array[0], node->len*sizeof(ldns_radix_array_t));
LDNS_FREE(node->array);
node->array = a;
node->capacity = size;
return 1;
}











static int
ldns_radix_str_create(ldns_radix_array_t* array, uint8_t* key,
radix_strlen_t pos, radix_strlen_t len)
{
array->str = LDNS_XMALLOC(uint8_t, (len-pos));
if (!array->str) {
return 0;
}
memmove(array->str, key+pos, len-pos);
array->len = (len-pos);
return 1;
}












static int
ldns_radix_prefix_remainder(radix_strlen_t prefix_len,
uint8_t* longer_str, radix_strlen_t longer_len,
uint8_t** split_str, radix_strlen_t* split_len)
{
*split_len = longer_len - prefix_len;
*split_str = LDNS_XMALLOC(uint8_t, (*split_len));
if (!*split_str) {
return 0;
}
memmove(*split_str, longer_str+prefix_len, longer_len-prefix_len);
return 1;
}












static int
ldns_radix_array_split(ldns_radix_array_t* array, uint8_t* key,
radix_strlen_t pos, radix_strlen_t len, ldns_radix_node_t* add)
{
uint8_t* str_to_add = key + pos;
radix_strlen_t strlen_to_add = len - pos;

if (ldns_radix_str_is_prefix(str_to_add, strlen_to_add,
array->str, array->len)) {

uint8_t* split_str = NULL, *dup_str = NULL;
radix_strlen_t split_len = 0;








assert(strlen_to_add < array->len);

if (array->len - strlen_to_add > 1) {
if (!ldns_radix_prefix_remainder(strlen_to_add+1,
array->str, array->len, &split_str,
&split_len)) {
return 0;
}
}

if (strlen_to_add != 0) {
dup_str = LDNS_XMALLOC(uint8_t, strlen_to_add);
if (!dup_str) {
LDNS_FREE(split_str);
return 0;
}
memcpy(dup_str, str_to_add, strlen_to_add);
}

if (!ldns_radix_array_space(add,
array->str[strlen_to_add])) {
LDNS_FREE(split_str);
LDNS_FREE(dup_str);
return 0;
}




add->parent = array->edge->parent;
add->parent_index = array->edge->parent_index;
add->array[0].edge = array->edge;
add->array[0].str = split_str;
add->array[0].len = split_len;
array->edge->parent = add;
array->edge->parent_index = 0;
LDNS_FREE(array->str);
array->edge = add;
array->str = dup_str;
array->len = strlen_to_add;
} else if (ldns_radix_str_is_prefix(array->str, array->len,
str_to_add, strlen_to_add)) {










uint8_t* split_str = NULL;
radix_strlen_t split_len = 0;
assert(array->len < strlen_to_add);
if (strlen_to_add - array->len > 1) {
if (!ldns_radix_prefix_remainder(array->len+1,
str_to_add, strlen_to_add, &split_str,
&split_len)) {
return 0;
}
}

if (!ldns_radix_array_space(array->edge,
str_to_add[array->len])) {
LDNS_FREE(split_str);
return 0;
}



add->parent = array->edge;
add->parent_index = str_to_add[array->len] -
array->edge->offset;
array->edge->array[add->parent_index].edge = add;
array->edge->array[add->parent_index].str = split_str;
array->edge->array[add->parent_index].len = split_len;
} else {












ldns_radix_node_t* common = NULL;
uint8_t* common_str = NULL, *s1 = NULL, *s2 = NULL;
radix_strlen_t common_len = 0, l1 = 0, l2 = 0;
common_len = ldns_radix_str_common(array->str, array->len,
str_to_add, strlen_to_add);
assert(common_len < array->len);
assert(common_len < strlen_to_add);

common = ldns_radix_new_node(NULL, (uint8_t*)"", 0);
if (!common) {
return 0;
}
if (array->len - common_len > 1) {
if (!ldns_radix_prefix_remainder(common_len+1,
array->str, array->len, &s1, &l1)) {
return 0;
}
}
if (strlen_to_add - common_len > 1) {
if (!ldns_radix_prefix_remainder(common_len+1,
str_to_add, strlen_to_add, &s2, &l2)) {
return 0;
}
}

if (common_len > 0) {
common_str = LDNS_XMALLOC(uint8_t, common_len);
if (!common_str) {
LDNS_FREE(common);
LDNS_FREE(s1);
LDNS_FREE(s2);
return 0;
}
memcpy(common_str, str_to_add, common_len);
}

if (!ldns_radix_array_space(common, array->str[common_len]) ||
!ldns_radix_array_space(common, str_to_add[common_len])) {
LDNS_FREE(common->array);
LDNS_FREE(common);
LDNS_FREE(common_str);
LDNS_FREE(s1);
LDNS_FREE(s2);
return 0;
}




common->parent = array->edge->parent;
common->parent_index = array->edge->parent_index;
array->edge->parent = common;
array->edge->parent_index = array->str[common_len] -
common->offset;
add->parent = common;
add->parent_index = str_to_add[common_len] - common->offset;
common->array[array->edge->parent_index].edge = array->edge;
common->array[array->edge->parent_index].str = s1;
common->array[array->edge->parent_index].len = l1;
common->array[add->parent_index].edge = add;
common->array[add->parent_index].str = s2;
common->array[add->parent_index].len = l2;
LDNS_FREE(array->str);
array->edge = common;
array->str = common_str;
array->len = common_len;
}
return 1;
}











static int
ldns_radix_str_is_prefix(uint8_t* str1, radix_strlen_t len1,
uint8_t* str2, radix_strlen_t len2)
{
if (len1 == 0) {
return 1;
}
if (len1 > len2) {
return 0;
}
return (memcmp(str1, str2, len1) == 0);
}











static radix_strlen_t
ldns_radix_str_common(uint8_t* str1, radix_strlen_t len1,
uint8_t* str2, radix_strlen_t len2)
{
radix_strlen_t i, max = (len1<len2)?len1:len2;
for (i=0; i<max; i++) {
if (str1[i] != str2[i]) {
return i;
}
}
return max;
}








static ldns_radix_node_t*
ldns_radix_next_in_subtree(ldns_radix_node_t* node)
{
uint16_t i;
ldns_radix_node_t* next;

for (i = 0; i < node->len; i++) {
if (node->array[i].edge) {

if (node->array[i].edge->data) {
return node->array[i].edge;
}

next = ldns_radix_next_in_subtree(node->array[i].edge);
if (next) {
return next;
}
}
}
return NULL;
}









static ldns_radix_node_t*
ldns_radix_prev_from_index(ldns_radix_node_t* node, uint8_t index)
{
uint8_t i = index;
while (i > 0) {
i--;
if (node->array[i].edge) {
ldns_radix_node_t* prev =
ldns_radix_last_in_subtree_incl_self(node);
if (prev) {
return prev;
}
}
}
return NULL;
}








static ldns_radix_node_t*
ldns_radix_last_in_subtree_incl_self(ldns_radix_node_t* node)
{
ldns_radix_node_t* last = ldns_radix_last_in_subtree(node);
if (last) {
return last;
} else if (node->data) {
return node;
}
return NULL;
}








static ldns_radix_node_t*
ldns_radix_last_in_subtree(ldns_radix_node_t* node)
{
int i;

for (i=(int)(node->len)-1; i >= 0; i--) {
if (node->array[i].edge) {

if (node->array[i].edge->len > 0) {
ldns_radix_node_t* last =
ldns_radix_last_in_subtree(
node->array[i].edge);
if (last) {
return last;
}
}

if (node->array[i].edge->data) {
return node->array[i].edge;
}
}
}
return NULL;
}








static void
ldns_radix_del_fix(ldns_radix_t* tree, ldns_radix_node_t* node)
{
while (node) {
if (node->data) {

return;
} else if (node->len == 1 && node->parent) {

ldns_radix_cleanup_onechild(node);
return;
} else if (node->len == 0) {

ldns_radix_node_t* parent = node->parent;
if (!parent) {

ldns_radix_node_free(node, NULL);
tree->root = NULL;
return;
}

ldns_radix_cleanup_leaf(node);
node = parent;
} else {




return;
}
}

return;
}







static void
ldns_radix_cleanup_onechild(ldns_radix_node_t* node)
{
uint8_t* join_str;
radix_strlen_t join_len;
uint8_t parent_index = node->parent_index;
ldns_radix_node_t* child = node->array[0].edge;
ldns_radix_node_t* parent = node->parent;


assert(parent_index < parent->len);
join_len = parent->array[parent_index].len + node->array[0].len + 1;

join_str = LDNS_XMALLOC(uint8_t, join_len);
if (!join_str) {





return;
}

memcpy(join_str, parent->array[parent_index].str,
parent->array[parent_index].len);
join_str[parent->array[parent_index].len] = child->parent_index +
node->offset;
memmove(join_str + parent->array[parent_index].len+1,
node->array[0].str, node->array[0].len);

LDNS_FREE(parent->array[parent_index].str);
parent->array[parent_index].str = join_str;
parent->array[parent_index].len = join_len;
parent->array[parent_index].edge = child;
child->parent = parent;
child->parent_index = parent_index;
ldns_radix_node_free(node, NULL);
return;
}







static void
ldns_radix_cleanup_leaf(ldns_radix_node_t* node)
{
uint8_t parent_index = node->parent_index;
ldns_radix_node_t* parent = node->parent;

assert(parent_index < parent->len);
ldns_radix_node_free(node, NULL);
LDNS_FREE(parent->array[parent_index].str);
parent->array[parent_index].str = NULL;
parent->array[parent_index].len = 0;
parent->array[parent_index].edge = NULL;

if (parent->len == 1) {
ldns_radix_node_array_free(parent);
} else if (parent_index == 0) {
ldns_radix_node_array_free_front(parent);
} else {
ldns_radix_node_array_free_end(parent);
}
return;
}








static void
ldns_radix_node_free(ldns_radix_node_t* node, void* arg)
{
uint16_t i;
(void) arg;
if (!node) {
return;
}
for (i=0; i < node->len; i++) {
LDNS_FREE(node->array[i].str);
}
node->key = NULL;
node->klen = 0;
LDNS_FREE(node->array);
LDNS_FREE(node);
return;
}







static void
ldns_radix_node_array_free(ldns_radix_node_t* node)
{
node->offset = 0;
node->len = 0;
LDNS_FREE(node->array);
node->array = NULL;
node->capacity = 0;
return;
}







static void
ldns_radix_node_array_free_front(ldns_radix_node_t* node)
{
uint16_t i, n = 0;

while (n < node->len && node->array[n].edge == NULL) {
n++;
}
if (n == 0) {
return;
}
if (n == node->len) {
ldns_radix_node_array_free(node);
return;
}
assert(n < node->len);
assert((int) n <= (255 - (int) node->offset));
memmove(&node->array[0], &node->array[n],
(node->len - n)*sizeof(ldns_radix_array_t));
node->offset += n;
node->len -= n;
for (i=0; i < node->len; i++) {
if (node->array[i].edge) {
node->array[i].edge->parent_index = i;
}
}
ldns_radix_array_reduce(node);
return;
}







static void
ldns_radix_node_array_free_end(ldns_radix_node_t* node)
{
uint16_t n = 0;

while (n < node->len && node->array[node->len-1-n].edge == NULL) {
n++;
}
if (n == 0) {
return;
}
if (n == node->len) {
ldns_radix_node_array_free(node);
return;
}
assert(n < node->len);
node->len -= n;
ldns_radix_array_reduce(node);
return;
}







static void
ldns_radix_array_reduce(ldns_radix_node_t* node)
{
if (node->len <= node->capacity/2 && node->len != node->capacity) {
ldns_radix_array_t* a = LDNS_XMALLOC(ldns_radix_array_t,
node->len);
if (!a) {
return;
}
memcpy(a, node->array, sizeof(ldns_radix_array_t)*node->len);
LDNS_FREE(node->array);
node->array = a;
node->capacity = node->len;
}
return;
}








static void
ldns_radix_self_or_prev(ldns_radix_node_t* node, ldns_radix_node_t** result)
{
if (node->data) {
*result = node;
} else {
*result = ldns_radix_prev(node);
}
return;
}
