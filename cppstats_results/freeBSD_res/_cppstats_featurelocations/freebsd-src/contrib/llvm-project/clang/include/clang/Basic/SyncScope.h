












#if !defined(LLVM_CLANG_BASIC_SYNCSCOPE_H)
#define LLVM_CLANG_BASIC_SYNCSCOPE_H

#include "clang/Basic/LangOptions.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include <memory>

namespace clang {



















enum class SyncScope {
OpenCLWorkGroup,
OpenCLDevice,
OpenCLAllSVMDevices,
OpenCLSubGroup,
Last = OpenCLSubGroup
};

inline llvm::StringRef getAsString(SyncScope S) {
switch (S) {
case SyncScope::OpenCLWorkGroup:
return "opencl_workgroup";
case SyncScope::OpenCLDevice:
return "opencl_device";
case SyncScope::OpenCLAllSVMDevices:
return "opencl_allsvmdevices";
case SyncScope::OpenCLSubGroup:
return "opencl_subgroup";
}
llvm_unreachable("Invalid synch scope");
}


enum class AtomicScopeModelKind { None, OpenCL };


class AtomicScopeModel {
public:
virtual ~AtomicScopeModel() {}


virtual SyncScope map(unsigned S) const = 0;



virtual bool isValid(unsigned S) const = 0;



virtual ArrayRef<unsigned> getRuntimeValues() const = 0;




virtual unsigned getFallBackValue() const = 0;



static std::unique_ptr<AtomicScopeModel> create(AtomicScopeModelKind K);
};


class AtomicScopeOpenCLModel : public AtomicScopeModel {
public:



enum ID {
WorkGroup = 1,
Device = 2,
AllSVMDevices = 3,
SubGroup = 4,
Last = SubGroup
};

AtomicScopeOpenCLModel() {}

SyncScope map(unsigned S) const override {
switch (static_cast<ID>(S)) {
case WorkGroup:
return SyncScope::OpenCLWorkGroup;
case Device:
return SyncScope::OpenCLDevice;
case AllSVMDevices:
return SyncScope::OpenCLAllSVMDevices;
case SubGroup:
return SyncScope::OpenCLSubGroup;
}
llvm_unreachable("Invalid language synch scope value");
}

bool isValid(unsigned S) const override {
return S >= static_cast<unsigned>(WorkGroup) &&
S <= static_cast<unsigned>(Last);
}

ArrayRef<unsigned> getRuntimeValues() const override {
static_assert(Last == SubGroup, "Does not include all synch scopes");
static const unsigned Scopes[] = {
static_cast<unsigned>(WorkGroup), static_cast<unsigned>(Device),
static_cast<unsigned>(AllSVMDevices), static_cast<unsigned>(SubGroup)};
return llvm::makeArrayRef(Scopes);
}

unsigned getFallBackValue() const override {
return static_cast<unsigned>(AllSVMDevices);
}
};

inline std::unique_ptr<AtomicScopeModel>
AtomicScopeModel::create(AtomicScopeModelKind K) {
switch (K) {
case AtomicScopeModelKind::None:
return std::unique_ptr<AtomicScopeModel>{};
case AtomicScopeModelKind::OpenCL:
return std::make_unique<AtomicScopeOpenCLModel>();
}
llvm_unreachable("Invalid atomic scope model kind");
}
}

#endif
