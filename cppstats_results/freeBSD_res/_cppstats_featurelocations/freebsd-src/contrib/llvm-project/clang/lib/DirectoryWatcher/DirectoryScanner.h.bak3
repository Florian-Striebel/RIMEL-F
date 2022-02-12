







#include "clang/DirectoryWatcher/DirectoryWatcher.h"
#include "llvm/Support/FileSystem.h"
#include <string>
#include <vector>

namespace clang {




std::vector<std::string> scanDirectory(llvm::StringRef Path);


std::vector<DirectoryWatcher::Event>
getAsFileEvents(const std::vector<std::string> &Scan);



llvm::Optional<llvm::sys::fs::file_status> getFileStatus(llvm::StringRef Path);

}
