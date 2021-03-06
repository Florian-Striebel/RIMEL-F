











#if !defined(LLVM_CLANG_LEX_DIRECTORYLOOKUP_H)
#define LLVM_CLANG_LEX_DIRECTORYLOOKUP_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/ModuleMap.h"

namespace clang {
class HeaderMap;
class HeaderSearch;
class Module;





class DirectoryLookup {
public:
enum LookupType_t {
LT_NormalDir,
LT_Framework,
LT_HeaderMap
};
private:
union DLU {


DirectoryEntryRef Dir;



const HeaderMap *Map;

DLU(DirectoryEntryRef Dir) : Dir(Dir) {}
DLU(const HeaderMap *Map) : Map(Map) {}
} u;



unsigned DirCharacteristic : 2;



unsigned LookupType : 2;


unsigned IsIndexHeaderMap : 1;



unsigned SearchedAllModuleMaps : 1;

public:

DirectoryLookup(DirectoryEntryRef Dir, SrcMgr::CharacteristicKind DT,
bool isFramework)
: u(Dir), DirCharacteristic(DT),
LookupType(isFramework ? LT_Framework : LT_NormalDir),
IsIndexHeaderMap(false), SearchedAllModuleMaps(false) {}


DirectoryLookup(const HeaderMap *Map, SrcMgr::CharacteristicKind DT,
bool isIndexHeaderMap)
: u(Map), DirCharacteristic(DT), LookupType(LT_HeaderMap),
IsIndexHeaderMap(isIndexHeaderMap), SearchedAllModuleMaps(false) {}



LookupType_t getLookupType() const { return (LookupType_t)LookupType; }



StringRef getName() const;



const DirectoryEntry *getDir() const {
return isNormalDir() ? &u.Dir.getDirEntry() : nullptr;
}



const DirectoryEntry *getFrameworkDir() const {
return isFramework() ? &u.Dir.getDirEntry() : nullptr;
}

Optional<DirectoryEntryRef> getFrameworkDirRef() const {
return isFramework() ? Optional<DirectoryEntryRef>(u.Dir) : None;
}



const HeaderMap *getHeaderMap() const {
return isHeaderMap() ? u.Map : nullptr;
}


bool isNormalDir() const { return getLookupType() == LT_NormalDir; }



bool isFramework() const { return getLookupType() == LT_Framework; }


bool isHeaderMap() const { return getLookupType() == LT_HeaderMap; }



bool haveSearchedAllModuleMaps() const { return SearchedAllModuleMaps; }



void setSearchedAllModuleMaps(bool SAMM) {
SearchedAllModuleMaps = SAMM;
}



SrcMgr::CharacteristicKind getDirCharacteristic() const {
return (SrcMgr::CharacteristicKind)DirCharacteristic;
}


bool isSystemHeaderDirectory() const {
return getDirCharacteristic() != SrcMgr::C_User;
}


bool isIndexHeaderMap() const {
return isHeaderMap() && IsIndexHeaderMap;
}


































Optional<FileEntryRef>
LookupFile(StringRef &Filename, HeaderSearch &HS, SourceLocation IncludeLoc,
SmallVectorImpl<char> *SearchPath,
SmallVectorImpl<char> *RelativePath, Module *RequestingModule,
ModuleMap::KnownHeader *SuggestedModule,
bool &InUserSpecifiedSystemFramework, bool &IsFrameworkFound,
bool &IsInHeaderMap, SmallVectorImpl<char> &MappedName) const;

private:
Optional<FileEntryRef> DoFrameworkLookup(
StringRef Filename, HeaderSearch &HS, SmallVectorImpl<char> *SearchPath,
SmallVectorImpl<char> *RelativePath, Module *RequestingModule,
ModuleMap::KnownHeader *SuggestedModule,
bool &InUserSpecifiedSystemFramework, bool &IsFrameworkFound) const;
};

}

#endif
