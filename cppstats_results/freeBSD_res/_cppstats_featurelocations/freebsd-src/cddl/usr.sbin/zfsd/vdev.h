













































#if !defined(_VDEV_H_)
#define _VDEV_H_


struct zpool_handle;
typedef struct zpool_handle zpool_handle_t;

struct nvlist;
typedef struct nvlist nvlist_t;







class Vdev
{
public:














Vdev(zpool_handle_t *pool, nvlist_t *vdevConfig);















Vdev(nvlist_t *poolConfig, nvlist_t *vdevConfig);








Vdev(nvlist_t *vdevConfig);




Vdev();





virtual ~Vdev();
bool DoesNotExist() const;




std::list<Vdev> Children();

virtual DevdCtl::Guid GUID() const;
bool IsSpare() const;
virtual DevdCtl::Guid PoolGUID() const;
virtual vdev_state State() const;
std::string Path() const;
virtual std::string PhysicalPath() const;
std::string GUIDString() const;
nvlist_t *PoolConfig() const;
nvlist_t *Config() const;
Vdev Parent();
Vdev RootVdev();
std::string Name(zpool_handle_t *, bool verbose) const;
bool IsSpare();
bool IsAvailableSpare() const;
bool IsActiveSpare() const;
bool IsResilvering() const;

private:
void VdevLookupGuid();
bool VdevLookupPoolGuid();
DevdCtl::Guid m_poolGUID;
DevdCtl::Guid m_vdevGUID;
nvlist_t *m_poolConfig;
nvlist_t *m_config;
};


extern Vdev NonexistentVdev;


inline Vdev::~Vdev()
{
}

inline DevdCtl::Guid
Vdev::PoolGUID() const
{
return (m_poolGUID);
}

inline DevdCtl::Guid
Vdev::GUID() const
{
return (m_vdevGUID);
}

inline nvlist_t *
Vdev::PoolConfig() const
{
return (m_poolConfig);
}

inline nvlist_t *
Vdev::Config() const
{
return (m_config);
}

inline bool
Vdev::DoesNotExist() const
{
return (m_config == NULL);
}

#endif
