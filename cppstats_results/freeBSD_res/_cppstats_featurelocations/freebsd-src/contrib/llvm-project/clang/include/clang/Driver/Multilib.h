







#if !defined(LLVM_CLANG_DRIVER_MULTILIB_H)
#define LLVM_CLANG_DRIVER_MULTILIB_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Compiler.h"
#include <cassert>
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace clang {
namespace driver {



class Multilib {
public:
using flags_list = std::vector<std::string>;

private:
std::string GCCSuffix;
std::string OSSuffix;
std::string IncludeSuffix;
flags_list Flags;
int Priority;

public:
Multilib(StringRef GCCSuffix = {}, StringRef OSSuffix = {},
StringRef IncludeSuffix = {}, int Priority = 0);



const std::string &gccSuffix() const {
assert(GCCSuffix.empty() ||
(StringRef(GCCSuffix).front() == '/' && GCCSuffix.size() > 1));
return GCCSuffix;
}


Multilib &gccSuffix(StringRef S);



const std::string &osSuffix() const {
assert(OSSuffix.empty() ||
(StringRef(OSSuffix).front() == '/' && OSSuffix.size() > 1));
return OSSuffix;
}


Multilib &osSuffix(StringRef S);



const std::string &includeSuffix() const {
assert(IncludeSuffix.empty() ||
(StringRef(IncludeSuffix).front() == '/' && IncludeSuffix.size() > 1));
return IncludeSuffix;
}


Multilib &includeSuffix(StringRef S);



const flags_list &flags() const { return Flags; }
flags_list &flags() { return Flags; }



int priority() const { return Priority; }








Multilib &flag(StringRef F) {
assert(F.front() == '+' || F.front() == '-');
Flags.push_back(std::string(F));
return *this;
}

LLVM_DUMP_METHOD void dump() const;

void print(raw_ostream &OS) const;


bool isValid() const;


bool isDefault() const
{ return GCCSuffix.empty() && OSSuffix.empty() && IncludeSuffix.empty(); }

bool operator==(const Multilib &Other) const;
};

raw_ostream &operator<<(raw_ostream &OS, const Multilib &M);

class MultilibSet {
public:
using multilib_list = std::vector<Multilib>;
using iterator = multilib_list::iterator;
using const_iterator = multilib_list::const_iterator;
using IncludeDirsFunc =
std::function<std::vector<std::string>(const Multilib &M)>;
using FilterCallback = llvm::function_ref<bool(const Multilib &)>;

private:
multilib_list Multilibs;
IncludeDirsFunc IncludeCallback;
IncludeDirsFunc FilePathsCallback;

public:
MultilibSet() = default;


MultilibSet &Maybe(const Multilib &M);


MultilibSet &Either(const Multilib &M1, const Multilib &M2);
MultilibSet &Either(const Multilib &M1, const Multilib &M2,
const Multilib &M3);
MultilibSet &Either(const Multilib &M1, const Multilib &M2,
const Multilib &M3, const Multilib &M4);
MultilibSet &Either(const Multilib &M1, const Multilib &M2,
const Multilib &M3, const Multilib &M4,
const Multilib &M5);
MultilibSet &Either(ArrayRef<Multilib> Ms);


MultilibSet &FilterOut(FilterCallback F);


MultilibSet &FilterOut(const char *Regex);


void push_back(const Multilib &M);


void combineWith(const MultilibSet &MS);


void clear() { Multilibs.clear(); }

iterator begin() { return Multilibs.begin(); }
const_iterator begin() const { return Multilibs.begin(); }

iterator end() { return Multilibs.end(); }
const_iterator end() const { return Multilibs.end(); }


bool select(const Multilib::flags_list &Flags, Multilib &M) const;

unsigned size() const { return Multilibs.size(); }

LLVM_DUMP_METHOD void dump() const;
void print(raw_ostream &OS) const;

MultilibSet &setIncludeDirsCallback(IncludeDirsFunc F) {
IncludeCallback = std::move(F);
return *this;
}

const IncludeDirsFunc &includeDirsCallback() const { return IncludeCallback; }

MultilibSet &setFilePathsCallback(IncludeDirsFunc F) {
FilePathsCallback = std::move(F);
return *this;
}

const IncludeDirsFunc &filePathsCallback() const { return FilePathsCallback; }

private:

static multilib_list filterCopy(FilterCallback F, const multilib_list &Ms);


static void filterInPlace(FilterCallback F, multilib_list &Ms);
};

raw_ostream &operator<<(raw_ostream &OS, const MultilibSet &MS);

}
}

#endif
