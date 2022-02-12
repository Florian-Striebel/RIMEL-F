







#if !defined(LLVM_FUZZER_FORK_H)
#define LLVM_FUZZER_FORK_H

#include "FuzzerDefs.h"
#include "FuzzerOptions.h"
#include "FuzzerRandom.h"

#include <string>

namespace fuzzer {
void FuzzWithFork(Random &Rand, const FuzzingOptions &Options,
const Vector<std::string> &Args,
const Vector<std::string> &CorpusDirs, int NumJobs);
}

#endif
