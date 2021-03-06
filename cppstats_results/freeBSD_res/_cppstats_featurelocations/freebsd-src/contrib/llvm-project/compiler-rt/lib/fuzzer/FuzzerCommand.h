










#if !defined(LLVM_FUZZER_COMMAND_H)
#define LLVM_FUZZER_COMMAND_H

#include "FuzzerDefs.h"
#include "FuzzerIO.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace fuzzer {

class Command final {
public:



static inline const char *ignoreRemainingArgs() {
return "-ignore_remaining_args=1";
}

Command() : CombinedOutAndErr(false) {}

explicit Command(const Vector<std::string> &ArgsToAdd)
: Args(ArgsToAdd), CombinedOutAndErr(false) {}

explicit Command(const Command &Other)
: Args(Other.Args), CombinedOutAndErr(Other.CombinedOutAndErr),
OutputFile(Other.OutputFile) {}

Command &operator=(const Command &Other) {
Args = Other.Args;
CombinedOutAndErr = Other.CombinedOutAndErr;
OutputFile = Other.OutputFile;
return *this;
}

~Command() {}



bool hasArgument(const std::string &Arg) const {
auto i = endMutableArgs();
return std::find(Args.begin(), i, Arg) != i;
}



const Vector<std::string> &getArguments() const { return Args; }



void addArgument(const std::string &Arg) {
Args.insert(endMutableArgs(), Arg);
}



void addArguments(const Vector<std::string> &ArgsToAdd) {
Args.insert(endMutableArgs(), ArgsToAdd.begin(), ArgsToAdd.end());
}



void removeArgument(const std::string &Arg) {
auto i = endMutableArgs();
Args.erase(std::remove(Args.begin(), i, Arg), i);
}


bool hasFlag(const std::string &Flag) const {
std::string Arg("-" + Flag + "=");
auto IsMatch = [&](const std::string &Other) {
return Arg.compare(0, std::string::npos, Other, 0, Arg.length()) == 0;
};
return std::any_of(Args.begin(), endMutableArgs(), IsMatch);
}




std::string getFlagValue(const std::string &Flag) const {
std::string Arg("-" + Flag + "=");
auto IsMatch = [&](const std::string &Other) {
return Arg.compare(0, std::string::npos, Other, 0, Arg.length()) == 0;
};
auto i = endMutableArgs();
auto j = std::find_if(Args.begin(), i, IsMatch);
std::string result;
if (j != i) {
result = j->substr(Arg.length());
}
return result;
}


void addFlag(const std::string &Flag, const std::string &Value) {
addArgument("-" + Flag + "=" + Value);
}


void removeFlag(const std::string &Flag) {
std::string Arg("-" + Flag + "=");
auto IsMatch = [&](const std::string &Other) {
return Arg.compare(0, std::string::npos, Other, 0, Arg.length()) == 0;
};
auto i = endMutableArgs();
Args.erase(std::remove_if(Args.begin(), i, IsMatch), i);
}


bool hasOutputFile() const { return !OutputFile.empty(); }


const std::string &getOutputFile() const { return OutputFile; }


void setOutputFile(const std::string &FileName) { OutputFile = FileName; }


bool isOutAndErrCombined() const { return CombinedOutAndErr; }


void combineOutAndErr(bool combine = true) { CombinedOutAndErr = combine; }



std::string toString() const {
std::stringstream SS;
for (auto arg : getArguments())
SS << arg << " ";
if (hasOutputFile())
SS << ">" << getOutputFile() << " ";
if (isOutAndErrCombined())
SS << "2>&1 ";
std::string result = SS.str();
if (!result.empty())
result = result.substr(0, result.length() - 1);
return result;
}

private:
Command(Command &&Other) = delete;
Command &operator=(Command &&Other) = delete;

Vector<std::string>::iterator endMutableArgs() {
return std::find(Args.begin(), Args.end(), ignoreRemainingArgs());
}

Vector<std::string>::const_iterator endMutableArgs() const {
return std::find(Args.begin(), Args.end(), ignoreRemainingArgs());
}


Vector<std::string> Args;


bool CombinedOutAndErr;


std::string OutputFile;
};

}

#endif
