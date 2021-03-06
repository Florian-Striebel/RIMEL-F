











#if !defined(LLVM_CLANG_AST_AVAILABILITY_H)
#define LLVM_CLANG_AST_AVAILABILITY_H

#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/VersionTuple.h"

namespace clang {









class AvailabilitySpec {



VersionTuple Version;


StringRef Platform;

SourceLocation BeginLoc, EndLoc;

public:
AvailabilitySpec(VersionTuple Version, StringRef Platform,
SourceLocation BeginLoc, SourceLocation EndLoc)
: Version(Version), Platform(Platform), BeginLoc(BeginLoc),
EndLoc(EndLoc) {}


AvailabilitySpec(SourceLocation StarLoc)
: BeginLoc(StarLoc), EndLoc(StarLoc) {}

VersionTuple getVersion() const { return Version; }
StringRef getPlatform() const { return Platform; }
SourceLocation getBeginLoc() const { return BeginLoc; }
SourceLocation getEndLoc() const { return EndLoc; }


bool isOtherPlatformSpec() const { return Version.empty(); }
};

}

#endif
