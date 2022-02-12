














#if !defined(SANITIZER_REPORT_DECORATOR_H)
#define SANITIZER_REPORT_DECORATOR_H

#include "sanitizer_common.h"

namespace __sanitizer {
class SanitizerCommonDecorator {


public:
SanitizerCommonDecorator() : ansi_(ColorizeReports()) {}
const char *Bold() const { return ansi_ ? "\033[1m" : ""; }
const char *Default() const { return ansi_ ? "\033[1m\033[0m" : ""; }
const char *Warning() const { return Red(); }
const char *Error() const { return Red(); }
const char *MemoryByte() const { return Magenta(); }

protected:
const char *Black() const { return ansi_ ? "\033[1m\033[30m" : ""; }
const char *Red() const { return ansi_ ? "\033[1m\033[31m" : ""; }
const char *Green() const { return ansi_ ? "\033[1m\033[32m" : ""; }
const char *Yellow() const { return ansi_ ? "\033[1m\033[33m" : ""; }
const char *Blue() const { return ansi_ ? "\033[1m\033[34m" : ""; }
const char *Magenta() const { return ansi_ ? "\033[1m\033[35m" : ""; }
const char *Cyan() const { return ansi_ ? "\033[1m\033[36m" : ""; }
const char *White() const { return ansi_ ? "\033[1m\033[37m" : ""; }
private:
bool ansi_;
};

}

#endif
