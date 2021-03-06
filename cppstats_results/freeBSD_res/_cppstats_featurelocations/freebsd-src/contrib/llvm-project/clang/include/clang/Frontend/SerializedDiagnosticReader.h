







#if !defined(LLVM_CLANG_FRONTEND_SERIALIZEDDIAGNOSTICREADER_H)
#define LLVM_CLANG_FRONTEND_SERIALIZEDDIAGNOSTICREADER_H

#include "clang/Basic/LLVM.h"
#include "llvm/Bitstream/BitstreamReader.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ErrorOr.h"
#include <system_error>

namespace clang {
namespace serialized_diags {

enum class SDError {
CouldNotLoad = 1,
InvalidSignature,
InvalidDiagnostics,
MalformedTopLevelBlock,
MalformedSubBlock,
MalformedBlockInfoBlock,
MalformedMetadataBlock,
MalformedDiagnosticBlock,
MalformedDiagnosticRecord,
MissingVersion,
VersionMismatch,
UnsupportedConstruct,


HandlerFailed
};

const std::error_category &SDErrorCategory();

inline std::error_code make_error_code(SDError E) {
return std::error_code(static_cast<int>(E), SDErrorCategory());
}


struct Location {
unsigned FileID;
unsigned Line;
unsigned Col;
unsigned Offset;

Location(unsigned FileID, unsigned Line, unsigned Col, unsigned Offset)
: FileID(FileID), Line(Line), Col(Col), Offset(Offset) {}
};





class SerializedDiagnosticReader {
public:
SerializedDiagnosticReader() = default;
virtual ~SerializedDiagnosticReader() = default;


std::error_code readDiagnostics(StringRef File);

private:
enum class Cursor;


llvm::ErrorOr<Cursor> skipUntilRecordOrBlock(llvm::BitstreamCursor &Stream,
unsigned &BlockOrRecordId);


std::error_code readMetaBlock(llvm::BitstreamCursor &Stream);


std::error_code readDiagnosticBlock(llvm::BitstreamCursor &Stream);

protected:

virtual std::error_code visitStartOfDiagnostic() { return {}; }


virtual std::error_code visitEndOfDiagnostic() { return {}; }


virtual std::error_code visitCategoryRecord(unsigned ID, StringRef Name) {
return {};
}


virtual std::error_code visitDiagFlagRecord(unsigned ID, StringRef Name) {
return {};
}


virtual std::error_code
visitDiagnosticRecord(unsigned Severity, const Location &Location,
unsigned Category, unsigned Flag, StringRef Message) {
return {};
}


virtual std::error_code visitFilenameRecord(unsigned ID, unsigned Size,
unsigned Timestamp,
StringRef Name) {
return {};
}


virtual std::error_code
visitFixitRecord(const Location &Start, const Location &End, StringRef Text) {
return {};
}


virtual std::error_code visitSourceRangeRecord(const Location &Start,
const Location &End) {
return {};
}


virtual std::error_code visitVersionRecord(unsigned Version) { return {}; }
};

}
}

namespace std {

template <>
struct is_error_code_enum<clang::serialized_diags::SDError> : std::true_type {};

}

#endif
