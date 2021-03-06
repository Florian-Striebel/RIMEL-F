







#if !defined(LLVM_CLANG_APINOTES_APINOTESYAMLCOMPILER_H)
#define LLVM_CLANG_APINOTES_APINOTESYAMLCOMPILER_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {
namespace api_notes {



bool parseAndDumpAPINotes(llvm::StringRef YI, llvm::raw_ostream &OS);
}
}

#endif
