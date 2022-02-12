















#if !defined(XRAY_XRAY_PROFILE_COLLECTOR_H)
#define XRAY_XRAY_PROFILE_COLLECTOR_H

#include "xray_function_call_trie.h"

#include "xray/xray_log_interface.h"

namespace __xray {





namespace profileCollectorService {








void post(BufferQueue *Q, FunctionCallTrie &&T,
FunctionCallTrie::Allocators &&A,
FunctionCallTrie::Allocators::Buffers &&B, tid_t TId);
















void serialize();




void reset();



XRayBuffer nextBuffer(XRayBuffer B);

}

}

#endif
