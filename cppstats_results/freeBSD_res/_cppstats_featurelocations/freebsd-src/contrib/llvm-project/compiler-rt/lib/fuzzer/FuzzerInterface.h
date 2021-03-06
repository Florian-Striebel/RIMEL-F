
















#if !defined(LLVM_FUZZER_INTERFACE_H)
#define LLVM_FUZZER_INTERFACE_H

#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif



#if defined(_WIN32)
#define FUZZER_INTERFACE_VISIBILITY __declspec(dllexport)
#else
#define FUZZER_INTERFACE_VISIBILITY __attribute__((visibility("default")))
#endif





FUZZER_INTERFACE_VISIBILITY int
LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size);





FUZZER_INTERFACE_VISIBILITY int LLVMFuzzerInitialize(int *argc, char ***argv);





FUZZER_INTERFACE_VISIBILITY size_t
LLVMFuzzerCustomMutator(uint8_t *Data, size_t Size, size_t MaxSize,
unsigned int Seed);





FUZZER_INTERFACE_VISIBILITY size_t
LLVMFuzzerCustomCrossOver(const uint8_t *Data1, size_t Size1,
const uint8_t *Data2, size_t Size2, uint8_t *Out,
size_t MaxOutSize, unsigned int Seed);





FUZZER_INTERFACE_VISIBILITY size_t
LLVMFuzzerMutate(uint8_t *Data, size_t Size, size_t MaxSize);

#undef FUZZER_INTERFACE_VISIBILITY

#if defined(__cplusplus)
}
#endif

#endif
