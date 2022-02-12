








#if !defined(LLDB_API_SBTYPEENUMMEMBER_H)
#define LLDB_API_SBTYPEENUMMEMBER_H

#include "lldb/API/SBDefines.h"

namespace lldb {

class LLDB_API SBTypeEnumMember {
public:
SBTypeEnumMember();

SBTypeEnumMember(const SBTypeEnumMember &rhs);

~SBTypeEnumMember();

SBTypeEnumMember &operator=(const SBTypeEnumMember &rhs);

explicit operator bool() const;

bool IsValid() const;

int64_t GetValueAsSigned();

uint64_t GetValueAsUnsigned();

const char *GetName();

lldb::SBType GetType();

bool GetDescription(lldb::SBStream &description,
lldb::DescriptionLevel description_level);

protected:
friend class SBType;
friend class SBTypeEnumMemberList;

void reset(lldb_private::TypeEnumMemberImpl *);

lldb_private::TypeEnumMemberImpl &ref();

const lldb_private::TypeEnumMemberImpl &ref() const;

lldb::TypeEnumMemberImplSP m_opaque_sp;

SBTypeEnumMember(const lldb::TypeEnumMemberImplSP &);
};

class SBTypeEnumMemberList {
public:
SBTypeEnumMemberList();

SBTypeEnumMemberList(const SBTypeEnumMemberList &rhs);

~SBTypeEnumMemberList();

SBTypeEnumMemberList &operator=(const SBTypeEnumMemberList &rhs);

explicit operator bool() const;

bool IsValid();

void Append(SBTypeEnumMember entry);

SBTypeEnumMember GetTypeEnumMemberAtIndex(uint32_t index);

uint32_t GetSize();

private:
std::unique_ptr<lldb_private::TypeEnumMemberListImpl> m_opaque_up;
};

}

#endif
