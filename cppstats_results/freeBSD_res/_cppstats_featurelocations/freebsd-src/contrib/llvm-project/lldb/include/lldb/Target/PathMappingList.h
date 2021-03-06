







#if !defined(LLDB_TARGET_PATHMAPPINGLIST_H)
#define LLDB_TARGET_PATHMAPPINGLIST_H

#include <map>
#include <vector>
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/Status.h"

namespace lldb_private {

class PathMappingList {
public:
typedef void (*ChangedCallback)(const PathMappingList &path_list,
void *baton);


PathMappingList();

PathMappingList(ChangedCallback callback, void *callback_baton);

PathMappingList(const PathMappingList &rhs);

~PathMappingList();

const PathMappingList &operator=(const PathMappingList &rhs);

void Append(ConstString path, ConstString replacement,
bool notify);

void Append(const PathMappingList &rhs, bool notify);

void Clear(bool notify);


void Dump(Stream *s, int pair_index = -1);

bool IsEmpty() const { return m_pairs.empty(); }

size_t GetSize() const { return m_pairs.size(); }

bool GetPathsAtIndex(uint32_t idx, ConstString &path,
ConstString &new_path) const;

void Insert(ConstString path, ConstString replacement,
uint32_t insert_idx, bool notify);

bool Remove(size_t index, bool notify);

bool Remove(ConstString path, bool notify);

bool Replace(ConstString path, ConstString replacement,
bool notify);

bool Replace(ConstString path, ConstString replacement,
uint32_t index, bool notify);
bool RemapPath(ConstString path, ConstString &new_path) const;





















llvm::Optional<FileSpec> RemapPath(llvm::StringRef path,
bool only_if_exists = false) const;
bool RemapPath(const char *, std::string &) const = delete;

bool ReverseRemapPath(const FileSpec &file, FileSpec &fixed) const;














llvm::Optional<FileSpec> FindFile(const FileSpec &orig_spec) const;

uint32_t FindIndexForPath(ConstString path) const;

uint32_t GetModificationID() const { return m_mod_id; }

protected:
typedef std::pair<ConstString, ConstString> pair;
typedef std::vector<pair> collection;
typedef collection::iterator iterator;
typedef collection::const_iterator const_iterator;

iterator FindIteratorForPath(ConstString path);

const_iterator FindIteratorForPath(ConstString path) const;

collection m_pairs;
ChangedCallback m_callback = nullptr;
void *m_callback_baton = nullptr;
uint32_t m_mod_id = 0;
};

}

#endif
