







#if !defined(LLDB_CORE_FORMATENTITY_H)
#define LLDB_CORE_FORMATENTITY_H

#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-types.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>

#include <string>
#include <vector>

namespace lldb_private {
class Address;
class CompletionRequest;
class ExecutionContext;
class FileSpec;
class Status;
class Stream;
class StringList;
class SymbolContext;
class ValueObject;
}

namespace llvm {
class StringRef;
}

namespace lldb_private {
class FormatEntity {
public:
struct Entry {
enum class Type {
Invalid,
ParentNumber,
ParentString,
EscapeCode,
Root,
String,
Scope,
Variable,
VariableSynthetic,
ScriptVariable,
ScriptVariableSynthetic,
AddressLoad,
AddressFile,
AddressLoadOrFile,
ProcessID,
ProcessFile,
ScriptProcess,
ThreadID,
ThreadProtocolID,
ThreadIndexID,
ThreadName,
ThreadQueue,
ThreadStopReason,
ThreadStopReasonRaw,
ThreadReturnValue,
ThreadCompletedExpression,
ScriptThread,
ThreadInfo,
TargetArch,
ScriptTarget,
ModuleFile,
File,
Lang,
FrameIndex,
FrameNoDebug,
FrameRegisterPC,
FrameRegisterSP,
FrameRegisterFP,
FrameRegisterFlags,
FrameRegisterByName,
FrameIsArtificial,
ScriptFrame,
FunctionID,
FunctionDidChange,
FunctionInitialFunction,
FunctionName,
FunctionNameWithArgs,
FunctionNameNoArgs,
FunctionMangledName,
FunctionAddrOffset,
FunctionAddrOffsetConcrete,
FunctionLineOffset,
FunctionPCOffset,
FunctionInitial,
FunctionChanged,
FunctionIsOptimized,
LineEntryFile,
LineEntryLineNumber,
LineEntryColumn,
LineEntryStartAddress,
LineEntryEndAddress,
CurrentPCArrow
};

struct Definition {

const char *name;

const char *string = nullptr;

const Entry::Type type;

const uint64_t data = 0;

const uint32_t num_children = 0;

const Definition *children = nullptr;


const bool keep_separator = false;

constexpr Definition(const char *name, const FormatEntity::Entry::Type t)
: name(name), type(t) {}

constexpr Definition(const char *name, const char *string)
: name(name), string(string), type(Entry::Type::EscapeCode) {}

constexpr Definition(const char *name, const FormatEntity::Entry::Type t,
const uint64_t data)
: name(name), type(t), data(data) {}

constexpr Definition(const char *name, const FormatEntity::Entry::Type t,
const uint64_t num_children,
const Definition *children,
const bool keep_separator = false)
: name(name), type(t), num_children(num_children), children(children),
keep_separator(keep_separator) {}
};

template <size_t N>
static constexpr Definition
DefinitionWithChildren(const char *name, const FormatEntity::Entry::Type t,
const Definition (&children)[N],
bool keep_separator = false) {
return Definition(name, t, N, children, keep_separator);
}

Entry(Type t = Type::Invalid, const char *s = nullptr,
const char *f = nullptr)
: string(s ? s : ""), printf_format(f ? f : ""), children(), type(t) {}

Entry(llvm::StringRef s);
Entry(char ch);

void AppendChar(char ch);

void AppendText(const llvm::StringRef &s);

void AppendText(const char *cstr);

void AppendEntry(const Entry &&entry) { children.push_back(entry); }

void Clear() {
string.clear();
printf_format.clear();
children.clear();
type = Type::Invalid;
fmt = lldb::eFormatDefault;
number = 0;
deref = false;
}

static const char *TypeToCString(Type t);

void Dump(Stream &s, int depth = 0) const;

bool operator==(const Entry &rhs) const {
if (string != rhs.string)
return false;
if (printf_format != rhs.printf_format)
return false;
const size_t n = children.size();
const size_t m = rhs.children.size();
for (size_t i = 0; i < std::min<size_t>(n, m); ++i) {
if (!(children[i] == rhs.children[i]))
return false;
}
if (children != rhs.children)
return false;
if (type != rhs.type)
return false;
if (fmt != rhs.fmt)
return false;
if (deref != rhs.deref)
return false;
return true;
}

std::string string;
std::string printf_format;
std::vector<Entry> children;
Type type;
lldb::Format fmt = lldb::eFormatDefault;
lldb::addr_t number = 0;
bool deref = false;
};

static bool Format(const Entry &entry, Stream &s, const SymbolContext *sc,
const ExecutionContext *exe_ctx, const Address *addr,
ValueObject *valobj, bool function_changed,
bool initial_function);

static bool FormatStringRef(const llvm::StringRef &format, Stream &s,
const SymbolContext *sc,
const ExecutionContext *exe_ctx,
const Address *addr, ValueObject *valobj,
bool function_changed, bool initial_function);

static bool FormatCString(const char *format, Stream &s,
const SymbolContext *sc,
const ExecutionContext *exe_ctx,
const Address *addr, ValueObject *valobj,
bool function_changed, bool initial_function);

static Status Parse(const llvm::StringRef &format, Entry &entry);

static Status ExtractVariableInfo(llvm::StringRef &format_str,
llvm::StringRef &variable_name,
llvm::StringRef &variable_format);

static void AutoComplete(lldb_private::CompletionRequest &request);







static bool FormatFileSpec(const FileSpec &file, Stream &s,
llvm::StringRef elements,
llvm::StringRef element_format);

protected:
static Status ParseInternal(llvm::StringRef &format, Entry &parent_entry,
uint32_t depth);
};
}

#endif
