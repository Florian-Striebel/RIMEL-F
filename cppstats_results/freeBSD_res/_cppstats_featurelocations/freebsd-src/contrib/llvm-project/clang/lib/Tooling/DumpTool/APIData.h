







#if !defined(LLVM_CLANG_LIB_TOOLING_DUMPTOOL_APIDATA_H)
#define LLVM_CLANG_LIB_TOOLING_DUMPTOOL_APIDATA_H

#include <string>
#include <vector>

namespace clang {
namespace tooling {

struct ClassData {
std::vector<std::string> ASTClassLocations;
std::vector<std::string> ASTClassRanges;
std::vector<std::string> TemplateParms;
std::vector<std::string> TypeSourceInfos;
std::vector<std::string> TypeLocs;
std::vector<std::string> NestedNameLocs;
std::vector<std::string> DeclNameInfos;
};

}
}

#endif
