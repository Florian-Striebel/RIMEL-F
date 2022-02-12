












#if !defined(LLD_CORE_INSTRUMENTATION_H)
#define LLD_CORE_INSTRUMENTATION_H

#include "llvm/Support/Compiler.h"
#include <utility>

#if defined(LLD_HAS_VTUNE)
#include <ittnotify.h>
#endif

namespace lld {
#if defined(LLD_HAS_VTUNE)




class Domain {
__itt_domain *_domain;

public:
explicit Domain(const char *name) : _domain(__itt_domain_createA(name)) {}

operator __itt_domain *() const { return _domain; }
__itt_domain *operator->() const { return _domain; }
};








class StringHandle {
__itt_string_handle *_handle;

public:
StringHandle(const char *name) : _handle(__itt_string_handle_createA(name)) {}

operator __itt_string_handle *() const { return _handle; }
};








class ScopedTask {
__itt_domain *_domain;

ScopedTask(const ScopedTask &) = delete;
ScopedTask &operator=(const ScopedTask &) = delete;

public:

ScopedTask(const Domain &d, const StringHandle &s) : _domain(d) {
__itt_task_begin(d, __itt_null, __itt_null, s);
}

ScopedTask(ScopedTask &&other) {
*this = std::move(other);
}

ScopedTask &operator=(ScopedTask &&other) {
_domain = other._domain;
other._domain = nullptr;
return *this;
}


void end() {
if (_domain)
__itt_task_end(_domain);
_domain = nullptr;
}

~ScopedTask() { end(); }
};


class Marker {
public:
Marker(const Domain &d, const StringHandle &s) {
__itt_marker(d, __itt_null, s, __itt_scope_global);
}
};
#else
class Domain {
public:
Domain(const char *name) {}
};

class StringHandle {
public:
StringHandle(const char *name) {}
};

class ScopedTask {
public:
ScopedTask(const Domain &d, const StringHandle &s) {}
void end() {}
};

class Marker {
public:
Marker(const Domain &d, const StringHandle &s) {}
};
#endif

inline const Domain &getDefaultDomain() {
static Domain domain("org.llvm.lld");
return domain;
}
}

#endif
