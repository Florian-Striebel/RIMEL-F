
















#if !defined(LLVM_CLANG_LIB_SERIALIZATION_MULTIONDISKHASHTABLE_H)
#define LLVM_CLANG_LIB_SERIALIZATION_MULTIONDISKHASHTABLE_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/OnDiskHashTable.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <cstdint>
#include <vector>

namespace clang {
namespace serialization {


template<typename Info> class MultiOnDiskHashTable {
public:

using file_type = typename Info::file_type;


using storage_type = const unsigned char *;

using external_key_type = typename Info::external_key_type;
using internal_key_type = typename Info::internal_key_type;
using data_type = typename Info::data_type;
using data_type_builder = typename Info::data_type_builder;
using hash_value_type = unsigned;

private:

template<typename ReaderInfo, typename WriterInfo>
friend class MultiOnDiskHashTableGenerator;


struct OnDiskTable {
using HashTable = llvm::OnDiskIterableChainedHashTable<Info>;

file_type File;
HashTable Table;

OnDiskTable(file_type File, unsigned NumBuckets, unsigned NumEntries,
storage_type Buckets, storage_type Payload, storage_type Base,
const Info &InfoObj)
: File(File),
Table(NumBuckets, NumEntries, Buckets, Payload, Base, InfoObj) {}
};

struct MergedTable {
std::vector<file_type> Files;
llvm::DenseMap<internal_key_type, data_type> Data;
};

using Table = llvm::PointerUnion<OnDiskTable *, MergedTable *>;
using TableVector = llvm::TinyPtrVector<void *>;






TableVector Tables;



llvm::TinyPtrVector<file_type> PendingOverrides;

struct AsOnDiskTable {
using result_type = OnDiskTable *;

result_type operator()(void *P) const {
return Table::getFromOpaqueValue(P).template get<OnDiskTable *>();
}
};

using table_iterator =
llvm::mapped_iterator<TableVector::iterator, AsOnDiskTable>;
using table_range = llvm::iterator_range<table_iterator>;


table_range tables() {
auto Begin = Tables.begin(), End = Tables.end();
if (getMergedTable())
++Begin;
return llvm::make_range(llvm::map_iterator(Begin, AsOnDiskTable()),
llvm::map_iterator(End, AsOnDiskTable()));
}

MergedTable *getMergedTable() const {

return Tables.empty() ? nullptr : Table::getFromOpaqueValue(*Tables.begin())
.template dyn_cast<MergedTable*>();
}


void clear() {
for (auto *T : tables())
delete T;
if (auto *M = getMergedTable())
delete M;
Tables.clear();
}

void removeOverriddenTables() {
llvm::DenseSet<file_type> Files;
Files.insert(PendingOverrides.begin(), PendingOverrides.end());

auto ShouldRemove = [&Files](void *T) -> bool {
auto *ODT = Table::getFromOpaqueValue(T).template get<OnDiskTable *>();
bool Remove = Files.count(ODT->File);
if (Remove)
delete ODT;
return Remove;
};
Tables.erase(std::remove_if(tables().begin().getCurrent(), Tables.end(),
ShouldRemove),
Tables.end());
PendingOverrides.clear();
}

void condense() {
MergedTable *Merged = getMergedTable();
if (!Merged)
Merged = new MergedTable;



for (auto *ODT : tables()) {
auto &HT = ODT->Table;
Info &InfoObj = HT.getInfoObj();

for (auto I = HT.data_begin(), E = HT.data_end(); I != E; ++I) {
auto *LocalPtr = I.getItem();


auto L = InfoObj.ReadKeyDataLength(LocalPtr);
const internal_key_type &Key = InfoObj.ReadKey(LocalPtr, L.first);
data_type_builder ValueBuilder(Merged->Data[Key]);
InfoObj.ReadDataInto(Key, LocalPtr + L.first, L.second,
ValueBuilder);
}

Merged->Files.push_back(ODT->File);
delete ODT;
}

Tables.clear();
Tables.push_back(Table(Merged).getOpaqueValue());
}

public:
MultiOnDiskHashTable() = default;

MultiOnDiskHashTable(MultiOnDiskHashTable &&O)
: Tables(std::move(O.Tables)),
PendingOverrides(std::move(O.PendingOverrides)) {
O.Tables.clear();
}

MultiOnDiskHashTable &operator=(MultiOnDiskHashTable &&O) {
if (&O == this)
return *this;
clear();
Tables = std::move(O.Tables);
O.Tables.clear();
PendingOverrides = std::move(O.PendingOverrides);
return *this;
}

~MultiOnDiskHashTable() { clear(); }


void add(file_type File, storage_type Data, Info InfoObj = Info()) {
using namespace llvm::support;

storage_type Ptr = Data;

uint32_t BucketOffset = endian::readNext<uint32_t, little, unaligned>(Ptr);


uint32_t NumFiles = endian::readNext<uint32_t, little, unaligned>(Ptr);


llvm::SmallVector<file_type, 16> OverriddenFiles;
OverriddenFiles.reserve(NumFiles);
for (; NumFiles != 0; --NumFiles)
OverriddenFiles.push_back(InfoObj.ReadFileRef(Ptr));
PendingOverrides.insert(PendingOverrides.end(), OverriddenFiles.begin(),
OverriddenFiles.end());


storage_type Buckets = Data + BucketOffset;
auto NumBucketsAndEntries =
OnDiskTable::HashTable::readNumBucketsAndEntries(Buckets);


Table NewTable = new OnDiskTable(File, NumBucketsAndEntries.first,
NumBucketsAndEntries.second,
Buckets, Ptr, Data, std::move(InfoObj));
Tables.push_back(NewTable.getOpaqueValue());
}


data_type find(const external_key_type &EKey) {
data_type Result;

if (!PendingOverrides.empty())
removeOverriddenTables();

if (Tables.size() > static_cast<unsigned>(Info::MaxTables))
condense();

internal_key_type Key = Info::GetInternalKey(EKey);
auto KeyHash = Info::ComputeHash(Key);

if (MergedTable *M = getMergedTable()) {
auto It = M->Data.find(Key);
if (It != M->Data.end())
Result = It->second;
}

data_type_builder ResultBuilder(Result);

for (auto *ODT : tables()) {
auto &HT = ODT->Table;
auto It = HT.find_hashed(Key, KeyHash);
if (It != HT.end())
HT.getInfoObj().ReadDataInto(Key, It.getDataPtr(), It.getDataLen(),
ResultBuilder);
}

return Result;
}



data_type findAll() {
data_type Result;
data_type_builder ResultBuilder(Result);

if (!PendingOverrides.empty())
removeOverriddenTables();

if (MergedTable *M = getMergedTable()) {
for (auto &KV : M->Data)
Info::MergeDataInto(KV.second, ResultBuilder);
}

for (auto *ODT : tables()) {
auto &HT = ODT->Table;
Info &InfoObj = HT.getInfoObj();
for (auto I = HT.data_begin(), E = HT.data_end(); I != E; ++I) {
auto *LocalPtr = I.getItem();


auto L = InfoObj.ReadKeyDataLength(LocalPtr);
const internal_key_type &Key = InfoObj.ReadKey(LocalPtr, L.first);
InfoObj.ReadDataInto(Key, LocalPtr + L.first, L.second, ResultBuilder);
}
}

return Result;
}
};


template<typename ReaderInfo, typename WriterInfo>
class MultiOnDiskHashTableGenerator {
using BaseTable = MultiOnDiskHashTable<ReaderInfo>;
using Generator = llvm::OnDiskChainedHashTableGenerator<WriterInfo>;

Generator Gen;

public:
MultiOnDiskHashTableGenerator() : Gen() {}

void insert(typename WriterInfo::key_type_ref Key,
typename WriterInfo::data_type_ref Data, WriterInfo &Info) {
Gen.insert(Key, Data, Info);
}

void emit(llvm::SmallVectorImpl<char> &Out, WriterInfo &Info,
const BaseTable *Base) {
using namespace llvm::support;

llvm::raw_svector_ostream OutStream(Out);


{
endian::Writer Writer(OutStream, little);


Writer.write<uint32_t>(0);

if (auto *Merged = Base ? Base->getMergedTable() : nullptr) {

Writer.write<uint32_t>(Merged->Files.size());
for (const auto &F : Merged->Files)
Info.EmitFileRef(OutStream, F);


for (auto &KV : Merged->Data) {
if (!Gen.contains(KV.first, Info))
Gen.insert(KV.first, Info.ImportData(KV.second), Info);
}
} else {
Writer.write<uint32_t>(0);
}
}


uint32_t BucketOffset = Gen.Emit(OutStream, Info);


endian::write32le(Out.data(), BucketOffset);
}
};

}
}

#endif
