







#if !defined(LLDB_API_SBREPRODUCER_H)
#define LLDB_API_SBREPRODUCER_H

#include "lldb/API/SBDefines.h"

namespace lldb_private {
namespace repro {
struct ReplayOptions;
}
}

namespace lldb {

class LLDB_API SBReplayOptions {
public:
SBReplayOptions();
SBReplayOptions(const SBReplayOptions &rhs);
~SBReplayOptions();

SBReplayOptions &operator=(const SBReplayOptions &rhs);

void SetVerify(bool verify);
bool GetVerify() const;

void SetCheckVersion(bool check);
bool GetCheckVersion() const;

private:
std::unique_ptr<lldb_private::repro::ReplayOptions> m_opaque_up;
};




class LLDB_API SBReproducer {
public:
static const char *Capture();
static const char *Capture(const char *path);
static const char *Replay(const char *path);
static const char *Replay(const char *path, bool skip_version_check);
static const char *Replay(const char *path, const SBReplayOptions &options);
static const char *PassiveReplay(const char *path);
static const char *Finalize(const char *path);
static const char *GetPath();
static bool SetAutoGenerate(bool b);
static bool Generate();






static void SetWorkingDirectory(const char *path);
};

}

#endif
