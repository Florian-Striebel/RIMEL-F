










#if !defined(UBSAN_DIAG_H)
#define UBSAN_DIAG_H

#include "ubsan_value.h"
#include "sanitizer_common/sanitizer_stacktrace.h"
#include "sanitizer_common/sanitizer_symbolizer.h"

namespace __ubsan {

class SymbolizedStackHolder {
SymbolizedStack *Stack;

void clear() {
if (Stack)
Stack->ClearAll();
}

public:
explicit SymbolizedStackHolder(SymbolizedStack *Stack = nullptr)
: Stack(Stack) {}
~SymbolizedStackHolder() { clear(); }
void reset(SymbolizedStack *S) {
if (Stack != S)
clear();
Stack = S;
}
const SymbolizedStack *get() const { return Stack; }
};

SymbolizedStack *getSymbolizedLocation(uptr PC);

inline SymbolizedStack *getCallerLocation(uptr CallerPC) {
CHECK(CallerPC);
uptr PC = StackTrace::GetPreviousInstructionPc(CallerPC);
return getSymbolizedLocation(PC);
}


typedef uptr MemoryLocation;



class Location {
public:
enum LocationKind { LK_Null, LK_Source, LK_Memory, LK_Symbolized };

private:
LocationKind Kind;

SourceLocation SourceLoc;
MemoryLocation MemoryLoc;
const SymbolizedStack *SymbolizedLoc;

public:
Location() : Kind(LK_Null) {}
Location(SourceLocation Loc) :
Kind(LK_Source), SourceLoc(Loc) {}
Location(MemoryLocation Loc) :
Kind(LK_Memory), MemoryLoc(Loc) {}

Location(const SymbolizedStackHolder &Stack) :
Kind(LK_Symbolized), SymbolizedLoc(Stack.get()) {}

LocationKind getKind() const { return Kind; }

bool isSourceLocation() const { return Kind == LK_Source; }
bool isMemoryLocation() const { return Kind == LK_Memory; }
bool isSymbolizedStack() const { return Kind == LK_Symbolized; }

SourceLocation getSourceLocation() const {
CHECK(isSourceLocation());
return SourceLoc;
}
MemoryLocation getMemoryLocation() const {
CHECK(isMemoryLocation());
return MemoryLoc;
}
const SymbolizedStack *getSymbolizedStack() const {
CHECK(isSymbolizedStack());
return SymbolizedLoc;
}
};


enum DiagLevel {
DL_Error,
DL_Note
};


class Range {
Location Start, End;
const char *Text;

public:
Range() : Start(), End(), Text() {}
Range(MemoryLocation Start, MemoryLocation End, const char *Text)
: Start(Start), End(End), Text(Text) {}
Location getStart() const { return Start; }
Location getEnd() const { return End; }
const char *getText() const { return Text; }
};


class TypeName {
const char *Name;
public:
TypeName(const char *Name) : Name(Name) {}
const char *getName() const { return Name; }
};

enum class ErrorType {
#define UBSAN_CHECK(Name, SummaryKind, FSanitizeFlagName) Name,
#include "ubsan_checks.inc"
#undef UBSAN_CHECK
};






class Diag {

Location Loc;


DiagLevel Level;


ErrorType ET;



const char *Message;

public:

enum ArgKind {
AK_String,
AK_TypeName,
AK_UInt,
AK_SInt,
AK_Float,
AK_Pointer
};


struct Arg {
Arg() {}
Arg(const char *String) : Kind(AK_String), String(String) {}
Arg(TypeName TN) : Kind(AK_TypeName), String(TN.getName()) {}
Arg(UIntMax UInt) : Kind(AK_UInt), UInt(UInt) {}
Arg(SIntMax SInt) : Kind(AK_SInt), SInt(SInt) {}
Arg(FloatMax Float) : Kind(AK_Float), Float(Float) {}
Arg(const void *Pointer) : Kind(AK_Pointer), Pointer(Pointer) {}

ArgKind Kind;
union {
const char *String;
UIntMax UInt;
SIntMax SInt;
FloatMax Float;
const void *Pointer;
};
};

private:
static const unsigned MaxArgs = 8;
static const unsigned MaxRanges = 1;


Arg Args[MaxArgs];
unsigned NumArgs;


Range Ranges[MaxRanges];
unsigned NumRanges;

Diag &AddArg(Arg A) {
CHECK(NumArgs != MaxArgs);
Args[NumArgs++] = A;
return *this;
}

Diag &AddRange(Range A) {
CHECK(NumRanges != MaxRanges);
Ranges[NumRanges++] = A;
return *this;
}


Diag(const Diag &);
Diag &operator=(const Diag &);

public:
Diag(Location Loc, DiagLevel Level, ErrorType ET, const char *Message)
: Loc(Loc), Level(Level), ET(ET), Message(Message), NumArgs(0),
NumRanges(0) {}
~Diag();

Diag &operator<<(const char *Str) { return AddArg(Str); }
Diag &operator<<(TypeName TN) { return AddArg(TN); }
Diag &operator<<(unsigned long long V) { return AddArg(UIntMax(V)); }
Diag &operator<<(const void *V) { return AddArg(V); }
Diag &operator<<(const TypeDescriptor &V);
Diag &operator<<(const Value &V);
Diag &operator<<(const Range &R) { return AddRange(R); }
};

struct ReportOptions {


bool FromUnrecoverableHandler;

uptr pc;
uptr bp;
};

bool ignoreReport(SourceLocation SLoc, ReportOptions Opts, ErrorType ET);

#define GET_REPORT_OPTIONS(unrecoverable_handler) GET_CALLER_PC_BP; ReportOptions Opts = {unrecoverable_handler, pc, bp}






class ScopedReport {
struct Initializer {
Initializer();
};
Initializer initializer_;
ScopedErrorReportLock report_lock_;

ReportOptions Opts;
Location SummaryLoc;
ErrorType Type;

public:
ScopedReport(ReportOptions Opts, Location SummaryLoc, ErrorType Type);
~ScopedReport();

static void CheckLocked() { ScopedErrorReportLock::CheckLocked(); }
};

void InitializeSuppressions();
bool IsVptrCheckSuppressed(const char *TypeName);


bool IsPCSuppressed(ErrorType ET, uptr PC, const char *Filename);

}

#endif
