











#if !defined(SANITIZER_SYMBOLIZER_INTERNAL_H)
#define SANITIZER_SYMBOLIZER_INTERNAL_H

#include "sanitizer_symbolizer.h"
#include "sanitizer_file.h"
#include "sanitizer_vector.h"

namespace __sanitizer {





const char *ExtractToken(const char *str, const char *delims, char **result);
const char *ExtractInt(const char *str, const char *delims, int *result);
const char *ExtractUptr(const char *str, const char *delims, uptr *result);
const char *ExtractTokenUpToDelimiter(const char *str, const char *delimiter,
char **result);

const char *DemangleSwiftAndCXX(const char *name);




class SymbolizerTool {
public:



SymbolizerTool *next;

SymbolizerTool() : next(nullptr) { }







virtual bool SymbolizePC(uptr addr, SymbolizedStack *stack) {
UNIMPLEMENTED();
}



virtual bool SymbolizeData(uptr addr, DataInfo *info) {
UNIMPLEMENTED();
}

virtual bool SymbolizeFrame(uptr addr, FrameInfo *info) {
return false;
}

virtual void Flush() {}


virtual const char *Demangle(const char *name) {
return nullptr;
}




virtual void LateInitialize() {}

protected:
~SymbolizerTool() {}
};




class SymbolizerProcess {
public:
explicit SymbolizerProcess(const char *path, bool use_posix_spawn = false);
const char *SendCommand(const char *command);

protected:
~SymbolizerProcess() {}


static const unsigned kArgVMax = 6;


virtual bool StartSymbolizerSubprocess();
virtual bool ReadFromSymbolizer(char *buffer, uptr max_length);

virtual char **GetEnvP() { return GetEnviron(); }

private:
virtual bool ReachedEndOfOutput(const char *buffer, uptr length) const {
UNIMPLEMENTED();
}


virtual void GetArgV(const char *path_to_binary,
const char *(&argv)[kArgVMax]) const {
UNIMPLEMENTED();
}

bool Restart();
const char *SendCommandImpl(const char *command);
bool WriteToSymbolizer(const char *buffer, uptr length);

const char *path_;
fd_t input_fd_;
fd_t output_fd_;

static const uptr kBufferSize = 16 * 1024;
char buffer_[kBufferSize];

static const uptr kMaxTimesRestarted = 5;
static const int kSymbolizerStartupTimeMillis = 10;
uptr times_restarted_;
bool failed_to_start_;
bool reported_invalid_path_;
bool use_posix_spawn_;
};

class LLVMSymbolizerProcess;



class LLVMSymbolizer final : public SymbolizerTool {
public:
explicit LLVMSymbolizer(const char *path, LowLevelAllocator *allocator);

bool SymbolizePC(uptr addr, SymbolizedStack *stack) override;
bool SymbolizeData(uptr addr, DataInfo *info) override;
bool SymbolizeFrame(uptr addr, FrameInfo *info) override;

private:
const char *FormatAndSendCommand(const char *command_prefix,
const char *module_name, uptr module_offset,
ModuleArch arch);

LLVMSymbolizerProcess *symbolizer_process_;
static const uptr kBufferSize = 16 * 1024;
char buffer_[kBufferSize];
};







void ParseSymbolizePCOutput(const char *str, SymbolizedStack *res);





void ParseSymbolizeDataOutput(const char *str, DataInfo *info);

}

#endif
