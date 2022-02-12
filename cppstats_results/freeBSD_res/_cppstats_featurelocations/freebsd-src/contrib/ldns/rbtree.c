










































#include <ldns/config.h>
#include <ldns/rbtree.h>
#include <ldns/util.h>
#include <stdlib.h>


#define BLACK 0

#define RED 1


ldns_rbnode_t ldns_rbtree_null_node = {
LDNS_RBTREE_NULL,
LDNS_RBTREE_NULL,
LDNS_RBTREE_NULL,
NULL,
NULL,
BLACK
};


static void ldns_rbtree_rotate_left(ldns_rbtree_t *rbtree, ldns_rbnode_t *node);

static void ldns_rbtree_rotate_right(ldns_rbtree_t *rbtree, ldns_rbnode_t *node);

static void ldns_rbtree_insert_fixup(ldns_rbtree_t *rbtree, ldns_rbnode_t *node);

static void ldns_rbtree_delete_fixup(ldns_rbtree_t* rbtree, ldns_rbnode_t* child, ldns_rbnode_t* child_parent);







ldns_rbtree_t *
ldns_rbtree_create (int (*cmpf)(const void *, const void *))
{
ldns_rbtree_t *rbtree;


rbtree = (ldns_rbtree_t *) LDNS_MALLOC(ldns_rbtree_t);
if (!rbtree) {
return NULL;
}


ldns_rbtree_init(rbtree, cmpf);

return rbtree;
}

void
ldns_rbtree_init(ldns_rbtree_t *rbtree, int (*cmpf)(const void *, const void *))
{

rbtree->root = LDNS_RBTREE_NULL;
rbtree->count = 0;
rbtree->cmp = cmpf;
}

void
ldns_rbtree_free(ldns_rbtree_t *rbtree)
{
LDNS_FREE(rbtree);
}





static void
ldns_rbtree_rotate_left(ldns_rbtree_t *rbtree, ldns_rbnode_t *node)
{
ldns_rbnode_t *right = node->right;
node->right = right->left;
if (right->left != LDNS_RBTREE_NULL)
right->left->parent = node;

right->parent = node->parent;

if (node->parent != LDNS_RBTREE_NULL) {
if (node == node->parent->left) {
node->parent->left = right;
} else {
node->parent->right = right;
}
} else {
rbtree->root = right;
}
right->left = node;
node->parent = right;
}





static void
ldns_rbtree_rotate_right(ldns_rbtree_t *rbtree, ldns_rbnode_t *node)
{
ldns_rbnode_t *left = node->left;
node->left = left->right;
if (left->right != LDNS_RBTREE_NULL)
left->right->parent = node;

left->parent = node->parent;

if (node->parent != LDNS_RBTREE_NULL) {
if (node == node->parent->right) {
node->parent->right = left;
} else {
node->parent->left = left;
}
} else {
rbtree->root = left;
}
left->right = node;
node->parent = left;
}

static void
ldns_rbtree_insert_fixup(ldns_rbtree_t *rbtree, ldns_rbnode_t *node)
{
ldns_rbnode_t *uncle;


while (node != rbtree->root && node->parent->color == RED) {

if (node->parent == node->parent->parent->left) {
uncle = node->parent->parent->right;


if (uncle->color == RED) {

node->parent->color = BLACK;
uncle->color = BLACK;


node->parent->parent->color = RED;


node = node->parent->parent;
} else {

if (node == node->parent->right) {
node = node->parent;
ldns_rbtree_rotate_left(rbtree, node);
}

node->parent->color = BLACK;
node->parent->parent->color = RED;
ldns_rbtree_rotate_right(rbtree, node->parent->parent);
}
} else {
uncle = node->parent->parent->left;


if (uncle->color == RED) {

node->parent->color = BLACK;
uncle->color = BLACK;


node->parent->parent->color = RED;


node = node->parent->parent;
} else {

if (node == node->parent->left) {
node = node->parent;
ldns_rbtree_rotate_right(rbtree, node);
}

node->parent->color = BLACK;
node->parent->parent->color = RED;
ldns_rbtree_rotate_left(rbtree, node->parent->parent);
}
}
}
rbtree->root->color = BLACK;
}

void
ldns_rbtree_insert_vref(ldns_rbnode_t *data, void *rbtree)
{
(void) ldns_rbtree_insert((ldns_rbtree_t *) rbtree,
data);
}







ldns_rbnode_t *
ldns_rbtree_insert (ldns_rbtree_t *rbtree, ldns_rbnode_t *data)
{

int r = 0;


ldns_rbnode_t *node = rbtree->root;
ldns_rbnode_t *parent = LDNS_RBTREE_NULL;


while (node != LDNS_RBTREE_NULL) {

if ((r = rbtree->cmp(data->key, node->key)) == 0) {
return NULL;
}
parent = node;

if (r < 0) {
node = node->left;
} else {
node = node->right;
}
}


data->parent = parent;
data->left = data->right = LDNS_RBTREE_NULL;
data->color = RED;
rbtree->count++;


if (parent != LDNS_RBTREE_NULL) {
if (r < 0) {
parent->left = data;
} else {
parent->right = data;
}
} else {
rbtree->root = data;
}


ldns_rbtree_insert_fixup(rbtree, data);

return data;
}





