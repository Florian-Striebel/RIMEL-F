







#if !defined(LLD_CORE_READER_H)
#define LLD_CORE_READER_H

#include "lld/Common/LLVM.h"
#include "lld/Core/Reference.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/BinaryFormat/Magic.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include <memory>
#include <vector>

namespace llvm {
namespace yaml {
class IO;
}
}

namespace lld {

class File;
class LinkingContext;
class MachOLinkingContext;





class Reader {
public:
virtual ~Reader() = default;





virtual bool canParse(llvm::file_magic magic, MemoryBufferRef mb) const = 0;




virtual ErrorOr<std::unique_ptr<File>>
loadFile(std::unique_ptr<MemoryBuffer> mb, const class Registry &) const = 0;
};










class YamlIOTaggedDocumentHandler {
public:
virtual ~YamlIOTaggedDocumentHandler();






virtual bool handledDocTag(llvm::yaml::IO &io, const lld::File *&f) const = 0;
};






class Registry {
public:
Registry();



ErrorOr<std::unique_ptr<File>>
loadFile(std::unique_ptr<MemoryBuffer> mb) const;



bool referenceKindFromString(StringRef inputStr, Reference::KindNamespace &ns,
Reference::KindArch &a,
Reference::KindValue &value) const;



bool referenceKindToString(Reference::KindNamespace ns, Reference::KindArch a,
Reference::KindValue value, StringRef &) const;



bool handleTaggedDoc(llvm::yaml::IO &io, const lld::File *&file) const;






void addSupportArchives(bool logLoading);
void addSupportYamlFiles();
void addSupportMachOObjects(MachOLinkingContext &);




struct KindStrings {
Reference::KindValue value;
StringRef name;
};




void addKindTable(Reference::KindNamespace ns, Reference::KindArch arch,
const KindStrings array[]);

private:
struct KindEntry {
Reference::KindNamespace ns;
Reference::KindArch arch;
const KindStrings *array;
};

void add(std::unique_ptr<Reader>);
void add(std::unique_ptr<YamlIOTaggedDocumentHandler>);

std::vector<std::unique_ptr<Reader>> _readers;
std::vector<std::unique_ptr<YamlIOTaggedDocumentHandler>> _yamlHandlers;
std::vector<KindEntry> _kindEntries;
};







#define LLD_KIND_STRING_ENTRY(name) { name, #name }
#define LLD_KIND_STRING_END { 0, "" }

}

#endif
