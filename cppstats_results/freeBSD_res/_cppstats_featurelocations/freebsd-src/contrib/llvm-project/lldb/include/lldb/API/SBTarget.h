







#if !defined(LLDB_API_SBTARGET_H)
#define LLDB_API_SBTARGET_H

#include "lldb/API/SBAddress.h"
#include "lldb/API/SBAttachInfo.h"
#include "lldb/API/SBBreakpoint.h"
#include "lldb/API/SBBroadcaster.h"
#include "lldb/API/SBDefines.h"
#include "lldb/API/SBFileSpec.h"
#include "lldb/API/SBFileSpecList.h"
#include "lldb/API/SBLaunchInfo.h"
#include "lldb/API/SBSymbolContextList.h"
#include "lldb/API/SBType.h"
#include "lldb/API/SBValue.h"
#include "lldb/API/SBWatchpoint.h"

namespace lldb {

class SBPlatform;

class LLDB_API SBTarget {
public:

enum {
eBroadcastBitBreakpointChanged = (1 << 0),
eBroadcastBitModulesLoaded = (1 << 1),
eBroadcastBitModulesUnloaded = (1 << 2),
eBroadcastBitWatchpointChanged = (1 << 3),
eBroadcastBitSymbolsLoaded = (1 << 4)
};


SBTarget();

SBTarget(const lldb::SBTarget &rhs);

SBTarget(const lldb::TargetSP &target_sp);


~SBTarget();

const lldb::SBTarget &operator=(const lldb::SBTarget &rhs);

explicit operator bool() const;

bool IsValid() const;

static bool EventIsTargetEvent(const lldb::SBEvent &event);

static lldb::SBTarget GetTargetFromEvent(const lldb::SBEvent &event);

static uint32_t GetNumModulesFromEvent(const lldb::SBEvent &event);

static lldb::SBModule GetModuleAtIndexFromEvent(const uint32_t idx,
const lldb::SBEvent &event);

static const char *GetBroadcasterClassName();

lldb::SBProcess GetProcess();





void SetCollectingStats(bool v);






bool GetCollectingStats();





lldb::SBStructuredData GetStatistics();








lldb::SBPlatform GetPlatform();








SBEnvironment GetEnvironment();













SBError Install();






















































lldb::SBProcess Launch(SBListener &listener, char const **argv,
char const **envp, const char *stdin_path,
const char *stdout_path, const char *stderr_path,
const char *working_directory,
uint32_t launch_flags,
bool stop_at_entry, lldb::SBError &error);

SBProcess LoadCore(const char *core_file);
SBProcess LoadCore(const char *core_file, lldb::SBError &error);



























SBProcess LaunchSimple(const char **argv, const char **envp,
const char *working_directory);

SBProcess Launch(SBLaunchInfo &launch_info, SBError &error);

SBProcess Attach(SBAttachInfo &attach_info, SBError &error);

















lldb::SBProcess AttachToProcessWithID(SBListener &listener, lldb::pid_t pid,
lldb::SBError &error);




















lldb::SBProcess AttachToProcessWithName(SBListener &listener,
const char *name, bool wait_for,
lldb::SBError &error);




















lldb::SBProcess ConnectRemote(SBListener &listener, const char *url,
const char *plugin_name, SBError &error);

lldb::SBFileSpec GetExecutable();


void AppendImageSearchPath(const char *from, const char *to,
lldb::SBError &error);

bool AddModule(lldb::SBModule &module);

lldb::SBModule AddModule(const char *path, const char *triple,
const char *uuid);

lldb::SBModule AddModule(const char *path, const char *triple,
const char *uuid_cstr, const char *symfile);

lldb::SBModule AddModule(const SBModuleSpec &module_spec);

uint32_t GetNumModules() const;

lldb::SBModule GetModuleAtIndex(uint32_t idx);

bool RemoveModule(lldb::SBModule module);

lldb::SBDebugger GetDebugger() const;

lldb::SBModule FindModule(const lldb::SBFileSpec &file_spec);











lldb::SBSymbolContextList
FindCompileUnits(const lldb::SBFileSpec &sb_file_spec);

lldb::ByteOrder GetByteOrder();

uint32_t GetAddressByteSize();

const char *GetTriple();






uint32_t GetDataByteSize();






uint32_t GetCodeByteSize();













lldb::SBError SetSectionLoadAddress(lldb::SBSection section,
lldb::addr_t section_base_addr);










lldb::SBError ClearSectionLoadAddress(lldb::SBSection section);



















lldb::SBError SetModuleLoadAddress(lldb::SBModule module,
int64_t sections_offset);









lldb::SBError ClearModuleLoadAddress(lldb::SBModule module);
















lldb::SBSymbolContextList
FindFunctions(const char *name,
uint32_t name_type_mask = lldb::eFunctionNameTypeAny);












lldb::SBValueList FindGlobalVariables(const char *name, uint32_t max_matches);









lldb::SBValue FindFirstGlobalVariable(const char *name);














lldb::SBValueList FindGlobalVariables(const char *name, uint32_t max_matches,
MatchType matchtype);














lldb::SBSymbolContextList FindGlobalFunctions(const char *name,
uint32_t max_matches,
MatchType matchtype);

void Clear();








lldb::SBAddress ResolveFileAddress(lldb::addr_t file_addr);












lldb::SBAddress ResolveLoadAddress(lldb::addr_t vm_addr);





















lldb::SBAddress ResolvePastLoadAddress(uint32_t stop_id,
lldb::addr_t vm_addr);

SBSymbolContext ResolveSymbolContextForAddress(const SBAddress &addr,
uint32_t resolve_scope);





















size_t ReadMemory(const SBAddress addr, void *buf, size_t size,
lldb::SBError &error);

lldb::SBBreakpoint BreakpointCreateByLocation(const char *file,
uint32_t line);

lldb::SBBreakpoint
BreakpointCreateByLocation(const lldb::SBFileSpec &file_spec, uint32_t line);

lldb::SBBreakpoint
BreakpointCreateByLocation(const lldb::SBFileSpec &file_spec, uint32_t line,
lldb::addr_t offset);

lldb::SBBreakpoint
BreakpointCreateByLocation(const lldb::SBFileSpec &file_spec, uint32_t line,
lldb::addr_t offset, SBFileSpecList &module_list);

lldb::SBBreakpoint
BreakpointCreateByLocation(const lldb::SBFileSpec &file_spec, uint32_t line,
uint32_t column, lldb::addr_t offset,
SBFileSpecList &module_list);

lldb::SBBreakpoint
BreakpointCreateByLocation(const lldb::SBFileSpec &file_spec, uint32_t line,
uint32_t column, lldb::addr_t offset,
SBFileSpecList &module_list,
bool move_to_nearest_code);

lldb::SBBreakpoint BreakpointCreateByName(const char *symbol_name,
const char *module_name = nullptr);


lldb::SBBreakpoint
BreakpointCreateByName(const char *symbol_name,
const SBFileSpecList &module_list,
const SBFileSpecList &comp_unit_list);

lldb::SBBreakpoint BreakpointCreateByName(
const char *symbol_name,
uint32_t
name_type_mask,
const SBFileSpecList &module_list,
const SBFileSpecList &comp_unit_list);

lldb::SBBreakpoint BreakpointCreateByName(
const char *symbol_name,
uint32_t
name_type_mask,
lldb::LanguageType symbol_language,
const SBFileSpecList &module_list, const SBFileSpecList &comp_unit_list);

lldb::SBBreakpoint BreakpointCreateByNames(
const char *symbol_name[], uint32_t num_names,
uint32_t
name_type_mask,
const SBFileSpecList &module_list,
const SBFileSpecList &comp_unit_list);

lldb::SBBreakpoint BreakpointCreateByNames(
const char *symbol_name[], uint32_t num_names,
uint32_t
name_type_mask,
lldb::LanguageType symbol_language,
const SBFileSpecList &module_list, const SBFileSpecList &comp_unit_list);

lldb::SBBreakpoint BreakpointCreateByNames(
const char *symbol_name[], uint32_t num_names,
uint32_t
name_type_mask,
lldb::LanguageType symbol_language,
lldb::addr_t offset, const SBFileSpecList &module_list,
const SBFileSpecList &comp_unit_list);

lldb::SBBreakpoint BreakpointCreateByRegex(const char *symbol_name_regex,
const char *module_name = nullptr);

lldb::SBBreakpoint
BreakpointCreateByRegex(const char *symbol_name_regex,
const SBFileSpecList &module_list,
const SBFileSpecList &comp_unit_list);

lldb::SBBreakpoint BreakpointCreateByRegex(
const char *symbol_name_regex, lldb::LanguageType symbol_language,
const SBFileSpecList &module_list, const SBFileSpecList &comp_unit_list);

lldb::SBBreakpoint
BreakpointCreateBySourceRegex(const char *source_regex,
const SBFileSpec &source_file,
const char *module_name = nullptr);

lldb::SBBreakpoint
BreakpointCreateBySourceRegex(const char *source_regex,
const SBFileSpecList &module_list,
const SBFileSpecList &source_file);

lldb::SBBreakpoint BreakpointCreateBySourceRegex(
const char *source_regex, const SBFileSpecList &module_list,
const SBFileSpecList &source_file, const SBStringList &func_names);

lldb::SBBreakpoint BreakpointCreateForException(lldb::LanguageType language,
bool catch_bp, bool throw_bp);

lldb::SBBreakpoint BreakpointCreateByAddress(addr_t address);

lldb::SBBreakpoint BreakpointCreateBySBAddress(SBAddress &address);























lldb::SBBreakpoint BreakpointCreateFromScript(
const char *class_name,
SBStructuredData &extra_args,
const SBFileSpecList &module_list,
const SBFileSpecList &file_list,
bool request_hardware = false);












lldb::SBError BreakpointsCreateFromFile(SBFileSpec &source_file,
SBBreakpointList &new_bps);
















lldb::SBError BreakpointsCreateFromFile(SBFileSpec &source_file,
SBStringList &matching_names,
SBBreakpointList &new_bps);








lldb::SBError BreakpointsWriteToFile(SBFileSpec &dest_file);
















lldb::SBError BreakpointsWriteToFile(SBFileSpec &dest_file,
SBBreakpointList &bkpt_list,
bool append = false);

uint32_t GetNumBreakpoints() const;

lldb::SBBreakpoint GetBreakpointAtIndex(uint32_t idx) const;

bool BreakpointDelete(break_id_t break_id);

lldb::SBBreakpoint FindBreakpointByID(break_id_t break_id);



bool FindBreakpointsByName(const char *name, SBBreakpointList &bkpt_list);

void GetBreakpointNames(SBStringList &names);

void DeleteBreakpointName(const char *name);

bool EnableAllBreakpoints();

bool DisableAllBreakpoints();

bool DeleteAllBreakpoints();

uint32_t GetNumWatchpoints() const;

lldb::SBWatchpoint GetWatchpointAtIndex(uint32_t idx) const;

bool DeleteWatchpoint(lldb::watch_id_t watch_id);

lldb::SBWatchpoint FindWatchpointByID(lldb::watch_id_t watch_id);

lldb::SBWatchpoint WatchAddress(lldb::addr_t addr, size_t size, bool read,
bool write, SBError &error);

bool EnableAllWatchpoints();

bool DisableAllWatchpoints();

bool DeleteAllWatchpoints();

lldb::SBBroadcaster GetBroadcaster() const;

lldb::SBType FindFirstType(const char *type);

lldb::SBTypeList FindTypes(const char *type);

lldb::SBType GetBasicType(lldb::BasicType type);

lldb::SBValue CreateValueFromAddress(const char *name, lldb::SBAddress addr,
lldb::SBType type);

lldb::SBValue CreateValueFromData(const char *name, lldb::SBData data,
lldb::SBType type);

lldb::SBValue CreateValueFromExpression(const char *name, const char *expr);

SBSourceManager GetSourceManager();

lldb::SBInstructionList ReadInstructions(lldb::SBAddress base_addr,
uint32_t count);

lldb::SBInstructionList ReadInstructions(lldb::SBAddress base_addr,
uint32_t count,
const char *flavor_string);

lldb::SBInstructionList GetInstructions(lldb::SBAddress base_addr,
const void *buf, size_t size);




lldb::SBInstructionList GetInstructionsWithFlavor(lldb::SBAddress base_addr,
const char *flavor_string,
const void *buf,
size_t size);

lldb::SBInstructionList GetInstructions(lldb::addr_t base_addr,
const void *buf, size_t size);

lldb::SBInstructionList GetInstructionsWithFlavor(lldb::addr_t base_addr,
const char *flavor_string,
const void *buf,
size_t size);

lldb::SBSymbolContextList FindSymbols(const char *name,
lldb::SymbolType type = eSymbolTypeAny);

bool operator==(const lldb::SBTarget &rhs) const;

bool operator!=(const lldb::SBTarget &rhs) const;

bool GetDescription(lldb::SBStream &description,
lldb::DescriptionLevel description_level);

lldb::SBValue EvaluateExpression(const char *expr);

lldb::SBValue EvaluateExpression(const char *expr,
const SBExpressionOptions &options);

lldb::addr_t GetStackRedZoneSize();

bool IsLoaded(const lldb::SBModule &module) const;

lldb::SBLaunchInfo GetLaunchInfo() const;

void SetLaunchInfo(const lldb::SBLaunchInfo &launch_info);







lldb::SBTrace GetTrace();






lldb::SBTrace CreateTrace(SBError &error);

protected:
friend class SBAddress;
friend class SBBlock;
friend class SBBreakpointList;
friend class SBBreakpointNameImpl;
friend class SBDebugger;
friend class SBExecutionContext;
friend class SBFunction;
friend class SBInstruction;
friend class SBModule;
friend class SBPlatform;
friend class SBProcess;
friend class SBSection;
friend class SBSourceManager;
friend class SBSymbol;
friend class SBValue;
friend class SBVariablesOptions;




lldb::TargetSP GetSP() const;

void SetSP(const lldb::TargetSP &target_sp);

private:
lldb::TargetSP m_opaque_sp;
};

}

#endif
