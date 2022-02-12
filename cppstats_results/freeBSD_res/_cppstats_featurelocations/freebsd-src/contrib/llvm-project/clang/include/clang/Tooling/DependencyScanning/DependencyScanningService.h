







#if !defined(LLVM_CLANG_TOOLING_DEPENDENCY_SCANNING_SERVICE_H)
#define LLVM_CLANG_TOOLING_DEPENDENCY_SCANNING_SERVICE_H

#include "clang/Tooling/DependencyScanning/DependencyScanningFilesystem.h"

namespace clang {
namespace tooling {
namespace dependencies {



enum class ScanningMode {



CanonicalPreprocessing,





MinimizedSourcePreprocessing
};


enum class ScanningOutputFormat {



Make,



Full,
};



class DependencyScanningService {
public:
DependencyScanningService(ScanningMode Mode, ScanningOutputFormat Format,
bool ReuseFileManager = true,
bool SkipExcludedPPRanges = true);

ScanningMode getMode() const { return Mode; }

ScanningOutputFormat getFormat() const { return Format; }

bool canReuseFileManager() const { return ReuseFileManager; }

bool canSkipExcludedPPRanges() const { return SkipExcludedPPRanges; }

DependencyScanningFilesystemSharedCache &getSharedCache() {
return SharedCache;
}

private:
const ScanningMode Mode;
const ScanningOutputFormat Format;
const bool ReuseFileManager;



const bool SkipExcludedPPRanges;

DependencyScanningFilesystemSharedCache SharedCache;
};

}
}
}

#endif
