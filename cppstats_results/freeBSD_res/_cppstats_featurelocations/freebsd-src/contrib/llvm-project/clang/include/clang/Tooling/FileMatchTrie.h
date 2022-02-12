












#if !defined(LLVM_CLANG_TOOLING_FILEMATCHTRIE_H)
#define LLVM_CLANG_TOOLING_FILEMATCHTRIE_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include <memory>

namespace clang {
namespace tooling {

class FileMatchTrieNode;

struct PathComparator {
virtual ~PathComparator() = default;

virtual bool equivalent(StringRef FileA, StringRef FileB) const = 0;
};

























class FileMatchTrie {
public:
FileMatchTrie();




FileMatchTrie(PathComparator* Comparator);

~FileMatchTrie();


void insert(StringRef NewPath);








StringRef findEquivalent(StringRef FileName,
raw_ostream &Error) const;

private:
FileMatchTrieNode *Root;
std::unique_ptr<PathComparator> Comparator;
};

}
}

#endif
