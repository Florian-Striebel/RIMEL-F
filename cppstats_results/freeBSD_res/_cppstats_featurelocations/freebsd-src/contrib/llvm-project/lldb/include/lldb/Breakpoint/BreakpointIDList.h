







#if !defined(LLDB_BREAKPOINT_BREAKPOINTIDLIST_H)
#define LLDB_BREAKPOINT_BREAKPOINTIDLIST_H

#include <utility>
#include <vector>


#include "lldb/lldb-enumerations.h"
#include "lldb/Breakpoint/BreakpointID.h"
#include "lldb/Breakpoint/BreakpointName.h"
#include "lldb/lldb-private.h"

namespace lldb_private {



class BreakpointIDList {
public:

typedef std::vector<BreakpointID> BreakpointIDArray;

BreakpointIDList();

virtual ~BreakpointIDList();

size_t GetSize() const;

const BreakpointID &GetBreakpointIDAtIndex(size_t index) const;

bool RemoveBreakpointIDAtIndex(size_t index);

void Clear();

bool AddBreakpointID(BreakpointID bp_id);

bool AddBreakpointID(const char *bp_id);


bool FindBreakpointID(BreakpointID &bp_id, size_t *position) const;

bool FindBreakpointID(const char *bp_id, size_t *position) const;

void InsertStringArray(llvm::ArrayRef<const char *> string_array,
CommandReturnObject &result);




static std::pair<llvm::StringRef, llvm::StringRef>
SplitIDRangeExpression(llvm::StringRef in_string);

static void FindAndReplaceIDRanges(Args &old_args, Target *target,
bool allow_locations,
BreakpointName::Permissions
::PermissionKinds purpose,
CommandReturnObject &result,
Args &new_args);

private:
BreakpointIDArray m_breakpoint_ids;
BreakpointID m_invalid_id;

BreakpointIDList(const BreakpointIDList &) = delete;
const BreakpointIDList &operator=(const BreakpointIDList &) = delete;
};

}

#endif
