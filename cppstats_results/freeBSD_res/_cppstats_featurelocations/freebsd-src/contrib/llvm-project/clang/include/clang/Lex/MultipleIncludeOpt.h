












#if !defined(LLVM_CLANG_LEX_MULTIPLEINCLUDEOPT_H)
#define LLVM_CLANG_LEX_MULTIPLEINCLUDEOPT_H

#include "clang/Basic/SourceLocation.h"

namespace clang {
class IdentifierInfo;







class MultipleIncludeOpt {





bool ReadAnyTokens;




bool ImmediatelyAfterTopLevelIfndef;






bool DidMacroExpansion;



const IdentifierInfo *TheMacro;


const IdentifierInfo *DefinedMacro;

SourceLocation MacroLoc;
SourceLocation DefinedLoc;
public:
MultipleIncludeOpt() {
ReadAnyTokens = false;
ImmediatelyAfterTopLevelIfndef = false;
DidMacroExpansion = false;
TheMacro = nullptr;
DefinedMacro = nullptr;
}

SourceLocation GetMacroLocation() const {
return MacroLoc;
}

SourceLocation GetDefinedLocation() const {
return DefinedLoc;
}

void resetImmediatelyAfterTopLevelIfndef() {
ImmediatelyAfterTopLevelIfndef = false;
}

void SetDefinedMacro(IdentifierInfo *M, SourceLocation Loc) {
DefinedMacro = M;
DefinedLoc = Loc;
}



void Invalidate() {


ReadAnyTokens = true;
ImmediatelyAfterTopLevelIfndef = false;
DefinedMacro = nullptr;
TheMacro = nullptr;
}




bool getHasReadAnyTokensVal() const { return ReadAnyTokens; }



bool getImmediatelyAfterTopLevelIfndef() const {
return ImmediatelyAfterTopLevelIfndef;
}


void ReadToken() {
ReadAnyTokens = true;
ImmediatelyAfterTopLevelIfndef = false;
}



void ExpandedMacro() { DidMacroExpansion = true; }








void EnterTopLevelIfndef(const IdentifierInfo *M, SourceLocation Loc) {

if (TheMacro)
return Invalidate();





if (DidMacroExpansion)
return Invalidate();


ReadAnyTokens = true;
ImmediatelyAfterTopLevelIfndef = true;
TheMacro = M;
MacroLoc = Loc;
}


void EnterTopLevelConditional() {


Invalidate();
}


void ExitTopLevelConditional() {



if (!TheMacro) return Invalidate();



ReadAnyTokens = false;
ImmediatelyAfterTopLevelIfndef = false;
}



const IdentifierInfo *GetControllingMacroAtEndOfFile() const {


if (!ReadAnyTokens)
return TheMacro;
return nullptr;
}



const IdentifierInfo *GetDefinedMacro() const {
return DefinedMacro;
}
};

}

#endif
