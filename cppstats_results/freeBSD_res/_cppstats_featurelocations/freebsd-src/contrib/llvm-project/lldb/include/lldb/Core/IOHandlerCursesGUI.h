







#if !defined(LLDB_CORE_IOHANDLERCURSESGUI_H)
#define LLDB_CORE_IOHANDLERCURSESGUI_H

#include "lldb/Core/IOHandler.h"

namespace lldb_private {

class IOHandlerCursesGUI : public IOHandler {
public:
IOHandlerCursesGUI(Debugger &debugger);

~IOHandlerCursesGUI() override;

void Run() override;

void Cancel() override;

bool Interrupt() override;

void GotEOF() override;

void Activate() override;

void Deactivate() override;

void TerminalSizeChanged() override;

protected:
curses::ApplicationAP m_app_ap;
};

}

#endif
