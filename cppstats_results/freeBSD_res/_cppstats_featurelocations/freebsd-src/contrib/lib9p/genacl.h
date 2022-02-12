









































































#if defined(__APPLE__)
#define HAVE_POSIX_ACLS
#define HAVE_DARWIN_ACLS
#endif

#if defined(__FreeBSD__)
#define HAVE_POSIX_ACLS
#define HAVE_FREEBSD_ACLS
#endif

#include <sys/types.h>
#include <sys/acl.h>




















struct l9p_acl;
struct l9p_fid;

void l9p_acl_free(struct l9p_acl *);


































#define L9P_ACE_READ_DATA 0x00001
#define L9P_ACE_LIST_DIRECTORY 0x00001
#define L9P_ACE_WRITE_DATA 0x00002
#define L9P_ACE_ADD_FILE 0x00002
#define L9P_ACE_APPEND_DATA 0x00004
#define L9P_ACE_ADD_SUBDIRECTORY 0x00004
#define L9P_ACE_READ_NAMED_ATTRS 0x00008
#define L9P_ACE_WRITE_NAMED_ATTRS 0x00010
#define L9P_ACE_EXECUTE 0x00020
#define L9P_ACE_DELETE_CHILD 0x00040
#define L9P_ACE_READ_ATTRIBUTES 0x00080
#define L9P_ACE_WRITE_ATTRIBUTES 0x00100
#define L9P_ACE_WRITE_RETENTION 0x00200
#define L9P_ACE_WRITE_RETENTION_HOLD 0x00400

#define L9P_ACE_DELETE 0x01000
#define L9P_ACE_READ_ACL 0x02000
#define L9P_ACE_WRITE_ACL 0x04000
#define L9P_ACE_WRITE_OWNER 0x08000
#define L9P_ACE_SYNCHRONIZE 0x10000







#define L9P_ACOP_UNLINK (L9P_ACE_DELETE_CHILD | L9P_ACE_DELETE)















struct l9p_acl_check_args {
uid_t aca_uid;
gid_t aca_gid;
gid_t *aca_groups;
size_t aca_ngroups;
struct l9p_acl *aca_parent;
struct stat *aca_pstat;
struct l9p_acl *aca_child;
struct stat *aca_cstat;
int aca_aclmode;
bool aca_superuser;
};













#define L9P_ACM_NFS_ACL 0x0001
#define L9P_ACM_ZFS_ACL 0x0002
#define L9P_ACM_POSIX_ACL 0x0004
#define L9P_ACM_STAT_MODE 0x0008



















int l9p_acl_check_access(int32_t op, struct l9p_acl_check_args *args);







int l9p_ace_mask_to_rwx(int32_t);






#define L9P_ACET_ACCESS_ALLOWED 0
#define L9P_ACET_ACCESS_DENIED 1
#define L9P_ACET_SYSTEM_AUDIT 2
#define L9P_ACET_SYSTEM_ALARM 3


#define L9P_ACEF_FILE_INHERIT_ACE 0x001
#define L9P_ACEF_DIRECTORY_INHERIT_ACE 0x002
#define L9P_ACEF_NO_PROPAGATE_INHERIT_ACE 0x004
#define L9P_ACEF_INHERIT_ONLY_ACE 0x008
#define L9P_ACEF_SUCCESSFUL_ACCESS_ACE_FLAG 0x010
#define L9P_ACEF_FAILED_ACCESS_ACE_FLAG 0x020
#define L9P_ACEF_IDENTIFIER_GROUP 0x040
#define L9P_ACEF_OWNER 0x080
#define L9P_ACEF_GROUP 0x100
#define L9P_ACEF_EVERYONE 0x200

#if defined(__APPLE__)
#define L9P_ACE_IDSIZE 16
#else
#define L9P_ACE_IDSIZE 4
#endif

struct l9p_ace {
uint16_t ace_type;
uint16_t ace_flags;
uint32_t ace_mask;
uint32_t ace_idsize;
unsigned char ace_idbytes[L9P_ACE_IDSIZE];
};

#define L9P_ACLTYPE_NFSv4 1
struct l9p_acl {
uint32_t acl_acetype;
uint32_t acl_nace;
uint32_t acl_aceasize;
struct l9p_ace acl_aces[];
};








#if defined(HAVE_DARWIN_ACLS)
struct l9p_acl *l9p_darwin_nfsv4acl_to_acl(acl_t acl);
#endif

#if defined(HAVE_FREEBSD_ACLS)
struct l9p_acl *l9p_freebsd_nfsv4acl_to_acl(acl_t acl);
#endif

#if defined(HAVE_POSIX_ACLS) && 0
struct l9p_acl *l9p_posix_acl_to_acl(acl_t acl);
#endif
