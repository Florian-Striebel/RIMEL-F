












#if !defined(LLVM_CLANG_BASIC_COMMENTOPTIONS_H)
#define LLVM_CLANG_BASIC_COMMENTOPTIONS_H

#include <string>
#include <vector>

namespace clang {


struct CommentOptions {
using BlockCommandNamesTy = std::vector<std::string>;



BlockCommandNamesTy BlockCommandNames;


bool ParseAllComments = false;

CommentOptions() = default;
};

}

#endif
