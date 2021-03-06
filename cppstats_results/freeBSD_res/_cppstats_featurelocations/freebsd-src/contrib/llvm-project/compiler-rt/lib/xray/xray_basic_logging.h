










#if !defined(XRAY_XRAY_INMEMORY_LOG_H)
#define XRAY_XRAY_INMEMORY_LOG_H

#include "xray/xray_log_interface.h"









namespace __xray {

XRayLogInitStatus basicLoggingInit(size_t BufferSize, size_t BufferMax,
void *Options, size_t OptionsSize);
XRayLogInitStatus basicLoggingFinalize();

void basicLoggingHandleArg0RealTSC(int32_t FuncId, XRayEntryType Entry);
void basicLoggingHandleArg0EmulateTSC(int32_t FuncId, XRayEntryType Entry);
void basicLoggingHandleArg1RealTSC(int32_t FuncId, XRayEntryType Entry,
uint64_t Arg1);
void basicLoggingHandleArg1EmulateTSC(int32_t FuncId, XRayEntryType Entry,
uint64_t Arg1);
XRayLogFlushStatus basicLoggingFlush();
XRayLogInitStatus basicLoggingReset();

}

#endif
