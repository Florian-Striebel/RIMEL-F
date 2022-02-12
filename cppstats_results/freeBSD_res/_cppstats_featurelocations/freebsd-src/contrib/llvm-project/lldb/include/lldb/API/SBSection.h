







#if !defined(LLDB_API_SBSECTION_H)
#define LLDB_API_SBSECTION_H

#include "lldb/API/SBData.h"
#include "lldb/API/SBDefines.h"

namespace lldb {

class LLDB_API SBSection {
public:
SBSection();

SBSection(const lldb::SBSection &rhs);

~SBSection();

const lldb::SBSection &operator=(const lldb::SBSection &rhs);

explicit operator bool() const;

bool IsValid() const;

const char *GetName();

lldb::SBSection GetParent();

lldb::SBSection FindSubSection(const char *sect_name);

size_t GetNumSubSections();

lldb::SBSection GetSubSectionAtIndex(size_t idx);

lldb::addr_t GetFileAddress();

lldb::addr_t GetLoadAddress(lldb::SBTarget &target);

lldb::addr_t GetByteSize();

uint64_t GetFileOffset();

uint64_t GetFileByteSize();

lldb::SBData GetSectionData();

lldb::SBData GetSectionData(uint64_t offset, uint64_t size);

SectionType GetSectionType();











uint32_t
GetPermissions() const;








uint32_t GetTargetByteSize();

bool operator==(const lldb::SBSection &rhs);

bool operator!=(const lldb::SBSection &rhs);

bool GetDescription(lldb::SBStream &description);

private:
friend class SBAddress;
friend class SBModule;
friend class SBTarget;

SBSection(const lldb::SectionSP &section_sp);

lldb::SectionSP GetSP() const;

void SetSP(const lldb::SectionSP &section_sp);

lldb::SectionWP m_opaque_wp;
};

}

#endif
