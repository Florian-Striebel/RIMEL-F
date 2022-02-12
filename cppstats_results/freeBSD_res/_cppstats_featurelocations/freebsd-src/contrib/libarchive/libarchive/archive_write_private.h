


























#if !defined(ARCHIVE_WRITE_PRIVATE_H_INCLUDED)
#define ARCHIVE_WRITE_PRIVATE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#if !defined(__LIBARCHIVE_TEST)
#error This header is only to be used internally to libarchive.
#endif
#endif

#include "archive.h"
#include "archive_string.h"
#include "archive_private.h"

#define ARCHIVE_WRITE_FILTER_STATE_NEW 1U
#define ARCHIVE_WRITE_FILTER_STATE_OPEN 2U
#define ARCHIVE_WRITE_FILTER_STATE_CLOSED 4U
#define ARCHIVE_WRITE_FILTER_STATE_FATAL 0x8000U

struct archive_write;

struct archive_write_filter {
int64_t bytes_written;
struct archive *archive;
struct archive_write_filter *next_filter;
int (*options)(struct archive_write_filter *,
const char *key, const char *value);
int (*open)(struct archive_write_filter *);
int (*write)(struct archive_write_filter *, const void *, size_t);
int (*close)(struct archive_write_filter *);
int (*free)(struct archive_write_filter *);
void *data;
const char *name;
int code;
int bytes_per_block;
int bytes_in_last_block;
int state;
};

#if ARCHIVE_VERSION < 4000000
void __archive_write_filters_free(struct archive *);
#endif

struct archive_write_filter *__archive_write_allocate_filter(struct archive *);

int __archive_write_output(struct archive_write *, const void *, size_t);
int __archive_write_nulls(struct archive_write *, size_t);
int __archive_write_filter(struct archive_write_filter *, const void *, size_t);

struct archive_write {
struct archive archive;


int skip_file_set;
int64_t skip_file_dev;
int64_t skip_file_ino;


const unsigned char *nulls;
size_t null_length;


archive_open_callback *client_opener;
archive_write_callback *client_writer;
archive_close_callback *client_closer;
archive_free_callback *client_freer;
void *client_data;







int bytes_per_block;
int bytes_in_last_block;




struct archive_write_filter *filter_first;
struct archive_write_filter *filter_last;





void *format_data;
const char *format_name;
int (*format_init)(struct archive_write *);
int (*format_options)(struct archive_write *,
const char *key, const char *value);
int (*format_finish_entry)(struct archive_write *);
int (*format_write_header)(struct archive_write *,
struct archive_entry *);
ssize_t (*format_write_data)(struct archive_write *,
const void *buff, size_t);
int (*format_close)(struct archive_write *);
int (*format_free)(struct archive_write *);





char *passphrase;
archive_passphrase_callback *passphrase_callback;
void *passphrase_client_data;
};











int
__archive_write_format_header_ustar(struct archive_write *, char buff[512],
struct archive_entry *, int tartype, int strict,
struct archive_string_conv *);

struct archive_write_program_data;
struct archive_write_program_data * __archive_write_program_allocate(const char *program_name);
int __archive_write_program_free(struct archive_write_program_data *);
int __archive_write_program_open(struct archive_write_filter *,
struct archive_write_program_data *, const char *);
int __archive_write_program_close(struct archive_write_filter *,
struct archive_write_program_data *);
int __archive_write_program_write(struct archive_write_filter *,
struct archive_write_program_data *, const void *, size_t);




const char * __archive_write_get_passphrase(struct archive_write *a);
#endif
