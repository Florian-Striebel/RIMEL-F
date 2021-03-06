







#if !defined(LLDB_HOST_PSEUDOTERMINAL_H)
#define LLDB_HOST_PSEUDOTERMINAL_H

#include "lldb/lldb-defines.h"
#include "llvm/Support/Error.h"
#include <fcntl.h>
#include <string>

namespace lldb_private {






class PseudoTerminal {
public:
enum {
invalid_fd = -1
};





PseudoTerminal();







~PseudoTerminal();


void ClosePrimaryFileDescriptor();


void CloseSecondaryFileDescriptor();




















llvm::Expected<lldb::pid_t> Fork();













int GetPrimaryFileDescriptor() const;













int GetSecondaryFileDescriptor() const;










std::string GetSecondaryName() const;




















llvm::Error OpenFirstAvailablePrimary(int oflag);




















llvm::Error OpenSecondary(int oflag);











int ReleasePrimaryFileDescriptor();











int ReleaseSecondaryFileDescriptor();

protected:

int m_primary_fd = invalid_fd;
int m_secondary_fd = invalid_fd;

private:
PseudoTerminal(const PseudoTerminal &) = delete;
const PseudoTerminal &operator=(const PseudoTerminal &) = delete;
};

}

#endif
