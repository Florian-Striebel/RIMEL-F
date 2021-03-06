







#include "lldb/Target/TraceCursor.h"

#if !defined(LLDB_TARGET_TRACE_INSTRUCTION_DUMPER_H)
#define LLDB_TARGET_TRACE_INSTRUCTION_DUMPER_H

namespace lldb_private {



class TraceInstructionDumper {
public:

















TraceInstructionDumper(lldb::TraceCursorUP &&cursor_up, int initial_index = 0,
bool raw = false, bool show_tsc = false);












void DumpInstructions(Stream &s, size_t count);


void SetNoMoreData();



bool HasMoreData();

private:




bool TryMoveOneStep();

lldb::TraceCursorUP m_cursor_up;
int m_index;
bool m_raw;
bool m_show_tsc;

bool m_no_more_data = false;
};

}

#endif
