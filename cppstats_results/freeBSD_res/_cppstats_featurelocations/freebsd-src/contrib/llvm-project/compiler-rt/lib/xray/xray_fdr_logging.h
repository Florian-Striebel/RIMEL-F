










#if !defined(XRAY_XRAY_FDR_LOGGING_H)
#define XRAY_XRAY_FDR_LOGGING_H

#include "xray/xray_log_interface.h"
#include "xray_fdr_log_records.h"










namespace __xray {
XRayLogInitStatus fdrLoggingInit(size_t BufferSize, size_t BufferMax,
void *Options, size_t OptionsSize);
XRayLogInitStatus fdrLoggingFinalize();
void fdrLoggingHandleArg0(int32_t FuncId, XRayEntryType Entry);
void fdrLoggingHandleArg1(int32_t FuncId, XRayEntryType Entry, uint64_t Arg1);
XRayLogFlushStatus fdrLoggingFlush();
XRayLogInitStatus fdrLoggingReset();

}

#endif
