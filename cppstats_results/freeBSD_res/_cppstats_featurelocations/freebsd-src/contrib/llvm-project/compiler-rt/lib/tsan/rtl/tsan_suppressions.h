










#if !defined(TSAN_SUPPRESSIONS_H)
#define TSAN_SUPPRESSIONS_H

#include "sanitizer_common/sanitizer_suppressions.h"
#include "tsan_report.h"

namespace __tsan {

const char kSuppressionNone[] = "none";
const char kSuppressionRace[] = "race";
const char kSuppressionRaceTop[] = "race_top";
const char kSuppressionMutex[] = "mutex";
const char kSuppressionThread[] = "thread";
const char kSuppressionSignal[] = "signal";
const char kSuppressionLib[] = "called_from_lib";
const char kSuppressionDeadlock[] = "deadlock";

void InitializeSuppressions();
SuppressionContext *Suppressions();
void PrintMatchedSuppressions();
uptr IsSuppressed(ReportType typ, const ReportStack *stack, Suppression **sp);
uptr IsSuppressed(ReportType typ, const ReportLocation *loc, Suppression **sp);

}

#endif
