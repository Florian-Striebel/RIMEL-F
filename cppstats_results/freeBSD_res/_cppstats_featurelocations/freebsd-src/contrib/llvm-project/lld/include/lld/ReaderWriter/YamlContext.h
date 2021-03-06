







#if !defined(LLD_READER_WRITER_YAML_CONTEXT_H)
#define LLD_READER_WRITER_YAML_CONTEXT_H

#include "lld/Common/LLVM.h"
#include <functional>
#include <memory>
#include <vector>

namespace lld {
class File;
class LinkingContext;
class Registry;
namespace mach_o {
namespace normalized {
struct NormalizedFile;
}
}

using lld::mach_o::normalized::NormalizedFile;




struct YamlContext {
const LinkingContext *_ctx = nullptr;
const Registry *_registry = nullptr;
File *_file = nullptr;
NormalizedFile *_normalizeMachOFile = nullptr;
StringRef _path;
};

}

#endif
