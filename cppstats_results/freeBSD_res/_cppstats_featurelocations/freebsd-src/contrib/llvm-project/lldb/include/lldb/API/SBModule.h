







#if !defined(LLDB_API_SBMODULE_H)
#define LLDB_API_SBMODULE_H

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBError.h"
#include "lldb/API/SBSection.h"
#include "lldb/API/SBSymbolContext.h"
#include "lldb/API/SBValueList.h"

namespace lldb {

class LLDB_API SBModule {
public:
SBModule();

SBModule(const SBModule &rhs);

SBModule(const SBModuleSpec &module_spec);

const SBModule &operator=(const SBModule &rhs);

SBModule(lldb::SBProcess &process, lldb::addr_t header_addr);

~SBModule();

explicit operator bool() const;

bool IsValid() const;

void Clear();









lldb::SBFileSpec GetFileSpec() const;














lldb::SBFileSpec GetPlatformFileSpec() const;

bool SetPlatformFileSpec(const lldb::SBFileSpec &platform_file);











lldb::SBFileSpec GetRemoteInstallFileSpec();

















bool SetRemoteInstallFileSpec(lldb::SBFileSpec &file);

lldb::ByteOrder GetByteOrder();

uint32_t GetAddressByteSize();

const char *GetTriple();

const uint8_t *GetUUIDBytes() const;

const char *GetUUIDString() const;

bool operator==(const lldb::SBModule &rhs) const;

bool operator!=(const lldb::SBModule &rhs) const;

lldb::SBSection FindSection(const char *sect_name);

lldb::SBAddress ResolveFileAddress(lldb::addr_t vm_addr);

lldb::SBSymbolContext
ResolveSymbolContextForAddress(const lldb::SBAddress &addr,
uint32_t resolve_scope);

bool GetDescription(lldb::SBStream &description);

uint32_t GetNumCompileUnits();

lldb::SBCompileUnit GetCompileUnitAtIndex(uint32_t);











lldb::SBSymbolContextList
FindCompileUnits(const lldb::SBFileSpec &sb_file_spec);

size_t GetNumSymbols();

lldb::SBSymbol GetSymbolAtIndex(size_t idx);

lldb::SBSymbol FindSymbol(const char *name,
lldb::SymbolType type = eSymbolTypeAny);

lldb::SBSymbolContextList FindSymbols(const char *name,
lldb::SymbolType type = eSymbolTypeAny);

size_t GetNumSections();

lldb::SBSection GetSectionAtIndex(size_t idx);















lldb::SBSymbolContextList
FindFunctions(const char *name,
uint32_t name_type_mask = lldb::eFunctionNameTypeAny);















lldb::SBValueList FindGlobalVariables(lldb::SBTarget &target,
const char *name, uint32_t max_matches);












lldb::SBValue FindFirstGlobalVariable(lldb::SBTarget &target,
const char *name);

lldb::SBType FindFirstType(const char *name);

lldb::SBTypeList FindTypes(const char *type);















lldb::SBType GetTypeByID(lldb::user_id_t uid);

lldb::SBType GetBasicType(lldb::BasicType type);













lldb::SBTypeList GetTypes(uint32_t type_mask = lldb::eTypeClassAny);







































uint32_t GetVersion(uint32_t *versions, uint32_t num_versions);










lldb::SBFileSpec GetSymbolFileSpec() const;

lldb::SBAddress GetObjectFileHeaderAddress() const;
lldb::SBAddress GetObjectFileEntryPointAddress() const;


static uint32_t GetNumberAllocatedModules();


static void GarbageCollectAllocatedModules();

private:
friend class SBAddress;
friend class SBFrame;
friend class SBSection;
friend class SBSymbolContext;
friend class SBTarget;
friend class SBType;

explicit SBModule(const lldb::ModuleSP &module_sp);

ModuleSP GetSP() const;

void SetSP(const ModuleSP &module_sp);

lldb::ModuleSP m_opaque_sp;
};

}

#endif
