







#if !defined(LLDB_HOST_FILEACTION_H)
#define LLDB_HOST_FILEACTION_H

#include "lldb/Utility/FileSpec.h"
#include <string>

namespace lldb_private {

class FileAction {
public:
enum Action {
eFileActionNone,
eFileActionClose,
eFileActionDuplicate,
eFileActionOpen
};

FileAction();

void Clear();

bool Close(int fd);

bool Duplicate(int fd, int dup_fd);

bool Open(int fd, const FileSpec &file_spec, bool read, bool write);

int GetFD() const { return m_fd; }

Action GetAction() const { return m_action; }

int GetActionArgument() const { return m_arg; }

llvm::StringRef GetPath() const;

const FileSpec &GetFileSpec() const;

void Dump(Stream &stream) const;

protected:
Action m_action = eFileActionNone;
int m_fd = -1;
int m_arg = -1;
FileSpec
m_file_spec;
};

}

#endif
