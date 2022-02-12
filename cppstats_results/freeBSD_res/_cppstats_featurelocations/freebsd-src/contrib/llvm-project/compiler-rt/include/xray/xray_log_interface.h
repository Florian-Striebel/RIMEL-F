


























































































#if !defined(XRAY_XRAY_LOG_INTERFACE_H)
#define XRAY_XRAY_LOG_INTERFACE_H

#include "xray/xray_interface.h"
#include <stddef.h>

extern "C" {



enum XRayLogInitStatus {


XRAY_LOG_UNINITIALIZED = 0,






XRAY_LOG_INITIALIZING = 1,





XRAY_LOG_INITIALIZED = 2,






XRAY_LOG_FINALIZING = 3,






XRAY_LOG_FINALIZED = 4,
};



enum XRayLogFlushStatus {
XRAY_LOG_NOT_FLUSHING = 0,
XRAY_LOG_FLUSHING = 1,
XRAY_LOG_FLUSHED = 2,
};




enum XRayLogRegisterStatus {
XRAY_REGISTRATION_OK = 0,
XRAY_DUPLICATE_MODE = 1,
XRAY_MODE_NOT_FOUND = 2,
XRAY_INCOMPLETE_IMPL = 3,
};




struct XRayLogImpl {















XRayLogInitStatus (*log_init)(size_t, size_t, void *, size_t);





XRayLogInitStatus (*log_finalize)();








void (*handle_arg0)(int32_t, XRayEntryType);






XRayLogFlushStatus (*flush_log)();
};




















void __xray_set_log_impl(XRayLogImpl Impl);














XRayLogRegisterStatus __xray_log_register_mode(const char *Mode,
XRayLogImpl Impl);











XRayLogRegisterStatus __xray_log_select_mode(const char *Mode);




const char *__xray_log_get_current_mode();
















void __xray_remove_log_impl();





XRayLogInitStatus __xray_log_init(size_t BufferSize, size_t MaxBuffers,
void *Args, size_t ArgsSize);
















XRayLogInitStatus __xray_log_init_mode(const char *Mode, const char *Config);





XRayLogInitStatus __xray_log_init_mode_bin(const char *Mode, const char *Config,
size_t ConfigSize);



XRayLogInitStatus __xray_log_finalize();



XRayLogFlushStatus __xray_log_flushLog();




struct XRayBuffer {
const void *Data;
size_t Size;
};








void __xray_log_set_buffer_iterator(XRayBuffer (*Iterator)(XRayBuffer));


void __xray_log_remove_buffer_iterator();


























XRayLogFlushStatus __xray_log_process_buffers(void (*Processor)(const char *,
XRayBuffer));

}

#endif
