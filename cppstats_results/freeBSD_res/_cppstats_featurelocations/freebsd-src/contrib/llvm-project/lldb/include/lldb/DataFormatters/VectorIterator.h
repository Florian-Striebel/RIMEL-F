








#if !defined(LLDB_DATAFORMATTERS_VECTORITERATOR_H)
#define LLDB_DATAFORMATTERS_VECTORITERATOR_H

#include "lldb/lldb-forward.h"

#include "lldb/DataFormatters/TypeSynthetic.h"
#include "lldb/Target/ExecutionContext.h"
#include "lldb/Utility/ConstString.h"

namespace lldb_private {
namespace formatters {
class VectorIteratorSyntheticFrontEnd : public SyntheticChildrenFrontEnd {
public:
VectorIteratorSyntheticFrontEnd(lldb::ValueObjectSP valobj_sp,
ConstString item_name);

size_t CalculateNumChildren() override;

lldb::ValueObjectSP GetChildAtIndex(size_t idx) override;

bool Update() override;

bool MightHaveChildren() override;

size_t GetIndexOfChildWithName(ConstString name) override;

private:
ExecutionContextRef m_exe_ctx_ref;
ConstString m_item_name;
lldb::ValueObjectSP m_item_sp;
};

}
}

#endif
