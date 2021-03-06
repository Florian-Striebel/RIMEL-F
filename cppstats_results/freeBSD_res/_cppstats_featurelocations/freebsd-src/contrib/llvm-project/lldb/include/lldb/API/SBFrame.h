







#if !defined(LLDB_API_SBFRAME_H)
#define LLDB_API_SBFRAME_H

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBValueList.h"

namespace lldb {

class LLDB_API SBFrame {
public:
SBFrame();

SBFrame(const lldb::SBFrame &rhs);

const lldb::SBFrame &operator=(const lldb::SBFrame &rhs);

~SBFrame();

bool IsEqual(const lldb::SBFrame &that) const;

explicit operator bool() const;

bool IsValid() const;

uint32_t GetFrameID() const;

lldb::addr_t GetCFA() const;

lldb::addr_t GetPC() const;

bool SetPC(lldb::addr_t new_pc);

lldb::addr_t GetSP() const;

lldb::addr_t GetFP() const;

lldb::SBAddress GetPCAddress() const;

lldb::SBSymbolContext GetSymbolContext(uint32_t resolve_scope) const;

lldb::SBModule GetModule() const;

lldb::SBCompileUnit GetCompileUnit() const;

lldb::SBFunction GetFunction() const;

lldb::SBSymbol GetSymbol() const;




lldb::SBBlock GetBlock() const;














const char *GetFunctionName();



const char *GetDisplayFunctionName();

const char *GetFunctionName() const;



lldb::LanguageType GuessLanguage() const;




bool IsInlined();

bool IsInlined() const;

bool IsArtificial();

bool IsArtificial() const;



lldb::SBValue EvaluateExpression(const char *expr);

lldb::SBValue EvaluateExpression(const char *expr,
lldb::DynamicValueType use_dynamic);

lldb::SBValue EvaluateExpression(const char *expr,
lldb::DynamicValueType use_dynamic,
bool unwind_on_error);

lldb::SBValue EvaluateExpression(const char *expr,
const SBExpressionOptions &options);













lldb::SBBlock GetFrameBlock() const;

lldb::SBLineEntry GetLineEntry() const;

lldb::SBThread GetThread() const;

const char *Disassemble() const;

void Clear();

bool operator==(const lldb::SBFrame &rhs) const;

bool operator!=(const lldb::SBFrame &rhs) const;



lldb::SBValueList GetVariables(bool arguments, bool locals, bool statics,
bool in_scope_only);

lldb::SBValueList GetVariables(bool arguments, bool locals, bool statics,
bool in_scope_only,
lldb::DynamicValueType use_dynamic);

lldb::SBValueList GetVariables(const lldb::SBVariablesOptions &options);

lldb::SBValueList GetRegisters();

lldb::SBValue FindRegister(const char *name);



lldb::SBValue FindVariable(const char *var_name);

lldb::SBValue FindVariable(const char *var_name,
lldb::DynamicValueType use_dynamic);






lldb::SBValue GetValueForVariablePath(const char *var_expr_cstr,
DynamicValueType use_dynamic);



lldb::SBValue GetValueForVariablePath(const char *var_path);









lldb::SBValue FindValue(const char *name, ValueType value_type);

lldb::SBValue FindValue(const char *name, ValueType value_type,
lldb::DynamicValueType use_dynamic);

bool GetDescription(lldb::SBStream &description);

SBFrame(const lldb::StackFrameSP &lldb_object_sp);

protected:
friend class SBBlock;
friend class SBExecutionContext;
friend class SBInstruction;
friend class SBThread;
friend class SBValue;

lldb::StackFrameSP GetFrameSP() const;

void SetFrameSP(const lldb::StackFrameSP &lldb_object_sp);

lldb::ExecutionContextRefSP m_opaque_sp;
};

}

#endif
