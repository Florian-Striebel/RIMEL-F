












#if defined(__APPLE__)

#include <TargetConditionals.h>
#include <dispatch/dispatch.h>
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int32_t GlobalMajor, GlobalMinor, GlobalSubminor;
static dispatch_once_t DispatchOnceCounter;
static dispatch_once_t CompatibilityDispatchOnceCounter;


typedef uint32_t dyld_platform_t;

typedef struct {
dyld_platform_t platform;
uint32_t version;
} dyld_build_version_t;

typedef bool (*AvailabilityVersionCheckFuncTy)(uint32_t count,
dyld_build_version_t versions[]);

static AvailabilityVersionCheckFuncTy AvailabilityVersionCheck;




typedef const void *CFDataRef, *CFAllocatorRef, *CFPropertyListRef,
*CFStringRef, *CFDictionaryRef, *CFTypeRef, *CFErrorRef;

#if __LLP64__
typedef unsigned long long CFTypeID;
typedef unsigned long long CFOptionFlags;
typedef signed long long CFIndex;
#else
typedef unsigned long CFTypeID;
typedef unsigned long CFOptionFlags;
typedef signed long CFIndex;
#endif

typedef unsigned char UInt8;
typedef _Bool Boolean;
typedef CFIndex CFPropertyListFormat;
typedef uint32_t CFStringEncoding;


#define CF_STRING_ENCODING_ASCII 0x0600

#define CF_STRING_ENCODING_UTF8 0x08000100
#define CF_PROPERTY_LIST_IMMUTABLE 0

typedef CFDataRef (*CFDataCreateWithBytesNoCopyFuncTy)(CFAllocatorRef,
const UInt8 *, CFIndex,
CFAllocatorRef);
typedef CFPropertyListRef (*CFPropertyListCreateWithDataFuncTy)(
CFAllocatorRef, CFDataRef, CFOptionFlags, CFPropertyListFormat *,
CFErrorRef *);
typedef CFPropertyListRef (*CFPropertyListCreateFromXMLDataFuncTy)(
CFAllocatorRef, CFDataRef, CFOptionFlags, CFStringRef *);
typedef CFStringRef (*CFStringCreateWithCStringNoCopyFuncTy)(CFAllocatorRef,
const char *,
CFStringEncoding,
CFAllocatorRef);
typedef const void *(*CFDictionaryGetValueFuncTy)(CFDictionaryRef,
const void *);
typedef CFTypeID (*CFGetTypeIDFuncTy)(CFTypeRef);
typedef CFTypeID (*CFStringGetTypeIDFuncTy)(void);
typedef Boolean (*CFStringGetCStringFuncTy)(CFStringRef, char *, CFIndex,
CFStringEncoding);
typedef void (*CFReleaseFuncTy)(CFTypeRef);

