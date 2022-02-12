struct zpool_handle;
typedef struct zpool_handle zpool_handle_t;
struct nvlist;
typedef struct nvlist nvlist_t;
class Vdev;
typedef bool VdevCallback_t(Vdev &vdev, void *cbArg);
class VdevIterator
{
public:
VdevIterator(zpool_handle_t *pool);
VdevIterator(nvlist_t *poolConfig);
void Reset();
nvlist_t *Next();
nvlist_t *Find(DevdCtl::Guid vdevGUID);
void Each(VdevCallback_t *cb, void *cbArg);
private:
nvlist_t *m_poolConfig;
std::list<nvlist_t *> m_vdevQueue;
};
