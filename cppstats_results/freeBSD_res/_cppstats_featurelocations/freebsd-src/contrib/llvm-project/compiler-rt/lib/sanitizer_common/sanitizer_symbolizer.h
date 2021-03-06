
















#if !defined(SANITIZER_SYMBOLIZER_H)
#define SANITIZER_SYMBOLIZER_H

#include "sanitizer_common.h"
#include "sanitizer_mutex.h"
#include "sanitizer_vector.h"

namespace __sanitizer {

struct AddressInfo {


uptr address;

char *module;
uptr module_offset;
ModuleArch module_arch;

static const uptr kUnknown = ~(uptr)0;
char *function;
uptr function_offset;

char *file;
int line;
int column;

AddressInfo();

void Clear();
void FillModuleInfo(const char *mod_name, uptr mod_offset, ModuleArch arch);
};


struct SymbolizedStack {
SymbolizedStack *next;
AddressInfo info;
static SymbolizedStack *New(uptr addr);


void ClearAll();

private:
SymbolizedStack();
};


struct DataInfo {


char *module;
uptr module_offset;
ModuleArch module_arch;

char *file;
uptr line;
char *name;
uptr start;
uptr size;

DataInfo();
void Clear();
};

struct LocalInfo {
char *function_name = nullptr;
char *name = nullptr;
char *decl_file = nullptr;
unsigned decl_line = 0;

bool has_frame_offset = false;
bool has_size = false;
bool has_tag_offset = false;

sptr frame_offset;
uptr size;
uptr tag_offset;

void Clear();
};

struct FrameInfo {
char *module;
uptr module_offset;
ModuleArch module_arch;

InternalMmapVector<LocalInfo> locals;
void Clear();
};

class SymbolizerTool;

class Symbolizer final {
public:


static Symbolizer *GetOrInit();
static void LateInitialize();


SymbolizedStack *SymbolizePC(uptr address);
bool SymbolizeData(uptr address, DataInfo *info);
bool SymbolizeFrame(uptr address, FrameInfo *info);



bool GetModuleNameAndOffsetForPC(uptr pc, const char **module_name,
uptr *module_address);
const char *GetModuleNameForPc(uptr pc) {
const char *module_name = nullptr;
uptr unused;
if (GetModuleNameAndOffsetForPC(pc, &module_name, &unused))
return module_name;
return nullptr;
}


void Flush();

const char *Demangle(const char *name);





typedef void (*StartSymbolizationHook)();
typedef void (*EndSymbolizationHook)();

void AddHooks(StartSymbolizationHook start_hook,
EndSymbolizationHook end_hook);

void RefreshModules();
const LoadedModule *FindModuleForAddress(uptr address);

void InvalidateModuleList();

private:





class ModuleNameOwner {
public:
explicit ModuleNameOwner(BlockingMutex *synchronized_by)
: last_match_(nullptr), mu_(synchronized_by) {
storage_.reserve(kInitialCapacity);
}
const char *GetOwnedCopy(const char *str);

private:
static const uptr kInitialCapacity = 1000;
InternalMmapVector<const char*> storage_;
const char *last_match_;

BlockingMutex *mu_;
} module_names_;


static Symbolizer *PlatformInit();

bool FindModuleNameAndOffsetForAddress(uptr address, const char **module_name,
uptr *module_offset,
ModuleArch *module_arch);
ListOfModules modules_;
ListOfModules fallback_modules_;

bool modules_fresh_;


const char *PlatformDemangle(const char *name);

static Symbolizer *symbolizer_;
static StaticSpinMutex init_mu_;




BlockingMutex mu_;

IntrusiveList<SymbolizerTool> tools_;

explicit Symbolizer(IntrusiveList<SymbolizerTool> tools);

static LowLevelAllocator symbolizer_allocator_;

StartSymbolizationHook start_hook_;
EndSymbolizationHook end_hook_;
class SymbolizerScope {
public:
explicit SymbolizerScope(const Symbolizer *sym);
~SymbolizerScope();
private:
const Symbolizer *sym_;
};


void LateInitializeTools();
};

#if defined(SANITIZER_WINDOWS)
void InitializeDbgHelpIfNeeded();
#endif

}

#endif
