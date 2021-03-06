







#if !defined(LLD_CORE_WRITER_H)
#define LLD_CORE_WRITER_H

#include "lld/Common/LLVM.h"
#include "llvm/Support/Error.h"
#include <memory>
#include <vector>

namespace lld {
class File;
class LinkingContext;
class MachOLinkingContext;




class Writer {
public:
virtual ~Writer();


virtual llvm::Error writeFile(const File &linkedFile, StringRef path) = 0;




virtual void createImplicitFiles(std::vector<std::unique_ptr<File>> &) {}

protected:

Writer();
};

std::unique_ptr<Writer> createWriterMachO(const MachOLinkingContext &);
std::unique_ptr<Writer> createWriterYAML(const LinkingContext &);
}

#endif