ldns_rbnode_t *
ldns_rbtree_search (ldns_rbtree_t *rbtree, const void *key)
{
ldns_rbnode_t *node;

if (ldns_rbtree_find_less_equal(rbtree, key, &node)) {
return node;
} else {
return NULL;
}
}


static void swap_int8(uint8_t* x, uint8_t* y)
{
uint8_t t = *x; *x = *y; *y = t;
}


static void swap_np(ldns_rbnode_t** x, ldns_rbnode_t** y)
{
ldns_rbnode_t* t = *x; *x = *y; *y = t;
}


static void change_parent_ptr(ldns_rbtree_t* rbtree, ldns_rbnode_t* parent, ldns_rbnode_t* old, ldns_rbnode_t* new)
{
if(parent == LDNS_RBTREE_NULL)
{
if(rbtree->root == old) rbtree->root = new;
return;
}
if(parent->left == old) parent->left = new;
if(parent->right == old) parent->right = new;
}

static void change_child_ptr(ldns_rbnode_t* child, ldns_rbnode_t* old, ldns_rbnode_t* new)
{
if(child == LDNS_RBTREE_NULL) return;
if(child->parent == old) child->parent = new;
}

ldns_rbnode_t*
ldns_rbtree_delete(ldns_rbtree_t *rbtree, const void *key)
{
ldns_rbnode_t *to_delete;
ldns_rbnode_t *child;
if((to_delete = ldns_rbtree_search(rbtree, key)) == 0) return 0;
rbtree->count--;


if(to_delete->left != LDNS_RBTREE_NULL &&
to_delete->right != LDNS_RBTREE_NULL)
{

ldns_rbnode_t *smright = to_delete->right;
while(smright->left != LDNS_RBTREE_NULL)
smright = smright->left;






swap_int8(&to_delete->color, &smright->color);


change_parent_ptr(rbtree, to_delete->parent, to_delete, smright);
if(to_delete->right != smright)
change_parent_ptr(rbtree, smright->parent, smright, to_delete);


change_child_ptr(smright->left, smright, to_delete);
change_child_ptr(smright->left, smright, to_delete);
change_child_ptr(smright->right, smright, to_delete);
change_child_ptr(smright->right, smright, to_delete);
change_child_ptr(to_delete->left, to_delete, smright);
if(to_delete->right != smright)
change_child_ptr(to_delete->right, to_delete, smright);
if(to_delete->right == smright)
{

to_delete->right = to_delete;
smright->parent = smright;
}


swap_np(&to_delete->parent, &smright->parent);
swap_np(&to_delete->left, &smright->left);
swap_np(&to_delete->right, &smright->right);


}

if(to_delete->left != LDNS_RBTREE_NULL) child = to_delete->left;
else child = to_delete->right;


change_parent_ptr(rbtree, to_delete->parent, to_delete, child);
change_child_ptr(child, to_delete, to_delete->parent);

if(to_delete->color == RED)
{

}
else if(child->color == RED)
{

if(child!=LDNS_RBTREE_NULL) child->color = BLACK;
}
else ldns_rbtree_delete_fixup(rbtree, child, to_delete->parent);


to_delete->parent = LDNS_RBTREE_NULL;
to_delete->left = LDNS_RBTREE_NULL;
to_delete->right = LDNS_RBTREE_NULL;
to_delete->color = BLACK;
return to_delete;
}

