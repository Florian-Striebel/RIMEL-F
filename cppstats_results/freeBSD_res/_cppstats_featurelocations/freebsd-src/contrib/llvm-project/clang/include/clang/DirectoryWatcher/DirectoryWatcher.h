







#if !defined(LLVM_CLANG_DIRECTORYWATCHER_DIRECTORYWATCHER_H)
#define LLVM_CLANG_DIRECTORYWATCHER_DIRECTORYWATCHER_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include <functional>
#include <memory>
#include <string>

namespace clang {












































class DirectoryWatcher {
public:
struct Event {
enum class EventKind {
Removed,

Modified,

WatchedDirRemoved,

















WatcherGotInvalidated
};

EventKind Kind;


std::string Filename;

Event(EventKind Kind, llvm::StringRef Filename)
: Kind(Kind), Filename(Filename) {}
};





static llvm::Expected<std::unique_ptr<DirectoryWatcher>>
create(llvm::StringRef Path,
std::function<void(llvm::ArrayRef<DirectoryWatcher::Event> Events,
bool IsInitial)>
Receiver,
bool WaitForInitialSync);

virtual ~DirectoryWatcher() = default;
DirectoryWatcher(const DirectoryWatcher &) = delete;
DirectoryWatcher &operator=(const DirectoryWatcher &) = delete;
DirectoryWatcher(DirectoryWatcher &&) = default;

protected:
DirectoryWatcher() = default;
};

}

#endif
