







#if !defined(LLDB_SYMBOL_VARIABLELIST_H)
#define LLDB_SYMBOL_VARIABLELIST_H

#include "lldb/Symbol/SymbolContext.h"
#include "lldb/Symbol/Variable.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class VariableList {
typedef std::vector<lldb::VariableSP> collection;

public:


VariableList();
virtual ~VariableList();

void AddVariable(const lldb::VariableSP &var_sp);

bool AddVariableIfUnique(const lldb::VariableSP &var_sp);

void AddVariables(VariableList *variable_list);

void Clear();

void Dump(Stream *s, bool show_context) const;

lldb::VariableSP GetVariableAtIndex(size_t idx) const;

lldb::VariableSP RemoveVariableAtIndex(size_t idx);

lldb::VariableSP FindVariable(ConstString name,
bool include_static_members = true);

lldb::VariableSP FindVariable(ConstString name,
lldb::ValueType value_type,
bool include_static_members = true);

uint32_t FindVariableIndex(const lldb::VariableSP &var_sp);

size_t AppendVariablesIfUnique(VariableList &var_list);






size_t AppendVariablesIfUnique(const RegularExpression &regex,
VariableList &var_list, size_t &total_matches);

size_t AppendVariablesWithScope(lldb::ValueType type, VariableList &var_list,
bool if_unique = true);

uint32_t FindIndexForVariable(Variable *variable);

size_t MemorySize() const;

size_t GetSize() const;
bool Empty() const { return m_variables.empty(); }

typedef collection::iterator iterator;
typedef collection::const_iterator const_iterator;

iterator begin() { return m_variables.begin(); }
iterator end() { return m_variables.end(); }
const_iterator begin() const { return m_variables.begin(); }
const_iterator end() const { return m_variables.end(); }

protected:
collection m_variables;

private:

VariableList(const VariableList &) = delete;
const VariableList &operator=(const VariableList &) = delete;
};

}

#endif
