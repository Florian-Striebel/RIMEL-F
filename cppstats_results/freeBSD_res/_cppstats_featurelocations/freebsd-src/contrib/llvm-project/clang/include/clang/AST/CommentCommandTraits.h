













#if !defined(LLVM_CLANG_AST_COMMENTCOMMANDTRAITS_H)
#define LLVM_CLANG_AST_COMMENTCOMMANDTRAITS_H

#include "clang/Basic/CommentOptions.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/ErrorHandling.h"

namespace clang {
namespace comments {





struct CommandInfo {
unsigned getID() const {
return ID;
}

const char *Name;


const char *EndCommandName;


enum { NumCommandIDBits = 20 };


unsigned ID : NumCommandIDBits;



unsigned NumArgs : 4;


unsigned IsInlineCommand : 1;


unsigned IsBlockCommand : 1;



unsigned IsBriefCommand : 1;


unsigned IsReturnsCommand : 1;



unsigned IsParamCommand : 1;



unsigned IsTParamCommand : 1;


unsigned IsThrowsCommand : 1;


unsigned IsDeprecatedCommand : 1;


unsigned IsHeaderfileCommand : 1;



unsigned IsEmptyParagraphAllowed : 1;





unsigned IsVerbatimBlockCommand : 1;


unsigned IsVerbatimBlockEndCommand : 1;





unsigned IsVerbatimLineCommand : 1;








unsigned IsDeclarationCommand : 1;


unsigned IsFunctionDeclarationCommand : 1;



unsigned IsRecordLikeDetailCommand : 1;


unsigned IsRecordLikeDeclarationCommand : 1;



unsigned IsUnknownCommand : 1;
};



class CommandTraits {
public:
enum KnownCommandIDs {
#define COMMENT_COMMAND(NAME) KCI_##NAME,
#include "clang/AST/CommentCommandList.inc"
#undef COMMENT_COMMAND
KCI_Last
};

CommandTraits(llvm::BumpPtrAllocator &Allocator,
const CommentOptions &CommentOptions);

void registerCommentOptions(const CommentOptions &CommentOptions);



const CommandInfo *getCommandInfoOrNULL(StringRef Name) const;

const CommandInfo *getCommandInfo(StringRef Name) const {
if (const CommandInfo *Info = getCommandInfoOrNULL(Name))
return Info;
llvm_unreachable("the command should be known");
}

const CommandInfo *getTypoCorrectCommandInfo(StringRef Typo) const;

const CommandInfo *getCommandInfo(unsigned CommandID) const;

const CommandInfo *registerUnknownCommand(StringRef CommandName);

const CommandInfo *registerBlockCommand(StringRef CommandName);



static const CommandInfo *getBuiltinCommandInfo(StringRef Name);



static const CommandInfo *getBuiltinCommandInfo(unsigned CommandID);

private:
CommandTraits(const CommandTraits &) = delete;
void operator=(const CommandTraits &) = delete;

const CommandInfo *getRegisteredCommandInfo(StringRef Name) const;
const CommandInfo *getRegisteredCommandInfo(unsigned CommandID) const;

CommandInfo *createCommandInfoWithName(StringRef CommandName);

unsigned NextID;


llvm::BumpPtrAllocator &Allocator;

SmallVector<CommandInfo *, 4> RegisteredCommands;
};

}
}

#endif

