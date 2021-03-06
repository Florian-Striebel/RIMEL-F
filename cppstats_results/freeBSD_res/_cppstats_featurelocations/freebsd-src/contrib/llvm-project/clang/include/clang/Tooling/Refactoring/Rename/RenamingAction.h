












#if !defined(LLVM_CLANG_TOOLING_REFACTOR_RENAME_RENAMING_ACTION_H)
#define LLVM_CLANG_TOOLING_REFACTOR_RENAME_RENAMING_ACTION_H

#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "clang/Tooling/Refactoring/RefactoringActionRules.h"
#include "clang/Tooling/Refactoring/RefactoringOptions.h"
#include "clang/Tooling/Refactoring/Rename/SymbolOccurrences.h"
#include "llvm/Support/Error.h"

namespace clang {
class ASTConsumer;
class CompilerInstance;

namespace tooling {

class RenamingAction {
public:
RenamingAction(const std::vector<std::string> &NewNames,
const std::vector<std::string> &PrevNames,
const std::vector<std::vector<std::string>> &USRList,
std::map<std::string, tooling::Replacements> &FileToReplaces,
bool PrintLocations = false)
: NewNames(NewNames), PrevNames(PrevNames), USRList(USRList),
FileToReplaces(FileToReplaces), PrintLocations(PrintLocations) {}

std::unique_ptr<ASTConsumer> newASTConsumer();

private:
const std::vector<std::string> &NewNames, &PrevNames;
const std::vector<std::vector<std::string>> &USRList;
std::map<std::string, tooling::Replacements> &FileToReplaces;
bool PrintLocations;
};

class RenameOccurrences final : public SourceChangeRefactoringRule {
public:
static Expected<RenameOccurrences> initiate(RefactoringRuleContext &Context,
SourceRange SelectionRange,
std::string NewName);

static const RefactoringDescriptor &describe();

const NamedDecl *getRenameDecl() const;

private:
RenameOccurrences(const NamedDecl *ND, std::string NewName)
: ND(ND), NewName(std::move(NewName)) {}

Expected<AtomicChanges>
createSourceReplacements(RefactoringRuleContext &Context) override;

const NamedDecl *ND;
std::string NewName;
};

class QualifiedRenameRule final : public SourceChangeRefactoringRule {
public:
static Expected<QualifiedRenameRule> initiate(RefactoringRuleContext &Context,
std::string OldQualifiedName,
std::string NewQualifiedName);

static const RefactoringDescriptor &describe();

private:
QualifiedRenameRule(const NamedDecl *ND,
std::string NewQualifiedName)
: ND(ND), NewQualifiedName(std::move(NewQualifiedName)) {}

Expected<AtomicChanges>
createSourceReplacements(RefactoringRuleContext &Context) override;


const NamedDecl *ND;

std::string NewQualifiedName;
};



llvm::Expected<std::vector<AtomicChange>>
createRenameReplacements(const SymbolOccurrences &Occurrences,
const SourceManager &SM, const SymbolName &NewName);


class QualifiedRenamingAction {
public:
QualifiedRenamingAction(
const std::vector<std::string> &NewNames,
const std::vector<std::vector<std::string>> &USRList,
std::map<std::string, tooling::Replacements> &FileToReplaces)
: NewNames(NewNames), USRList(USRList), FileToReplaces(FileToReplaces) {}

std::unique_ptr<ASTConsumer> newASTConsumer();

private:

const std::vector<std::string> &NewNames;


const std::vector<std::vector<std::string>> &USRList;


std::map<std::string, tooling::Replacements> &FileToReplaces;
};

}
}

#endif
