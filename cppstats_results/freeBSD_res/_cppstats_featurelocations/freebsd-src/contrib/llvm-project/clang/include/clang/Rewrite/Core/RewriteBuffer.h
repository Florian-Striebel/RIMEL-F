







#if !defined(LLVM_CLANG_REWRITE_CORE_REWRITEBUFFER_H)
#define LLVM_CLANG_REWRITE_CORE_REWRITEBUFFER_H

#include "clang/Basic/LLVM.h"
#include "clang/Rewrite/Core/DeltaTree.h"
#include "clang/Rewrite/Core/RewriteRope.h"
#include "llvm/ADT/StringRef.h"

namespace clang {







class RewriteBuffer {
friend class Rewriter;



DeltaTree Deltas;

RewriteRope Buffer;

public:
using iterator = RewriteRope::const_iterator;

iterator begin() const { return Buffer.begin(); }
iterator end() const { return Buffer.end(); }
unsigned size() const { return Buffer.size(); }



void Initialize(const char *BufStart, const char *BufEnd) {
Buffer.assign(BufStart, BufEnd);
}
void Initialize(StringRef Input) {
Initialize(Input.begin(), Input.end());
}








raw_ostream &write(raw_ostream &Stream) const;


void RemoveText(unsigned OrigOffset, unsigned Size,
bool removeLineIfEmpty = false);




void InsertText(unsigned OrigOffset, StringRef Str,
bool InsertAfter = true);






void InsertTextBefore(unsigned OrigOffset, StringRef Str) {
InsertText(OrigOffset, Str, false);
}




void InsertTextAfter(unsigned OrigOffset, StringRef Str) {
InsertText(OrigOffset, Str);
}




void ReplaceText(unsigned OrigOffset, unsigned OrigLength,
StringRef NewStr);

private:





unsigned getMappedOffset(unsigned OrigOffset,
bool AfterInserts = false) const{
return Deltas.getDeltaAt(2*OrigOffset+AfterInserts)+OrigOffset;
}



void AddInsertDelta(unsigned OrigOffset, int Change) {
return Deltas.AddDelta(2*OrigOffset, Change);
}



void AddReplaceDelta(unsigned OrigOffset, int Change) {
return Deltas.AddDelta(2*OrigOffset+1, Change);
}
};

}

#endif
