







#if !defined(LLDB_HOST_OPTIONPARSER_H)
#define LLDB_HOST_OPTIONPARSER_H

#include <mutex>
#include <string>

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/ArrayRef.h"

struct option;

namespace lldb_private {

struct OptionDefinition;

struct Option {

const OptionDefinition *definition;

int *flag;

int val;
};

class OptionParser {
public:
enum OptionArgument { eNoArgument = 0, eRequiredArgument, eOptionalArgument };

static void Prepare(std::unique_lock<std::mutex> &lock);

static void EnableError(bool error);



static int Parse(llvm::MutableArrayRef<char *> argv,
llvm::StringRef optstring, const Option *longopts,
int *longindex);

static char *GetOptionArgument();
static int GetOptionIndex();
static int GetOptionErrorCause();
static std::string GetShortOptionString(struct option *long_options);
};
}

#endif
