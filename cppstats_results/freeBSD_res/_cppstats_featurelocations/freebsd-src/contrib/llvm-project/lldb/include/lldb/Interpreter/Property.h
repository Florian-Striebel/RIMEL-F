







#if !defined(LLDB_INTERPRETER_PROPERTY_H)
#define LLDB_INTERPRETER_PROPERTY_H

#include "lldb/Interpreter/OptionValue.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/Flags.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-private-types.h"

#include <string>

namespace lldb_private {



struct PropertyDefinition {
const char *name;
OptionValue::Type type;
bool global;
uintptr_t default_uint_value;
const char *default_cstr_value;
OptionEnumValues enum_values;
const char *description;
};

using PropertyDefinitions = llvm::ArrayRef<PropertyDefinition>;

class Property {
public:
Property(const PropertyDefinition &definition);

Property(ConstString name, ConstString desc, bool is_global,
const lldb::OptionValueSP &value_sp);

llvm::StringRef GetName() const { return m_name.GetStringRef(); }
llvm::StringRef GetDescription() const {
return m_description.GetStringRef();
}

const lldb::OptionValueSP &GetValue() const { return m_value_sp; }

void SetOptionValue(const lldb::OptionValueSP &value_sp) {
m_value_sp = value_sp;
}

bool IsValid() const { return (bool)m_value_sp; }

bool IsGlobal() const { return m_is_global; }

void Dump(const ExecutionContext *exe_ctx, Stream &strm,
uint32_t dump_mask) const;

bool DumpQualifiedName(Stream &strm) const;

void DumpDescription(CommandInterpreter &interpreter, Stream &strm,
uint32_t output_width,
bool display_qualified_name) const;

void SetValueChangedCallback(std::function<void()> callback);

protected:
ConstString m_name;
ConstString m_description;
lldb::OptionValueSP m_value_sp;
bool m_is_global;
};

}

#endif
