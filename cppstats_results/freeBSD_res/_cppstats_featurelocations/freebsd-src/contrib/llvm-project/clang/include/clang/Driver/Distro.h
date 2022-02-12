







#if !defined(LLVM_CLANG_DRIVER_DISTRO_H)
#define LLVM_CLANG_DRIVER_DISTRO_H

#include "llvm/ADT/Triple.h"
#include "llvm/Support/VirtualFileSystem.h"

namespace clang {
namespace driver {






class Distro {
public:
enum DistroType {

UninitializedDistro,



AlpineLinux,
ArchLinux,
DebianLenny,
DebianSqueeze,
DebianWheezy,
DebianJessie,
DebianStretch,
DebianBuster,
DebianBullseye,
Exherbo,
RHEL5,
RHEL6,
RHEL7,
Fedora,
Gentoo,
OpenSUSE,
UbuntuHardy,
UbuntuIntrepid,
UbuntuJaunty,
UbuntuKarmic,
UbuntuLucid,
UbuntuMaverick,
UbuntuNatty,
UbuntuOneiric,
UbuntuPrecise,
UbuntuQuantal,
UbuntuRaring,
UbuntuSaucy,
UbuntuTrusty,
UbuntuUtopic,
UbuntuVivid,
UbuntuWily,
UbuntuXenial,
UbuntuYakkety,
UbuntuZesty,
UbuntuArtful,
UbuntuBionic,
UbuntuCosmic,
UbuntuDisco,
UbuntuEoan,
UbuntuFocal,
UbuntuGroovy,
UbuntuHirsute,
UbuntuImpish,
UnknownDistro
};

private:

DistroType DistroVal;

public:




Distro() : DistroVal() {}


Distro(DistroType D) : DistroVal(D) {}


explicit Distro(llvm::vfs::FileSystem &VFS, const llvm::Triple &TargetOrHost);

bool operator==(const Distro &Other) const {
return DistroVal == Other.DistroVal;
}

bool operator!=(const Distro &Other) const {
return DistroVal != Other.DistroVal;
}

bool operator>=(const Distro &Other) const {
return DistroVal >= Other.DistroVal;
}

bool operator<=(const Distro &Other) const {
return DistroVal <= Other.DistroVal;
}





bool IsRedhat() const {
return DistroVal == Fedora || (DistroVal >= RHEL5 && DistroVal <= RHEL7);
}

bool IsOpenSUSE() const { return DistroVal == OpenSUSE; }

bool IsDebian() const {
return DistroVal >= DebianLenny && DistroVal <= DebianBullseye;
}

bool IsUbuntu() const {
return DistroVal >= UbuntuHardy && DistroVal <= UbuntuImpish;
}

bool IsAlpineLinux() const { return DistroVal == AlpineLinux; }

bool IsGentoo() const { return DistroVal == Gentoo; }


};

}
}

#endif