static void _initializeAvailabilityCheck(bool LoadPlist) {
if (AvailabilityVersionCheck && !LoadPlist) {


return;
}


AvailabilityVersionCheck = (AvailabilityVersionCheckFuncTy)dlsym(
RTLD_DEFAULT, "_availability_version_check");

if (AvailabilityVersionCheck && !LoadPlist) {


return;
}




const void *NullAllocator = dlsym(RTLD_DEFAULT, "kCFAllocatorNull");
if (!NullAllocator)
return;
const CFAllocatorRef AllocatorNull = *(const CFAllocatorRef *)NullAllocator;
CFDataCreateWithBytesNoCopyFuncTy CFDataCreateWithBytesNoCopyFunc =
(CFDataCreateWithBytesNoCopyFuncTy)dlsym(RTLD_DEFAULT,
"CFDataCreateWithBytesNoCopy");
if (!CFDataCreateWithBytesNoCopyFunc)
return;
CFPropertyListCreateWithDataFuncTy CFPropertyListCreateWithDataFunc =
(CFPropertyListCreateWithDataFuncTy)dlsym(RTLD_DEFAULT,
"CFPropertyListCreateWithData");


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
CFPropertyListCreateFromXMLDataFuncTy CFPropertyListCreateFromXMLDataFunc =
(CFPropertyListCreateFromXMLDataFuncTy)dlsym(
RTLD_DEFAULT, "CFPropertyListCreateFromXMLData");
#pragma clang diagnostic pop


if (!CFPropertyListCreateWithDataFunc && !CFPropertyListCreateFromXMLDataFunc)
return;
CFStringCreateWithCStringNoCopyFuncTy CFStringCreateWithCStringNoCopyFunc =
(CFStringCreateWithCStringNoCopyFuncTy)dlsym(
RTLD_DEFAULT, "CFStringCreateWithCStringNoCopy");
if (!CFStringCreateWithCStringNoCopyFunc)
return;
CFDictionaryGetValueFuncTy CFDictionaryGetValueFunc =
(CFDictionaryGetValueFuncTy)dlsym(RTLD_DEFAULT, "CFDictionaryGetValue");
if (!CFDictionaryGetValueFunc)
return;
CFGetTypeIDFuncTy CFGetTypeIDFunc =
(CFGetTypeIDFuncTy)dlsym(RTLD_DEFAULT, "CFGetTypeID");
if (!CFGetTypeIDFunc)
return;
CFStringGetTypeIDFuncTy CFStringGetTypeIDFunc =
(CFStringGetTypeIDFuncTy)dlsym(RTLD_DEFAULT, "CFStringGetTypeID");
if (!CFStringGetTypeIDFunc)
return;
CFStringGetCStringFuncTy CFStringGetCStringFunc =
(CFStringGetCStringFuncTy)dlsym(RTLD_DEFAULT, "CFStringGetCString");
if (!CFStringGetCStringFunc)
return;
CFReleaseFuncTy CFReleaseFunc =
(CFReleaseFuncTy)dlsym(RTLD_DEFAULT, "CFRelease");
if (!CFReleaseFunc)
return;

char *PListPath = "/System/Library/CoreServices/SystemVersion.plist";

#if TARGET_OS_SIMULATOR
char *PListPathPrefix = getenv("IPHONE_SIMULATOR_ROOT");
if (!PListPathPrefix)
return;
char FullPath[strlen(PListPathPrefix) + strlen(PListPath) + 1];
strcpy(FullPath, PListPathPrefix);
strcat(FullPath, PListPath);
PListPath = FullPath;
#endif
FILE *PropertyList = fopen(PListPath, "r");
if (!PropertyList)
return;


CFDictionaryRef PListRef = NULL;
CFDataRef FileContentsRef = NULL;
UInt8 *PListBuf = NULL;

fseek(PropertyList, 0, SEEK_END);
long PListFileSize = ftell(PropertyList);
if (PListFileSize < 0)
goto Fail;
rewind(PropertyList);

PListBuf = malloc((size_t)PListFileSize);
if (!PListBuf)
goto Fail;

size_t NumRead = fread(PListBuf, 1, (size_t)PListFileSize, PropertyList);
if (NumRead != (size_t)PListFileSize)
goto Fail;



FileContentsRef = (*CFDataCreateWithBytesNoCopyFunc)(
NULL, PListBuf, (CFIndex)NumRead, AllocatorNull);
if (!FileContentsRef)
goto Fail;

if (CFPropertyListCreateWithDataFunc)
PListRef = (*CFPropertyListCreateWithDataFunc)(
NULL, FileContentsRef, CF_PROPERTY_LIST_IMMUTABLE, NULL, NULL);
else
PListRef = (*CFPropertyListCreateFromXMLDataFunc)(
NULL, FileContentsRef, CF_PROPERTY_LIST_IMMUTABLE, NULL);
if (!PListRef)
goto Fail;

CFStringRef ProductVersion = (*CFStringCreateWithCStringNoCopyFunc)(
NULL, "ProductVersion", CF_STRING_ENCODING_ASCII, AllocatorNull);
if (!ProductVersion)
goto Fail;
CFTypeRef OpaqueValue = (*CFDictionaryGetValueFunc)(PListRef, ProductVersion);
(*CFReleaseFunc)(ProductVersion);
if (!OpaqueValue ||
(*CFGetTypeIDFunc)(OpaqueValue) != (*CFStringGetTypeIDFunc)())
goto Fail;

char VersionStr[32];
if (!(*CFStringGetCStringFunc)((CFStringRef)OpaqueValue, VersionStr,
sizeof(VersionStr), CF_STRING_ENCODING_UTF8))
goto Fail;
sscanf(VersionStr, "%d.%d.%d", &GlobalMajor, &GlobalMinor, &GlobalSubminor);

Fail:
if (PListRef)
(*CFReleaseFunc)(PListRef);
if (FileContentsRef)
(*CFReleaseFunc)(FileContentsRef);
free(PListBuf);
fclose(PropertyList);
}


static void compatibilityInitializeAvailabilityCheck(void *Unused) {
(void)Unused;
_initializeAvailabilityCheck(true);
}

static void initializeAvailabilityCheck(void *Unused) {
(void)Unused;
_initializeAvailabilityCheck(false);
}




int32_t __isOSVersionAtLeast(int32_t Major, int32_t Minor, int32_t Subminor) {

dispatch_once_f(&CompatibilityDispatchOnceCounter, NULL,
compatibilityInitializeAvailabilityCheck);

if (Major < GlobalMajor)
return 1;
if (Major > GlobalMajor)
return 0;
if (Minor < GlobalMinor)
return 1;
if (Minor > GlobalMinor)
return 0;
return Subminor <= GlobalSubminor;
}

static inline uint32_t ConstructVersion(uint32_t Major, uint32_t Minor,
uint32_t Subminor) {
return ((Major & 0xffff) << 16) | ((Minor & 0xff) << 8) | (Subminor & 0xff);
}

int32_t __isPlatformVersionAtLeast(uint32_t Platform, uint32_t Major,
uint32_t Minor, uint32_t Subminor) {
dispatch_once_f(&DispatchOnceCounter, NULL, initializeAvailabilityCheck);

if (!AvailabilityVersionCheck) {
return __isOSVersionAtLeast(Major, Minor, Subminor);
}
dyld_build_version_t Versions[] = {
{Platform, ConstructVersion(Major, Minor, Subminor)}};
return AvailabilityVersionCheck(1, Versions);
}

#elif __ANDROID__

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/system_properties.h>

static int SdkVersion;
static int IsPreRelease;

static void readSystemProperties(void) {
char buf[PROP_VALUE_MAX];

if (__system_property_get("ro.build.version.sdk", buf) == 0) {

SdkVersion = __ANDROID_API_FUTURE__;
} else {
SdkVersion = atoi(buf);
}

if (__system_property_get("ro.build.version.codename", buf) == 0) {
IsPreRelease = 1;
} else {
IsPreRelease = strcmp(buf, "REL") != 0;
}
return;
}

int32_t __isOSVersionAtLeast(int32_t Major, int32_t Minor, int32_t Subminor) {
(int32_t) Minor;
(int32_t) Subminor;
static pthread_once_t once = PTHREAD_ONCE_INIT;
pthread_once(&once, readSystemProperties);

return SdkVersion >= Major ||
(IsPreRelease && Major == __ANDROID_API_FUTURE__);
}

#else


typedef int unused;

#endif
