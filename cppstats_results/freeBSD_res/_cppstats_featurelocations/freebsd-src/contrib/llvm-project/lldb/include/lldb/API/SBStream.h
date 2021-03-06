







#if !defined(LLDB_API_SBSTREAM_H)
#define LLDB_API_SBSTREAM_H

#include <cstdio>

#include "lldb/API/SBDefines.h"

namespace lldb {

class LLDB_API SBStream {
public:
SBStream();

SBStream(SBStream &&rhs);

~SBStream();

explicit operator bool() const;

bool IsValid() const;



const char *GetData();



size_t GetSize();

void Printf(const char *format, ...) __attribute__((format(printf, 2, 3)));

void Print(const char *str);

void RedirectToFile(const char *path, bool append);

void RedirectToFile(lldb::SBFile file);

void RedirectToFile(lldb::FileSP file);

void RedirectToFileHandle(FILE *fh, bool transfer_fh_ownership);

void RedirectToFileDescriptor(int fd, bool transfer_fh_ownership);




void Clear();

protected:
friend class SBAddress;
friend class SBBlock;
friend class SBBreakpoint;
friend class SBBreakpointLocation;
friend class SBBreakpointName;
friend class SBCommandReturnObject;
friend class SBCompileUnit;
friend class SBData;
friend class SBDebugger;
friend class SBDeclaration;
friend class SBEvent;
friend class SBFileSpec;
friend class SBFileSpecList;
friend class SBFrame;
friend class SBFunction;
friend class SBInstruction;
friend class SBInstructionList;
friend class SBLaunchInfo;
friend class SBLineEntry;
friend class SBMemoryRegionInfo;
friend class SBModule;
friend class SBModuleSpec;
friend class SBModuleSpecList;
friend class SBProcess;
friend class SBSection;
friend class SBSourceManager;
friend class SBStructuredData;
friend class SBSymbol;
friend class SBSymbolContext;
friend class SBSymbolContextList;
friend class SBTarget;
friend class SBThread;
friend class SBThreadPlan;
friend class SBType;
friend class SBTypeEnumMember;
friend class SBTypeMemberFunction;
friend class SBTypeMember;
friend class SBValue;
friend class SBWatchpoint;

lldb_private::Stream *operator->();

lldb_private::Stream *get();

lldb_private::Stream &ref();

private:
SBStream(const SBStream &) = delete;
const SBStream &operator=(const SBStream &) = delete;
std::unique_ptr<lldb_private::Stream> m_opaque_up;
bool m_is_file = false;
};

}

#endif
