







#if !defined(LLVM_CLANG_DRIVER_ACTION_H)
#define LLVM_CLANG_DRIVER_ACTION_H

#include "clang/Basic/LLVM.h"
#include "clang/Driver/Types.h"
#include "clang/Driver/Util.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include <string>

namespace llvm {
namespace opt {

class Arg;

}
}

namespace clang {
namespace driver {

class ToolChain;













class Action {
public:
using size_type = ActionList::size_type;
using input_iterator = ActionList::iterator;
using input_const_iterator = ActionList::const_iterator;
using input_range = llvm::iterator_range<input_iterator>;
using input_const_range = llvm::iterator_range<input_const_iterator>;

enum ActionClass {
InputClass = 0,
BindArchClass,
OffloadClass,
PreprocessJobClass,
PrecompileJobClass,
HeaderModulePrecompileJobClass,
AnalyzeJobClass,
MigrateJobClass,
CompileJobClass,
BackendJobClass,
AssembleJobClass,
LinkJobClass,
IfsMergeJobClass,
LipoJobClass,
DsymutilJobClass,
VerifyDebugInfoJobClass,
VerifyPCHJobClass,
OffloadBundlingJobClass,
OffloadUnbundlingJobClass,
OffloadWrapperJobClass,
StaticLibJobClass,

JobClassFirst = PreprocessJobClass,
JobClassLast = StaticLibJobClass
};




enum OffloadKind {
OFK_None = 0x00,


OFK_Host = 0x01,


OFK_Cuda = 0x02,
OFK_OpenMP = 0x04,
OFK_HIP = 0x08,
};

static const char *getClassName(ActionClass AC);

private:
ActionClass Kind;


types::ID Type;

ActionList Inputs;





bool CanBeCollapsedWithNextDependentAction = true;

protected:







unsigned ActiveOffloadKindMask = 0u;


OffloadKind OffloadingDeviceKind = OFK_None;


const char *OffloadingArch = nullptr;

Action(ActionClass Kind, types::ID Type) : Action(Kind, ActionList(), Type) {}
Action(ActionClass Kind, Action *Input, types::ID Type)
: Action(Kind, ActionList({Input}), Type) {}
Action(ActionClass Kind, Action *Input)
: Action(Kind, ActionList({Input}), Input->getType()) {}
Action(ActionClass Kind, const ActionList &Inputs, types::ID Type)
: Kind(Kind), Type(Type), Inputs(Inputs) {}

public:
virtual ~Action();

const char *getClassName() const { return Action::getClassName(getKind()); }

ActionClass getKind() const { return Kind; }
types::ID getType() const { return Type; }

ActionList &getInputs() { return Inputs; }
const ActionList &getInputs() const { return Inputs; }

size_type size() const { return Inputs.size(); }

input_iterator input_begin() { return Inputs.begin(); }
input_iterator input_end() { return Inputs.end(); }
input_range inputs() { return input_range(input_begin(), input_end()); }
input_const_iterator input_begin() const { return Inputs.begin(); }
input_const_iterator input_end() const { return Inputs.end(); }
input_const_range inputs() const {
return input_const_range(input_begin(), input_end());
}


void setCannotBeCollapsedWithNextDependentAction() {
CanBeCollapsedWithNextDependentAction = false;
}


bool isCollapsingWithNextDependentActionLegal() const {
return CanBeCollapsedWithNextDependentAction;
}


std::string getOffloadingKindPrefix() const;




static std::string
GetOffloadingFileNamePrefix(OffloadKind Kind,
StringRef NormalizedTriple,
bool CreatePrefixForHost = false);


static StringRef GetOffloadKindName(OffloadKind Kind);



void propagateDeviceOffloadInfo(OffloadKind OKind, const char *OArch);



void propagateHostOffloadInfo(unsigned OKinds, const char *OArch);



void propagateOffloadInfo(const Action *A);

unsigned getOffloadingHostActiveKinds() const {
return ActiveOffloadKindMask;
}

OffloadKind getOffloadingDeviceKind() const { return OffloadingDeviceKind; }
const char *getOffloadingArch() const { return OffloadingArch; }



bool isHostOffloading(OffloadKind OKind) const {
return ActiveOffloadKindMask & OKind;
}
bool isDeviceOffloading(OffloadKind OKind) const {
return OffloadingDeviceKind == OKind;
}
bool isOffloading(OffloadKind OKind) const {
return isHostOffloading(OKind) || isDeviceOffloading(OKind);
}
};

class InputAction : public Action {
const llvm::opt::Arg &Input;
std::string Id;
virtual void anchor();

public:
InputAction(const llvm::opt::Arg &Input, types::ID Type,
StringRef Id = StringRef());

const llvm::opt::Arg &getInputArg() const { return Input; }

void setId(StringRef _Id) { Id = _Id.str(); }
StringRef getId() const { return Id; }

static bool classof(const Action *A) {
return A->getKind() == InputClass;
}
};

class BindArchAction : public Action {
virtual void anchor();



StringRef ArchName;

public:
BindArchAction(Action *Input, StringRef ArchName);

StringRef getArchName() const { return ArchName; }

static bool classof(const Action *A) {
return A->getKind() == BindArchClass;
}
};




class OffloadAction final : public Action {
virtual void anchor();

public:


class DeviceDependences final {
public:
using ToolChainList = SmallVector<const ToolChain *, 3>;
using BoundArchList = SmallVector<const char *, 3>;
using OffloadKindList = SmallVector<OffloadKind, 3>;

private:






ActionList DeviceActions;


ToolChainList DeviceToolChains;


BoundArchList DeviceBoundArchs;


OffloadKindList DeviceOffloadKinds;

public:


void add(Action &A, const ToolChain &TC, const char *BoundArch,
OffloadKind OKind);


const ActionList &getActions() const { return DeviceActions; }
const ToolChainList &getToolChains() const { return DeviceToolChains; }
const BoundArchList &getBoundArchs() const { return DeviceBoundArchs; }
const OffloadKindList &getOffloadKinds() const {
return DeviceOffloadKinds;
}
};



class HostDependence final {

Action &HostAction;


const ToolChain &HostToolChain;


const char *HostBoundArch = nullptr;


unsigned HostOffloadKinds = 0u;

public:
HostDependence(Action &A, const ToolChain &TC, const char *BoundArch,
const unsigned OffloadKinds)
: HostAction(A), HostToolChain(TC), HostBoundArch(BoundArch),
HostOffloadKinds(OffloadKinds) {}



HostDependence(Action &A, const ToolChain &TC, const char *BoundArch,
const DeviceDependences &DDeps);
Action *getAction() const { return &HostAction; }
const ToolChain *getToolChain() const { return &HostToolChain; }
const char *getBoundArch() const { return HostBoundArch; }
unsigned getOffloadKinds() const { return HostOffloadKinds; }
};

using OffloadActionWorkTy =
llvm::function_ref<void(Action *, const ToolChain *, const char *)>;

private:

const ToolChain *HostTC = nullptr;


DeviceDependences::ToolChainList DevToolChains;

public:
OffloadAction(const HostDependence &HDep);
OffloadAction(const DeviceDependences &DDeps, types::ID Ty);
OffloadAction(const HostDependence &HDep, const DeviceDependences &DDeps);


void doOnHostDependence(const OffloadActionWorkTy &Work) const;


void doOnEachDeviceDependence(const OffloadActionWorkTy &Work) const;


void doOnEachDependence(const OffloadActionWorkTy &Work) const;



void doOnEachDependence(bool IsHostDependence,
const OffloadActionWorkTy &Work) const;


bool hasHostDependence() const;



Action *getHostDependence() const;




bool hasSingleDeviceDependence(bool DoNotConsiderHostActions = false) const;




Action *
getSingleDeviceDependence(bool DoNotConsiderHostActions = false) const;

static bool classof(const Action *A) { return A->getKind() == OffloadClass; }
};

class JobAction : public Action {
virtual void anchor();

protected:
JobAction(ActionClass Kind, Action *Input, types::ID Type);
JobAction(ActionClass Kind, const ActionList &Inputs, types::ID Type);

public:
static bool classof(const Action *A) {
return (A->getKind() >= JobClassFirst &&
A->getKind() <= JobClassLast);
}
};

class PreprocessJobAction : public JobAction {
void anchor() override;

public:
PreprocessJobAction(Action *Input, types::ID OutputType);

static bool classof(const Action *A) {
return A->getKind() == PreprocessJobClass;
}
};

class PrecompileJobAction : public JobAction {
void anchor() override;

protected:
PrecompileJobAction(ActionClass Kind, Action *Input, types::ID OutputType);

public:
PrecompileJobAction(Action *Input, types::ID OutputType);

static bool classof(const Action *A) {
return A->getKind() == PrecompileJobClass ||
A->getKind() == HeaderModulePrecompileJobClass;
}
};

class HeaderModulePrecompileJobAction : public PrecompileJobAction {
void anchor() override;

const char *ModuleName;

public:
HeaderModulePrecompileJobAction(Action *Input, types::ID OutputType,
const char *ModuleName);

static bool classof(const Action *A) {
return A->getKind() == HeaderModulePrecompileJobClass;
}

void addModuleHeaderInput(Action *Input) {
getInputs().push_back(Input);
}

const char *getModuleName() const { return ModuleName; }
};

class AnalyzeJobAction : public JobAction {
void anchor() override;

public:
AnalyzeJobAction(Action *Input, types::ID OutputType);

static bool classof(const Action *A) {
return A->getKind() == AnalyzeJobClass;
}
};

class MigrateJobAction : public JobAction {
void anchor() override;

public:
MigrateJobAction(Action *Input, types::ID OutputType);

static bool classof(const Action *A) {
return A->getKind() == MigrateJobClass;
}
};

class CompileJobAction : public JobAction {
void anchor() override;

public:
CompileJobAction(Action *Input, types::ID OutputType);

static bool classof(const Action *A) {
return A->getKind() == CompileJobClass;
}
};

class BackendJobAction : public JobAction {
void anchor() override;

public:
BackendJobAction(Action *Input, types::ID OutputType);

static bool classof(const Action *A) {
return A->getKind() == BackendJobClass;
}
};

class AssembleJobAction : public JobAction {
void anchor() override;

public:
AssembleJobAction(Action *Input, types::ID OutputType);

static bool classof(const Action *A) {
return A->getKind() == AssembleJobClass;
}
};

class IfsMergeJobAction : public JobAction {
void anchor() override;

public:
IfsMergeJobAction(ActionList &Inputs, types::ID Type);

static bool classof(const Action *A) {
return A->getKind() == IfsMergeJobClass;
}
};

class LinkJobAction : public JobAction {
void anchor() override;

public:
LinkJobAction(ActionList &Inputs, types::ID Type);

static bool classof(const Action *A) {
return A->getKind() == LinkJobClass;
}
};

class LipoJobAction : public JobAction {
void anchor() override;

public:
LipoJobAction(ActionList &Inputs, types::ID Type);

static bool classof(const Action *A) {
return A->getKind() == LipoJobClass;
}
};

class DsymutilJobAction : public JobAction {
void anchor() override;

public:
DsymutilJobAction(ActionList &Inputs, types::ID Type);

static bool classof(const Action *A) {
return A->getKind() == DsymutilJobClass;
}
};

class VerifyJobAction : public JobAction {
void anchor() override;

public:
VerifyJobAction(ActionClass Kind, Action *Input, types::ID Type);

static bool classof(const Action *A) {
return A->getKind() == VerifyDebugInfoJobClass ||
A->getKind() == VerifyPCHJobClass;
}
};

class VerifyDebugInfoJobAction : public VerifyJobAction {
void anchor() override;

public:
VerifyDebugInfoJobAction(Action *Input, types::ID Type);

static bool classof(const Action *A) {
return A->getKind() == VerifyDebugInfoJobClass;
}
};

class VerifyPCHJobAction : public VerifyJobAction {
void anchor() override;

public:
VerifyPCHJobAction(Action *Input, types::ID Type);

static bool classof(const Action *A) {
return A->getKind() == VerifyPCHJobClass;
}
};

class OffloadBundlingJobAction : public JobAction {
void anchor() override;

public:

OffloadBundlingJobAction(ActionList &Inputs);

static bool classof(const Action *A) {
return A->getKind() == OffloadBundlingJobClass;
}
};

class OffloadUnbundlingJobAction final : public JobAction {
void anchor() override;

public:


struct DependentActionInfo final {

const ToolChain *DependentToolChain = nullptr;


StringRef DependentBoundArch;


const OffloadKind DependentOffloadKind = OFK_None;

DependentActionInfo(const ToolChain *DependentToolChain,
StringRef DependentBoundArch,
const OffloadKind DependentOffloadKind)
: DependentToolChain(DependentToolChain),
DependentBoundArch(DependentBoundArch),
DependentOffloadKind(DependentOffloadKind) {}
};

private:


SmallVector<DependentActionInfo, 6> DependentActionInfoArray;

public:

OffloadUnbundlingJobAction(Action *Input);


void registerDependentActionInfo(const ToolChain *TC, StringRef BoundArch,
OffloadKind Kind) {
DependentActionInfoArray.push_back({TC, BoundArch, Kind});
}


ArrayRef<DependentActionInfo> getDependentActionsInfo() const {
return DependentActionInfoArray;
}

static bool classof(const Action *A) {
return A->getKind() == OffloadUnbundlingJobClass;
}
};

class OffloadWrapperJobAction : public JobAction {
void anchor() override;

public:
OffloadWrapperJobAction(ActionList &Inputs, types::ID Type);

static bool classof(const Action *A) {
return A->getKind() == OffloadWrapperJobClass;
}
};

class StaticLibJobAction : public JobAction {
void anchor() override;

public:
StaticLibJobAction(ActionList &Inputs, types::ID Type);

static bool classof(const Action *A) {
return A->getKind() == StaticLibJobClass;
}
};

}
}

#endif
