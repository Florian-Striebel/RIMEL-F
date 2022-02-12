







#if !defined(LLDB_CORE_RICHMANGLINGCONTEXT_H)
#define LLDB_CORE_RICHMANGLINGCONTEXT_H

#include "lldb/lldb-forward.h"
#include "lldb/lldb-private.h"

#include "lldb/Utility/ConstString.h"

#include "llvm/ADT/Any.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Demangle/Demangle.h"

namespace lldb_private {



class RichManglingContext {
public:
RichManglingContext() {
m_ipd_buf = static_cast<char *>(std::malloc(m_ipd_buf_size));
m_ipd_buf[0] = '\0';
}

~RichManglingContext();



bool FromItaniumName(ConstString mangled);



bool FromCxxMethodName(ConstString demangled);


bool IsCtorOrDtor() const;


bool IsFunction() const;




void ParseFunctionBaseName();




void ParseFunctionDeclContextName();



void ParseFullName();



llvm::StringRef GetBufferRef() const {
assert(m_provider != None && "Initialize a provider first");
return m_buffer;
}

private:
enum InfoProvider { None, ItaniumPartialDemangler, PluginCxxLanguage };


InfoProvider m_provider = None;


llvm::StringRef m_buffer;


llvm::ItaniumPartialDemangler m_ipd;



char *m_ipd_buf;
size_t m_ipd_buf_size = 2048;





llvm::Any m_cxx_method_parser;


void ResetCxxMethodParser();


void ResetProvider(InfoProvider new_provider);


void processIPDStrResult(char *ipd_res, size_t res_len);




template <class ParserT> static ParserT *get(llvm::Any parser) {
assert(parser.hasValue());
assert(llvm::any_isa<ParserT *>(parser));
return llvm::any_cast<ParserT *>(parser);
}
};

}

#endif
