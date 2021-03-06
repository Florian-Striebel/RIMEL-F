











#if !defined(LLD_CORE_ERROR_H)
#define LLD_CORE_ERROR_H

#include "lld/Common/LLVM.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/Error.h"
#include <system_error>

namespace lld {

const std::error_category &YamlReaderCategory();

enum class YamlReaderError {
unknown_keyword,
illegal_value
};

inline std::error_code make_error_code(YamlReaderError e) {
return std::error_code(static_cast<int>(e), YamlReaderCategory());
}







std::error_code make_dynamic_error_code(StringRef msg);





class GenericError : public llvm::ErrorInfo<GenericError> {
public:
static char ID;
GenericError(Twine Msg);
const std::string &getMessage() const { return Msg; }
void log(llvm::raw_ostream &OS) const override;

std::error_code convertToErrorCode() const override {
return make_dynamic_error_code(getMessage());
}

private:
std::string Msg;
};

}

namespace std {
template <> struct is_error_code_enum<lld::YamlReaderError> : std::true_type {};
}

#endif
