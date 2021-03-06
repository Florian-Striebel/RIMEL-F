







#if !defined(LLDB_HOST_TERMINAL_H)
#define LLDB_HOST_TERMINAL_H
#if defined(__cplusplus)

#include "lldb/Host/Config.h"
#include "lldb/lldb-private.h"

struct termios;

namespace lldb_private {

class Terminal {
public:
Terminal(int fd = -1) : m_fd(fd) {}

~Terminal() = default;

bool IsATerminal() const;

int GetFileDescriptor() const { return m_fd; }

void SetFileDescriptor(int fd) { m_fd = fd; }

bool FileDescriptorIsValid() const { return m_fd != -1; }

void Clear() { m_fd = -1; }

bool SetEcho(bool enabled);

bool SetCanonical(bool enabled);

protected:
int m_fd;
};






class TerminalState {
public:

TerminalState();


~TerminalState();

















bool Save(int fd, bool save_process_group);









bool Restore() const;







bool IsValid() const;

void Clear();

protected:





bool TFlagsIsValid() const;






bool TTYStateIsValid() const;






bool ProcessGroupIsValid() const;


Terminal m_tty;
int m_tflags = -1;
#if LLDB_ENABLE_TERMIOS
std::unique_ptr<struct termios>
m_termios_up;
#endif
lldb::pid_t m_process_group = -1;
};






class TerminalStateSwitcher {
public:

TerminalStateSwitcher();


~TerminalStateSwitcher();





uint32_t GetNumberOfStates() const;






bool Restore(uint32_t idx) const;


















bool Save(uint32_t idx, int fd, bool save_process_group);

protected:

mutable uint32_t m_currentState =
UINT32_MAX;
TerminalState
m_ttystates[2];
};

}

#endif
#endif
