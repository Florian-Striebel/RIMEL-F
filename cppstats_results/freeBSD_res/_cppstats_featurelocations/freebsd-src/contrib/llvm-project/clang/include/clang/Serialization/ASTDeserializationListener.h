












#if !defined(LLVM_CLANG_SERIALIZATION_ASTDESERIALIZATIONLISTENER_H)
#define LLVM_CLANG_SERIALIZATION_ASTDESERIALIZATIONLISTENER_H

#include "clang/Basic/IdentifierTable.h"
#include "clang/Serialization/ASTBitCodes.h"

namespace clang {

class Decl;
class ASTReader;
class QualType;
class MacroDefinitionRecord;
class MacroInfo;
class Module;
class SourceLocation;

class ASTDeserializationListener {
public:
virtual ~ASTDeserializationListener();


virtual void ReaderInitialized(ASTReader *Reader) { }


virtual void IdentifierRead(serialization::IdentID ID,
IdentifierInfo *II) { }

virtual void MacroRead(serialization::MacroID ID, MacroInfo *MI) { }



virtual void TypeRead(serialization::TypeIdx Idx, QualType T) { }

virtual void DeclRead(serialization::DeclID ID, const Decl *D) { }

virtual void SelectorRead(serialization::SelectorID iD, Selector Sel) {}

virtual void MacroDefinitionRead(serialization::PreprocessedEntityID,
MacroDefinitionRecord *MD) {}

virtual void ModuleRead(serialization::SubmoduleID ID, Module *Mod) {}

virtual void ModuleImportRead(serialization::SubmoduleID ID,
SourceLocation ImportLoc) {}
};
}

#endif
