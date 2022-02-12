







#if !defined(LLDB_SYMBOL_FUNCTION_H)
#define LLDB_SYMBOL_FUNCTION_H

#include "lldb/Core/AddressRange.h"
#include "lldb/Core/Declaration.h"
#include "lldb/Core/Mangled.h"
#include "lldb/Expression/DWARFExpression.h"
#include "lldb/Symbol/Block.h"
#include "lldb/Utility/UserID.h"
#include "llvm/ADT/ArrayRef.h"

#include <mutex>

namespace lldb_private {

class ExecutionContext;






class FunctionInfo {
public:











FunctionInfo(const char *name, const Declaration *decl_ptr);











FunctionInfo(ConstString name, const Declaration *decl_ptr);




virtual ~FunctionInfo();
















static int Compare(const FunctionInfo &lhs, const FunctionInfo &rhs);








void Dump(Stream *s, bool show_fullpaths) const;





Declaration &GetDeclaration();





const Declaration &GetDeclaration() const;





ConstString GetName() const;









virtual size_t MemorySize() const;

protected:

ConstString m_name;


Declaration m_declaration;
};



class InlineFunctionInfo : public FunctionInfo {
public:



















InlineFunctionInfo(const char *name, llvm::StringRef mangled,
const Declaration *decl_ptr,
const Declaration *call_decl_ptr);



















InlineFunctionInfo(ConstString name, const Mangled &mangled,
const Declaration *decl_ptr,
const Declaration *call_decl_ptr);


~InlineFunctionInfo() override;


















int Compare(const InlineFunctionInfo &lhs, const InlineFunctionInfo &rhs);








void Dump(Stream *s, bool show_fullpaths) const;

void DumpStopContext(Stream *s) const;

ConstString GetName() const;

ConstString GetDisplayName() const;





Declaration &GetCallSite();





const Declaration &GetCallSite() const;





Mangled &GetMangled();





const Mangled &GetMangled() const;









size_t MemorySize() const override;

private:


Mangled m_mangled;

Declaration m_call_decl;
};

class Function;





struct CallSiteParameter {
DWARFExpression LocationInCallee;
DWARFExpression LocationInCaller;
};


using CallSiteParameterArray = llvm::SmallVector<CallSiteParameter, 0>;





class CallEdge {
public:
enum class AddrType : uint8_t { Call, AfterCall };
virtual ~CallEdge() = default;





virtual Function *GetCallee(ModuleList &images,
ExecutionContext &exe_ctx) = 0;





lldb::addr_t GetReturnPCAddress(Function &caller, Target &target) const;



std::pair<AddrType, lldb::addr_t> GetCallerAddress(Function &caller,
Target &target) const {
return {caller_address_type,
GetLoadAddress(caller_address, caller, target)};
}

bool IsTailCall() const { return is_tail_call; }


llvm::ArrayRef<CallSiteParameter> GetCallSiteParameters() const {
return parameters;
}



std::pair<bool, lldb::addr_t> GetSortKey() const {
return {is_tail_call, GetUnresolvedReturnPCAddress()};
}

protected:
CallEdge(AddrType caller_address_type, lldb::addr_t caller_address,
bool is_tail_call, CallSiteParameterArray &&parameters)
: caller_address(caller_address),
caller_address_type(caller_address_type), is_tail_call(is_tail_call),
parameters(std::move(parameters)) {}



static lldb::addr_t GetLoadAddress(lldb::addr_t unresolved_pc,
Function &caller, Target &target);


lldb::addr_t GetUnresolvedReturnPCAddress() const {
return caller_address_type == AddrType::AfterCall && !is_tail_call
? caller_address
: LLDB_INVALID_ADDRESS;
}

private:
lldb::addr_t caller_address;
AddrType caller_address_type;
bool is_tail_call;

CallSiteParameterArray parameters;
};




class DirectCallEdge : public CallEdge {
public:


DirectCallEdge(const char *symbol_name, AddrType caller_address_type,
lldb::addr_t caller_address, bool is_tail_call,
CallSiteParameterArray &&parameters)
: CallEdge(caller_address_type, caller_address, is_tail_call,
std::move(parameters)) {
lazy_callee.symbol_name = symbol_name;
}

Function *GetCallee(ModuleList &images, ExecutionContext &exe_ctx) override;

private:
void ParseSymbolFileAndResolve(ModuleList &images);





union {
const char *symbol_name;
Function *def;
} lazy_callee;


bool resolved = false;
};




class IndirectCallEdge : public CallEdge {
public:


IndirectCallEdge(DWARFExpression call_target, AddrType caller_address_type,
lldb::addr_t caller_address, bool is_tail_call,
CallSiteParameterArray &&parameters)
: CallEdge(caller_address_type, caller_address, is_tail_call,
std::move(parameters)),
call_target(std::move(call_target)) {}

Function *GetCallee(ModuleList &images, ExecutionContext &exe_ctx) override;

private:



DWARFExpression call_target;
};





















class Function : public UserID, public SymbolContextScope {
public:




























Function(CompileUnit *comp_unit, lldb::user_id_t func_uid,
lldb::user_id_t func_type_uid, const Mangled &mangled,
Type *func_type, const AddressRange &range);


~Function() override;




void CalculateSymbolContext(SymbolContext *sc) override;

lldb::ModuleSP CalculateSymbolContextModule() override;

CompileUnit *CalculateSymbolContextCompileUnit() override;

Function *CalculateSymbolContextFunction() override;

const AddressRange &GetAddressRange() { return m_range; }

lldb::LanguageType GetLanguage() const;










void GetStartLineSourceInfo(FileSpec &source_file, uint32_t &line_no);










void GetEndLineSourceInfo(FileSpec &source_file, uint32_t &line_no);



llvm::ArrayRef<std::unique_ptr<CallEdge>> GetCallEdges();



llvm::ArrayRef<std::unique_ptr<CallEdge>> GetTailCallingEdges();




CallEdge *GetCallEdgeForReturnAddress(lldb::addr_t return_pc, Target &target);








Block &GetBlock(bool can_create);





CompileUnit *GetCompileUnit();





const CompileUnit *GetCompileUnit() const;

void GetDescription(Stream *s, lldb::DescriptionLevel level, Target *target);






DWARFExpression &GetFrameBaseExpression() { return m_frame_base; }





const DWARFExpression &GetFrameBaseExpression() const { return m_frame_base; }

ConstString GetName() const;

ConstString GetNameNoArguments() const;

ConstString GetDisplayName() const;

const Mangled &GetMangled() const { return m_mangled; }





CompilerDeclContext GetDeclContext();






Type *GetType();






const Type *GetType() const;

CompilerType GetCompilerType();







uint32_t GetPrologueByteSize();












void Dump(Stream *s, bool show_context) const;




void DumpSymbolContext(Stream *s) override;









size_t MemorySize() const;














bool GetIsOptimized();














bool IsTopLevelFunction();

lldb::DisassemblerSP GetInstructions(const ExecutionContext &exe_ctx,
const char *flavor,
bool force_live_memory = false);

bool GetDisassembly(const ExecutionContext &exe_ctx, const char *flavor,
Stream &strm, bool force_live_memory = false);

protected:
enum {

flagsCalculatedPrologueSize = (1 << 0)
};


CompileUnit *m_comp_unit;


lldb::user_id_t m_type_uid;



Type *m_type;



Mangled m_mangled;


Block m_block;



AddressRange m_range;



DWARFExpression m_frame_base;

Flags m_flags;


uint32_t m_prologue_byte_size;



std::mutex m_call_edges_lock;


bool m_call_edges_resolved = false;


std::vector<std::unique_ptr<CallEdge>> m_call_edges;

private:
Function(const Function &) = delete;
const Function &operator=(const Function &) = delete;
};

}

#endif
