







#if !defined(LLDB_BREAKPOINT_BREAKPOINTID_H)
#define LLDB_BREAKPOINT_BREAKPOINTID_H

#include "lldb/lldb-private.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"

namespace lldb_private {



class BreakpointID {
public:
BreakpointID(lldb::break_id_t bp_id = LLDB_INVALID_BREAK_ID,
lldb::break_id_t loc_id = LLDB_INVALID_BREAK_ID);

virtual ~BreakpointID();

lldb::break_id_t GetBreakpointID() const { return m_break_id; }

lldb::break_id_t GetLocationID() const { return m_location_id; }

void SetID(lldb::break_id_t bp_id, lldb::break_id_t loc_id) {
m_break_id = bp_id;
m_location_id = loc_id;
}

void SetBreakpointID(lldb::break_id_t bp_id) { m_break_id = bp_id; }

void SetBreakpointLocationID(lldb::break_id_t loc_id) {
m_location_id = loc_id;
}

void GetDescription(Stream *s, lldb::DescriptionLevel level);

static bool IsRangeIdentifier(llvm::StringRef str);
static bool IsValidIDExpression(llvm::StringRef str);
static llvm::ArrayRef<llvm::StringRef> GetRangeSpecifiers();











static llvm::Optional<BreakpointID>
ParseCanonicalReference(llvm::StringRef input);













static bool StringIsBreakpointName(llvm::StringRef str, Status &error);











static void GetCanonicalReference(Stream *s, lldb::break_id_t break_id,
lldb::break_id_t break_loc_id);

protected:
lldb::break_id_t m_break_id;
lldb::break_id_t m_location_id;
};

}

#endif
