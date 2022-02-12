











#if !defined(SANITIZER_PROCMAPS_H)
#define SANITIZER_PROCMAPS_H

#include "sanitizer_platform.h"

#if SANITIZER_LINUX || SANITIZER_FREEBSD || SANITIZER_NETBSD || SANITIZER_MAC || SANITIZER_SOLARIS || SANITIZER_FUCHSIA



#include "sanitizer_common.h"
#include "sanitizer_internal_defs.h"
#include "sanitizer_fuchsia.h"
#include "sanitizer_linux.h"
#include "sanitizer_mac.h"
#include "sanitizer_mutex.h"

namespace __sanitizer {


static const uptr kProtectionRead = 1;
static const uptr kProtectionWrite = 2;
static const uptr kProtectionExecute = 4;
static const uptr kProtectionShared = 8;

struct MemoryMappedSegmentData;

class MemoryMappedSegment {
public:
explicit MemoryMappedSegment(char *buff = nullptr, uptr size = 0)
: filename(buff), filename_size(size), data_(nullptr) {}
~MemoryMappedSegment() {}

bool IsReadable() const { return protection & kProtectionRead; }
bool IsWritable() const { return protection & kProtectionWrite; }
bool IsExecutable() const { return protection & kProtectionExecute; }
bool IsShared() const { return protection & kProtectionShared; }

void AddAddressRanges(LoadedModule *module);

uptr start;
uptr end;
uptr offset;
char *filename;
uptr filename_size;
uptr protection;
ModuleArch arch;
u8 uuid[kModuleUUIDSize];

private:
friend class MemoryMappingLayout;


MemoryMappedSegmentData *data_;
};

class MemoryMappingLayout {
public:
explicit MemoryMappingLayout(bool cache_enabled);
~MemoryMappingLayout();
bool Next(MemoryMappedSegment *segment);
bool Error() const;
void Reset();



static void CacheMemoryMappings();


void DumpListOfModules(InternalMmapVectorNoCtor<LoadedModule> *modules);

private:
void LoadFromCache();

MemoryMappingLayoutData data_;
};


bool GetCodeRangeForFile(const char *module, uptr *start, uptr *end);

bool IsDecimal(char c);
uptr ParseDecimal(const char **p);
bool IsHex(char c);
uptr ParseHex(const char **p);

}

#endif
#endif
