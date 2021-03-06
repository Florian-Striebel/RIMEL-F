












#if !defined(LLVM_CLANG_TOOLING_REFACTOR_ATOMICCHANGE_H)
#define LLVM_CLANG_TOOLING_REFACTOR_ATOMICCHANGE_H

#include "clang/Basic/SourceManager.h"
#include "clang/Format/Format.h"
#include "clang/Tooling/Core/Replacement.h"
#include "llvm/ADT/Any.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"

namespace clang {
namespace tooling {











class AtomicChange {
public:




AtomicChange(const SourceManager &SM, SourceLocation KeyPosition);

AtomicChange(const SourceManager &SM, SourceLocation KeyPosition,
llvm::Any Metadata);


AtomicChange(llvm::StringRef FilePath, llvm::StringRef Key)
: Key(Key), FilePath(FilePath) {}

AtomicChange(AtomicChange &&) = default;
AtomicChange(const AtomicChange &) = default;

AtomicChange &operator=(AtomicChange &&) = default;
AtomicChange &operator=(const AtomicChange &) = default;

bool operator==(const AtomicChange &Other) const;


std::string toYAMLString();


static AtomicChange convertFromYAML(llvm::StringRef YAMLContent);



const std::string &getKey() const { return Key; }


const std::string &getFilePath() const { return FilePath; }





void setError(llvm::StringRef Error) { this->Error = std::string(Error); }


bool hasError() const { return !Error.empty(); }


const std::string &getError() const { return Error; }




llvm::Error replace(const SourceManager &SM, const CharSourceRange &Range,
llvm::StringRef ReplacementText);




llvm::Error replace(const SourceManager &SM, SourceLocation Loc,
unsigned Length, llvm::StringRef Text);









llvm::Error insert(const SourceManager &SM, SourceLocation Loc,
llvm::StringRef Text, bool InsertAfter = true);




void addHeader(llvm::StringRef Header);


void removeHeader(llvm::StringRef Header);


const Replacements &getReplacements() const { return Replaces; }

llvm::ArrayRef<std::string> getInsertedHeaders() const {
return InsertedHeaders;
}

llvm::ArrayRef<std::string> getRemovedHeaders() const {
return RemovedHeaders;
}

const llvm::Any &getMetadata() const { return Metadata; }

private:
AtomicChange() {}

AtomicChange(std::string Key, std::string FilePath, std::string Error,
std::vector<std::string> InsertedHeaders,
std::vector<std::string> RemovedHeaders,
clang::tooling::Replacements Replaces);


std::string Key;
std::string FilePath;
std::string Error;
std::vector<std::string> InsertedHeaders;
std::vector<std::string> RemovedHeaders;
tooling::Replacements Replaces;





llvm::Any Metadata;
};

using AtomicChanges = std::vector<AtomicChange>;


struct ApplyChangesSpec {



bool Cleanup = true;

format::FormatStyle Style = format::getNoStyle();





enum FormatOption { kAll, kNone, kViolations };

FormatOption Format = kNone;
};











llvm::Expected<std::string>
applyAtomicChanges(llvm::StringRef FilePath, llvm::StringRef Code,
llvm::ArrayRef<AtomicChange> Changes,
const ApplyChangesSpec &Spec);

}
}

#endif
