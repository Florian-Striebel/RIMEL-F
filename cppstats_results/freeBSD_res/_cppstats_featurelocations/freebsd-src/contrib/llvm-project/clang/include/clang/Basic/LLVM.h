













#if !defined(LLVM_CLANG_BASIC_LLVM_H)
#define LLVM_CLANG_BASIC_LLVM_H




#include "llvm/Support/Casting.h"


#include "llvm/ADT/None.h"


#include "llvm/ADT/SmallVector.h"

namespace llvm {

class StringRef;
class Twine;
class VersionTuple;
template<typename T> class ArrayRef;
template<typename T> class MutableArrayRef;
template<typename T> class OwningArrayRef;
template<unsigned InternalLen> class SmallString;
template<typename T, unsigned N> class SmallVector;
template<typename T> class SmallVectorImpl;
template<typename T> class Optional;
template <class T> class Expected;

template<typename T>
struct SaveAndRestore;


template <typename T> class IntrusiveRefCntPtr;
template <typename T> struct IntrusiveRefCntPtrInfo;
template <class Derived> class RefCountedBase;

class raw_ostream;
class raw_pwrite_stream;

}


namespace clang {

using llvm::isa;
using llvm::isa_and_nonnull;
using llvm::cast;
using llvm::dyn_cast;
using llvm::dyn_cast_or_null;
using llvm::cast_or_null;


using llvm::ArrayRef;
using llvm::MutableArrayRef;
using llvm::None;
using llvm::Optional;
using llvm::OwningArrayRef;
using llvm::SaveAndRestore;
using llvm::SmallString;
using llvm::SmallVector;
using llvm::SmallVectorImpl;
using llvm::StringRef;
using llvm::Twine;
using llvm::VersionTuple;


using llvm::Expected;


using llvm::IntrusiveRefCntPtr;
using llvm::IntrusiveRefCntPtrInfo;
using llvm::RefCountedBase;

using llvm::raw_ostream;
using llvm::raw_pwrite_stream;
}

#endif
