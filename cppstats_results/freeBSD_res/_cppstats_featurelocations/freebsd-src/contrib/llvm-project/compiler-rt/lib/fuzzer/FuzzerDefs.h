









#if !defined(LLVM_FUZZER_DEFS_H)
#define LLVM_FUZZER_DEFS_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <set>
#include <string>
#include <vector>


namespace fuzzer {

template <class T> T Min(T a, T b) { return a < b ? a : b; }
template <class T> T Max(T a, T b) { return a > b ? a : b; }

class Random;
class Dictionary;
class DictionaryEntry;
class MutationDispatcher;
struct FuzzingOptions;
class InputCorpus;
struct InputInfo;
struct ExternalFunctions;


extern ExternalFunctions *EF;



template<typename T>
class fuzzer_allocator: public std::allocator<T> {
public:
fuzzer_allocator() = default;

template<class U>
fuzzer_allocator(const fuzzer_allocator<U>&) {}

template<class Other>
struct rebind { typedef fuzzer_allocator<Other> other; };
};

template<typename T>
using Vector = std::vector<T, fuzzer_allocator<T>>;

template<typename T>
using Set = std::set<T, std::less<T>, fuzzer_allocator<T>>;

typedef Vector<uint8_t> Unit;
typedef Vector<Unit> UnitVector;
typedef int (*UserCallback)(const uint8_t *Data, size_t Size);

int FuzzerDriver(int *argc, char ***argv, UserCallback Callback);

uint8_t *ExtraCountersBegin();
uint8_t *ExtraCountersEnd();
void ClearExtraCounters();

extern bool RunningUserCallback;

}

#endif
