







#if !defined(LLDB_INTERPRETER_OPTIONGROUPARCHITECTURE_H)
#define LLDB_INTERPRETER_OPTIONGROUPARCHITECTURE_H

#include "lldb/Interpreter/Options.h"
#include "lldb/Utility/ArchSpec.h"

namespace lldb_private {



class OptionGroupArchitecture : public OptionGroup {
public:
OptionGroupArchitecture() = default;

~OptionGroupArchitecture() override = default;

llvm::ArrayRef<OptionDefinition> GetDefinitions() override;

Status SetOptionValue(uint32_t option_idx, llvm::StringRef option_value,
ExecutionContext *execution_context) override;

void OptionParsingStarting(ExecutionContext *execution_context) override;

bool GetArchitecture(Platform *platform, ArchSpec &arch);

bool ArchitectureWasSpecified() const { return !m_arch_str.empty(); }

llvm::StringRef GetArchitectureName() const { return m_arch_str; }

protected:
std::string m_arch_str;

};

}

#endif
