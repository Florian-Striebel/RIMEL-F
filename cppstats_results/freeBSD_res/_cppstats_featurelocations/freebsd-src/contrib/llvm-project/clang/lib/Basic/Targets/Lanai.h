











#if !defined(LLVM_CLANG_LIB_BASIC_TARGETS_LANAI_H)
#define LLVM_CLANG_LIB_BASIC_TARGETS_LANAI_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY LanaiTargetInfo : public TargetInfo {


enum CPUKind {
CK_NONE,
CK_V11,
} CPU;

static const TargetInfo::GCCRegAlias GCCRegAliases[];
static const char *const GCCRegNames[];

public:
LanaiTargetInfo(const llvm::Triple &Triple, const TargetOptions &)
: TargetInfo(Triple) {

resetDataLayout("E"
"-m:e"
"-p:32:32"
"-i64:64"
"-a:0:32"
"-n32"
"-S64"
);



RegParmMax = 4;


CPU = CK_V11;





MinGlobalAlign = 32;
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;

bool isValidCPUName(StringRef Name) const override;

void fillValidCPUList(SmallVectorImpl<StringRef> &Values) const override;

bool setCPU(const std::string &Name) override;

bool hasFeature(StringRef Feature) const override;

ArrayRef<const char *> getGCCRegNames() const override;

ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override;

BuiltinVaListKind getBuiltinVaListKind() const override {
return TargetInfo::VoidPtrBuiltinVaList;
}

ArrayRef<Builtin::Info> getTargetBuiltins() const override { return None; }

bool validateAsmConstraint(const char *&Name,
TargetInfo::ConstraintInfo &info) const override {
return false;
}

const char *getClobbers() const override { return ""; }

bool hasExtIntType() const override { return true; }
};
}
}

#endif
