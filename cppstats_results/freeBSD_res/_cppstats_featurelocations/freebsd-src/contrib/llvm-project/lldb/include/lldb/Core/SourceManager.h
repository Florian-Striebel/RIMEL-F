







#if !defined(LLDB_CORE_SOURCEMANAGER_H)
#define LLDB_CORE_SOURCEMANAGER_H

#include "lldb/Utility/FileSpec.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-forward.h"

#include "llvm/Support/Chrono.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace lldb_private {
class RegularExpression;
class Stream;
class SymbolContextList;
class Target;

class SourceManager {
public:
class File {
friend bool operator==(const SourceManager::File &lhs,
const SourceManager::File &rhs);

public:
File(const FileSpec &file_spec, Target *target);
File(const FileSpec &file_spec, lldb::DebuggerSP debugger_sp);
~File() = default;

void UpdateIfNeeded();

size_t DisplaySourceLines(uint32_t line, llvm::Optional<size_t> column,
uint32_t context_before, uint32_t context_after,
Stream *s);
void FindLinesMatchingRegex(RegularExpression &regex, uint32_t start_line,
uint32_t end_line,
std::vector<uint32_t> &match_lines);

bool GetLine(uint32_t line_no, std::string &buffer);

uint32_t GetLineOffset(uint32_t line);

bool LineIsValid(uint32_t line);

const FileSpec &GetFileSpec() { return m_file_spec; }

uint32_t GetSourceMapModificationID() const { return m_source_map_mod_id; }

const char *PeekLineData(uint32_t line);

uint32_t GetLineLength(uint32_t line, bool include_newline_chars);

uint32_t GetNumLines();

protected:
bool CalculateLineOffsets(uint32_t line = UINT32_MAX);

FileSpec m_file_spec_orig;

FileSpec m_file_spec;




llvm::sys::TimePoint<> m_mod_time;



uint32_t m_source_map_mod_id = 0;
lldb::DataBufferSP m_data_sp;
typedef std::vector<uint32_t> LineOffsets;
LineOffsets m_offsets;
lldb::DebuggerWP m_debugger_wp;

private:
void CommonInitializer(const FileSpec &file_spec, Target *target);
};

typedef std::shared_ptr<File> FileSP;




class SourceFileCache {
public:
SourceFileCache() = default;
~SourceFileCache() = default;

void AddSourceFile(const FileSP &file_sp);
FileSP FindSourceFile(const FileSpec &file_spec) const;


void Clear() { m_file_cache.clear(); }

protected:
typedef std::map<FileSpec, FileSP> FileCache;
FileCache m_file_cache;
};






SourceManager(const lldb::DebuggerSP &debugger_sp);
SourceManager(const lldb::TargetSP &target_sp);

~SourceManager();

FileSP GetLastFile() { return GetFile(m_last_file_spec); }

size_t
DisplaySourceLinesWithLineNumbers(const FileSpec &file, uint32_t line,
uint32_t column, uint32_t context_before,
uint32_t context_after,
const char *current_line_cstr, Stream *s,
const SymbolContextList *bp_locs = nullptr);


size_t DisplaySourceLinesWithLineNumbersUsingLastFile(
uint32_t start_line, uint32_t count, uint32_t curr_line, uint32_t column,
const char *current_line_cstr, Stream *s,
const SymbolContextList *bp_locs = nullptr);

size_t DisplayMoreWithLineNumbers(Stream *s, uint32_t count, bool reverse,
const SymbolContextList *bp_locs = nullptr);

bool SetDefaultFileAndLine(const FileSpec &file_spec, uint32_t line);

bool GetDefaultFileAndLine(FileSpec &file_spec, uint32_t &line);

bool DefaultFileAndLineSet() {
return (GetFile(m_last_file_spec).get() != nullptr);
}

void FindLinesMatchingRegex(FileSpec &file_spec, RegularExpression &regex,
uint32_t start_line, uint32_t end_line,
std::vector<uint32_t> &match_lines);

FileSP GetFile(const FileSpec &file_spec);

protected:
FileSpec m_last_file_spec;
uint32_t m_last_line;
uint32_t m_last_count;
bool m_default_set;
lldb::TargetWP m_target_wp;
lldb::DebuggerWP m_debugger_wp;

private:
SourceManager(const SourceManager &) = delete;
const SourceManager &operator=(const SourceManager &) = delete;
};

bool operator==(const SourceManager::File &lhs, const SourceManager::File &rhs);

}

#endif
