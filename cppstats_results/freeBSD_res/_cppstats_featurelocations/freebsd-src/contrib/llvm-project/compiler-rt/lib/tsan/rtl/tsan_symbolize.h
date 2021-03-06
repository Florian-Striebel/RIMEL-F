










#if !defined(TSAN_SYMBOLIZE_H)
#define TSAN_SYMBOLIZE_H

#include "tsan_defs.h"
#include "tsan_report.h"

namespace __tsan {

void EnterSymbolizer();
void ExitSymbolizer();
SymbolizedStack *SymbolizeCode(uptr addr);
ReportLocation *SymbolizeData(uptr addr);
void SymbolizeFlush();

ReportStack *NewReportStackEntry(uptr addr);

}

#endif
