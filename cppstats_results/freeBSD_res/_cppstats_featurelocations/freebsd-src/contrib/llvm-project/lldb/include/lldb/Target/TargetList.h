







#if !defined(LLDB_TARGET_TARGETLIST_H)
#define LLDB_TARGET_TARGETLIST_H

#include <mutex>
#include <vector>

#include "lldb/Target/Target.h"
#include "lldb/Utility/Broadcaster.h"
#include "lldb/Utility/Iterable.h"

namespace lldb_private {

class TargetList : public Broadcaster {
private:
friend class Debugger;








TargetList(Debugger &debugger);

public:

enum { eBroadcastBitInterrupt = (1 << 0) };



static ConstString &GetStaticBroadcasterClass();

ConstString &GetBroadcasterClass() const override {
return GetStaticBroadcasterClass();
}

typedef std::vector<lldb::TargetSP> collection;
typedef LockingAdaptedIterable<collection, lldb::TargetSP, vector_adapter,
std::recursive_mutex>
TargetIterable;





































Status CreateTarget(Debugger &debugger, llvm::StringRef user_exe_path,
llvm::StringRef triple_str,
LoadDependentFiles get_dependent_modules,
const OptionGroupPlatform *platform_options,
lldb::TargetSP &target_sp);





Status CreateTarget(Debugger &debugger, llvm::StringRef user_exe_path,
const ArchSpec &arch,
LoadDependentFiles get_dependent_modules,
lldb::PlatformSP &platform_sp, lldb::TargetSP &target_sp);















bool DeleteTarget(lldb::TargetSP &target_sp);

int GetNumTargets() const;

lldb::TargetSP GetTargetAtIndex(uint32_t index) const;

uint32_t GetIndexOfTarget(lldb::TargetSP target_sp) const;






















lldb::TargetSP FindTargetWithExecutableAndArchitecture(
const FileSpec &exe_file_spec,
const ArchSpec *exe_arch_ptr = nullptr) const;











lldb::TargetSP FindTargetWithProcessID(lldb::pid_t pid) const;

lldb::TargetSP FindTargetWithProcess(lldb_private::Process *process) const;

lldb::TargetSP GetTargetSP(Target *target) const;














uint32_t SendAsyncInterrupt(lldb::pid_t pid = LLDB_INVALID_PROCESS_ID);

uint32_t SignalIfRunning(lldb::pid_t pid, int signo);

void SetSelectedTarget(uint32_t index);

void SetSelectedTarget(const lldb::TargetSP &target);

lldb::TargetSP GetSelectedTarget();

TargetIterable Targets() {
return TargetIterable(m_target_list, m_target_list_mutex);
}

private:
collection m_target_list;
mutable std::recursive_mutex m_target_list_mutex;
uint32_t m_selected_target_idx;

static Status CreateTargetInternal(
Debugger &debugger, llvm::StringRef user_exe_path,
llvm::StringRef triple_str, LoadDependentFiles load_dependent_files,
const OptionGroupPlatform *platform_options, lldb::TargetSP &target_sp);

static Status CreateTargetInternal(Debugger &debugger,
llvm::StringRef user_exe_path,
const ArchSpec &arch,
LoadDependentFiles get_dependent_modules,
lldb::PlatformSP &platform_sp,
lldb::TargetSP &target_sp);

void AddTargetInternal(lldb::TargetSP target_sp, bool do_select);

void SetSelectedTargetInternal(uint32_t index);

TargetList(const TargetList &) = delete;
const TargetList &operator=(const TargetList &) = delete;
};

}

#endif
