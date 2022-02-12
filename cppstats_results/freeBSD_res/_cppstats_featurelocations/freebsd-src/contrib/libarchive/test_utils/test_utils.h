

























#if !defined(TEST_UTILS_H)
#define TEST_UTILS_H

struct test_list_t
{
void (*func)(void);
const char *name;
int failures;
};

int get_test_set(int *, int, const char *, struct test_list_t *);

#endif
