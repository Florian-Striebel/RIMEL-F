












#if !defined(XRAY_INTERFACE_INTERNAL_H)
#define XRAY_INTERFACE_INTERNAL_H

#include "sanitizer_common/sanitizer_platform.h"
#include "xray/xray_interface.h"
#include <cstddef>
#include <cstdint>

extern "C" {

struct XRaySledEntry {
#if SANITIZER_WORDSIZE == 64
uint64_t Address;
uint64_t Function;
unsigned char Kind;
unsigned char AlwaysInstrument;
unsigned char Version;
unsigned char Padding[13];
uint64_t function() const {
if (Version < 2)
return Function;

return reinterpret_cast<uint64_t>(&Function) + Function;
}
uint64_t address() const {
if (Version < 2)
return Address;

return reinterpret_cast<uint64_t>(&Address) + Address;
}
#elif SANITIZER_WORDSIZE == 32
uint32_t Address;
uint32_t Function;
unsigned char Kind;
unsigned char AlwaysInstrument;
unsigned char Version;
unsigned char Padding[5];
uint32_t function() const {
if (Version < 2)
return Function;

return reinterpret_cast<uint32_t>(&Function) + Function;
}
uint32_t address() const {
if (Version < 2)
return Address;

return reinterpret_cast<uint32_t>(&Address) + Address;
}
#else
#error "Unsupported word size."
#endif
};

struct XRayFunctionSledIndex {
const XRaySledEntry *Begin;
const XRaySledEntry *End;
};
}

namespace __xray {

struct XRaySledMap {
const XRaySledEntry *Sleds;
size_t Entries;
const XRayFunctionSledIndex *SledsIndex;
size_t Functions;
};

bool patchFunctionEntry(bool Enable, uint32_t FuncId, const XRaySledEntry &Sled,
void (*Trampoline)());
bool patchFunctionExit(bool Enable, uint32_t FuncId, const XRaySledEntry &Sled);
bool patchFunctionTailExit(bool Enable, uint32_t FuncId,
const XRaySledEntry &Sled);
bool patchCustomEvent(bool Enable, uint32_t FuncId, const XRaySledEntry &Sled);
bool patchTypedEvent(bool Enable, uint32_t FuncId, const XRaySledEntry &Sled);

}

extern "C" {


extern void __xray_FunctionEntry();
extern void __xray_FunctionExit();
extern void __xray_FunctionTailExit();
extern void __xray_ArgLoggerEntry();
extern void __xray_CustomEvent();
extern void __xray_TypedEvent();
}

#endif
