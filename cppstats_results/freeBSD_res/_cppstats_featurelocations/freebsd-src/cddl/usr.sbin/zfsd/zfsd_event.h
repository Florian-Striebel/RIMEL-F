
















































#if !defined(_ZFSD_EVENT_H_)
#define _ZFSD_EVENT_H_


using std::string;


struct zpool_handle;
typedef struct zpool_handle zpool_handle_t;

struct nvlist;
typedef struct nvlist nvlist_t;


class ZfsEvent : public DevdCtl::ZfsEvent
{
public:

static BuildMethod Builder;

virtual DevdCtl::Event *DeepCopy() const;






virtual bool Process() const;

protected:

ZfsEvent(const ZfsEvent &src);


ZfsEvent(Type, DevdCtl::NVPairMap &, const string &);





virtual void CleanupSpares() const;
virtual void ProcessPoolEvent() const;
static VdevCallback_t TryDetach;
};

class GeomEvent : public DevdCtl::GeomEvent
{
public:
static BuildMethod Builder;

virtual DevdCtl::Event *DeepCopy() const;

virtual bool Process() const;

protected:

GeomEvent(const GeomEvent &src);


GeomEvent(Type, DevdCtl::NVPairMap &, const string &);















static bool OnlineByLabel(const string &devPath,
const string& physPath,
nvlist_t *devConfig);













static nvlist_t *ReadLabel(int devFd, bool &inUse, bool &degraded);

};
#endif
