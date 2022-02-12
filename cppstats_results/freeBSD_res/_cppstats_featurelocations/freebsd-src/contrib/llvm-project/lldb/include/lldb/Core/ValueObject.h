







#if !defined(LLDB_CORE_VALUEOBJECT_H)
#define LLDB_CORE_VALUEOBJECT_H

#include "lldb/Core/Value.h"
#include "lldb/Symbol/CompilerType.h"
#include "lldb/Symbol/Type.h"
#include "lldb/Target/ExecutionContext.h"
#include "lldb/Target/Process.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/DataExtractor.h"
#include "lldb/Utility/SharedCluster.h"
#include "lldb/Utility/Status.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private-enumerations.h"
#include "lldb/lldb-types.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include <functional>
#include <initializer_list>
#include <map>
#include <mutex>
#include <string>
#include <utility>

#include <cstddef>
#include <cstdint>

namespace lldb_private {
class Declaration;
class DumpValueObjectOptions;
class EvaluateExpressionOptions;
class ExecutionContextScope;
class Log;
class Scalar;
class Stream;
class SymbolContextScope;
class TypeFormatImpl;
class TypeSummaryImpl;
class TypeSummaryOptions;


















































class ValueObject {
public:
enum GetExpressionPathFormat {
eGetExpressionPathFormatDereferencePointers = 1,
eGetExpressionPathFormatHonorPointers
};

enum ValueObjectRepresentationStyle {
eValueObjectRepresentationStyleValue = 1,
eValueObjectRepresentationStyleSummary,
eValueObjectRepresentationStyleLanguageSpecific,
eValueObjectRepresentationStyleLocation,
eValueObjectRepresentationStyleChildrenCount,
eValueObjectRepresentationStyleType,
eValueObjectRepresentationStyleName,
eValueObjectRepresentationStyleExpressionPath
};

enum ExpressionPathScanEndReason {

eExpressionPathScanEndReasonEndOfString = 1,

eExpressionPathScanEndReasonNoSuchChild,

eExpressionPathScanEndReasonNoSuchSyntheticChild,

eExpressionPathScanEndReasonEmptyRangeNotAllowed,

eExpressionPathScanEndReasonDotInsteadOfArrow,

eExpressionPathScanEndReasonArrowInsteadOfDot,

eExpressionPathScanEndReasonFragileIVarNotAllowed,

eExpressionPathScanEndReasonRangeOperatorNotAllowed,

eExpressionPathScanEndReasonRangeOperatorInvalid,

eExpressionPathScanEndReasonArrayRangeOperatorMet,

eExpressionPathScanEndReasonBitfieldRangeOperatorMet,

eExpressionPathScanEndReasonUnexpectedSymbol,

eExpressionPathScanEndReasonTakingAddressFailed,

eExpressionPathScanEndReasonDereferencingFailed,

eExpressionPathScanEndReasonRangeOperatorExpanded,

eExpressionPathScanEndReasonSyntheticValueMissing,
eExpressionPathScanEndReasonUnknown = 0xFFFF
};

enum ExpressionPathEndResultType {

eExpressionPathEndResultTypePlain = 1,

eExpressionPathEndResultTypeBitfield,

eExpressionPathEndResultTypeBoundedRange,

eExpressionPathEndResultTypeUnboundedRange,

eExpressionPathEndResultTypeValueObjectList,
eExpressionPathEndResultTypeInvalid = 0xFFFF
};

enum ExpressionPathAftermath {

eExpressionPathAftermathNothing = 1,

eExpressionPathAftermathDereference,

eExpressionPathAftermathTakeAddress
};

enum ClearUserVisibleDataItems {
eClearUserVisibleDataItemsNothing = 1u << 0,
eClearUserVisibleDataItemsValue = 1u << 1,
eClearUserVisibleDataItemsSummary = 1u << 2,
eClearUserVisibleDataItemsLocation = 1u << 3,
eClearUserVisibleDataItemsDescription = 1u << 4,
eClearUserVisibleDataItemsSyntheticChildren = 1u << 5,
eClearUserVisibleDataItemsAllStrings =
eClearUserVisibleDataItemsValue | eClearUserVisibleDataItemsSummary |
eClearUserVisibleDataItemsLocation |
eClearUserVisibleDataItemsDescription,
eClearUserVisibleDataItemsAll = 0xFFFF
};

struct GetValueForExpressionPathOptions {
enum class SyntheticChildrenTraversal {
None,
ToSynthetic,
FromSynthetic,
Both
};

bool m_check_dot_vs_arrow_syntax;
bool m_no_fragile_ivar;
bool m_allow_bitfields_syntax;
SyntheticChildrenTraversal m_synthetic_children_traversal;

GetValueForExpressionPathOptions(
bool dot = false, bool no_ivar = false, bool bitfield = true,
SyntheticChildrenTraversal synth_traverse =
SyntheticChildrenTraversal::ToSynthetic)
: m_check_dot_vs_arrow_syntax(dot), m_no_fragile_ivar(no_ivar),
m_allow_bitfields_syntax(bitfield),
m_synthetic_children_traversal(synth_traverse) {}

GetValueForExpressionPathOptions &DoCheckDotVsArrowSyntax() {
m_check_dot_vs_arrow_syntax = true;
return *this;
}

GetValueForExpressionPathOptions &DontCheckDotVsArrowSyntax() {
m_check_dot_vs_arrow_syntax = false;
return *this;
}

GetValueForExpressionPathOptions &DoAllowFragileIVar() {
m_no_fragile_ivar = false;
return *this;
}

GetValueForExpressionPathOptions &DontAllowFragileIVar() {
m_no_fragile_ivar = true;
return *this;
}

GetValueForExpressionPathOptions &DoAllowBitfieldSyntax() {
m_allow_bitfields_syntax = true;
return *this;
}

GetValueForExpressionPathOptions &DontAllowBitfieldSyntax() {
m_allow_bitfields_syntax = false;
return *this;
}

GetValueForExpressionPathOptions &
SetSyntheticChildrenTraversal(SyntheticChildrenTraversal traverse) {
m_synthetic_children_traversal = traverse;
return *this;
}

static const GetValueForExpressionPathOptions DefaultOptions() {
static GetValueForExpressionPathOptions g_default_options;

return g_default_options;
}
};

class EvaluationPoint {
public:
EvaluationPoint();

EvaluationPoint(ExecutionContextScope *exe_scope,
bool use_selected = false);

EvaluationPoint(const EvaluationPoint &rhs);

~EvaluationPoint();

const ExecutionContextRef &GetExecutionContextRef() const {
return m_exe_ctx_ref;
}

void SetIsConstant() {
SetUpdated();
m_mod_id.SetInvalid();
}

bool IsConstant() const { return !m_mod_id.IsValid(); }

ProcessModID GetModID() const { return m_mod_id; }

void SetUpdateID(ProcessModID new_id) { m_mod_id = new_id; }

void SetNeedsUpdate() { m_needs_update = true; }

void SetUpdated();

bool NeedsUpdating(bool accept_invalid_exe_ctx) {
SyncWithProcessState(accept_invalid_exe_ctx);
return m_needs_update;
}

bool IsValid() {
const bool accept_invalid_exe_ctx = false;
if (!m_mod_id.IsValid())
return false;
else if (SyncWithProcessState(accept_invalid_exe_ctx)) {
if (!m_mod_id.IsValid())
return false;
}
return true;
}

void SetInvalid() {


m_mod_id.SetInvalid();


m_needs_update = false;
}

private:
bool SyncWithProcessState(bool accept_invalid_exe_ctx);

ProcessModID m_mod_id;

ExecutionContextRef m_exe_ctx_ref;
bool m_needs_update = true;
};

virtual ~ValueObject();

const EvaluationPoint &GetUpdatePoint() const { return m_update_point; }

EvaluationPoint &GetUpdatePoint() { return m_update_point; }

const ExecutionContextRef &GetExecutionContextRef() const {
return m_update_point.GetExecutionContextRef();
}

lldb::TargetSP GetTargetSP() const {
return m_update_point.GetExecutionContextRef().GetTargetSP();
}

lldb::ProcessSP GetProcessSP() const {
return m_update_point.GetExecutionContextRef().GetProcessSP();
}

lldb::ThreadSP GetThreadSP() const {
return m_update_point.GetExecutionContextRef().GetThreadSP();
}

lldb::StackFrameSP GetFrameSP() const {
return m_update_point.GetExecutionContextRef().GetFrameSP();
}

void SetNeedsUpdate();

CompilerType GetCompilerType() { return MaybeCalculateCompleteType(); }


virtual TypeImpl GetTypeImpl() { return TypeImpl(GetCompilerType()); }

virtual bool CanProvideValue();


virtual llvm::Optional<uint64_t> GetByteSize() = 0;

virtual lldb::ValueType GetValueType() const = 0;


virtual ConstString GetTypeName() { return GetCompilerType().GetTypeName(); }

virtual ConstString GetDisplayTypeName() { return GetTypeName(); }

virtual ConstString GetQualifiedTypeName() {
return GetCompilerType().GetTypeName();
}

virtual lldb::LanguageType GetObjectRuntimeLanguage() {
return GetCompilerType().GetMinimumLanguage();
}

virtual uint32_t
GetTypeInfo(CompilerType *pointee_or_element_compiler_type = nullptr) {
return GetCompilerType().GetTypeInfo(pointee_or_element_compiler_type);
}

virtual bool IsPointerType() { return GetCompilerType().IsPointerType(); }

virtual bool IsArrayType() { return GetCompilerType().IsArrayType(); }

virtual bool IsScalarType() { return GetCompilerType().IsScalarType(); }

virtual bool IsPointerOrReferenceType() {
return GetCompilerType().IsPointerOrReferenceType();
}

virtual bool IsPossibleDynamicType();

bool IsNilReference();

bool IsUninitializedReference();

virtual bool IsBaseClass() { return false; }

bool IsBaseClass(uint32_t &depth);

virtual bool IsDereferenceOfParent() { return false; }

bool IsIntegerType(bool &is_signed) {
return GetCompilerType().IsIntegerType(is_signed);
}

virtual void GetExpressionPath(
Stream &s,
GetExpressionPathFormat = eGetExpressionPathFormatDereferencePointers);

lldb::ValueObjectSP GetValueForExpressionPath(
llvm::StringRef expression,
ExpressionPathScanEndReason *reason_to_stop = nullptr,
ExpressionPathEndResultType *final_value_type = nullptr,
const GetValueForExpressionPathOptions &options =
GetValueForExpressionPathOptions::DefaultOptions(),
ExpressionPathAftermath *final_task_on_target = nullptr);

virtual bool IsInScope() { return true; }

virtual lldb::offset_t GetByteOffset() { return 0; }

virtual uint32_t GetBitfieldBitSize() { return 0; }

virtual uint32_t GetBitfieldBitOffset() { return 0; }

bool IsBitfield() {
return (GetBitfieldBitSize() != 0) || (GetBitfieldBitOffset() != 0);
}

virtual bool IsArrayItemForPointer() {
return m_flags.m_is_array_item_for_pointer;
}

virtual const char *GetValueAsCString();

virtual bool GetValueAsCString(const lldb_private::TypeFormatImpl &format,
std::string &destination);

bool GetValueAsCString(lldb::Format format, std::string &destination);

virtual uint64_t GetValueAsUnsigned(uint64_t fail_value,
bool *success = nullptr);

virtual int64_t GetValueAsSigned(int64_t fail_value, bool *success = nullptr);

virtual bool SetValueFromCString(const char *value_str, Status &error);




virtual lldb::ModuleSP GetModule();

ValueObject *GetRoot();




ValueObject *FollowParentChain(std::function<bool(ValueObject *)>);

virtual bool GetDeclaration(Declaration &decl);


const Status &GetError();

ConstString GetName() const { return m_name; }


lldb::user_id_t GetID() const { return m_id.GetID(); }

virtual lldb::ValueObjectSP GetChildAtIndex(size_t idx, bool can_create);


lldb::ValueObjectSP GetChildAtIndexPath(llvm::ArrayRef<size_t> idxs,
size_t *index_of_error = nullptr);

lldb::ValueObjectSP
GetChildAtIndexPath(llvm::ArrayRef<std::pair<size_t, bool>> idxs,
size_t *index_of_error = nullptr);


lldb::ValueObjectSP GetChildAtNamePath(llvm::ArrayRef<ConstString> names,
ConstString *name_of_error = nullptr);

lldb::ValueObjectSP
GetChildAtNamePath(llvm::ArrayRef<std::pair<ConstString, bool>> names,
ConstString *name_of_error = nullptr);

virtual lldb::ValueObjectSP GetChildMemberWithName(ConstString name,
bool can_create);

virtual size_t GetIndexOfChildWithName(ConstString name);

size_t GetNumChildren(uint32_t max = UINT32_MAX);

const Value &GetValue() const { return m_value; }

Value &GetValue() { return m_value; }

virtual bool ResolveValue(Scalar &scalar);




virtual bool IsLogicalTrue(Status &error);

virtual const char *GetLocationAsCString() {
return GetLocationAsCStringImpl(m_value, m_data);
}

const char *
GetSummaryAsCString(lldb::LanguageType lang = lldb::eLanguageTypeUnknown);

bool
GetSummaryAsCString(TypeSummaryImpl *summary_ptr, std::string &destination,
lldb::LanguageType lang = lldb::eLanguageTypeUnknown);

bool GetSummaryAsCString(std::string &destination,
const TypeSummaryOptions &options);

bool GetSummaryAsCString(TypeSummaryImpl *summary_ptr,
std::string &destination,
const TypeSummaryOptions &options);

const char *GetObjectDescription();

bool HasSpecialPrintableRepresentation(
ValueObjectRepresentationStyle val_obj_display,
lldb::Format custom_format);

enum class PrintableRepresentationSpecialCases : bool {
eDisable = false,
eAllow = true
};

bool
DumpPrintableRepresentation(Stream &s,
ValueObjectRepresentationStyle val_obj_display =
eValueObjectRepresentationStyleSummary,
lldb::Format custom_format = lldb::eFormatInvalid,
PrintableRepresentationSpecialCases special =
PrintableRepresentationSpecialCases::eAllow,
bool do_dump_error = true);
bool GetValueIsValid() const { return m_flags.m_value_is_valid; }



bool GetValueDidChange() { return m_flags.m_value_did_change; }

bool UpdateValueIfNeeded(bool update_format = true);

bool UpdateFormatsIfNeeded();

lldb::ValueObjectSP GetSP() { return m_manager->GetSharedPointer(this); }




void SetName(ConstString name) { m_name = name; }

virtual lldb::addr_t GetAddressOf(bool scalar_is_load_address = true,
AddressType *address_type = nullptr);

lldb::addr_t GetPointerValue(AddressType *address_type = nullptr);

lldb::ValueObjectSP GetSyntheticChild(ConstString key) const;

lldb::ValueObjectSP GetSyntheticArrayMember(size_t index, bool can_create);

lldb::ValueObjectSP GetSyntheticBitFieldChild(uint32_t from, uint32_t to,
bool can_create);

lldb::ValueObjectSP GetSyntheticExpressionPathChild(const char *expression,
bool can_create);

virtual lldb::ValueObjectSP
GetSyntheticChildAtOffset(uint32_t offset, const CompilerType &type,
bool can_create,
ConstString name_const_str = ConstString());

virtual lldb::ValueObjectSP
GetSyntheticBase(uint32_t offset, const CompilerType &type, bool can_create,
ConstString name_const_str = ConstString());

virtual lldb::ValueObjectSP GetDynamicValue(lldb::DynamicValueType valueType);

lldb::DynamicValueType GetDynamicValueType();

virtual lldb::ValueObjectSP GetStaticValue() { return GetSP(); }

virtual lldb::ValueObjectSP GetNonSyntheticValue() { return GetSP(); }

lldb::ValueObjectSP GetSyntheticValue();

virtual bool HasSyntheticValue();

virtual bool IsSynthetic() { return false; }

lldb::ValueObjectSP
GetQualifiedRepresentationIfAvailable(lldb::DynamicValueType dynValue,
bool synthValue);

virtual lldb::ValueObjectSP CreateConstantValue(ConstString name);

virtual lldb::ValueObjectSP Dereference(Status &error);





virtual lldb::ValueObjectSP Clone(ConstString new_name);

virtual lldb::ValueObjectSP AddressOf(Status &error);

virtual lldb::addr_t GetLiveAddress() { return LLDB_INVALID_ADDRESS; }

virtual void SetLiveAddress(lldb::addr_t addr = LLDB_INVALID_ADDRESS,
AddressType address_type = eAddressTypeLoad) {}

virtual lldb::ValueObjectSP Cast(const CompilerType &compiler_type);

virtual lldb::ValueObjectSP CastPointerType(const char *name,
CompilerType &ast_type);

virtual lldb::ValueObjectSP CastPointerType(const char *name,
lldb::TypeSP &type_sp);



virtual void ValueUpdated() {
ClearUserVisibleData(eClearUserVisibleDataItemsValue |
eClearUserVisibleDataItemsSummary |
eClearUserVisibleDataItemsDescription);
}

virtual bool IsDynamic() { return false; }

virtual bool DoesProvideSyntheticValue() { return false; }

virtual bool IsSyntheticChildrenGenerated() {
return m_flags.m_is_synthetic_children_generated;
}

virtual void SetSyntheticChildrenGenerated(bool b) {
m_flags.m_is_synthetic_children_generated = b;
}

virtual SymbolContextScope *GetSymbolContextScope();

void Dump(Stream &s);

void Dump(Stream &s, const DumpValueObjectOptions &options);

static lldb::ValueObjectSP
CreateValueObjectFromExpression(llvm::StringRef name,
llvm::StringRef expression,
const ExecutionContext &exe_ctx);

static lldb::ValueObjectSP
CreateValueObjectFromExpression(llvm::StringRef name,
llvm::StringRef expression,
const ExecutionContext &exe_ctx,
const EvaluateExpressionOptions &options);

static lldb::ValueObjectSP
CreateValueObjectFromAddress(llvm::StringRef name, uint64_t address,
const ExecutionContext &exe_ctx,
CompilerType type);

static lldb::ValueObjectSP
CreateValueObjectFromData(llvm::StringRef name, const DataExtractor &data,
const ExecutionContext &exe_ctx, CompilerType type);

lldb::ValueObjectSP Persist();



bool IsCStringContainer(bool check_pointer = false);

std::pair<size_t, bool>
ReadPointedString(lldb::DataBufferSP &buffer_sp, Status &error,
uint32_t max_length = 0, bool honor_array = true,
lldb::Format item_format = lldb::eFormatCharArray);

virtual size_t GetPointeeData(DataExtractor &data, uint32_t item_idx = 0,
uint32_t item_count = 1);

virtual uint64_t GetData(DataExtractor &data, Status &error);

virtual bool SetData(DataExtractor &data, Status &error);

virtual bool GetIsConstant() const { return m_update_point.IsConstant(); }

bool NeedsUpdating() {
const bool accept_invalid_exe_ctx =
(CanUpdateWithInvalidExecutionContext() == eLazyBoolYes);
return m_update_point.NeedsUpdating(accept_invalid_exe_ctx);
}

void SetIsConstant() { m_update_point.SetIsConstant(); }

lldb::Format GetFormat() const;

virtual void SetFormat(lldb::Format format) {
if (format != m_format)
ClearUserVisibleData(eClearUserVisibleDataItemsValue);
m_format = format;
}

virtual lldb::LanguageType GetPreferredDisplayLanguage();

void SetPreferredDisplayLanguage(lldb::LanguageType lt) {
m_preferred_display_language = lt;
}

lldb::TypeSummaryImplSP GetSummaryFormat() {
UpdateFormatsIfNeeded();
return m_type_summary_sp;
}

void SetSummaryFormat(lldb::TypeSummaryImplSP format) {
m_type_summary_sp = std::move(format);
ClearUserVisibleData(eClearUserVisibleDataItemsSummary);
}

void SetValueFormat(lldb::TypeFormatImplSP format) {
m_type_format_sp = std::move(format);
ClearUserVisibleData(eClearUserVisibleDataItemsValue);
}

lldb::TypeFormatImplSP GetValueFormat() {
UpdateFormatsIfNeeded();
return m_type_format_sp;
}

void SetSyntheticChildren(const lldb::SyntheticChildrenSP &synth_sp) {
if (synth_sp.get() == m_synthetic_children_sp.get())
return;
ClearUserVisibleData(eClearUserVisibleDataItemsSyntheticChildren);
m_synthetic_children_sp = synth_sp;
}

lldb::SyntheticChildrenSP GetSyntheticChildren() {
UpdateFormatsIfNeeded();
return m_synthetic_children_sp;
}





virtual ValueObject *GetParent() { return m_parent; }

virtual const ValueObject *GetParent() const { return m_parent; }

ValueObject *GetNonBaseClassParent();

void SetAddressTypeOfChildren(AddressType at) {
m_address_type_of_ptr_or_ref_children = at;
}

AddressType GetAddressTypeOfChildren();

void SetHasCompleteType() {
m_flags.m_did_calculate_complete_objc_class_type = true;
}















virtual bool MightHaveChildren();

virtual lldb::VariableSP GetVariable() { return nullptr; }

virtual bool IsRuntimeSupportValue();

virtual uint64_t GetLanguageFlags() { return m_language_flags; }

virtual void SetLanguageFlags(uint64_t flags) { m_language_flags = flags; }

protected:
typedef ClusterManager<ValueObject> ValueObjectManager;

class ChildrenManager {
public:
ChildrenManager() : m_mutex(), m_children() {}

bool HasChildAtIndex(size_t idx) {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
return (m_children.find(idx) != m_children.end());
}

ValueObject *GetChildAtIndex(size_t idx) {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
const auto iter = m_children.find(idx);
return ((iter == m_children.end()) ? nullptr : iter->second);
}

void SetChildAtIndex(size_t idx, ValueObject *valobj) {

ChildrenPair pair(idx, valobj);
std::lock_guard<std::recursive_mutex> guard(m_mutex);
m_children.insert(pair);
}

void SetChildrenCount(size_t count) { Clear(count); }

size_t GetChildrenCount() { return m_children_count; }

void Clear(size_t new_count = 0) {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
m_children_count = new_count;
m_children.clear();
}

private:
typedef std::map<size_t, ValueObject *> ChildrenMap;
typedef ChildrenMap::iterator ChildrenIterator;
typedef ChildrenMap::value_type ChildrenPair;
std::recursive_mutex m_mutex;
ChildrenMap m_children;
size_t m_children_count = 0;
};




ValueObject *m_parent = nullptr;


ValueObject *m_root = nullptr;



EvaluationPoint m_update_point;

ConstString m_name;

DataExtractor m_data;
Value m_value;


Status m_error;

std::string m_value_str;

std::string m_old_value_str;

std::string m_location_str;

std::string m_summary_str;


std::string m_object_desc_str;

CompilerType m_override_type;







ValueObjectManager *m_manager = nullptr;

ChildrenManager m_children;
std::map<ConstString, ValueObject *> m_synthetic_children;

ValueObject *m_dynamic_value = nullptr;
ValueObject *m_synthetic_value = nullptr;
ValueObject *m_deref_valobj = nullptr;



lldb::ValueObjectSP m_addr_of_valobj_sp;

lldb::Format m_format = lldb::eFormatDefault;
lldb::Format m_last_format = lldb::eFormatDefault;
uint32_t m_last_format_mgr_revision = 0;
lldb::TypeSummaryImplSP m_type_summary_sp;
lldb::TypeFormatImplSP m_type_format_sp;
lldb::SyntheticChildrenSP m_synthetic_children_sp;
ProcessModID m_user_id_of_forced_summary;
AddressType m_address_type_of_ptr_or_ref_children = eAddressTypeInvalid;

llvm::SmallVector<uint8_t, 16> m_value_checksum;

lldb::LanguageType m_preferred_display_language = lldb::eLanguageTypeUnknown;

uint64_t m_language_flags = 0;


UserID m_id;



struct Bitflags {
bool m_value_is_valid : 1, m_value_did_change : 1,
m_children_count_valid : 1, m_old_value_valid : 1,
m_is_deref_of_parent : 1, m_is_array_item_for_pointer : 1,
m_is_bitfield_for_scalar : 1, m_is_child_at_offset : 1,
m_is_getting_summary : 1, m_did_calculate_complete_objc_class_type : 1,
m_is_synthetic_children_generated : 1;
Bitflags() {
m_value_is_valid = false;
m_value_did_change = false;
m_children_count_valid = false;
m_old_value_valid = false;
m_is_deref_of_parent = false;
m_is_array_item_for_pointer = false;
m_is_bitfield_for_scalar = false;
m_is_child_at_offset = false;
m_is_getting_summary = false;
m_did_calculate_complete_objc_class_type = false;
m_is_synthetic_children_generated = false;
}
} m_flags;

friend class ValueObjectChild;
friend class ExpressionVariable;
friend class Target;
friend class ValueObjectConstResultImpl;
friend class ValueObjectSynthetic;



ValueObject(ExecutionContextScope *exe_scope, ValueObjectManager &manager,
AddressType child_ptr_or_ref_addr_type = eAddressTypeLoad);



ValueObject(ValueObject &parent);

ValueObjectManager *GetManager() { return m_manager; }

virtual bool UpdateValue() = 0;

virtual LazyBool CanUpdateWithInvalidExecutionContext() {
return eLazyBoolCalculate;
}

virtual void CalculateDynamicValue(lldb::DynamicValueType use_dynamic);

virtual lldb::DynamicValueType GetDynamicValueTypeImpl() {
return lldb::eNoDynamicValues;
}

virtual bool HasDynamicValueTypeInfo() { return false; }

virtual void CalculateSyntheticValue();




virtual ValueObject *CreateChildAtIndex(size_t idx,
bool synthetic_array_member,
int32_t synthetic_index);


virtual size_t CalculateNumChildren(uint32_t max = UINT32_MAX) = 0;

void SetNumChildren(size_t num_children);

void SetValueDidChange(bool value_changed) {
m_flags.m_value_did_change = value_changed;
}

void SetValueIsValid(bool valid) { m_flags.m_value_is_valid = valid; }

void ClearUserVisibleData(
uint32_t items = ValueObject::eClearUserVisibleDataItemsAllStrings);

void AddSyntheticChild(ConstString key, ValueObject *valobj);

DataExtractor &GetDataExtractor();

void ClearDynamicTypeInformation();



virtual CompilerType GetCompilerTypeImpl() = 0;

const char *GetLocationAsCStringImpl(const Value &value,
const DataExtractor &data);

bool IsChecksumEmpty() { return m_value_checksum.empty(); }

void SetPreferredDisplayLanguageIfNeeded(lldb::LanguageType);

protected:
virtual void DoUpdateChildrenAddressType(ValueObject &valobj) { return; };

private:
virtual CompilerType MaybeCalculateCompleteType();
void UpdateChildrenAddressType() {
GetRoot()->DoUpdateChildrenAddressType(*this);
}

lldb::ValueObjectSP GetValueForExpressionPath_Impl(
llvm::StringRef expression_cstr,
ExpressionPathScanEndReason *reason_to_stop,
ExpressionPathEndResultType *final_value_type,
const GetValueForExpressionPathOptions &options,
ExpressionPathAftermath *final_task_on_target);

ValueObject(const ValueObject &) = delete;
const ValueObject &operator=(const ValueObject &) = delete;
};

}

#endif
