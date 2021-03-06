







#if !defined(LLDB_CORE_VALUEOBJECTCAST_H)
#define LLDB_CORE_VALUEOBJECTCAST_H

#include "lldb/Core/ValueObject.h"
#include "lldb/Symbol/CompilerType.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-forward.h"

#include <cstddef>
#include <cstdint>

namespace lldb_private {
class ConstString;


class ValueObjectCast : public ValueObject {
public:
~ValueObjectCast() override;

static lldb::ValueObjectSP Create(ValueObject &parent,
ConstString name,
const CompilerType &cast_type);

llvm::Optional<uint64_t> GetByteSize() override;

size_t CalculateNumChildren(uint32_t max) override;

lldb::ValueType GetValueType() const override;

bool IsInScope() override;

ValueObject *GetParent() override {
return ((m_parent != nullptr) ? m_parent->GetParent() : nullptr);
}

const ValueObject *GetParent() const override {
return ((m_parent != nullptr) ? m_parent->GetParent() : nullptr);
}

protected:
ValueObjectCast(ValueObject &parent, ConstString name,
const CompilerType &cast_type);

bool UpdateValue() override;

CompilerType GetCompilerTypeImpl() override;

CompilerType m_cast_type;

private:
ValueObjectCast(const ValueObjectCast &) = delete;
const ValueObjectCast &operator=(const ValueObjectCast &) = delete;
};

}

#endif
