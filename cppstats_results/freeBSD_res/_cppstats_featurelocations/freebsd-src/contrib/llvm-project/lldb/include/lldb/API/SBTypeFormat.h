








#if !defined(LLDB_API_SBTYPEFORMAT_H)
#define LLDB_API_SBTYPEFORMAT_H

#include "lldb/API/SBDefines.h"

namespace lldb {

class LLDB_API SBTypeFormat {
public:
SBTypeFormat();

SBTypeFormat(lldb::Format format,
uint32_t options = 0);

SBTypeFormat(const char *type,
uint32_t options = 0);

SBTypeFormat(const lldb::SBTypeFormat &rhs);

~SBTypeFormat();

explicit operator bool() const;

bool IsValid() const;

lldb::Format GetFormat();

const char *GetTypeName();

uint32_t GetOptions();

void SetFormat(lldb::Format);

void SetTypeName(const char *);

void SetOptions(uint32_t);

bool GetDescription(lldb::SBStream &description,
lldb::DescriptionLevel description_level);

lldb::SBTypeFormat &operator=(const lldb::SBTypeFormat &rhs);

bool IsEqualTo(lldb::SBTypeFormat &rhs);

bool operator==(lldb::SBTypeFormat &rhs);

bool operator!=(lldb::SBTypeFormat &rhs);

protected:
friend class SBDebugger;
friend class SBTypeCategory;
friend class SBValue;

lldb::TypeFormatImplSP GetSP();

void SetSP(const lldb::TypeFormatImplSP &typeformat_impl_sp);

lldb::TypeFormatImplSP m_opaque_sp;

SBTypeFormat(const lldb::TypeFormatImplSP &);

enum class Type { eTypeKeepSame, eTypeFormat, eTypeEnum };

bool CopyOnWrite_Impl(Type);
};

}

#endif
