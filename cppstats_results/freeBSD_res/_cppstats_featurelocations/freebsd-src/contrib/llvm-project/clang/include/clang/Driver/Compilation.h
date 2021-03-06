







#if !defined(LLVM_CLANG_DRIVER_COMPILATION_H)
#define LLVM_CLANG_DRIVER_COMPILATION_H

#include "clang/Basic/LLVM.h"
#include "clang/Driver/Action.h"
#include "clang/Driver/Job.h"
#include "clang/Driver/Util.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include <cassert>
#include <iterator>
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace llvm {
namespace opt {

class DerivedArgList;
class InputArgList;

}
}

namespace clang {
namespace driver {

class Driver;
class ToolChain;



class Compilation {

const Driver &TheDriver;


const ToolChain &DefaultToolChain;



unsigned ActiveOffloadMask = 0;





std::multimap<Action::OffloadKind, const ToolChain *>
OrderedOffloadingToolchains;


llvm::opt::InputArgList *Args;



llvm::opt::DerivedArgList *TranslatedArgs;



std::vector<std::unique_ptr<Action>> AllActions;



ActionList Actions;


JobList Jobs;



struct TCArgsKey final {
const ToolChain *TC = nullptr;
StringRef BoundArch;
Action::OffloadKind DeviceOffloadKind = Action::OFK_None;

TCArgsKey(const ToolChain *TC, StringRef BoundArch,
Action::OffloadKind DeviceOffloadKind)
: TC(TC), BoundArch(BoundArch), DeviceOffloadKind(DeviceOffloadKind) {}

bool operator<(const TCArgsKey &K) const {
if (TC < K.TC)
return true;
else if (TC == K.TC && BoundArch < K.BoundArch)
return true;
else if (TC == K.TC && BoundArch == K.BoundArch &&
DeviceOffloadKind < K.DeviceOffloadKind)
return true;
return false;
}
};
std::map<TCArgsKey, llvm::opt::DerivedArgList *> TCArgs;


llvm::opt::ArgStringList TempFiles;


ArgStringMap ResultFiles;



ArgStringMap FailureResultFiles;


std::vector<Optional<StringRef>> Redirects;




std::function<void(const Command &, int)> PostCallback;


bool ForDiagnostics = false;


bool ContainsError;


bool ForceKeepTempFiles = false;

public:
Compilation(const Driver &D, const ToolChain &DefaultToolChain,
llvm::opt::InputArgList *Args,
llvm::opt::DerivedArgList *TranslatedArgs, bool ContainsError);
~Compilation();

const Driver &getDriver() const { return TheDriver; }

const ToolChain &getDefaultToolChain() const { return DefaultToolChain; }

unsigned isOffloadingHostKind(Action::OffloadKind Kind) const {
return ActiveOffloadMask & Kind;
}


using const_offload_toolchains_iterator =
const std::multimap<Action::OffloadKind,
const ToolChain *>::const_iterator;
using const_offload_toolchains_range =
std::pair<const_offload_toolchains_iterator,
const_offload_toolchains_iterator>;

template <Action::OffloadKind Kind>
const_offload_toolchains_range getOffloadToolChains() const {
return OrderedOffloadingToolchains.equal_range(Kind);
}


template <Action::OffloadKind Kind> bool hasOffloadToolChain() const {
return OrderedOffloadingToolchains.find(Kind) !=
OrderedOffloadingToolchains.end();
}



template <Action::OffloadKind Kind>
const ToolChain *getSingleOffloadToolChain() const {
auto TCs = getOffloadToolChains<Kind>();

assert(TCs.first != TCs.second &&
"No tool chains of the selected kind exist!");
assert(std::next(TCs.first) == TCs.second &&
"More than one tool chain of the this kind exist.");
return TCs.first->second;
}

void addOffloadDeviceToolChain(const ToolChain *DeviceToolChain,
Action::OffloadKind OffloadKind) {
assert(OffloadKind != Action::OFK_Host && OffloadKind != Action::OFK_None &&
"This is not a device tool chain!");


ActiveOffloadMask |= OffloadKind;
OrderedOffloadingToolchains.insert(
std::make_pair(OffloadKind, DeviceToolChain));
}

const llvm::opt::InputArgList &getInputArgs() const { return *Args; }

const llvm::opt::DerivedArgList &getArgs() const { return *TranslatedArgs; }

llvm::opt::DerivedArgList &getArgs() { return *TranslatedArgs; }

ActionList &getActions() { return Actions; }
const ActionList &getActions() const { return Actions; }




template <typename T, typename... Args> T *MakeAction(Args &&... Arg) {
T *RawPtr = new T(std::forward<Args>(Arg)...);
AllActions.push_back(std::unique_ptr<Action>(RawPtr));
return RawPtr;
}

JobList &getJobs() { return Jobs; }
const JobList &getJobs() const { return Jobs; }

void addCommand(std::unique_ptr<Command> C) { Jobs.addJob(std::move(C)); }

const llvm::opt::ArgStringList &getTempFiles() const { return TempFiles; }

const ArgStringMap &getResultFiles() const { return ResultFiles; }

const ArgStringMap &getFailureResultFiles() const {
return FailureResultFiles;
}





void setPostCallback(const std::function<void(const Command &, int)> &CB) {
PostCallback = CB;
}


StringRef getSysRoot() const;









const llvm::opt::DerivedArgList &
getArgsForToolChain(const ToolChain *TC, StringRef BoundArch,
Action::OffloadKind DeviceOffloadKind);



const char *addTempFile(const char *Name) {
TempFiles.push_back(Name);
return Name;
}



const char *addResultFile(const char *Name, const JobAction *JA) {
ResultFiles[JA] = Name;
return Name;
}



const char *addFailureResultFile(const char *Name, const JobAction *JA) {
FailureResultFiles[JA] = Name;
return Name;
}





bool CleanupFile(const char *File, bool IssueErrors = false) const;





bool CleanupFileList(const llvm::opt::ArgStringList &Files,
bool IssueErrors = false) const;







bool CleanupFileMap(const ArgStringMap &Files,
const JobAction *JA,
bool IssueErrors = false) const;






int ExecuteCommand(const Command &C, const Command *&FailingCommand) const;





void ExecuteJobs(
const JobList &Jobs,
SmallVectorImpl<std::pair<int, const Command *>> &FailingCommands) const;




void initCompilationForDiagnostics();


bool isForDiagnostics() const { return ForDiagnostics; }


bool containsError() const { return ContainsError; }



void setContainsError() { ContainsError = true; }






void Redirect(ArrayRef<Optional<StringRef>> Redirects);
};

}
}

#endif
