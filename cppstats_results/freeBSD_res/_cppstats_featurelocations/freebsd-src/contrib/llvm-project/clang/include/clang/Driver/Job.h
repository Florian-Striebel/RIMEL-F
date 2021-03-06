







#if !defined(LLVM_CLANG_DRIVER_JOB_H)
#define LLVM_CLANG_DRIVER_JOB_H

#include "clang/Basic/LLVM.h"
#include "clang/Driver/InputInfo.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/Program.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace clang {
namespace driver {

class Action;
class InputInfo;
class Tool;

struct CrashReportInfo {
StringRef Filename;
StringRef VFSPath;

CrashReportInfo(StringRef Filename, StringRef VFSPath)
: Filename(Filename), VFSPath(VFSPath) {}
};




struct ResponseFileSupport {
enum ResponseFileKind {


RF_Full,


RF_FileList,


RF_None
};

ResponseFileKind ResponseKind;












llvm::sys::WindowsEncodingMethod ResponseEncoding;



const char *ResponseFlag;



static constexpr ResponseFileSupport None() {
return {RF_None, llvm::sys::WEM_UTF8, nullptr};
}




static constexpr ResponseFileSupport AtFileUTF8() {
return {RF_Full, llvm::sys::WEM_UTF8, "@"};
}




static constexpr ResponseFileSupport AtFileCurCP() {
return {RF_Full, llvm::sys::WEM_CurrentCodePage, "@"};
}




static constexpr ResponseFileSupport AtFileUTF16() {
return {RF_Full, llvm::sys::WEM_UTF16, "@"};
}
};



class Command {

const Action &Source;


const Tool &Creator;


ResponseFileSupport ResponseSupport;


const char *Executable;



llvm::opt::ArgStringList Arguments;


std::vector<InputInfo> InputInfoList;


std::vector<std::string> OutputFilenames;



const char *ResponseFile = nullptr;



llvm::opt::ArgStringList InputFileList;



std::string ResponseFileFlag;


std::vector<const char *> Environment;


mutable Optional<llvm::sys::ProcessStatistics> ProcStat;





void buildArgvForResponseFile(llvm::SmallVectorImpl<const char *> &Out) const;





void writeResponseFile(raw_ostream &OS) const;

public:

bool PrintInputFilenames = false;


bool InProcess = false;

Command(const Action &Source, const Tool &Creator,
ResponseFileSupport ResponseSupport, const char *Executable,
const llvm::opt::ArgStringList &Arguments, ArrayRef<InputInfo> Inputs,
ArrayRef<InputInfo> Outputs = None);


Command(const Command &) = default;
virtual ~Command() = default;

virtual void Print(llvm::raw_ostream &OS, const char *Terminator, bool Quote,
CrashReportInfo *CrashInfo = nullptr) const;

virtual int Execute(ArrayRef<Optional<StringRef>> Redirects,
std::string *ErrMsg, bool *ExecutionFailed) const;


const Action &getSource() const { return Source; }


const Tool &getCreator() const { return Creator; }


const ResponseFileSupport &getResponseFileSupport() {
return ResponseSupport;
}


void setResponseFile(const char *FileName);



void setInputFileList(llvm::opt::ArgStringList List) {
InputFileList = std::move(List);
}





virtual void setEnvironment(llvm::ArrayRef<const char *> NewEnvironment);

const char *getExecutable() const { return Executable; }

const llvm::opt::ArgStringList &getArguments() const { return Arguments; }

const std::vector<InputInfo> &getInputInfos() const { return InputInfoList; }

const std::vector<std::string> &getOutputFilenames() const {
return OutputFilenames;
}

Optional<llvm::sys::ProcessStatistics> getProcessStatistics() const {
return ProcStat;
}

protected:

void PrintFileNames() const;
};


class CC1Command : public Command {
public:
CC1Command(const Action &Source, const Tool &Creator,
ResponseFileSupport ResponseSupport, const char *Executable,
const llvm::opt::ArgStringList &Arguments,
ArrayRef<InputInfo> Inputs, ArrayRef<InputInfo> Outputs = None);

void Print(llvm::raw_ostream &OS, const char *Terminator, bool Quote,
CrashReportInfo *CrashInfo = nullptr) const override;

int Execute(ArrayRef<Optional<StringRef>> Redirects, std::string *ErrMsg,
bool *ExecutionFailed) const override;

void setEnvironment(llvm::ArrayRef<const char *> NewEnvironment) override;
};


class ForceSuccessCommand : public Command {
public:
ForceSuccessCommand(const Action &Source_, const Tool &Creator_,
ResponseFileSupport ResponseSupport,
const char *Executable_,
const llvm::opt::ArgStringList &Arguments_,
ArrayRef<InputInfo> Inputs,
ArrayRef<InputInfo> Outputs = None);

void Print(llvm::raw_ostream &OS, const char *Terminator, bool Quote,
CrashReportInfo *CrashInfo = nullptr) const override;

int Execute(ArrayRef<Optional<StringRef>> Redirects, std::string *ErrMsg,
bool *ExecutionFailed) const override;
};


class JobList {
public:
using list_type = SmallVector<std::unique_ptr<Command>, 4>;
using size_type = list_type::size_type;
using iterator = llvm::pointee_iterator<list_type::iterator>;
using const_iterator = llvm::pointee_iterator<list_type::const_iterator>;

private:
list_type Jobs;

public:
void Print(llvm::raw_ostream &OS, const char *Terminator,
bool Quote, CrashReportInfo *CrashInfo = nullptr) const;


void addJob(std::unique_ptr<Command> J) { Jobs.push_back(std::move(J)); }


void clear();

const list_type &getJobs() const { return Jobs; }

bool empty() const { return Jobs.empty(); }
size_type size() const { return Jobs.size(); }
iterator begin() { return Jobs.begin(); }
const_iterator begin() const { return Jobs.begin(); }
iterator end() { return Jobs.end(); }
const_iterator end() const { return Jobs.end(); }
};

}
}

#endif
