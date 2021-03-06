











#if !defined(LLVM_CLANG_AST_INTERP_OPCODE_H)
#define LLVM_CLANG_AST_INTERP_OPCODE_H

#include <cstdint>

namespace clang {
namespace interp {

enum Opcode : uint32_t {
#define GET_OPCODE_NAMES
#include "Opcodes.inc"
#undef GET_OPCODE_NAMES
};

}
}

#endif
