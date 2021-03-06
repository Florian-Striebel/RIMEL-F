












#if !defined(XRAY_XRAY_INTERFACE_H)
#define XRAY_XRAY_INTERFACE_H

#include <cstddef>
#include <cstdint>

extern "C" {


enum XRayEntryType {
ENTRY = 0,
EXIT = 1,
TAIL = 2,
LOG_ARGS_ENTRY = 3,
CUSTOM_EVENT = 4,
TYPED_EVENT = 5,
};





















extern int __xray_set_handler(void (*entry)(int32_t, XRayEntryType));



extern int __xray_remove_handler();






extern int __xray_set_handler_arg1(void (*entry)(int32_t, XRayEntryType,
uint64_t));



extern int __xray_remove_handler_arg1();


extern int __xray_set_customevent_handler(void (*entry)(void *, std::size_t));



extern int __xray_remove_customevent_handler();



extern int __xray_set_typedevent_handler(void (*entry)(uint16_t, const void *,
std::size_t));



extern int __xray_remove_typedevent_handler();

extern uint16_t __xray_register_event_type(const char *event_type);

enum XRayPatchingStatus {
NOT_INITIALIZED = 0,
SUCCESS = 1,
ONGOING = 2,
FAILED = 3,
};



extern XRayPatchingStatus __xray_patch();



extern XRayPatchingStatus __xray_unpatch();



extern XRayPatchingStatus __xray_patch_function(int32_t FuncId);



extern XRayPatchingStatus __xray_unpatch_function(int32_t FuncId);




extern uintptr_t __xray_function_address(int32_t FuncId);



extern size_t __xray_max_function_id();







extern void __xray_init();

}

#endif
