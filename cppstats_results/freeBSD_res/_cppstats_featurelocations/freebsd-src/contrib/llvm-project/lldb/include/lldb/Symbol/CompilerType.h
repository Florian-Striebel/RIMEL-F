







#if !defined(LLDB_SYMBOL_COMPILERTYPE_H)
#define LLDB_SYMBOL_COMPILERTYPE_H

#include <functional>
#include <string>
#include <vector>

#include "lldb/lldb-private.h"
#include "llvm/ADT/APSInt.h"

namespace lldb_private {

class DataExtractor;











class CompilerType {
public:






CompilerType(TypeSystem *type_system, lldb::opaque_compiler_type_t type)
: m_type(type), m_type_system(type_system) {
assert(Verify() && "verification failed");
}

CompilerType(const CompilerType &rhs)
: m_type(rhs.m_type), m_type_system(rhs.m_type_system) {}

CompilerType() = default;



const CompilerType &operator=(const CompilerType &rhs) {
m_type = rhs.m_type;
m_type_system = rhs.m_type_system;
return *this;
}

bool operator<(const CompilerType &rhs) const {
if (m_type_system == rhs.m_type_system)
return m_type < rhs.m_type;
return m_type_system < rhs.m_type_system;
}




explicit operator bool() const {
return m_type != nullptr && m_type_system != nullptr;
}

bool IsValid() const { return m_type != nullptr && m_type_system != nullptr; }

bool IsArrayType(CompilerType *element_type = nullptr,
uint64_t *size = nullptr,
bool *is_incomplete = nullptr) const;

bool IsVectorType(CompilerType *element_type = nullptr,
uint64_t *size = nullptr) const;

bool IsArrayOfScalarType() const;

bool IsAggregateType() const;

bool IsAnonymousType() const;

bool IsScopedEnumerationType() const;

bool IsBeingDefined() const;

bool IsCharType() const;

bool IsCompleteType() const;

bool IsConst() const;

bool IsCStringType(uint32_t &length) const;

bool IsDefined() const;

bool IsFloatingPointType(uint32_t &count, bool &is_complex) const;

bool IsFunctionType() const;

uint32_t IsHomogeneousAggregate(CompilerType *base_type_ptr) const;

size_t GetNumberOfFunctionArguments() const;

CompilerType GetFunctionArgumentAtIndex(const size_t index) const;

bool IsVariadicFunctionType() const;

bool IsFunctionPointerType() const;

bool
IsBlockPointerType(CompilerType *function_pointer_type_ptr = nullptr) const;

bool IsIntegerType(bool &is_signed) const;

bool IsEnumerationType(bool &is_signed) const;

bool IsIntegerOrEnumerationType(bool &is_signed) const;

bool IsPolymorphicClass() const;


bool IsPossibleDynamicType(CompilerType *target_type, bool check_cplusplus,
bool check_objc) const;

bool IsPointerToScalarType() const;

bool IsRuntimeGeneratedType() const;

bool IsPointerType(CompilerType *pointee_type = nullptr) const;

bool IsPointerOrReferenceType(CompilerType *pointee_type = nullptr) const;

bool IsReferenceType(CompilerType *pointee_type = nullptr,
bool *is_rvalue = nullptr) const;

bool ShouldTreatScalarValueAsAddress() const;

bool IsScalarType() const;

bool IsTypedefType() const;

bool IsVoidType() const;




bool GetCompleteType() const;




size_t GetPointerByteSize() const;




TypeSystem *GetTypeSystem() const { return m_type_system; }

ConstString GetTypeName() const;

ConstString GetDisplayTypeName() const;

uint32_t
GetTypeInfo(CompilerType *pointee_or_element_compiler_type = nullptr) const;

lldb::LanguageType GetMinimumLanguage();

lldb::opaque_compiler_type_t GetOpaqueQualType() const { return m_type; }

lldb::TypeClass GetTypeClass() const;

void SetCompilerType(TypeSystem *type_system,
lldb::opaque_compiler_type_t type);

unsigned GetTypeQualifiers() const;




CompilerType GetArrayElementType(ExecutionContextScope *exe_scope) const;

CompilerType GetArrayType(uint64_t size) const;

CompilerType GetCanonicalType() const;

CompilerType GetFullyUnqualifiedType() const;

CompilerType GetEnumerationIntegerType() const;



int GetFunctionArgumentCount() const;

CompilerType GetFunctionArgumentTypeAtIndex(size_t idx) const;

CompilerType GetFunctionReturnType() const;

size_t GetNumMemberFunctions() const;

TypeMemberFunctionImpl GetMemberFunctionAtIndex(size_t idx);




CompilerType GetNonReferenceType() const;



CompilerType GetPointeeType() const;


CompilerType GetPointerType() const;




CompilerType GetLValueReferenceType() const;




CompilerType GetRValueReferenceType() const;




CompilerType AddConstModifier() const;




CompilerType AddVolatileModifier() const;




CompilerType GetAtomicType() const;




CompilerType AddRestrictModifier() const;





CompilerType CreateTypedef(const char *name,
const CompilerDeclContext &decl_ctx,
uint32_t payload) const;


CompilerType GetTypedefedType() const;


CompilerType GetBasicTypeFromAST(lldb::BasicType basic_type) const;




struct IntegralTemplateArgument;


llvm::Optional<uint64_t> GetByteSize(ExecutionContextScope *exe_scope) const;

llvm::Optional<uint64_t> GetBitSize(ExecutionContextScope *exe_scope) const;

lldb::Encoding GetEncoding(uint64_t &count) const;

lldb::Format GetFormat() const;

llvm::Optional<size_t>
GetTypeBitAlign(ExecutionContextScope *exe_scope) const;

uint32_t GetNumChildren(bool omit_empty_base_classes,
const ExecutionContext *exe_ctx) const;

lldb::BasicType GetBasicTypeEnumeration() const;

static lldb::BasicType GetBasicTypeEnumeration(ConstString name);




void ForEachEnumerator(
std::function<bool(const CompilerType &integer_type, ConstString name,
const llvm::APSInt &value)> const &callback) const;

uint32_t GetNumFields() const;

CompilerType GetFieldAtIndex(size_t idx, std::string &name,
uint64_t *bit_offset_ptr,
uint32_t *bitfield_bit_size_ptr,
bool *is_bitfield_ptr) const;

uint32_t GetNumDirectBaseClasses() const;

uint32_t GetNumVirtualBaseClasses() const;

CompilerType GetDirectBaseClassAtIndex(size_t idx,
uint32_t *bit_offset_ptr) const;

CompilerType GetVirtualBaseClassAtIndex(size_t idx,
uint32_t *bit_offset_ptr) const;

uint32_t GetIndexOfFieldWithName(const char *name,
CompilerType *field_compiler_type = nullptr,
uint64_t *bit_offset_ptr = nullptr,
uint32_t *bitfield_bit_size_ptr = nullptr,
bool *is_bitfield_ptr = nullptr) const;

CompilerType GetChildCompilerTypeAtIndex(
ExecutionContext *exe_ctx, size_t idx, bool transparent_pointers,
bool omit_empty_base_classes, bool ignore_array_bounds,
std::string &child_name, uint32_t &child_byte_size,
int32_t &child_byte_offset, uint32_t &child_bitfield_bit_size,
uint32_t &child_bitfield_bit_offset, bool &child_is_base_class,
bool &child_is_deref_of_parent, ValueObject *valobj,
uint64_t &language_flags) const;



uint32_t GetIndexOfChildWithName(const char *name,
bool omit_empty_base_classes) const;







size_t
GetIndexOfChildMemberWithName(const char *name, bool omit_empty_base_classes,
std::vector<uint32_t> &child_indexes) const;

size_t GetNumTemplateArguments() const;

lldb::TemplateArgumentKind GetTemplateArgumentKind(size_t idx) const;
CompilerType GetTypeTemplateArgument(size_t idx) const;


llvm::Optional<IntegralTemplateArgument>
GetIntegralTemplateArgument(size_t idx) const;

CompilerType GetTypeForFormatters() const;

LazyBool ShouldPrintAsOneLiner(ValueObject *valobj) const;

bool IsMeaninglessWithoutDynamicResolution() const;




#if !defined(NDEBUG)


LLVM_DUMP_METHOD void dump() const;
#endif

void DumpValue(ExecutionContext *exe_ctx, Stream *s, lldb::Format format,
const DataExtractor &data, lldb::offset_t data_offset,
size_t data_byte_size, uint32_t bitfield_bit_size,
uint32_t bitfield_bit_offset, bool show_types,
bool show_summary, bool verbose, uint32_t depth);

bool DumpTypeValue(Stream *s, lldb::Format format, const DataExtractor &data,
lldb::offset_t data_offset, size_t data_byte_size,
uint32_t bitfield_bit_size, uint32_t bitfield_bit_offset,
ExecutionContextScope *exe_scope);

void DumpSummary(ExecutionContext *exe_ctx, Stream *s,
const DataExtractor &data, lldb::offset_t data_offset,
size_t data_byte_size);


void DumpTypeDescription(lldb::DescriptionLevel level =
lldb::eDescriptionLevelFull) const;





void DumpTypeDescription(Stream *s, lldb::DescriptionLevel level =
lldb::eDescriptionLevelFull) const;


bool GetValueAsScalar(const DataExtractor &data, lldb::offset_t data_offset,
size_t data_byte_size, Scalar &value,
ExecutionContextScope *exe_scope) const;
void Clear() {
m_type = nullptr;
m_type_system = nullptr;
}

private:
#if !defined(NDEBUG)



bool Verify() const;
#endif

lldb::opaque_compiler_type_t m_type = nullptr;
TypeSystem *m_type_system = nullptr;
};

bool operator==(const CompilerType &lhs, const CompilerType &rhs);
bool operator!=(const CompilerType &lhs, const CompilerType &rhs);

struct CompilerType::IntegralTemplateArgument {
llvm::APSInt value;
CompilerType type;
};

}

#endif
