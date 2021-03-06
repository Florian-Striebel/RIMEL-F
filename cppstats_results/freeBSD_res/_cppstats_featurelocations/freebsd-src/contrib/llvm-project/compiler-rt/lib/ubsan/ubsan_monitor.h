











#if !defined(UBSAN_MONITOR_H)
#define UBSAN_MONITOR_H

#include "ubsan_diag.h"
#include "ubsan_value.h"

namespace __ubsan {

struct UndefinedBehaviorReport {
const char *IssueKind;
Location &Loc;
InternalScopedString Buffer;

UndefinedBehaviorReport(const char *IssueKind, Location &Loc,
InternalScopedString &Msg);
};

SANITIZER_INTERFACE_ATTRIBUTE void
RegisterUndefinedBehaviorReport(UndefinedBehaviorReport *UBR);



extern "C" SANITIZER_INTERFACE_ATTRIBUTE void __ubsan_on_report(void);




extern "C" SANITIZER_INTERFACE_ATTRIBUTE void
__ubsan_get_current_report_data(const char **OutIssueKind,
const char **OutMessage,
const char **OutFilename, unsigned *OutLine,
unsigned *OutCol, char **OutMemoryAddr);

}

#endif
