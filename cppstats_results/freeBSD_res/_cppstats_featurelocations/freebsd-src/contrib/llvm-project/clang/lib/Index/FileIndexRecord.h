







#if !defined(LLVM_CLANG_LIB_INDEX_FILEINDEXRECORD_H)
#define LLVM_CLANG_LIB_INDEX_FILEINDEXRECORD_H

#include "clang/Basic/SourceLocation.h"
#include "clang/Index/DeclOccurrence.h"
#include "clang/Index/IndexSymbol.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include <vector>

namespace clang {
class IdentifierInfo;

namespace index {



class FileIndexRecord {
private:
FileID FID;
bool IsSystem;
mutable bool IsSorted = false;
mutable std::vector<DeclOccurrence> Decls;

public:
FileIndexRecord(FileID FID, bool IsSystem) : FID(FID), IsSystem(IsSystem) {}

ArrayRef<DeclOccurrence> getDeclOccurrencesSortedByOffset() const;

FileID getFileID() const { return FID; }
bool isSystem() const { return IsSystem; }








void addDeclOccurence(SymbolRoleSet Roles, unsigned Offset, const Decl *D,
ArrayRef<SymbolRelation> Relations);







void addMacroOccurence(SymbolRoleSet Roles, unsigned Offset,
const IdentifierInfo *Name, const MacroInfo *MI);



void removeHeaderGuardMacros();

void print(llvm::raw_ostream &OS, SourceManager &SM) const;
};

}
}

#endif
