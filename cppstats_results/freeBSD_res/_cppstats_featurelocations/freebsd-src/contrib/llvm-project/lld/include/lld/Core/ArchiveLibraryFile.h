







#if !defined(LLD_CORE_ARCHIVE_LIBRARY_FILE_H)
#define LLD_CORE_ARCHIVE_LIBRARY_FILE_H

#include "lld/Core/File.h"
#include <set>

namespace lld {










class ArchiveLibraryFile : public File {
public:
static bool classof(const File *f) {
return f->kind() == kindArchiveLibrary;
}



virtual File *find(StringRef name) = 0;

virtual std::error_code
parseAllMembers(std::vector<std::unique_ptr<File>> &result) = 0;

protected:

ArchiveLibraryFile(StringRef path) : File(path, kindArchiveLibrary) {}
};

}

#endif
