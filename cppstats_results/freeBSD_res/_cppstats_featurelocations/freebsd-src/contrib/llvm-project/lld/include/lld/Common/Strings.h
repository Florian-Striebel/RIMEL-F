







#if !defined(LLD_STRINGS_H)
#define LLD_STRINGS_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/GlobPattern.h"
#include <string>
#include <vector>

namespace lld {


std::string demangleItanium(llvm::StringRef name);

std::vector<uint8_t> parseHex(llvm::StringRef s);
bool isValidCIdentifier(llvm::StringRef s);


void saveBuffer(llvm::StringRef buffer, const llvm::Twine &path);




class SingleStringMatcher {
public:


SingleStringMatcher(llvm::StringRef Pattern);


bool match(llvm::StringRef s) const;


bool isTrivialMatchAll() const {
return !ExactMatch && GlobPatternMatcher.isTrivialMatchAll();
}

private:

bool ExactMatch;


llvm::GlobPattern GlobPatternMatcher;


llvm::StringRef ExactPattern;
};




class StringMatcher {
private:

std::vector<SingleStringMatcher> patterns;

public:
StringMatcher() = default;


StringMatcher(llvm::StringRef Pattern)
: patterns({SingleStringMatcher(Pattern)}) {}


void addPattern(SingleStringMatcher Matcher) { patterns.push_back(Matcher); }

bool empty() const { return patterns.empty(); }


bool match(llvm::StringRef s) const;
};

}

#endif
