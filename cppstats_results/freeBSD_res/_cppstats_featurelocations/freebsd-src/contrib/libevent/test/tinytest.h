
























#if !defined(TINYTEST_H_INCLUDED_)
#define TINYTEST_H_INCLUDED_


#define TT_FORK (1<<0)

#define TT_SKIP (1<<1)

#define TT_ENABLED_ (1<<2)

#define TT_OFF_BY_DEFAULT (1<<3)

#define TT_FIRST_USER_FLAG (1<<4)

typedef void (*testcase_fn)(void *);

struct testcase_t;


struct testcase_setup_t {

void *(*setup_fn)(const struct testcase_t *);

int (*cleanup_fn)(const struct testcase_t *, void *);
};


struct testcase_t {
const char *name;
testcase_fn fn;
unsigned long flags;
const struct testcase_setup_t *setup;
void *setup_data;
};
#define END_OF_TESTCASES { NULL, NULL, 0, NULL, NULL }


struct testgroup_t {
const char *prefix;
struct testcase_t *cases;
};
#define END_OF_GROUPS { NULL, NULL}

struct testlist_alias_t {
const char *name;
const char **tests;
};
#define END_OF_ALIASES { NULL, NULL }


void tinytest_set_test_failed_(void);

void tinytest_set_test_skipped_(void);

int tinytest_get_verbosity_(void);


int tinytest_set_flag_(struct testgroup_t *, const char *, int set, unsigned long);

char *tinytest_format_hex_(const void *, unsigned long);


#define tinytest_skip(groups, named) tinytest_set_flag_(groups, named, 1, TT_SKIP)



int testcase_run_one(const struct testgroup_t *,const struct testcase_t *);

void tinytest_set_aliases(const struct testlist_alias_t *aliases);



int tinytest_main(int argc, const char **argv, struct testgroup_t *groups);

#endif
