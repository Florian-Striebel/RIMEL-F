
























#if !defined(ATF_C_DETAIL_FS_H)
#define ATF_C_DETAIL_FS_H

#include <sys/types.h>
#include <sys/stat.h>

#include <stdarg.h>
#include <stdbool.h>

#include <atf-c/detail/dynstr.h>
#include <atf-c/error_fwd.h>





struct atf_fs_path {
atf_dynstr_t m_data;
};
typedef struct atf_fs_path atf_fs_path_t;


atf_error_t atf_fs_path_init_ap(atf_fs_path_t *, const char *, va_list);
atf_error_t atf_fs_path_init_fmt(atf_fs_path_t *, const char *, ...);
atf_error_t atf_fs_path_copy(atf_fs_path_t *, const atf_fs_path_t *);
void atf_fs_path_fini(atf_fs_path_t *);


atf_error_t atf_fs_path_branch_path(const atf_fs_path_t *, atf_fs_path_t *);
const char *atf_fs_path_cstring(const atf_fs_path_t *);
atf_error_t atf_fs_path_leaf_name(const atf_fs_path_t *, atf_dynstr_t *);
bool atf_fs_path_is_absolute(const atf_fs_path_t *);
bool atf_fs_path_is_root(const atf_fs_path_t *);


atf_error_t atf_fs_path_append_ap(atf_fs_path_t *, const char *, va_list);
atf_error_t atf_fs_path_append_fmt(atf_fs_path_t *, const char *, ...);
atf_error_t atf_fs_path_append_path(atf_fs_path_t *, const atf_fs_path_t *);
atf_error_t atf_fs_path_to_absolute(const atf_fs_path_t *, atf_fs_path_t *);


bool atf_equal_fs_path_fs_path(const atf_fs_path_t *,
const atf_fs_path_t *);





struct atf_fs_stat {
int m_type;
struct stat m_sb;
};
typedef struct atf_fs_stat atf_fs_stat_t;


extern const int atf_fs_stat_blk_type;
extern const int atf_fs_stat_chr_type;
extern const int atf_fs_stat_dir_type;
extern const int atf_fs_stat_fifo_type;
extern const int atf_fs_stat_lnk_type;
extern const int atf_fs_stat_reg_type;
extern const int atf_fs_stat_sock_type;
extern const int atf_fs_stat_wht_type;


atf_error_t atf_fs_stat_init(atf_fs_stat_t *, const atf_fs_path_t *);
void atf_fs_stat_copy(atf_fs_stat_t *, const atf_fs_stat_t *);
void atf_fs_stat_fini(atf_fs_stat_t *);


dev_t atf_fs_stat_get_device(const atf_fs_stat_t *);
ino_t atf_fs_stat_get_inode(const atf_fs_stat_t *);
mode_t atf_fs_stat_get_mode(const atf_fs_stat_t *);
off_t atf_fs_stat_get_size(const atf_fs_stat_t *);
int atf_fs_stat_get_type(const atf_fs_stat_t *);
bool atf_fs_stat_is_owner_readable(const atf_fs_stat_t *);
bool atf_fs_stat_is_owner_writable(const atf_fs_stat_t *);
bool atf_fs_stat_is_owner_executable(const atf_fs_stat_t *);
bool atf_fs_stat_is_group_readable(const atf_fs_stat_t *);
bool atf_fs_stat_is_group_writable(const atf_fs_stat_t *);
bool atf_fs_stat_is_group_executable(const atf_fs_stat_t *);
bool atf_fs_stat_is_other_readable(const atf_fs_stat_t *);
bool atf_fs_stat_is_other_writable(const atf_fs_stat_t *);
bool atf_fs_stat_is_other_executable(const atf_fs_stat_t *);





extern const int atf_fs_access_f;
extern const int atf_fs_access_r;
extern const int atf_fs_access_w;
extern const int atf_fs_access_x;

atf_error_t atf_fs_eaccess(const atf_fs_path_t *, int);
atf_error_t atf_fs_exists(const atf_fs_path_t *, bool *);
atf_error_t atf_fs_getcwd(atf_fs_path_t *);
atf_error_t atf_fs_mkdtemp(atf_fs_path_t *);
atf_error_t atf_fs_mkstemp(atf_fs_path_t *, int *);
atf_error_t atf_fs_rmdir(const atf_fs_path_t *);
atf_error_t atf_fs_unlink(const atf_fs_path_t *);

#endif
