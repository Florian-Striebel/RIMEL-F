



























#if !defined(_SETFACL_H)
#define _SETFACL_H

#include <stdbool.h>

#include <sys/types.h>
#include <sys/acl.h>
#include <sys/queue.h>


acl_t get_acl_from_file(const char *filename);

int merge_acl(acl_t acl, acl_t *prev_acl, const char *filename);
int add_acl(acl_t acl, uint entry_number, acl_t *prev_acl,
const char *filename);

int remove_acl(acl_t acl, acl_t *prev_acl, const char *filename);
int remove_by_number(uint entry_number, acl_t *prev_acl,
const char *filename);
int remove_default(acl_t *prev_acl, const char *filename);
void remove_ext(acl_t *prev_acl, const char *filename);

int set_acl_mask(acl_t *prev_acl, const char *filename);

void *zmalloc(size_t size);
void *zrealloc(void *ptr, size_t size);
const char *brand_name(int brand);
int branding_mismatch(int brand1, int brand2);

extern bool have_mask;
extern bool have_stdin;
extern bool n_flag;

#endif
