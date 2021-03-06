







#if !defined(LLDB_CORE_FILESPECLIST_H)
#define LLDB_CORE_FILESPECLIST_H
#if defined(__cplusplus)

#include "lldb/Utility/FileSpec.h"

#include <vector>

#include <cstddef>

namespace lldb_private {
class Stream;





class FileSpecList {
public:
typedef std::vector<FileSpec> collection;
typedef collection::const_iterator const_iterator;




FileSpecList();


FileSpecList(const FileSpecList &rhs) = default;


FileSpecList(FileSpecList &&rhs) = default;


FileSpecList(std::vector<FileSpec> &&rhs) : m_files(std::move(rhs)) {}


~FileSpecList();










FileSpecList &operator=(const FileSpecList &rhs) = default;


FileSpecList &operator=(FileSpecList &&rhs) = default;







void Append(const FileSpec &file);











bool AppendIfUnique(const FileSpec &file);






template <class... Args> void EmplaceBack(Args &&... args) {
m_files.emplace_back(std::forward<Args>(args)...);
}


void Clear();





void Dump(Stream *s, const char *separator_cstr = "\n") const;


















size_t FindFileIndex(size_t idx, const FileSpec &file, bool full) const;














const FileSpec &GetFileSpecAtIndex(size_t idx) const;












const FileSpec *GetFileSpecPointerAtIndex(size_t idx) const;











size_t MemorySize() const;

bool IsEmpty() const { return m_files.empty(); }





size_t GetSize() const;

bool Insert(size_t idx, const FileSpec &file) {
if (idx < m_files.size()) {
m_files.insert(m_files.begin() + idx, file);
return true;
} else if (idx == m_files.size()) {
m_files.push_back(file);
return true;
}
return false;
}

bool Replace(size_t idx, const FileSpec &file) {
if (idx < m_files.size()) {
m_files[idx] = file;
return true;
}
return false;
}

bool Remove(size_t idx) {
if (idx < m_files.size()) {
m_files.erase(m_files.begin() + idx);
return true;
}
return false;
}

static size_t GetFilesMatchingPartialPath(const char *path, bool dir_okay,
FileSpecList &matches);

const_iterator begin() const { return m_files.begin(); }
const_iterator end() const { return m_files.end(); }

protected:
collection m_files;
};

}

#endif
#endif
