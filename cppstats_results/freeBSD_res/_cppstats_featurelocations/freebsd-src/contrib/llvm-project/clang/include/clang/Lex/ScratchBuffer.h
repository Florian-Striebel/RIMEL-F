











#if !defined(LLVM_CLANG_LEX_SCRATCHBUFFER_H)
#define LLVM_CLANG_LEX_SCRATCHBUFFER_H

#include "clang/Basic/SourceLocation.h"

namespace clang {
class SourceManager;




class ScratchBuffer {
SourceManager &SourceMgr;
char *CurBuffer;
SourceLocation BufferStartLoc;
unsigned BytesUsed;
public:
ScratchBuffer(SourceManager &SM);





SourceLocation getToken(const char *Buf, unsigned Len, const char *&DestPtr);

private:
void AllocScratchBuffer(unsigned RequestLen);
};

}

#endif
