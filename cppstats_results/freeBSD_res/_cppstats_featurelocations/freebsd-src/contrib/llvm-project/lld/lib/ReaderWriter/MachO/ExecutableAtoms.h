







#if !defined(LLD_READER_WRITER_MACHO_EXECUTABLE_ATOMS_H)
#define LLD_READER_WRITER_MACHO_EXECUTABLE_ATOMS_H

#include "Atoms.h"
#include "File.h"

#include "llvm/BinaryFormat/MachO.h"

#include "lld/Core/DefinedAtom.h"
#include "lld/Core/File.h"
#include "lld/Core/LinkingContext.h"
#include "lld/Core/Reference.h"
#include "lld/Core/Simple.h"
#include "lld/Core/UndefinedAtom.h"
#include "lld/ReaderWriter/MachOLinkingContext.h"

namespace lld {
namespace mach_o {






class CEntryFile : public SimpleFile {
public:
CEntryFile(const MachOLinkingContext &context)
: SimpleFile("C entry", kindCEntryObject),
_undefMain(*this, context.entrySymbolName()) {
this->addAtom(_undefMain);
}

private:
SimpleUndefinedAtom _undefMain;
};






class StubHelperFile : public SimpleFile {
public:
StubHelperFile(const MachOLinkingContext &context)
: SimpleFile("stub runtime", kindStubHelperObject),
_undefBinder(*this, context.binderSymbolName()) {
this->addAtom(_undefBinder);
}

private:
SimpleUndefinedAtom _undefBinder;
};






class MachHeaderAliasFile : public SimpleFile {
public:
MachHeaderAliasFile(const MachOLinkingContext &context)
: SimpleFile("mach_header symbols", kindHeaderObject) {
StringRef machHeaderSymbolName;
DefinedAtom::Scope symbolScope = DefinedAtom::scopeLinkageUnit;
StringRef dsoHandleName;
switch (context.outputMachOType()) {
case llvm::MachO::MH_OBJECT:
machHeaderSymbolName = "__mh_object_header";
break;
case llvm::MachO::MH_EXECUTE:
machHeaderSymbolName = "__mh_execute_header";
symbolScope = DefinedAtom::scopeGlobal;
dsoHandleName = "___dso_handle";
break;
case llvm::MachO::MH_FVMLIB:
llvm_unreachable("no mach_header symbol for file type");
case llvm::MachO::MH_CORE:
llvm_unreachable("no mach_header symbol for file type");
case llvm::MachO::MH_PRELOAD:
llvm_unreachable("no mach_header symbol for file type");
case llvm::MachO::MH_DYLIB:
machHeaderSymbolName = "__mh_dylib_header";
dsoHandleName = "___dso_handle";
break;
case llvm::MachO::MH_DYLINKER:
machHeaderSymbolName = "__mh_dylinker_header";
dsoHandleName = "___dso_handle";
break;
case llvm::MachO::MH_BUNDLE:
machHeaderSymbolName = "__mh_bundle_header";
dsoHandleName = "___dso_handle";
break;
case llvm::MachO::MH_DYLIB_STUB:
llvm_unreachable("no mach_header symbol for file type");
case llvm::MachO::MH_DSYM:
llvm_unreachable("no mach_header symbol for file type");
case llvm::MachO::MH_KEXT_BUNDLE:
dsoHandleName = "___dso_handle";
break;
}
if (!machHeaderSymbolName.empty())
_definedAtoms.push_back(new (allocator()) MachODefinedAtom(
*this, machHeaderSymbolName, symbolScope,
DefinedAtom::typeMachHeader, DefinedAtom::mergeNo, false,
true ,
ArrayRef<uint8_t>(), DefinedAtom::Alignment(4096)));

if (!dsoHandleName.empty())
_definedAtoms.push_back(new (allocator()) MachODefinedAtom(
*this, dsoHandleName, DefinedAtom::scopeLinkageUnit,
DefinedAtom::typeDSOHandle, DefinedAtom::mergeNo, false,
true ,
ArrayRef<uint8_t>(), DefinedAtom::Alignment(1)));
}

const AtomRange<DefinedAtom> defined() const override {
return _definedAtoms;
}
const AtomRange<UndefinedAtom> undefined() const override {
return _noUndefinedAtoms;
}

const AtomRange<SharedLibraryAtom> sharedLibrary() const override {
return _noSharedLibraryAtoms;
}

const AtomRange<AbsoluteAtom> absolute() const override {
return _noAbsoluteAtoms;
}

void clearAtoms() override {
_definedAtoms.clear();
_noUndefinedAtoms.clear();
_noSharedLibraryAtoms.clear();
_noAbsoluteAtoms.clear();
}


private:
mutable AtomVector<DefinedAtom> _definedAtoms;
};

}
}

#endif
