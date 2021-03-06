







#if !defined(LLDB_API_SBVALUE_H)
#define LLDB_API_SBVALUE_H

#include "lldb/API/SBData.h"
#include "lldb/API/SBDefines.h"
#include "lldb/API/SBType.h"

class ValueImpl;
class ValueLocker;

namespace lldb {

class LLDB_API SBValue {
public:
SBValue();

SBValue(const lldb::SBValue &rhs);

lldb::SBValue &operator=(const lldb::SBValue &rhs);

~SBValue();

explicit operator bool() const;

bool IsValid();

void Clear();

SBError GetError();

lldb::user_id_t GetID();

const char *GetName();

const char *GetTypeName();

const char *GetDisplayTypeName();

size_t GetByteSize();

bool IsInScope();

lldb::Format GetFormat();

void SetFormat(lldb::Format format);

const char *GetValue();

int64_t GetValueAsSigned(lldb::SBError &error, int64_t fail_value = 0);

uint64_t GetValueAsUnsigned(lldb::SBError &error, uint64_t fail_value = 0);

int64_t GetValueAsSigned(int64_t fail_value = 0);

uint64_t GetValueAsUnsigned(uint64_t fail_value = 0);

ValueType GetValueType();



bool GetValueDidChange();

const char *GetSummary();

const char *GetSummary(lldb::SBStream &stream,
lldb::SBTypeSummaryOptions &options);

const char *GetObjectDescription();

lldb::SBValue GetDynamicValue(lldb::DynamicValueType use_dynamic);

lldb::SBValue GetStaticValue();

lldb::SBValue GetNonSyntheticValue();

lldb::DynamicValueType GetPreferDynamicValue();

void SetPreferDynamicValue(lldb::DynamicValueType use_dynamic);

bool GetPreferSyntheticValue();

void SetPreferSyntheticValue(bool use_synthetic);

bool IsDynamic();

bool IsSynthetic();

bool IsSyntheticChildrenGenerated();

void SetSyntheticChildrenGenerated(bool);

const char *GetLocation();


bool SetValueFromCString(const char *value_str);

bool SetValueFromCString(const char *value_str, lldb::SBError &error);

lldb::SBTypeFormat GetTypeFormat();

lldb::SBTypeSummary GetTypeSummary();

lldb::SBTypeFilter GetTypeFilter();

lldb::SBTypeSynthetic GetTypeSynthetic();

lldb::SBValue GetChildAtIndex(uint32_t idx);

lldb::SBValue CreateChildAtOffset(const char *name, uint32_t offset,
lldb::SBType type);


lldb::SBValue Cast(lldb::SBType type);

lldb::SBValue CreateValueFromExpression(const char *name,
const char *expression);

lldb::SBValue CreateValueFromExpression(const char *name,
const char *expression,
SBExpressionOptions &options);

lldb::SBValue CreateValueFromAddress(const char *name, lldb::addr_t address,
lldb::SBType type);



lldb::SBValue CreateValueFromData(const char *name, lldb::SBData data,
lldb::SBType type);





















































lldb::SBValue GetChildAtIndex(uint32_t idx,
lldb::DynamicValueType use_dynamic,
bool can_create_synthetic);



uint32_t GetIndexOfChildWithName(const char *name);



lldb::SBValue GetChildMemberWithName(const char *name);



lldb::SBValue GetChildMemberWithName(const char *name,
lldb::DynamicValueType use_dynamic);


lldb::SBValue GetValueForExpressionPath(const char *expr_path);

lldb::SBValue AddressOf();

lldb::addr_t GetLoadAddress();

lldb::SBAddress GetAddress();




















lldb::SBData GetPointeeData(uint32_t item_idx = 0, uint32_t item_count = 1);









lldb::SBData GetData();

bool SetData(lldb::SBData &data, lldb::SBError &error);

lldb::SBDeclaration GetDeclaration();















bool MightHaveChildren();

bool IsRuntimeSupportValue();

uint32_t GetNumChildren();

uint32_t GetNumChildren(uint32_t max);

void *GetOpaqueType();

lldb::SBTarget GetTarget();

lldb::SBProcess GetProcess();

lldb::SBThread GetThread();

lldb::SBFrame GetFrame();

lldb::SBValue Dereference();


bool TypeIsPointerType();

lldb::SBType GetType();

lldb::SBValue Persist();

bool GetDescription(lldb::SBStream &description);

bool GetExpressionPath(lldb::SBStream &description);

bool GetExpressionPath(lldb::SBStream &description,
bool qualify_cxx_base_classes);

lldb::SBValue EvaluateExpression(const char *expr) const;
lldb::SBValue EvaluateExpression(const char *expr,
const SBExpressionOptions &options) const;
lldb::SBValue EvaluateExpression(const char *expr,
const SBExpressionOptions &options,
const char *name) const;

SBValue(const lldb::ValueObjectSP &value_sp);

























lldb::SBWatchpoint Watch(bool resolve_location, bool read, bool write,
SBError &error);


lldb::SBWatchpoint Watch(bool resolve_location, bool read, bool write);

























lldb::SBWatchpoint WatchPointee(bool resolve_location, bool read, bool write,
SBError &error);












lldb::ValueObjectSP GetSP() const;

protected:
friend class SBBlock;
friend class SBFrame;
friend class SBTarget;
friend class SBThread;
friend class SBValueList;
























lldb::ValueObjectSP GetSP(ValueLocker &value_locker) const;



void SetSP(const lldb::ValueObjectSP &sp);

void SetSP(const lldb::ValueObjectSP &sp, bool use_synthetic);

void SetSP(const lldb::ValueObjectSP &sp, lldb::DynamicValueType use_dynamic);

void SetSP(const lldb::ValueObjectSP &sp, lldb::DynamicValueType use_dynamic,
bool use_synthetic);

void SetSP(const lldb::ValueObjectSP &sp, lldb::DynamicValueType use_dynamic,
bool use_synthetic, const char *name);

private:
typedef std::shared_ptr<ValueImpl> ValueImplSP;
ValueImplSP m_opaque_sp;

void SetSP(ValueImplSP impl_sp);
};

}

#endif
