









#if !defined(LLVM_FUZZER_DICTIONARY_H)
#define LLVM_FUZZER_DICTIONARY_H

#include "FuzzerDefs.h"
#include "FuzzerIO.h"
#include "FuzzerUtil.h"
#include <algorithm>
#include <limits>

namespace fuzzer {

template <size_t kMaxSizeT> class FixedWord {
public:
static const size_t kMaxSize = kMaxSizeT;
FixedWord() {}
FixedWord(const uint8_t *B, size_t S) { Set(B, S); }

void Set(const uint8_t *B, size_t S) {
static_assert(kMaxSizeT <= std::numeric_limits<uint8_t>::max(),
"FixedWord::kMaxSizeT cannot fit in a uint8_t.");
assert(S <= kMaxSize);
memcpy(Data, B, S);
Size = static_cast<uint8_t>(S);
}

bool operator==(const FixedWord<kMaxSize> &w) const {
return Size == w.Size && 0 == memcmp(Data, w.Data, Size);
}

static size_t GetMaxSize() { return kMaxSize; }
const uint8_t *data() const { return Data; }
uint8_t size() const { return Size; }

private:
uint8_t Size = 0;
uint8_t Data[kMaxSize];
};

typedef FixedWord<64> Word;

class DictionaryEntry {
public:
DictionaryEntry() {}
DictionaryEntry(Word W) : W(W) {}
DictionaryEntry(Word W, size_t PositionHint) : W(W), PositionHint(PositionHint) {}
const Word &GetW() const { return W; }

bool HasPositionHint() const { return PositionHint != std::numeric_limits<size_t>::max(); }
size_t GetPositionHint() const {
assert(HasPositionHint());
return PositionHint;
}
void IncUseCount() { UseCount++; }
void IncSuccessCount() { SuccessCount++; }
size_t GetUseCount() const { return UseCount; }
size_t GetSuccessCount() const {return SuccessCount; }

void Print(const char *PrintAfter = "\n") {
PrintASCII(W.data(), W.size());
if (HasPositionHint())
Printf("@%zd", GetPositionHint());
Printf("%s", PrintAfter);
}

private:
Word W;
size_t PositionHint = std::numeric_limits<size_t>::max();
size_t UseCount = 0;
size_t SuccessCount = 0;
};

class Dictionary {
public:
static const size_t kMaxDictSize = 1 << 14;

bool ContainsWord(const Word &W) const {
return std::any_of(begin(), end(), [&](const DictionaryEntry &DE) {
return DE.GetW() == W;
});
}
const DictionaryEntry *begin() const { return &DE[0]; }
const DictionaryEntry *end() const { return begin() + Size; }
DictionaryEntry & operator[] (size_t Idx) {
assert(Idx < Size);
return DE[Idx];
}
void push_back(DictionaryEntry DE) {
if (Size < kMaxDictSize)
this->DE[Size++] = DE;
}
void clear() { Size = 0; }
bool empty() const { return Size == 0; }
size_t size() const { return Size; }

private:
DictionaryEntry DE[kMaxDictSize];
size_t Size = 0;
};




bool ParseOneDictionaryEntry(const std::string &Str, Unit *U);


bool ParseDictionaryFile(const std::string &Text, Vector<Unit> *Units);

}

#endif
