







#if !defined(LLVM_CLANG_SERIALIZATION_INMEMORYMODULECACHE_H)
#define LLVM_CLANG_SERIALIZATION_INMEMORYMODULECACHE_H

#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/MemoryBuffer.h"
#include <memory>

namespace clang {











class InMemoryModuleCache : public llvm::RefCountedBase<InMemoryModuleCache> {
struct PCM {
std::unique_ptr<llvm::MemoryBuffer> Buffer;




bool IsFinal = false;

PCM() = default;
PCM(std::unique_ptr<llvm::MemoryBuffer> Buffer)
: Buffer(std::move(Buffer)) {}
};


llvm::StringMap<PCM> PCMs;

public:









enum State { Unknown, Tentative, ToBuild, Final };


State getPCMState(llvm::StringRef Filename) const;






llvm::MemoryBuffer &addPCM(llvm::StringRef Filename,
std::unique_ptr<llvm::MemoryBuffer> Buffer);






llvm::MemoryBuffer &addBuiltPCM(llvm::StringRef Filename,
std::unique_ptr<llvm::MemoryBuffer> Buffer);






bool tryToDropPCM(llvm::StringRef Filename);





void finalizePCM(llvm::StringRef Filename);


llvm::MemoryBuffer *lookupPCM(llvm::StringRef Filename) const;




bool isPCMFinal(llvm::StringRef Filename) const;




bool shouldBuildPCM(llvm::StringRef Filename) const;
};

}

#endif