static void ldns_rbtree_delete_fixup(ldns_rbtree_t* rbtree, ldns_rbnode_t* child, ldns_rbnode_t* child_parent)
{
ldns_rbnode_t* sibling;
int go_up = 1;


if(child_parent->right == child) sibling = child_parent->left;
else sibling = child_parent->right;

while(go_up)
{
if(child_parent == LDNS_RBTREE_NULL)
{

return;
}

if(sibling->color == RED)
{
child_parent->color = RED;
sibling->color = BLACK;
if(child_parent->right == child)
ldns_rbtree_rotate_right(rbtree, child_parent);
else ldns_rbtree_rotate_left(rbtree, child_parent);

if(child_parent->right == child) sibling = child_parent->left;
else sibling = child_parent->right;
}

if(child_parent->color == BLACK
&& sibling->color == BLACK
&& sibling->left->color == BLACK
&& sibling->right->color == BLACK)
{
if(sibling != LDNS_RBTREE_NULL)
sibling->color = RED;

child = child_parent;
child_parent = child_parent->parent;

if(child_parent->right == child) sibling = child_parent->left;
else sibling = child_parent->right;
}
else go_up = 0;
}

if(child_parent->color == RED
&& sibling->color == BLACK
&& sibling->left->color == BLACK
&& sibling->right->color == BLACK)
{

if(sibling != LDNS_RBTREE_NULL)
sibling->color = RED;
child_parent->color = BLACK;
return;
}



if(child_parent->right == child
&& sibling->color == BLACK
&& sibling->right->color == RED
&& sibling->left->color == BLACK)
{
sibling->color = RED;
sibling->right->color = BLACK;
ldns_rbtree_rotate_left(rbtree, sibling);

if(child_parent->right == child) sibling = child_parent->left;
else sibling = child_parent->right;
}
else if(child_parent->left == child
&& sibling->color == BLACK
&& sibling->left->color == RED
&& sibling->right->color == BLACK)
{
sibling->color = RED;
sibling->left->color = BLACK;
ldns_rbtree_rotate_right(rbtree, sibling);

if(child_parent->right == child) sibling = child_parent->left;
else sibling = child_parent->right;
}


sibling->color = child_parent->color;
child_parent->color = BLACK;
if(child_parent->right == child)
{
sibling->left->color = BLACK;
ldns_rbtree_rotate_right(rbtree, child_parent);
}
else
{
sibling->right->color = BLACK;
ldns_rbtree_rotate_left(rbtree, child_parent);
}
}

int
ldns_rbtree_find_less_equal(ldns_rbtree_t *rbtree, const void *key, ldns_rbnode_t **result)
{
int r;
ldns_rbnode_t *node;


node = rbtree->root;

*result = NULL;


while (node != LDNS_RBTREE_NULL) {
r = rbtree->cmp(key, node->key);
if (r == 0) {

*result = node;
return 1;
}
if (r < 0) {
node = node->left;
} else {

*result = node;
node = node->right;
}
}
return 0;
}





ldns_rbnode_t *
ldns_rbtree_first(const ldns_rbtree_t *rbtree)
{
ldns_rbnode_t *node = rbtree->root;

if (rbtree->root != LDNS_RBTREE_NULL) {
for (node = rbtree->root; node->left != LDNS_RBTREE_NULL; node = node->left);
}
return node;
}

ldns_rbnode_t *
ldns_rbtree_last(const ldns_rbtree_t *rbtree)
{
ldns_rbnode_t *node = rbtree->root;

if (rbtree->root != LDNS_RBTREE_NULL) {
for (node = rbtree->root; node->right != LDNS_RBTREE_NULL; node = node->right);
}
return node;
}





ldns_rbnode_t *
ldns_rbtree_next(ldns_rbnode_t *node)
{
ldns_rbnode_t *parent;

if (node->right != LDNS_RBTREE_NULL) {

for (node = node->right;
node->left != LDNS_RBTREE_NULL;
node = node->left);
} else {
parent = node->parent;
while (parent != LDNS_RBTREE_NULL && node == parent->right) {
node = parent;
parent = parent->parent;
}
node = parent;
}
return node;
}

ldns_rbnode_t *
ldns_rbtree_previous(ldns_rbnode_t *node)
{
ldns_rbnode_t *parent;

if (node->left != LDNS_RBTREE_NULL) {

for (node = node->left;
node->right != LDNS_RBTREE_NULL;
node = node->right);
} else {
parent = node->parent;
while (parent != LDNS_RBTREE_NULL && node == parent->left) {
node = parent;
parent = parent->parent;
}
node = parent;
}
return node;
}





ldns_rbtree_t *
ldns_rbtree_split(ldns_rbtree_t *tree,
size_t elements)
{
ldns_rbtree_t *new_tree;
ldns_rbnode_t *cur_node;
ldns_rbnode_t *move_node;
size_t count = 0;

new_tree = ldns_rbtree_create(tree->cmp);

cur_node = ldns_rbtree_first(tree);
while (count < elements && cur_node != LDNS_RBTREE_NULL) {
move_node = ldns_rbtree_delete(tree, cur_node->key);
(void)ldns_rbtree_insert(new_tree, move_node);
cur_node = ldns_rbtree_first(tree);
count++;
}

return new_tree;
}





void
ldns_rbtree_join(ldns_rbtree_t *tree1, ldns_rbtree_t *tree2)
{
ldns_traverse_postorder(tree2, ldns_rbtree_insert_vref, tree1);
}


static void
traverse_post(void (*func)(ldns_rbnode_t*, void*), void* arg,
ldns_rbnode_t* node)
{
if(!node || node == LDNS_RBTREE_NULL)
return;

traverse_post(func, arg, node->left);
traverse_post(func, arg, node->right);

(*func)(node, arg);
}

void
ldns_traverse_postorder(ldns_rbtree_t* tree,
void (*func)(ldns_rbnode_t*, void*), void* arg)
{
traverse_post(func, arg, tree->root);
}
