
























#if !defined(LLVM_CLANG_AST_DATACOLLECTION_H)
#define LLVM_CLANG_AST_DATACOLLECTION_H

#include "clang/AST/ASTContext.h"

namespace clang {
namespace data_collection {






std::string getMacroStack(SourceLocation Loc, ASTContext &Context);



template <class T>
void addDataToConsumer(T &DataConsumer, llvm::StringRef Str) {
DataConsumer.update(Str);
}

template <class T> void addDataToConsumer(T &DataConsumer, const QualType &QT) {
addDataToConsumer(DataConsumer, QT.getAsString());
}

template <class T, class Type>
std::enable_if_t<std::is_integral<Type>::value || std::is_enum<Type>::value ||
std::is_convertible<Type, size_t>::value
>
addDataToConsumer(T &DataConsumer, Type Data) {
DataConsumer.update(StringRef(reinterpret_cast<char *>(&Data), sizeof(Data)));
}

}
}

#endif
