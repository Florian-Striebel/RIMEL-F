
































#include "flexdef.h"







struct hash_entry {
struct hash_entry *prev, *next;
char *name;
char *str_val;
int int_val;
};

typedef struct hash_entry **hash_table;

#define NAME_TABLE_HASH_SIZE 101
#define START_COND_HASH_SIZE 101
#define CCL_HASH_SIZE 101

static struct hash_entry *ndtbl[NAME_TABLE_HASH_SIZE];
static struct hash_entry *sctbl[START_COND_HASH_SIZE];
static struct hash_entry *ccltab[CCL_HASH_SIZE];




static int addsym(char[], char *, int, hash_table, int);
static struct hash_entry *findsym (const char *sym, hash_table table,
int table_size);
static int hashfunct(const char *, int);







static int addsym (char sym[], char *str_def, int int_def, hash_table table, int table_size)
{
int hash_val = hashfunct (sym, table_size);
struct hash_entry *sym_entry = table[hash_val];
struct hash_entry *new_entry;
struct hash_entry *successor;

while (sym_entry) {
if (!strcmp (sym, sym_entry->name)) {
return -1;
}

sym_entry = sym_entry->next;
}


new_entry = malloc(sizeof(struct hash_entry));

if (new_entry == NULL)
flexfatal (_("symbol table memory allocation failed"));

if ((successor = table[hash_val]) != 0) {
new_entry->next = successor;
successor->prev = new_entry;
}
else
new_entry->next = NULL;

new_entry->prev = NULL;
new_entry->name = sym;
new_entry->str_val = str_def;
new_entry->int_val = int_def;

table[hash_val] = new_entry;

return 0;
}




void cclinstal (char ccltxt[], int cclnum)
{




(void) addsym (xstrdup(ccltxt),
(char *) 0, cclnum, ccltab, CCL_HASH_SIZE);
}







int ccllookup (char ccltxt[])
{
return findsym (ccltxt, ccltab, CCL_HASH_SIZE)->int_val;
}




static struct hash_entry *findsym (const char *sym, hash_table table, int table_size)
{
static struct hash_entry empty_entry = {
NULL, NULL, NULL, NULL, 0,
};
struct hash_entry *sym_entry =

table[hashfunct (sym, table_size)];

while (sym_entry) {
if (!strcmp (sym, sym_entry->name))
return sym_entry;
sym_entry = sym_entry->next;
}

return &empty_entry;
}



static int hashfunct (const char *str, int hash_size)
{
int hashval;
int locstr;

hashval = 0;
locstr = 0;

while (str[locstr]) {
hashval = (hashval << 1) + (unsigned char) str[locstr++];
hashval %= hash_size;
}

return hashval;
}




void ndinstal (const char *name, char definition[])
{

if (addsym (xstrdup(name),
xstrdup(definition), 0,
ndtbl, NAME_TABLE_HASH_SIZE))
synerr (_("name defined twice"));
}







char *ndlookup (const char *nd)
{
return findsym (nd, ndtbl, NAME_TABLE_HASH_SIZE)->str_val;
}




void scextend (void)
{
current_max_scs += MAX_SCS_INCREMENT;

++num_reallocs;

scset = reallocate_integer_array (scset, current_max_scs);
scbol = reallocate_integer_array (scbol, current_max_scs);
scxclu = reallocate_integer_array (scxclu, current_max_scs);
sceof = reallocate_integer_array (sceof, current_max_scs);
scname = reallocate_char_ptr_array (scname, current_max_scs);
}








void scinstal (const char *str, int xcluflg)
{

if (++lastsc >= current_max_scs)
scextend ();

scname[lastsc] = xstrdup(str);

if (addsym(scname[lastsc], NULL, lastsc,
sctbl, START_COND_HASH_SIZE))
format_pinpoint_message (_
("start condition %s declared twice"),
str);

scset[lastsc] = mkstate (SYM_EPSILON);
scbol[lastsc] = mkstate (SYM_EPSILON);
scxclu[lastsc] = xcluflg;
sceof[lastsc] = false;
}







int sclookup (const char *str)
{
return findsym (str, sctbl, START_COND_HASH_SIZE)->int_val;
}
