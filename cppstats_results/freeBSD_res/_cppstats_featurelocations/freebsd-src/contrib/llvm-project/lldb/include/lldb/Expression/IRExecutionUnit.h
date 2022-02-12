







#if !defined(LLDB_EXPRESSION_IREXECUTIONUNIT_H)
#define LLDB_EXPRESSION_IREXECUTIONUNIT_H

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/Module.h"

#include "lldb/Expression/IRMemoryMap.h"
#include "lldb/Symbol/ObjectFile.h"
#include "lldb/Symbol/SymbolContext.h"
#include "lldb/Utility/DataBufferHeap.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private.h"

namespace llvm {

class Module;
class ExecutionEngine;
class ObjectCache;

}

namespace lldb_private {

class Status;


















class IRExecutionUnit : public std::enable_shared_from_this<IRExecutionUnit>,
public IRMemoryMap,
public ObjectFileJITDelegate {
public:

IRExecutionUnit(std::unique_ptr<llvm::LLVMContext> &context_up,
std::unique_ptr<llvm::Module> &module_up, ConstString &name,
const lldb::TargetSP &target_sp, const SymbolContext &sym_ctx,
std::vector<std::string> &cpu_features);


~IRExecutionUnit() override;

ConstString GetFunctionName() { return m_name; }

llvm::Module *GetModule() { return m_module; }

llvm::Function *GetFunction() {
return ((m_module != nullptr) ? m_module->getFunction(m_name.GetStringRef())
: nullptr);
}

void GetRunnableInfo(Status &error, lldb::addr_t &func_addr,
lldb::addr_t &func_end);





lldb::addr_t WriteNow(const uint8_t *bytes, size_t size, Status &error);

void FreeNow(lldb::addr_t allocation);


lldb::ByteOrder GetByteOrder() const override;

uint32_t GetAddressByteSize() const override;

void PopulateSymtab(lldb_private::ObjectFile *obj_file,
lldb_private::Symtab &symtab) override;

void PopulateSectionList(lldb_private::ObjectFile *obj_file,
lldb_private::SectionList &section_list) override;

ArchSpec GetArchitecture() override;

lldb::ModuleSP GetJITModule();

lldb::addr_t FindSymbol(ConstString name, bool &missing_weak);

void GetStaticInitializers(std::vector<lldb::addr_t> &static_initializers);








struct JittedEntity {
ConstString m_name;
lldb::addr_t m_local_addr;
lldb::addr_t
m_remote_addr;















JittedEntity(const char *name,
lldb::addr_t local_addr = LLDB_INVALID_ADDRESS,
lldb::addr_t remote_addr = LLDB_INVALID_ADDRESS)
: m_name(name), m_local_addr(local_addr), m_remote_addr(remote_addr) {}
};

struct JittedFunction : JittedEntity {
bool m_external;
JittedFunction(const char *name, bool external,
lldb::addr_t local_addr = LLDB_INVALID_ADDRESS,
lldb::addr_t remote_addr = LLDB_INVALID_ADDRESS)
: JittedEntity(name, local_addr, remote_addr), m_external(external) {}
};

struct JittedGlobalVariable : JittedEntity {
JittedGlobalVariable(const char *name,
lldb::addr_t local_addr = LLDB_INVALID_ADDRESS,
lldb::addr_t remote_addr = LLDB_INVALID_ADDRESS)
: JittedEntity(name, local_addr, remote_addr) {}
};

const std::vector<JittedFunction> &GetJittedFunctions() {
return m_jitted_functions;
}

const std::vector<JittedGlobalVariable> &GetJittedGlobalVariables() {
return m_jitted_global_variables;
}

private:









lldb::addr_t GetRemoteAddressForLocal(lldb::addr_t local_address);

typedef std::pair<lldb::addr_t, uintptr_t> AddrRange;










AddrRange GetRemoteRangeForLocal(lldb::addr_t local_address);










bool CommitAllocations(lldb::ProcessSP &process_sp);





void ReportAllocations(llvm::ExecutionEngine &engine);








bool WriteData(lldb::ProcessSP &process_sp);

Status DisassembleFunction(Stream &stream, lldb::ProcessSP &process_sp);

struct SearchSpec;

void CollectCandidateCNames(std::vector<SearchSpec> &C_specs,
ConstString name);

void CollectCandidateCPlusPlusNames(std::vector<SearchSpec> &CPP_specs,
const std::vector<SearchSpec> &C_specs,
const SymbolContext &sc);

void CollectFallbackNames(std::vector<SearchSpec> &fallback_specs,
const std::vector<SearchSpec> &C_specs);

lldb::addr_t FindInSymbols(const std::vector<SearchSpec> &specs,
const lldb_private::SymbolContext &sc,
bool &symbol_was_missing_weak);

lldb::addr_t FindInRuntimes(const std::vector<SearchSpec> &specs,
const lldb_private::SymbolContext &sc);

lldb::addr_t FindInUserDefinedSymbols(const std::vector<SearchSpec> &specs,
const lldb_private::SymbolContext &sc);

void ReportSymbolLookupError(ConstString name);

class MemoryManager : public llvm::SectionMemoryManager {
public:
MemoryManager(IRExecutionUnit &parent);

~MemoryManager() override;















uint8_t *allocateCodeSection(uintptr_t Size, unsigned Alignment,
unsigned SectionID,
llvm::StringRef SectionName) override;

















uint8_t *allocateDataSection(uintptr_t Size, unsigned Alignment,
unsigned SectionID,
llvm::StringRef SectionName,
bool IsReadOnly) override;









bool finalizeMemory(std::string *ErrMsg) override {




return false;
}


void registerEHFrames(uint8_t *Addr, uint64_t LoadAddr,
size_t Size) override {}
void deregisterEHFrames() override {}

uint64_t getSymbolAddress(const std::string &Name) override;



uint64_t GetSymbolAddressAndPresence(const std::string &Name,
bool &missing_weak);

llvm::JITSymbol findSymbol(const std::string &Name) override;

void *getPointerToNamedFunction(const std::string &Name,
bool AbortOnFailure = true) override;

private:
std::unique_ptr<SectionMemoryManager> m_default_mm_up;






IRExecutionUnit &m_parent;
};

static const unsigned eSectionIDInvalid = (unsigned)-1;

enum class AllocationKind { Stub, Code, Data, Global, Bytes };

static lldb::SectionType
GetSectionTypeFromSectionName(const llvm::StringRef &name,
AllocationKind alloc_kind);





struct AllocationRecord {
std::string m_name;
lldb::addr_t m_process_address;
uintptr_t m_host_address;
uint32_t m_permissions;
lldb::SectionType m_sect_type;
size_t m_size;
unsigned m_alignment;
unsigned m_section_id;

AllocationRecord(uintptr_t host_address, uint32_t permissions,
lldb::SectionType sect_type, size_t size,
unsigned alignment, unsigned section_id, const char *name)
: m_name(), m_process_address(LLDB_INVALID_ADDRESS),
m_host_address(host_address), m_permissions(permissions),
m_sect_type(sect_type), m_size(size), m_alignment(alignment),
m_section_id(section_id) {
if (name && name[0])
m_name = name;
}

void dump(Log *log);
};

bool CommitOneAllocation(lldb::ProcessSP &process_sp, Status &error,
AllocationRecord &record);

typedef std::vector<AllocationRecord> RecordVector;
RecordVector m_records;

std::unique_ptr<llvm::LLVMContext> m_context_up;
std::unique_ptr<llvm::ExecutionEngine> m_execution_engine_up;
std::unique_ptr<llvm::ObjectCache> m_object_cache_up;
std::unique_ptr<llvm::Module>
m_module_up;
llvm::Module *m_module;
std::vector<std::string> m_cpu_features;
std::vector<JittedFunction> m_jitted_functions;


std::vector<JittedGlobalVariable> m_jitted_global_variables;




const ConstString m_name;
SymbolContext m_sym_ctx;
std::vector<ConstString> m_failed_lookups;

std::atomic<bool> m_did_jit;

lldb::addr_t m_function_load_addr;
lldb::addr_t m_function_end_load_addr;

bool m_strip_underscore = true;

bool m_reported_allocations;






};

}

#endif
