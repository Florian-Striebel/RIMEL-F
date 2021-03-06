







#if !defined(LLDB_DATAFORMATTERS_CXXFUNCTIONPOINTER_H)
#define LLDB_DATAFORMATTERS_CXXFUNCTIONPOINTER_H

#include "lldb/lldb-forward.h"

namespace lldb_private {
namespace formatters {
bool CXXFunctionPointerSummaryProvider(ValueObject &valobj, Stream &stream,
const TypeSummaryOptions &options);
}
}

#endif
