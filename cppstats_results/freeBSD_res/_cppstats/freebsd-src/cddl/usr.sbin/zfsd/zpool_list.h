using std::string;
struct zpool_handle;
typedef struct zpool_handle zpool_handle_t;
struct nvlist;
typedef struct nvlist nvlist_t;
class Vdev;
class ZpoolList;
typedef bool PoolFilter_t(zpool_handle_t *pool, nvlist_t *poolConfig,
void *filterArg);
class ZpoolList : public std::list<zpool_handle_t *>
{
public:
static PoolFilter_t ZpoolAll;
static PoolFilter_t ZpoolByGUID;
static PoolFilter_t ZpoolByName;
ZpoolList(PoolFilter_t *filter = ZpoolAll, void *filterArg = NULL);
~ZpoolList();
private:
static int LoadIterator(zpool_handle_t *pool, void *data);
PoolFilter_t *m_filter;
void *m_filterArg;
};
