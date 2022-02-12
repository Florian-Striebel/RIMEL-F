







#if !defined(LLDB_INTERPRETER_OPTIONS_H)
#define LLDB_INTERPRETER_OPTIONS_H

#include <set>
#include <vector>

#include "lldb/Utility/Args.h"
#include "lldb/Utility/CompletionRequest.h"
#include "lldb/Utility/OptionDefinition.h"
#include "lldb/Utility/Status.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-private.h"

#include "llvm/ADT/ArrayRef.h"

namespace lldb_private {

struct Option;

typedef std::vector<std::tuple<std::string, int, std::string>> OptionArgVector;
typedef std::shared_ptr<OptionArgVector> OptionArgVectorSP;

struct OptionArgElement {
enum { eUnrecognizedArg = -1, eBareDash = -2, eBareDoubleDash = -3 };

OptionArgElement(int defs_index, int pos, int arg_pos)
: opt_defs_index(defs_index), opt_pos(pos), opt_arg_pos(arg_pos) {}

int opt_defs_index;
int opt_pos;
int opt_arg_pos;
};

typedef std::vector<OptionArgElement> OptionElementVector;














class Options {
public:
Options();

virtual ~Options();

void BuildGetoptTable();

void BuildValidOptionSets();

uint32_t NumCommandOptions();





Option *GetLongOptions();


void OptionSeen(int short_option);

bool VerifyOptions(CommandReturnObject &result);




bool VerifyPartialOptions(CommandReturnObject &result);

void OutputFormattedUsageText(Stream &strm,
const OptionDefinition &option_def,
uint32_t output_max_columns);

void GenerateOptionUsage(Stream &strm, CommandObject *cmd,
uint32_t screen_width);

bool SupportsLongOption(const char *long_option);




virtual llvm::ArrayRef<OptionDefinition> GetDefinitions() {
return llvm::ArrayRef<OptionDefinition>();
}






void NotifyOptionParsingStarting(ExecutionContext *execution_context);

















llvm::Expected<Args> Parse(const Args &args,
ExecutionContext *execution_context,
lldb::PlatformSP platform_sp,
bool require_validation);

llvm::Expected<Args> ParseAlias(const Args &args,
OptionArgVector *option_arg_vector,
std::string &input_line);

OptionElementVector ParseForCompletion(const Args &args,
uint32_t cursor_index);

Status NotifyOptionParsingFinished(ExecutionContext *execution_context);


















virtual Status SetOptionValue(uint32_t option_idx, llvm::StringRef option_arg,
ExecutionContext *execution_context) = 0;
















bool HandleOptionCompletion(lldb_private::CompletionRequest &request,
OptionElementVector &option_map,
CommandInterpreter &interpreter);









virtual void
HandleOptionArgumentCompletion(lldb_private::CompletionRequest &request,
OptionElementVector &opt_element_vector,
int opt_element_index,
CommandInterpreter &interpreter);

protected:


typedef std::set<int> OptionSet;
typedef std::vector<OptionSet> OptionSetVector;

std::vector<Option> m_getopt_table;
OptionSet m_seen_options;
OptionSetVector m_required_options;
OptionSetVector m_optional_options;

OptionSetVector &GetRequiredOptions() {
BuildValidOptionSets();
return m_required_options;
}

OptionSetVector &GetOptionalOptions() {
BuildValidOptionSets();
return m_optional_options;
}

bool IsASubset(const OptionSet &set_a, const OptionSet &set_b);

size_t OptionsSetDiff(const OptionSet &set_a, const OptionSet &set_b,
OptionSet &diffs);

void OptionsSetUnion(const OptionSet &set_a, const OptionSet &set_b,
OptionSet &union_set);




virtual void OptionParsingStarting(ExecutionContext *execution_context) = 0;

virtual Status OptionParsingFinished(ExecutionContext *execution_context) {


Status error;
return error;
}
};

class OptionGroup {
public:
OptionGroup() = default;

virtual ~OptionGroup() = default;

virtual llvm::ArrayRef<OptionDefinition> GetDefinitions() = 0;

virtual Status SetOptionValue(uint32_t option_idx,
llvm::StringRef option_value,
ExecutionContext *execution_context) = 0;

virtual void OptionParsingStarting(ExecutionContext *execution_context) = 0;

virtual Status OptionParsingFinished(ExecutionContext *execution_context) {


Status error;
return error;
}
};

class OptionGroupOptions : public Options {
public:
OptionGroupOptions() = default;

~OptionGroupOptions() override = default;









void Append(OptionGroup *group);





















void Append(OptionGroup *group, uint32_t src_mask, uint32_t dst_mask);

void Finalize();

bool DidFinalize() { return m_did_finalize; }

Status SetOptionValue(uint32_t option_idx, llvm::StringRef option_arg,
ExecutionContext *execution_context) override;

void OptionParsingStarting(ExecutionContext *execution_context) override;

Status OptionParsingFinished(ExecutionContext *execution_context) override;

llvm::ArrayRef<OptionDefinition> GetDefinitions() override {
assert(m_did_finalize);
return m_option_defs;
}

const OptionGroup *GetGroupWithOption(char short_opt);

struct OptionInfo {
OptionInfo(OptionGroup *g, uint32_t i) : option_group(g), option_index(i) {}
OptionGroup *option_group;
uint32_t option_index;
};
typedef std::vector<OptionInfo> OptionInfos;

std::vector<OptionDefinition> m_option_defs;
OptionInfos m_option_infos;
bool m_did_finalize = false;
};

}

#endif
