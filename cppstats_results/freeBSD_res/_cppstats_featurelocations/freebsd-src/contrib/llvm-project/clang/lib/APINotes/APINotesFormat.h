







#if !defined(LLVM_CLANG_LIB_APINOTES_APINOTESFORMAT_H)
#define LLVM_CLANG_LIB_APINOTES_APINOTESFORMAT_H

#include "llvm/ADT/PointerEmbeddedInt.h"
#include "llvm/Bitcode/BitcodeConvenience.h"

namespace clang {
namespace api_notes {

const unsigned char API_NOTES_SIGNATURE[] = {0xE2, 0x9C, 0xA8, 0x01};


const uint16_t VERSION_MAJOR = 0;




const uint16_t VERSION_MINOR = 24;

using IdentifierID = llvm::PointerEmbeddedInt<unsigned, 31>;
using IdentifierIDField = llvm::BCVBR<16>;

using SelectorID = llvm::PointerEmbeddedInt<unsigned, 31>;
using SelectorIDField = llvm::BCVBR<16>;





enum BlockID {




CONTROL_BLOCK_ID = llvm::bitc::FIRST_APPLICATION_BLOCKID,


IDENTIFIER_BLOCK_ID,



OBJC_CONTEXT_BLOCK_ID,




OBJC_PROPERTY_BLOCK_ID,




OBJC_METHOD_BLOCK_ID,




OBJC_SELECTOR_BLOCK_ID,



GLOBAL_VARIABLE_BLOCK_ID,



GLOBAL_FUNCTION_BLOCK_ID,



TAG_BLOCK_ID,



TYPEDEF_BLOCK_ID,



ENUM_CONSTANT_BLOCK_ID,
};

namespace control_block {


enum {
METADATA = 1,
MODULE_NAME = 2,
MODULE_OPTIONS = 3,
SOURCE_FILE = 4,
};

using MetadataLayout =
llvm::BCRecordLayout<METADATA,
llvm::BCFixed<16>,
llvm::BCFixed<16>
>;

using ModuleNameLayout = llvm::BCRecordLayout<MODULE_NAME,
llvm::BCBlob
>;

using ModuleOptionsLayout =
llvm::BCRecordLayout<MODULE_OPTIONS,
llvm::BCFixed<1>
>;

using SourceFileLayout = llvm::BCRecordLayout<SOURCE_FILE,
llvm::BCVBR<16>,
llvm::BCVBR<16>
>;
}

namespace identifier_block {
enum {
IDENTIFIER_DATA = 1,
};

using IdentifierDataLayout = llvm::BCRecordLayout<
IDENTIFIER_DATA,
llvm::BCVBR<16>,
llvm::BCBlob
>;
}

namespace objc_context_block {
enum {
OBJC_CONTEXT_ID_DATA = 1,
OBJC_CONTEXT_INFO_DATA = 2,
};

using ObjCContextIDLayout =
llvm::BCRecordLayout<OBJC_CONTEXT_ID_DATA,
llvm::BCVBR<16>,

llvm::BCBlob

>;

using ObjCContextInfoLayout = llvm::BCRecordLayout<
OBJC_CONTEXT_INFO_DATA,
llvm::BCVBR<16>,
llvm::BCBlob
>;
}

namespace objc_property_block {
enum {
OBJC_PROPERTY_DATA = 1,
};

using ObjCPropertyDataLayout = llvm::BCRecordLayout<
OBJC_PROPERTY_DATA,
llvm::BCVBR<16>,
llvm::BCBlob

>;
}

namespace objc_method_block {
enum {
OBJC_METHOD_DATA = 1,
};

using ObjCMethodDataLayout =
llvm::BCRecordLayout<OBJC_METHOD_DATA,
llvm::BCVBR<16>,

llvm::BCBlob


>;
}

namespace objc_selector_block {
enum {
OBJC_SELECTOR_DATA = 1,
};

using ObjCSelectorDataLayout =
llvm::BCRecordLayout<OBJC_SELECTOR_DATA,
llvm::BCVBR<16>,

llvm::BCBlob

>;
}

namespace global_variable_block {
enum { GLOBAL_VARIABLE_DATA = 1 };

using GlobalVariableDataLayout = llvm::BCRecordLayout<
GLOBAL_VARIABLE_DATA,
llvm::BCVBR<16>,
llvm::BCBlob
>;
}

namespace global_function_block {
enum { GLOBAL_FUNCTION_DATA = 1 };

using GlobalFunctionDataLayout = llvm::BCRecordLayout<
GLOBAL_FUNCTION_DATA,
llvm::BCVBR<16>,
llvm::BCBlob
>;
}

namespace tag_block {
enum { TAG_DATA = 1 };

using TagDataLayout =
llvm::BCRecordLayout<TAG_DATA,
llvm::BCVBR<16>,

llvm::BCBlob
>;
};

namespace typedef_block {
enum { TYPEDEF_DATA = 1 };

using TypedefDataLayout =
llvm::BCRecordLayout<TYPEDEF_DATA,
llvm::BCVBR<16>,

llvm::BCBlob
>;
};

namespace enum_constant_block {
enum { ENUM_CONSTANT_DATA = 1 };

using EnumConstantDataLayout =
llvm::BCRecordLayout<ENUM_CONSTANT_DATA,
llvm::BCVBR<16>,

llvm::BCBlob
>;
}


struct StoredObjCSelector {
unsigned NumPieces;
llvm::SmallVector<IdentifierID, 2> Identifiers;
};
}
}

#endif
