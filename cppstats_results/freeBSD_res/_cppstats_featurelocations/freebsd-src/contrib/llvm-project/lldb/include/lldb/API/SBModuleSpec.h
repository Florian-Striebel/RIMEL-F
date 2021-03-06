







#if !defined(LLDB_API_SBMODULESPEC_H)
#define LLDB_API_SBMODULESPEC_H

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBFileSpec.h"

namespace lldb {

class LLDB_API SBModuleSpec {
public:
SBModuleSpec();

SBModuleSpec(const SBModuleSpec &rhs);

~SBModuleSpec();

const SBModuleSpec &operator=(const SBModuleSpec &rhs);

explicit operator bool() const;

bool IsValid() const;

void Clear();









lldb::SBFileSpec GetFileSpec();

void SetFileSpec(const lldb::SBFileSpec &fspec);














lldb::SBFileSpec GetPlatformFileSpec();

void SetPlatformFileSpec(const lldb::SBFileSpec &fspec);

lldb::SBFileSpec GetSymbolFileSpec();

void SetSymbolFileSpec(const lldb::SBFileSpec &fspec);

const char *GetObjectName();

void SetObjectName(const char *name);

const char *GetTriple();

void SetTriple(const char *triple);

const uint8_t *GetUUIDBytes();

size_t GetUUIDLength();

bool SetUUIDBytes(const uint8_t *uuid, size_t uuid_len);

bool GetDescription(lldb::SBStream &description);

private:
friend class SBModuleSpecList;
friend class SBModule;
friend class SBTarget;

std::unique_ptr<lldb_private::ModuleSpec> m_opaque_up;
};

class SBModuleSpecList {
public:
SBModuleSpecList();

SBModuleSpecList(const SBModuleSpecList &rhs);

~SBModuleSpecList();

SBModuleSpecList &operator=(const SBModuleSpecList &rhs);

static SBModuleSpecList GetModuleSpecifications(const char *path);

void Append(const SBModuleSpec &spec);

void Append(const SBModuleSpecList &spec_list);

SBModuleSpec FindFirstMatchingSpec(const SBModuleSpec &match_spec);

SBModuleSpecList FindMatchingSpecs(const SBModuleSpec &match_spec);

size_t GetSize();

SBModuleSpec GetSpecAtIndex(size_t i);

bool GetDescription(lldb::SBStream &description);

private:
std::unique_ptr<lldb_private::ModuleSpecList> m_opaque_up;
};

}

#endif
