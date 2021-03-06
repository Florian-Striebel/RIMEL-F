







#if !defined(LLDB_TARGET_REGISTERNUMBER_H)
#define LLDB_TARGET_REGISTERNUMBER_H

#include "lldb/lldb-private.h"
#include <map>





class RegisterNumber {
public:
RegisterNumber(lldb_private::Thread &thread, lldb::RegisterKind kind,
uint32_t num);





RegisterNumber();

void init(lldb_private::Thread &thread, lldb::RegisterKind kind,
uint32_t num);

const RegisterNumber &operator=(const RegisterNumber &rhs);

bool operator==(RegisterNumber &rhs);

bool operator!=(RegisterNumber &rhs);

bool IsValid() const;

uint32_t GetAsKind(lldb::RegisterKind kind);

uint32_t GetRegisterNumber() const;

lldb::RegisterKind GetRegisterKind() const;

const char *GetName();

private:
typedef std::map<lldb::RegisterKind, uint32_t> Collection;

lldb::RegisterContextSP m_reg_ctx_sp;
uint32_t m_regnum = LLDB_INVALID_REGNUM;
lldb::RegisterKind m_kind = lldb::kNumRegisterKinds;
Collection m_kind_regnum_map;
const char *m_name = nullptr;
};

#endif
