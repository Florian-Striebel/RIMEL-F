struct pidfh;
struct zpool_handle;
typedef struct zpool_handle zpool_handle_t;
struct zfs_handle;
typedef struct libzfs_handle libzfs_handle_t;
struct nvlist;
typedef struct nvlist nvlist_t;
typedef int LeafIterFunc(zpool_handle_t *, nvlist_t *, void *);
extern int g_debug;
extern libzfs_handle_t *g_zfsHandle;
class ZfsDaemon : public DevdCtl::Consumer
{
public:
static ZfsDaemon &Get();
static void WakeEventLoop();
static void RequestSystemRescan();
static void Run();
private:
ZfsDaemon();
~ZfsDaemon();
static VdevCallback_t VdevAddCaseFile;
void PurgeCaseFiles();
void BuildCaseFiles();
void RescanSystem();
void DetectMissedEvents();
void EventLoop();
static void InfoSignalHandler(int sigNum);
static void RescanSignalHandler(int sigNum);
static void QuitSignalHandler(int sigNum);
static void OpenPIDFile();
static void UpdatePIDFile();
static void ClosePIDFile();
static void InitializeSyslog();
static ZfsDaemon *s_theZfsDaemon;
static bool s_logCaseFiles;
static bool s_terminateEventLoop;
static char s_pidFilePath[];
static pidfh *s_pidFH;
static int s_signalPipeFD[2];
static bool s_systemRescanRequested;
static bool s_consumingEvents;
static DevdCtl::EventFactory::Record s_registryEntries[];
};
