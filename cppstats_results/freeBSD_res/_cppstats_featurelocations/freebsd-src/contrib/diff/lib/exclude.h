

























#define EXCLUDE_ANCHORED (1 << 30)


#define EXCLUDE_INCLUDE (1 << 29)



#define EXCLUDE_WILDCARDS (1 << 28)

struct exclude;

struct exclude *new_exclude (void);
void free_exclude (struct exclude *);
void add_exclude (struct exclude *, char const *, int);
int add_exclude_file (void (*) (struct exclude *, char const *, int),
struct exclude *, char const *, int, char);
bool excluded_filename (struct exclude const *, char const *);
