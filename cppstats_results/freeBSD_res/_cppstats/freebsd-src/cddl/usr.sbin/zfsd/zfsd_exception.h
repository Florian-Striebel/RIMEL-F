struct zpool_handle;
typedef struct zpool_handle zpool_handle_t;
struct nvlist;
typedef struct nvlist nvlist_t;
class ZfsdException : public DevdCtl::Exception
{
public:
ZfsdException(const char *fmt, ...);
ZfsdException(zpool_handle_t *pool, const char *, ...);
ZfsdException(nvlist_t *poolConfig, const char *, ...);
virtual void Log() const;
private:
nvlist_t *m_poolConfig;
nvlist_t *m_vdevConfig;
};
