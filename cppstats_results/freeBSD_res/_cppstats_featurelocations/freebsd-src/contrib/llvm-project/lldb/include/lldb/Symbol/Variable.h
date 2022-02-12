







#if !defined(LLDB_SYMBOL_VARIABLE_H)
#define LLDB_SYMBOL_VARIABLE_H

#include "lldb/Core/Declaration.h"
#include "lldb/Core/Mangled.h"
#include "lldb/Expression/DWARFExpression.h"
#include "lldb/Utility/CompletionRequest.h"
#include "lldb/Utility/RangeMap.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-private.h"
#include <memory>
#include <vector>

namespace lldb_private {

class Variable : public UserID, public std::enable_shared_from_this<Variable> {
public:
typedef RangeVector<lldb::addr_t, lldb::addr_t> RangeList;




Variable(lldb::user_id_t uid, const char *name, const char *mangled,
const lldb::SymbolFileTypeSP &symfile_type_sp, lldb::ValueType scope,
SymbolContextScope *owner_scope, const RangeList &scope_range,
Declaration *decl, const DWARFExpression &location, bool external,
bool artificial, bool location_is_constant_data,
bool static_member = false);

virtual ~Variable();

void Dump(Stream *s, bool show_context) const;

bool DumpDeclaration(Stream *s, bool show_fullpaths, bool show_module);

const Declaration &GetDeclaration() const { return m_declaration; }

ConstString GetName() const;

ConstString GetUnqualifiedName() const;

SymbolContextScope *GetSymbolContextScope() const { return m_owner_scope; }






bool NameMatches(ConstString name) const;

bool NameMatches(const RegularExpression &regex) const;

Type *GetType();

lldb::LanguageType GetLanguage() const;

lldb::ValueType GetScope() const { return m_scope; }

const RangeList &GetScopeRange() const { return m_scope_range; }

bool IsExternal() const { return m_external; }

bool IsArtificial() const { return m_artificial; }

bool IsStaticMember() const { return m_static_member; }

DWARFExpression &LocationExpression() { return m_location; }

const DWARFExpression &LocationExpression() const { return m_location; }

bool DumpLocationForAddress(Stream *s, const Address &address);

size_t MemorySize() const;

void CalculateSymbolContext(SymbolContext *sc);

bool IsInScope(StackFrame *frame);

bool LocationIsValidForFrame(StackFrame *frame);

bool LocationIsValidForAddress(const Address &address);

bool GetLocationIsConstantValueData() const { return m_loc_is_const_data; }

void SetLocationIsConstantValueData(bool b) { m_loc_is_const_data = b; }

typedef size_t (*GetVariableCallback)(void *baton, const char *name,
VariableList &var_list);

static Status GetValuesForVariableExpressionPath(
llvm::StringRef variable_expr_path, ExecutionContextScope *scope,
GetVariableCallback callback, void *baton, VariableList &variable_list,
ValueObjectList &valobj_list);

static void AutoComplete(const ExecutionContext &exe_ctx,
CompletionRequest &request);

CompilerDeclContext GetDeclContext();

CompilerDecl GetDecl();

protected:

ConstString m_name;

Mangled m_mangled;


lldb::SymbolFileTypeSP m_symfile_type_sp;
lldb::ValueType m_scope;

SymbolContextScope *m_owner_scope;


RangeList m_scope_range;

Declaration m_declaration;


DWARFExpression m_location;

unsigned m_external : 1;

unsigned m_artificial : 1;


unsigned m_loc_is_const_data : 1;

unsigned m_static_member : 1;

private:
Variable(const Variable &rhs) = delete;
Variable &operator=(const Variable &rhs) = delete;
};

}

#endif
