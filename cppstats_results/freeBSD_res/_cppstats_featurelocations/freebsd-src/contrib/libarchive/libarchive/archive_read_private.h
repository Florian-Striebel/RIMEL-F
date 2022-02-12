


























#if !defined(ARCHIVE_READ_PRIVATE_H_INCLUDED)
#define ARCHIVE_READ_PRIVATE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#if !defined(__LIBARCHIVE_TEST)
#error This header is only to be used internally to libarchive.
#endif
#endif

#include "archive.h"
#include "archive_string.h"
#include "archive_private.h"

struct archive_read;
struct archive_read_filter_bidder;
struct archive_read_filter;
















struct archive_read_filter_bidder {

void *data;

const char *name;

int (*bid)(struct archive_read_filter_bidder *,
struct archive_read_filter *);

int (*init)(struct archive_read_filter *);

int (*options)(struct archive_read_filter_bidder *,
const char *key, const char *value);

int (*free)(struct archive_read_filter_bidder *);
};






struct archive_read_filter {
int64_t position;


struct archive_read_filter_bidder *bidder;
struct archive_read_filter *upstream;
struct archive_read *archive;

int (*open)(struct archive_read_filter *self);

ssize_t (*read)(struct archive_read_filter *, const void **);

int64_t (*skip)(struct archive_read_filter *self, int64_t request);

int64_t (*seek)(struct archive_read_filter *self, int64_t offset, int whence);

int (*close)(struct archive_read_filter *self);

int (*sswitch)(struct archive_read_filter *self, unsigned int iindex);

int (*read_header)(struct archive_read_filter *self, struct archive_entry *entry);

void *data;

const char *name;
int code;


char *buffer;
size_t buffer_size;
char *next;
size_t avail;
const void *client_buff;
size_t client_total;
const char *client_next;
size_t client_avail;
char end_of_file;
char closed;
char fatal;
};









struct archive_read_data_node {
int64_t begin_position;
int64_t total_size;
void *data;
};
struct archive_read_client {
archive_open_callback *opener;
archive_read_callback *reader;
archive_skip_callback *skipper;
archive_seek_callback *seeker;
archive_close_callback *closer;
archive_switch_callback *switcher;
unsigned int nodes;
unsigned int cursor;
int64_t position;
struct archive_read_data_node *dataset;
};
struct archive_read_passphrase {
char *passphrase;
struct archive_read_passphrase *next;
};

struct archive_read_extract {
struct archive *ad;


void (*extract_progress)(void *);
void *extract_progress_user_data;
};

struct archive_read {
struct archive archive;

struct archive_entry *entry;


int skip_file_set;
int64_t skip_file_dev;
int64_t skip_file_ino;


struct archive_read_client client;


struct archive_read_filter_bidder bidders[16];


struct archive_read_filter *filter;


int bypass_filter_bidding;


int64_t header_position;


unsigned int data_start_node;
unsigned int data_end_node;









struct archive_format_descriptor {
void *data;
const char *name;
int (*bid)(struct archive_read *, int best_bid);
int (*options)(struct archive_read *, const char *key,
const char *value);
int (*read_header)(struct archive_read *, struct archive_entry *);
int (*read_data)(struct archive_read *, const void **, size_t *, int64_t *);
int (*read_data_skip)(struct archive_read *);
int64_t (*seek_data)(struct archive_read *, int64_t, int);
int (*cleanup)(struct archive_read *);
int (*format_capabilties)(struct archive_read *);
int (*has_encrypted_entries)(struct archive_read *);
} formats[16];
struct archive_format_descriptor *format;




struct archive_read_extract *extract;
int (*cleanup_archive_extract)(struct archive_read *);




struct {
struct archive_read_passphrase *first;
struct archive_read_passphrase **last;
int candidate;
archive_passphrase_callback *callback;
void *client_data;
} passphrases;
};

int __archive_read_register_format(struct archive_read *a,
void *format_data,
const char *name,
int (*bid)(struct archive_read *, int),
int (*options)(struct archive_read *, const char *, const char *),
int (*read_header)(struct archive_read *, struct archive_entry *),
int (*read_data)(struct archive_read *, const void **, size_t *, int64_t *),
int (*read_data_skip)(struct archive_read *),
int64_t (*seek_data)(struct archive_read *, int64_t, int),
int (*cleanup)(struct archive_read *),
int (*format_capabilities)(struct archive_read *),
int (*has_encrypted_entries)(struct archive_read *));

int __archive_read_get_bidder(struct archive_read *a,
struct archive_read_filter_bidder **bidder);

const void *__archive_read_ahead(struct archive_read *, size_t, ssize_t *);
const void *__archive_read_filter_ahead(struct archive_read_filter *,
size_t, ssize_t *);
int64_t __archive_read_seek(struct archive_read*, int64_t, int);
int64_t __archive_read_filter_seek(struct archive_read_filter *, int64_t, int);
int64_t __archive_read_consume(struct archive_read *, int64_t);
int64_t __archive_read_filter_consume(struct archive_read_filter *, int64_t);
int __archive_read_header(struct archive_read *, struct archive_entry *);
int __archive_read_program(struct archive_read_filter *, const char *);
void __archive_read_free_filters(struct archive_read *);
struct archive_read_extract *__archive_read_get_extract(struct archive_read *);





void __archive_read_reset_passphrase(struct archive_read *a);
const char * __archive_read_next_passphrase(struct archive_read *a);
#endif
