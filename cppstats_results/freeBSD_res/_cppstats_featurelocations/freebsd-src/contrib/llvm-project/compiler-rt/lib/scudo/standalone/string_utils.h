







#if !defined(SCUDO_STRING_UTILS_H_)
#define SCUDO_STRING_UTILS_H_

#include "internal_defs.h"
#include "vector.h"

#include <stdarg.h>

namespace scudo {

class ScopedString {
public:
explicit ScopedString() { String.push_back('\0'); }
uptr length() { return String.size() - 1; }
const char *data() { return String.data(); }
void clear() {
String.clear();
String.push_back('\0');
}
void append(const char *Format, va_list Args);
void append(const char *Format, ...);
void output() const { outputRaw(String.data()); }

private:
Vector<char> String;
};

int formatString(char *Buffer, uptr BufferLength, const char *Format, ...);
void Printf(const char *Format, ...);

}

#endif
