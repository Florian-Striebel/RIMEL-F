







#if !defined(LLDB_DATAFORMATTERS_VECTORTYPE_H)
#define LLDB_DATAFORMATTERS_VECTORTYPE_H

#include "lldb/lldb-forward.h"

namespace lldb_private {
namespace formatters {
bool VectorTypeSummaryProvider(ValueObject &, Stream &,
const TypeSummaryOptions &);

SyntheticChildrenFrontEnd *
VectorTypeSyntheticFrontEndCreator(CXXSyntheticChildren *, lldb::ValueObjectSP);
}
}

#endif
