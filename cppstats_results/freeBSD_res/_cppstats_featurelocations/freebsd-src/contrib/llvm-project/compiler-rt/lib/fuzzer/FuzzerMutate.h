









#if !defined(LLVM_FUZZER_MUTATE_H)
#define LLVM_FUZZER_MUTATE_H

#include "FuzzerDefs.h"
#include "FuzzerDictionary.h"
#include "FuzzerOptions.h"
#include "FuzzerRandom.h"

namespace fuzzer {

class MutationDispatcher {
public:
MutationDispatcher(Random &Rand, const FuzzingOptions &Options);
~MutationDispatcher() {}

void StartMutationSequence();


void PrintMutationSequence(bool Verbose = true);

std::string MutationSequence();

void RecordSuccessfulMutationSequence();

size_t Mutate_Custom(uint8_t *Data, size_t Size, size_t MaxSize);

size_t Mutate_CustomCrossOver(uint8_t *Data, size_t Size, size_t MaxSize);

size_t Mutate_ShuffleBytes(uint8_t *Data, size_t Size, size_t MaxSize);

size_t Mutate_EraseBytes(uint8_t *Data, size_t Size, size_t MaxSize);

size_t Mutate_InsertByte(uint8_t *Data, size_t Size, size_t MaxSize);

size_t Mutate_InsertRepeatedBytes(uint8_t *Data, size_t Size, size_t MaxSize);

size_t Mutate_ChangeByte(uint8_t *Data, size_t Size, size_t MaxSize);

size_t Mutate_ChangeBit(uint8_t *Data, size_t Size, size_t MaxSize);

size_t Mutate_CopyPart(uint8_t *Data, size_t Size, size_t MaxSize);


size_t Mutate_AddWordFromManualDictionary(uint8_t *Data, size_t Size,
size_t MaxSize);


size_t Mutate_AddWordFromTORC(uint8_t *Data, size_t Size, size_t MaxSize);


size_t Mutate_AddWordFromPersistentAutoDictionary(uint8_t *Data, size_t Size,
size_t MaxSize);


size_t Mutate_ChangeASCIIInteger(uint8_t *Data, size_t Size, size_t MaxSize);

size_t Mutate_ChangeBinaryInteger(uint8_t *Data, size_t Size, size_t MaxSize);


size_t Mutate_CrossOver(uint8_t *Data, size_t Size, size_t MaxSize);



size_t Mutate(uint8_t *Data, size_t Size, size_t MaxSize);




size_t MutateWithMask(uint8_t *Data, size_t Size, size_t MaxSize,
const Vector<uint8_t> &Mask);



size_t DefaultMutate(uint8_t *Data, size_t Size, size_t MaxSize);


size_t CrossOver(const uint8_t *Data1, size_t Size1, const uint8_t *Data2,
size_t Size2, uint8_t *Out, size_t MaxOutSize);

void AddWordToManualDictionary(const Word &W);

void PrintRecommendedDictionary();

void SetCrossOverWith(const Unit *U) { CrossOverWith = U; }

Random &GetRand() { return Rand; }

private:
struct Mutator {
size_t (MutationDispatcher::*Fn)(uint8_t *Data, size_t Size, size_t Max);
const char *Name;
};

size_t AddWordFromDictionary(Dictionary &D, uint8_t *Data, size_t Size,
size_t MaxSize);
size_t MutateImpl(uint8_t *Data, size_t Size, size_t MaxSize,
Vector<Mutator> &Mutators);

size_t InsertPartOf(const uint8_t *From, size_t FromSize, uint8_t *To,
size_t ToSize, size_t MaxToSize);
size_t CopyPartOf(const uint8_t *From, size_t FromSize, uint8_t *To,
size_t ToSize);
size_t ApplyDictionaryEntry(uint8_t *Data, size_t Size, size_t MaxSize,
DictionaryEntry &DE);

template <class T>
DictionaryEntry MakeDictionaryEntryFromCMP(T Arg1, T Arg2,
const uint8_t *Data, size_t Size);
DictionaryEntry MakeDictionaryEntryFromCMP(const Word &Arg1, const Word &Arg2,
const uint8_t *Data, size_t Size);
DictionaryEntry MakeDictionaryEntryFromCMP(const void *Arg1, const void *Arg2,
const void *Arg1Mutation,
const void *Arg2Mutation,
size_t ArgSize,
const uint8_t *Data, size_t Size);

Random &Rand;
const FuzzingOptions Options;


Dictionary ManualDictionary;


Dictionary PersistentAutoDictionary;

Vector<DictionaryEntry *> CurrentDictionaryEntrySequence;

static const size_t kCmpDictionaryEntriesDequeSize = 16;
DictionaryEntry CmpDictionaryEntriesDeque[kCmpDictionaryEntriesDequeSize];
size_t CmpDictionaryEntriesDequeIdx = 0;

const Unit *CrossOverWith = nullptr;
Vector<uint8_t> MutateInPlaceHere;
Vector<uint8_t> MutateWithMaskTemp;


Vector<uint8_t> CustomCrossOverInPlaceHere;

Vector<Mutator> Mutators;
Vector<Mutator> DefaultMutators;
Vector<Mutator> CurrentMutatorSequence;
};

}

#endif
