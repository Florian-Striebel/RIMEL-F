














































#if !defined(_CASE_FILE_H_)
#define _CASE_FILE_H_


class CaseFile;
class Vdev;






typedef std::list< CaseFile *> CaseFileList;
























class CaseFile
{
public:











static CaseFile *Find(DevdCtl::Guid poolGUID, DevdCtl::Guid vdevGUID);










static CaseFile *Find(const string &physPath);







static void ReEvaluateByGuid(DevdCtl::Guid poolGUID,
const ZfsEvent &event);









static CaseFile &Create(Vdev &vdev);





static void DeSerialize();




static bool Empty();




static void LogAll();







static void PurgeAll();

DevdCtl::Guid PoolGUID() const;
DevdCtl::Guid VdevGUID() const;
vdev_state VdevState() const;
const string &PoolGUIDString() const;
const string &VdevGUIDString() const;
const string &PhysicalPath() const;















bool ReEvaluate(const string &devPath, const string &physPath,
Vdev *vdev);










virtual bool ReEvaluate(const ZfsEvent &event);




virtual void RegisterCallout(const DevdCtl::Event &event);











bool CloseIfSolved();




void Log();




bool ShouldDegrade() const;




bool ShouldFault() const;

protected:
enum {





ZFS_DEGRADE_IO_COUNT = 50
};

static CalloutFunc_t OnGracePeriodEnded;










static int DeSerializeSelector(const struct dirent *dirEntry);









static void DeSerializeFile(const char *fileName);


CaseFile(const Vdev &vdev);





virtual ~CaseFile();







virtual bool RefreshVdevState();




void PurgeEvents();




void PurgeTentativeEvents();




void Serialize();






void DeSerialize(std::ifstream &caseStream);







void SerializeEvList(const DevdCtl::EventList events, int fd,
const char* prefix=NULL) const;




virtual void Close();









void OnGracePeriodEnded();











bool ActivateSpare();











bool Replace(const char* vdev_type, const char* path, bool isspare);









Vdev BeingReplacedBy(zpool_handle_t *zhp);




static CaseFileList s_activeCases;




static const string s_caseFilePath;





static const timeval s_removeGracePeriod;





DevdCtl::EventList m_events;






DevdCtl::EventList m_tentativeEvents;

DevdCtl::Guid m_poolGUID;
DevdCtl::Guid m_vdevGUID;
vdev_state m_vdevState;
string m_poolGUIDString;
string m_vdevGUIDString;
string m_vdevPhysPath;




Callout m_tentativeTimer;

private:
nvlist_t *CaseVdev(zpool_handle_t *zhp) const;
};

inline DevdCtl::Guid
CaseFile::PoolGUID() const
{
return (m_poolGUID);
}

inline DevdCtl::Guid
CaseFile::VdevGUID() const
{
return (m_vdevGUID);
}

inline vdev_state
CaseFile::VdevState() const
{
return (m_vdevState);
}

inline const string &
CaseFile::PoolGUIDString() const
{
return (m_poolGUIDString);
}

inline const string &
CaseFile::VdevGUIDString() const
{
return (m_vdevGUIDString);
}

inline const string &
CaseFile::PhysicalPath() const
{
return (m_vdevPhysPath);
}

#endif
