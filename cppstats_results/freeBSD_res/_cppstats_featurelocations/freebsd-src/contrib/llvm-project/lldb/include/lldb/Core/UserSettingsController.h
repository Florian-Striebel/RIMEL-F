







#if !defined(LLDB_CORE_USERSETTINGSCONTROLLER_H)
#define LLDB_CORE_USERSETTINGSCONTROLLER_H

#include "lldb/Utility/Status.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private-enumerations.h"

#include "llvm/ADT/StringRef.h"

#include <vector>

#include <cstddef>
#include <cstdint>

namespace lldb_private {
class CommandInterpreter;
class ConstString;
class ExecutionContext;
class Property;
class Stream;
}

namespace lldb_private {

class Properties {
public:
Properties() = default;

Properties(const lldb::OptionValuePropertiesSP &collection_sp)
: m_collection_sp(collection_sp) {}

virtual ~Properties() = default;

virtual lldb::OptionValuePropertiesSP GetValueProperties() const {


return m_collection_sp;
}

virtual lldb::OptionValueSP GetPropertyValue(const ExecutionContext *exe_ctx,
llvm::StringRef property_path,
bool will_modify,
Status &error) const;

virtual Status SetPropertyValue(const ExecutionContext *exe_ctx,
VarSetOperationType op,
llvm::StringRef property_path,
llvm::StringRef value);

virtual Status DumpPropertyValue(const ExecutionContext *exe_ctx,
Stream &strm, llvm::StringRef property_path,
uint32_t dump_mask);

virtual void DumpAllPropertyValues(const ExecutionContext *exe_ctx,
Stream &strm, uint32_t dump_mask);

virtual void DumpAllDescriptions(CommandInterpreter &interpreter,
Stream &strm) const;

size_t Apropos(llvm::StringRef keyword,
std::vector<const Property *> &matching_properties) const;

lldb::OptionValuePropertiesSP GetSubProperty(const ExecutionContext *exe_ctx,
ConstString name);








static const char *GetExperimentalSettingsName();

static bool IsSettingExperimental(llvm::StringRef setting);

protected:
lldb::OptionValuePropertiesSP m_collection_sp;
};

}

#endif
