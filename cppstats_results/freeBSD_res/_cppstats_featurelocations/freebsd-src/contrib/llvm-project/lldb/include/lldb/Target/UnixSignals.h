







#if !defined(LLDB_TARGET_UNIXSIGNALS_H)
#define LLDB_TARGET_UNIXSIGNALS_H

#include <map>
#include <string>
#include <vector>

#include "lldb/Utility/ConstString.h"
#include "lldb/lldb-private.h"
#include "llvm/ADT/Optional.h"

namespace lldb_private {

class UnixSignals {
public:
static lldb::UnixSignalsSP Create(const ArchSpec &arch);
static lldb::UnixSignalsSP CreateForHost();


UnixSignals();

virtual ~UnixSignals();

const char *GetSignalAsCString(int32_t signo) const;

bool SignalIsValid(int32_t signo) const;

int32_t GetSignalNumberFromName(const char *name) const;

const char *GetSignalInfo(int32_t signo, bool &should_suppress,
bool &should_stop, bool &should_notify) const;

bool GetShouldSuppress(int32_t signo) const;

bool SetShouldSuppress(int32_t signo, bool value);

bool SetShouldSuppress(const char *signal_name, bool value);

bool GetShouldStop(int32_t signo) const;

bool SetShouldStop(int32_t signo, bool value);
bool SetShouldStop(const char *signal_name, bool value);

bool GetShouldNotify(int32_t signo) const;

bool SetShouldNotify(int32_t signo, bool value);

bool SetShouldNotify(const char *signal_name, bool value);




int32_t GetFirstSignalNumber() const;

int32_t GetNextSignalNumber(int32_t current_signal) const;

int32_t GetNumSignals() const;

int32_t GetSignalAtIndex(int32_t index) const;

ConstString GetShortName(ConstString name) const;







void AddSignal(int signo, const char *name, bool default_suppress,
bool default_stop, bool default_notify,
const char *description, const char *alias = nullptr);

void RemoveSignal(int signo);



uint64_t GetVersion() const;





std::vector<int32_t> GetFilteredSignals(llvm::Optional<bool> should_suppress,
llvm::Optional<bool> should_stop,
llvm::Optional<bool> should_notify);

protected:


struct Signal {
ConstString m_name;
ConstString m_alias;
std::string m_description;
bool m_suppress : 1, m_stop : 1, m_notify : 1;

Signal(const char *name, bool default_suppress, bool default_stop,
bool default_notify, const char *description, const char *alias);

~Signal() = default;
};

virtual void Reset();

typedef std::map<int32_t, Signal> collection;

collection m_signals;





uint64_t m_version = 0;


UnixSignals(const UnixSignals &rhs);

const UnixSignals &operator=(const UnixSignals &rhs) = delete;
};

}
#endif
