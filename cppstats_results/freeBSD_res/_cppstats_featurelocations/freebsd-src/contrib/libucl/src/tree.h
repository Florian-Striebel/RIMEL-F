








































#if !defined(__tree_h)
#define __tree_h


#define TREE_DELTA_MAX 1
#if !defined(_HU_FUNCTION)
#if defined(__GNUC__) || defined(__clang__)
#define _HU_FUNCTION(x) __attribute__((__unused__)) x
#else
#define _HU_FUNCTION(x) x
#endif
#endif

#define TREE_ENTRY(type) struct { struct type *avl_left; struct type *avl_right; int avl_height; }






#define TREE_HEAD(name, type) struct name { struct type *th_root; int (*th_cmp)(struct type *lhs, struct type *rhs); }





#define TREE_INITIALIZER(cmp) { 0, cmp }

#define TREE_DELTA(self, field) (( (((self)->field.avl_left) ? (self)->field.avl_left->field.avl_height : 0)) - (((self)->field.avl_right) ? (self)->field.avl_right->field.avl_height : 0))





#define TREE_DEFINE(node, field) static struct node *_HU_FUNCTION(TREE_BALANCE_##node##_##field)(struct node *); static struct node *_HU_FUNCTION(TREE_ROTL_##node##_##field)(struct node *self) { struct node *r= self->field.avl_right; self->field.avl_right= r->field.avl_left; r->field.avl_left= TREE_BALANCE_##node##_##field(self); return TREE_BALANCE_##node##_##field(r); } static struct node *_HU_FUNCTION(TREE_ROTR_##node##_##field)(struct node *self) { struct node *l= self->field.avl_left; self->field.avl_left= l->field.avl_right; l->field.avl_right= TREE_BALANCE_##node##_##field(self); return TREE_BALANCE_##node##_##field(l); } static struct node *_HU_FUNCTION(TREE_BALANCE_##node##_##field)(struct node *self) { int delta= TREE_DELTA(self, field); if (delta < -TREE_DELTA_MAX) { if (TREE_DELTA(self->field.avl_right, field) > 0) self->field.avl_right= TREE_ROTR_##node##_##field(self->field.avl_right); return TREE_ROTL_##node##_##field(self); } else if (delta > TREE_DELTA_MAX) { if (TREE_DELTA(self->field.avl_left, field) < 0) self->field.avl_left= TREE_ROTL_##node##_##field(self->field.avl_left); return TREE_ROTR_##node##_##field(self); } self->field.avl_height= 0; if (self->field.avl_left && (self->field.avl_left->field.avl_height > self->field.avl_height)) self->field.avl_height= self->field.avl_left->field.avl_height; if (self->field.avl_right && (self->field.avl_right->field.avl_height > self->field.avl_height)) self->field.avl_height= self->field.avl_right->field.avl_height; self->field.avl_height += 1; return self; } static struct node *_HU_FUNCTION(TREE_INSERT_##node##_##field) (struct node *self, struct node *elm, int (*compare)(struct node *lhs, struct node *rhs)) { if (!self) return elm; if (compare(elm, self) < 0) self->field.avl_left= TREE_INSERT_##node##_##field(self->field.avl_left, elm, compare); else self->field.avl_right= TREE_INSERT_##node##_##field(self->field.avl_right, elm, compare); return TREE_BALANCE_##node##_##field(self); } static struct node *_HU_FUNCTION(TREE_FIND_##node##_##field) (struct node *self, struct node *elm, int (*compare)(struct node *lhs, struct node *rhs)) { if (!self) return 0; if (compare(elm, self) == 0) return self; if (compare(elm, self) < 0) return TREE_FIND_##node##_##field(self->field.avl_left, elm, compare); else return TREE_FIND_##node##_##field(self->field.avl_right, elm, compare); } static struct node *_HU_FUNCTION(TREE_MOVE_RIGHT)(struct node *self, struct node *rhs) { if (!self) return rhs; self->field.avl_right= TREE_MOVE_RIGHT(self->field.avl_right, rhs); return TREE_BALANCE_##node##_##field(self); } static struct node *_HU_FUNCTION(TREE_REMOVE_##node##_##field) (struct node *self, struct node *elm, int (*compare)(struct node *lhs, struct node *rhs)) { if (!self) return 0; if (compare(elm, self) == 0) { struct node *tmp= TREE_MOVE_RIGHT(self->field.avl_left, self->field.avl_right); self->field.avl_left= 0; self->field.avl_right= 0; return tmp; } if (compare(elm, self) < 0) self->field.avl_left= TREE_REMOVE_##node##_##field(self->field.avl_left, elm, compare); else self->field.avl_right= TREE_REMOVE_##node##_##field(self->field.avl_right, elm, compare); return TREE_BALANCE_##node##_##field(self); } static void _HU_FUNCTION(TREE_FORWARD_APPLY_ALL_##node##_##field) (struct node *self, void (*function)(struct node *node, void *data), void *data) { if (self) { TREE_FORWARD_APPLY_ALL_##node##_##field(self->field.avl_left, function, data); function(self, data); TREE_FORWARD_APPLY_ALL_##node##_##field(self->field.avl_right, function, data); } } static void _HU_FUNCTION(TREE_REVERSE_APPLY_ALL_##node##_##field) (struct node *self, void (*function)(struct node *node, void *data), void *data) { if (self) { TREE_REVERSE_APPLY_ALL_##node##_##field(self->field.avl_right, function, data); function(self, data); TREE_REVERSE_APPLY_ALL_##node##_##field(self->field.avl_left, function, data); } }






















































































































#define TREE_INSERT(head, node, field, elm) ((head)->th_root= TREE_INSERT_##node##_##field((head)->th_root, (elm), (head)->th_cmp))


#define TREE_FIND(head, node, field, elm) (TREE_FIND_##node##_##field((head)->th_root, (elm), (head)->th_cmp))


#define TREE_REMOVE(head, node, field, elm) ((head)->th_root= TREE_REMOVE_##node##_##field((head)->th_root, (elm), (head)->th_cmp))


#define TREE_DEPTH(head, field) ((head)->th_root->field.avl_height)


#define TREE_FORWARD_APPLY(head, node, field, function, data) TREE_FORWARD_APPLY_ALL_##node##_##field((head)->th_root, function, data)


#define TREE_REVERSE_APPLY(head, node, field, function, data) TREE_REVERSE_APPLY_ALL_##node##_##field((head)->th_root, function, data)


#define TREE_INIT(head, cmp) do { (head)->th_root= 0; (head)->th_cmp= (cmp); } while (0)





#endif
