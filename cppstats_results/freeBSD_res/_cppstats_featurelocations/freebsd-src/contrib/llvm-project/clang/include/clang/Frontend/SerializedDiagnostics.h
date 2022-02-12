







#if !defined(LLVM_CLANG_FRONTEND_SERIALIZE_DIAGNOSTICS_H_)
#define LLVM_CLANG_FRONTEND_SERIALIZE_DIAGNOSTICS_H_

#include "llvm/Bitstream/BitCodes.h"

namespace clang {
namespace serialized_diags {

enum BlockIDs {


BLOCK_META = llvm::bitc::FIRST_APPLICATION_BLOCKID,



BLOCK_DIAG
};

enum RecordIDs {
RECORD_VERSION = 1,
RECORD_DIAG,
RECORD_SOURCE_RANGE,
RECORD_DIAG_FLAG,
RECORD_CATEGORY,
RECORD_FILENAME,
RECORD_FIXIT,
RECORD_FIRST = RECORD_VERSION,
RECORD_LAST = RECORD_FIXIT
};





enum Level {
Ignored = 0,
Note,
Warning,
Error,
Fatal,
Remark
};


enum { VersionNumber = 2 };

}
}

#endif
