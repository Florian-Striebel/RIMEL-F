
















#if !defined(LLVM_CLANG_STATICANALYZER_FRONTEND_CHECKERREGISTRY_H)
#define LLVM_CLANG_STATICANALYZER_FRONTEND_CHECKERREGISTRY_H

#include "clang/Basic/LLVM.h"
#include "clang/StaticAnalyzer/Core/CheckerRegistryData.h"
#include "llvm/ADT/StringRef.h"









































#if !defined(CLANG_ANALYZER_API_VERSION_STRING)




#include "clang/Basic/Version.h"
#define CLANG_ANALYZER_API_VERSION_STRING CLANG_VERSION_STRING
#endif

namespace clang {

class AnalyzerOptions;
class DiagnosticsEngine;

namespace ento {

class CheckerManager;







class CheckerRegistry {
public:
CheckerRegistry(CheckerRegistryData &Data, ArrayRef<std::string> Plugins,
DiagnosticsEngine &Diags, AnalyzerOptions &AnOpts,
ArrayRef<std::function<void(CheckerRegistry &)>>
CheckerRegistrationFns = {});




void initializeRegistry(const CheckerManager &Mgr);


private:




template <typename MGR, typename T> static void initializeManager(MGR &mgr) {
mgr.template registerChecker<T>();
}

template <typename T> static bool returnTrue(const CheckerManager &mgr) {
return true;
}

public:


void addChecker(RegisterCheckerFn Fn, ShouldRegisterFunction sfn,
StringRef FullName, StringRef Desc, StringRef DocsUri,
bool IsHidden);






template <class T>
void addChecker(StringRef FullName, StringRef Desc, StringRef DocsUri,
bool IsHidden = false) {


addChecker(&CheckerRegistry::initializeManager<CheckerManager, T>,
&CheckerRegistry::returnTrue<T>, FullName, Desc, DocsUri,
IsHidden);
}



void addDependency(StringRef FullName, StringRef Dependency);



void addWeakDependency(StringRef FullName, StringRef Dependency);










void addCheckerOption(StringRef OptionType, StringRef CheckerFullName,
StringRef OptionName, StringRef DefaultValStr,
StringRef Description, StringRef DevelopmentStatus,
bool IsHidden = false);


void addPackage(StringRef FullName);










void addPackageOption(StringRef OptionType, StringRef PackageFullName,
StringRef OptionName, StringRef DefaultValStr,
StringRef Description, StringRef DevelopmentStatus,
bool IsHidden = false);






void initializeManager(CheckerManager &CheckerMgr) const;


void validateCheckerOptions() const;

private:
template <bool IsWeak> void resolveDependencies();
void resolveCheckerAndPackageOptions();

CheckerRegistryData &Data;

DiagnosticsEngine &Diags;
AnalyzerOptions &AnOpts;
};

}
}

#endif
