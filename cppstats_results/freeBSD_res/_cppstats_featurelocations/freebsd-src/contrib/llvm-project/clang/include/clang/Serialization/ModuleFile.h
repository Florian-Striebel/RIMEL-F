












#if !defined(LLVM_CLANG_SERIALIZATION_MODULEFILE_H)
#define LLVM_CLANG_SERIALIZATION_MODULEFILE_H

#include "clang/Basic/FileManager.h"
#include "clang/Basic/Module.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Serialization/ASTBitCodes.h"
#include "clang/Serialization/ContinuousRangeMap.h"
#include "clang/Serialization/ModuleFileExtension.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Bitstream/BitstreamReader.h"
#include "llvm/Support/Endian.h"
#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace clang {

namespace serialization {


enum ModuleKind {

MK_ImplicitModule,


MK_ExplicitModule,


MK_PCH,


MK_Preamble,


MK_MainFile,


MK_PrebuiltModule
};




class InputFile {
enum {
Overridden = 1,
OutOfDate = 2,
NotFound = 3
};
llvm::PointerIntPair<const FileEntryRef::MapEntry *, 2, unsigned> Val;

public:
InputFile() = default;

InputFile(FileEntryRef File, bool isOverridden = false,
bool isOutOfDate = false) {
assert(!(isOverridden && isOutOfDate) &&
"an overridden cannot be out-of-date");
unsigned intVal = 0;
if (isOverridden)
intVal = Overridden;
else if (isOutOfDate)
intVal = OutOfDate;
Val.setPointerAndInt(&File.getMapEntry(), intVal);
}

static InputFile getNotFound() {
InputFile File;
File.Val.setInt(NotFound);
return File;
}

OptionalFileEntryRefDegradesToFileEntryPtr getFile() const {
if (auto *P = Val.getPointer())
return FileEntryRef(*P);
return None;
}
bool isOverridden() const { return Val.getInt() == Overridden; }
bool isOutOfDate() const { return Val.getInt() == OutOfDate; }
bool isNotFound() const { return Val.getInt() == NotFound; }
};








class ModuleFile {
public:
ModuleFile(ModuleKind Kind, unsigned Generation)
: Kind(Kind), Generation(Generation) {}
~ModuleFile();




unsigned Index = 0;


ModuleKind Kind;


std::string FileName;


std::string ModuleName;


std::string BaseDirectory;

std::string getTimestampFilename() const {
return FileName + ".timestamp";
}




std::string OriginalSourceFileName;



std::string ActualOriginalSourceFileName;



FileID OriginalSourceFileID;



std::string OriginalDir;

std::string ModuleMapPath;


bool RelocatablePCH = false;


bool HasTimestamps = false;


bool DidReadTopLevelSubmodule = false;


OptionalFileEntryRefDegradesToFileEntryPtr File;



ASTFileSignature Signature;



ASTFileSignature ASTBlockHash;



bool DirectlyImported = false;


unsigned Generation;



llvm::MemoryBuffer *Buffer;


uint64_t SizeInBits = 0;


uint64_t GlobalBitOffset = 0;


uint64_t ASTBlockStartOffset = 0;


StringRef Data;


llvm::BitstreamCursor Stream;










SourceLocation DirectImportLoc;


SourceLocation ImportLoc;


SourceLocation FirstLoc;



std::vector<std::unique_ptr<ModuleFileExtensionReader>> ExtensionReaders;



StringRef ModuleOffsetMap;




llvm::BitstreamCursor InputFilesCursor;


const llvm::support::unaligned_uint64_t *InputFileOffsets = nullptr;


std::vector<InputFile> InputFilesLoaded;



unsigned NumUserInputFiles = 0;





uint64_t InputFilesValidationTimestamp = 0;




llvm::BitstreamCursor SLocEntryCursor;


uint64_t SourceManagerBlockStartOffset = 0;


unsigned LocalNumSLocEntries = 0;


int SLocEntryBaseID = 0;


SourceLocation::UIntTy SLocEntryBaseOffset = 0;



uint64_t SLocEntryOffsetsBase = 0;



const uint32_t *SLocEntryOffsets = nullptr;


SmallVector<uint64_t, 4> PreloadSLocEntries;


ContinuousRangeMap<SourceLocation::UIntTy, SourceLocation::IntTy, 2>
SLocRemap;




unsigned LocalNumIdentifiers = 0;






const uint32_t *IdentifierOffsets = nullptr;


serialization::IdentID BaseIdentifierID = 0;


ContinuousRangeMap<uint32_t, int, 2> IdentifierRemap;





const unsigned char *IdentifierTableData = nullptr;



void *IdentifierLookupTable = nullptr;



std::vector<unsigned> PreloadIdentifierOffsets;





llvm::BitstreamCursor MacroCursor;


unsigned LocalNumMacros = 0;



uint64_t MacroOffsetsBase = 0;






const uint32_t *MacroOffsets = nullptr;


serialization::MacroID BaseMacroID = 0;


ContinuousRangeMap<uint32_t, int, 2> MacroRemap;


uint64_t MacroStartOffset = 0;





llvm::BitstreamCursor PreprocessorDetailCursor;


uint64_t PreprocessorDetailStartOffset = 0;



serialization::PreprocessedEntityID BasePreprocessedEntityID = 0;


ContinuousRangeMap<uint32_t, int, 2> PreprocessedEntityRemap;

const PPEntityOffset *PreprocessedEntityOffsets = nullptr;
unsigned NumPreprocessedEntities = 0;


unsigned BasePreprocessedSkippedRangeID = 0;

const PPSkippedRange *PreprocessedSkippedRangeOffsets = nullptr;
unsigned NumPreprocessedSkippedRanges = 0;




unsigned LocalNumHeaderFileInfos = 0;






const char *HeaderFileInfoTableData = nullptr;



void *HeaderFileInfoTable = nullptr;




unsigned LocalNumSubmodules = 0;


serialization::SubmoduleID BaseSubmoduleID = 0;


ContinuousRangeMap<uint32_t, int, 2> SubmoduleRemap;






unsigned LocalNumSelectors = 0;



const uint32_t *SelectorOffsets = nullptr;


serialization::SelectorID BaseSelectorID = 0;


ContinuousRangeMap<uint32_t, int, 2> SelectorRemap;




const unsigned char *SelectorLookupTableData = nullptr;






void *SelectorLookupTable = nullptr;






llvm::BitstreamCursor DeclsCursor;


uint64_t DeclsBlockStartOffset = 0;


unsigned LocalNumDecls = 0;



const DeclOffset *DeclOffsets = nullptr;


serialization::DeclID BaseDeclID = 0;


ContinuousRangeMap<uint32_t, int, 2> DeclRemap;








llvm::DenseMap<ModuleFile *, serialization::DeclID> GlobalToLocalDeclIDs;


const serialization::DeclID *FileSortedDecls = nullptr;
unsigned NumFileSortedDecls = 0;



const serialization::ObjCCategoriesInfo *ObjCCategoriesMap = nullptr;


unsigned LocalNumObjCCategoriesInMap = 0;



SmallVector<uint64_t, 1> ObjCCategories;




unsigned LocalNumTypes = 0;



const UnderalignedInt64 *TypeOffsets = nullptr;



serialization::TypeID BaseTypeIndex = 0;


ContinuousRangeMap<uint32_t, int, 2> TypeRemap;




SmallVector<uint64_t, 8> PragmaDiagMappings;


llvm::SetVector<ModuleFile *> ImportedBy;


llvm::SetVector<ModuleFile *> Imports;



bool isDirectlyImported() const { return DirectlyImported; }


bool isModule() const {
return Kind == MK_ImplicitModule || Kind == MK_ExplicitModule ||
Kind == MK_PrebuiltModule;
}


void dump();
};

}

}

#endif
