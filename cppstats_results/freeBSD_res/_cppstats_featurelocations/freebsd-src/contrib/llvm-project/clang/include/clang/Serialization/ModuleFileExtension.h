







#if !defined(LLVM_CLANG_SERIALIZATION_MODULEFILEEXTENSION_H)
#define LLVM_CLANG_SERIALIZATION_MODULEFILEEXTENSION_H

#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/Support/ExtensibleRTTI.h"
#include <memory>
#include <string>

namespace llvm {
class BitstreamCursor;
class BitstreamWriter;
class hash_code;
class raw_ostream;
}

namespace clang {

class ASTReader;
class ASTWriter;
class Sema;

namespace serialization {
class ModuleFile;
}


struct ModuleFileExtensionMetadata {




std::string BlockName;


unsigned MajorVersion;


unsigned MinorVersion;



std::string UserInfo;
};

class ModuleFileExtensionReader;
class ModuleFileExtensionWriter;










class ModuleFileExtension
: public llvm::RTTIExtends<ModuleFileExtension, llvm::RTTIRoot> {
public:

static char ID;

virtual ~ModuleFileExtension();


virtual ModuleFileExtensionMetadata getExtensionMetadata() const = 0;














virtual llvm::hash_code hashExtension(llvm::hash_code c) const;




virtual std::unique_ptr<ModuleFileExtensionWriter>
createExtensionWriter(ASTWriter &Writer) = 0;







virtual std::unique_ptr<ModuleFileExtensionReader>
createExtensionReader(const ModuleFileExtensionMetadata &Metadata,
ASTReader &Reader, serialization::ModuleFile &Mod,
const llvm::BitstreamCursor &Stream) = 0;
};



class ModuleFileExtensionWriter {
ModuleFileExtension *Extension;

protected:
ModuleFileExtensionWriter(ModuleFileExtension *Extension)
: Extension(Extension) { }

public:
virtual ~ModuleFileExtensionWriter();



ModuleFileExtension *getExtension() const { return Extension; }






virtual void writeExtensionContents(Sema &SemaRef,
llvm::BitstreamWriter &Stream) = 0;
};





class ModuleFileExtensionReader {
ModuleFileExtension *Extension;

protected:
ModuleFileExtensionReader(ModuleFileExtension *Extension)
: Extension(Extension) { }

public:


ModuleFileExtension *getExtension() const { return Extension; }

virtual ~ModuleFileExtensionReader();
};

}

#endif
