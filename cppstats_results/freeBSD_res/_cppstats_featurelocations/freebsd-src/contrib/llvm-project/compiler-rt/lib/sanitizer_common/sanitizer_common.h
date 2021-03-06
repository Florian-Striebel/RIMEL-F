













#if !defined(SANITIZER_COMMON_H)
#define SANITIZER_COMMON_H

#include "sanitizer_flags.h"
#include "sanitizer_interface_internal.h"
#include "sanitizer_internal_defs.h"
#include "sanitizer_libc.h"
#include "sanitizer_list.h"
#include "sanitizer_mutex.h"

#if defined(_MSC_VER) && !defined(__clang__)
extern "C" void _ReadWriteBarrier();
#pragma intrinsic(_ReadWriteBarrier)
#endif

namespace __sanitizer {

struct AddressInfo;
struct BufferedStackTrace;
struct SignalContext;
struct StackTrace;


const uptr kWordSize = SANITIZER_WORDSIZE / 8;
const uptr kWordSizeInBits = 8 * kWordSize;

const uptr kCacheLineSize = SANITIZER_CACHE_LINE_SIZE;

const uptr kMaxPathLength = 4096;

const uptr kMaxThreadStackSize = 1 << 30;

const uptr kErrorMessageBufferSize = 1 << 16;



const u64 kExternalPCBit = 1ULL << 60;

extern const char *SanitizerToolName;

extern atomic_uint32_t current_verbosity;
inline void SetVerbosity(int verbosity) {
atomic_store(&current_verbosity, verbosity, memory_order_relaxed);
}
inline int Verbosity() {
return atomic_load(&current_verbosity, memory_order_relaxed);
}

#if SANITIZER_ANDROID
inline uptr GetPageSize() {

return 4096;
}
inline uptr GetPageSizeCached() {
return 4096;
}
#else
uptr GetPageSize();
extern uptr PageSizeCached;
inline uptr GetPageSizeCached() {
if (!PageSizeCached)
PageSizeCached = GetPageSize();
return PageSizeCached;
}
#endif
uptr GetMmapGranularity();
uptr GetMaxVirtualAddress();
uptr GetMaxUserVirtualAddress();

tid_t GetTid();
int TgKill(pid_t pid, tid_t tid, int sig);
uptr GetThreadSelf();
void GetThreadStackTopAndBottom(bool at_initialization, uptr *stack_top,
uptr *stack_bottom);
void GetThreadStackAndTls(bool main, uptr *stk_addr, uptr *stk_size,
uptr *tls_addr, uptr *tls_size);


void *MmapOrDie(uptr size, const char *mem_type, bool raw_report = false);
inline void *MmapOrDieQuietly(uptr size, const char *mem_type) {
return MmapOrDie(size, mem_type, true);
}
void UnmapOrDie(void *addr, uptr size);


void *MmapOrDieOnFatalError(uptr size, const char *mem_type);
bool MmapFixedNoReserve(uptr fixed_addr, uptr size, const char *name = nullptr)
WARN_UNUSED_RESULT;
bool MmapFixedSuperNoReserve(uptr fixed_addr, uptr size,
const char *name = nullptr) WARN_UNUSED_RESULT;
void *MmapNoReserveOrDie(uptr size, const char *mem_type);
void *MmapFixedOrDie(uptr fixed_addr, uptr size, const char *name = nullptr);


void *MmapFixedOrDieOnFatalError(uptr fixed_addr, uptr size,
const char *name = nullptr);
void *MmapFixedNoAccess(uptr fixed_addr, uptr size, const char *name = nullptr);
void *MmapNoAccess(uptr size);


void *MmapAlignedOrDieOnFatalError(uptr size, uptr alignment,
const char *mem_type);


bool MprotectNoAccess(uptr addr, uptr size);
bool MprotectReadOnly(uptr addr, uptr size);

void MprotectMallocZones(void *addr, int prot);

#if SANITIZER_LINUX

void UnmapFromTo(uptr from, uptr to);
#endif







uptr MapDynamicShadow(uptr shadow_size_bytes, uptr shadow_scale,
uptr min_shadow_base_alignment, uptr &high_mem_end);







uptr MapDynamicShadowAndAliases(uptr shadow_size, uptr alias_size,
uptr num_aliases, uptr ring_buffer_size);



void ReserveShadowMemoryRange(uptr beg, uptr end, const char *name,
bool madvise_shadow = true);




void ProtectGap(uptr addr, uptr size, uptr zero_base_shadow_start,
uptr zero_base_max_shadow_start);


uptr FindAvailableMemoryRange(uptr size, uptr alignment, uptr left_padding,
uptr *largest_gap_found, uptr *max_occupied_addr);


bool MemoryRangeIsAvailable(uptr range_start, uptr range_end);


void ReleaseMemoryPagesToOS(uptr beg, uptr end);
void IncreaseTotalMmap(uptr size);
void DecreaseTotalMmap(uptr size);
uptr GetRSS();
void SetShadowRegionHugePageMode(uptr addr, uptr length);
bool DontDumpShadowMemory(uptr addr, uptr length);

void CheckVMASize();
void RunMallocHooks(const void *ptr, uptr size);
void RunFreeHooks(const void *ptr);

class ReservedAddressRange {
public:
uptr Init(uptr size, const char *name = nullptr, uptr fixed_addr = 0);
uptr InitAligned(uptr size, uptr align, const char *name = nullptr);
uptr Map(uptr fixed_addr, uptr size, const char *name = nullptr);
uptr MapOrDie(uptr fixed_addr, uptr size, const char *name = nullptr);
void Unmap(uptr addr, uptr size);
void *base() const { return base_; }
uptr size() const { return size_; }

private:
void* base_;
uptr size_;
const char* name_;
uptr os_handle_;
};

typedef void (*fill_profile_f)(uptr start, uptr rss, bool file,
uptr *stats, uptr stats_size);




void GetMemoryProfile(fill_profile_f cb, uptr *stats, uptr stats_size);




class LowLevelAllocator {
public:

void *Allocate(uptr size);
private:
char *allocated_end_;
char *allocated_current_;
};

void SetLowLevelAllocateMinAlignment(uptr alignment);
typedef void (*LowLevelAllocateCallback)(uptr ptr, uptr size);


void SetLowLevelAllocateCallback(LowLevelAllocateCallback callback);


void CatastrophicErrorWrite(const char *buffer, uptr length);
void RawWrite(const char *buffer);
bool ColorizeReports();
void RemoveANSIEscapeSequencesFromString(char *buffer);
void Printf(const char *format, ...);
void Report(const char *format, ...);
void SetPrintfAndReportCallback(void (*callback)(const char *));
#define VReport(level, ...) do { if ((uptr)Verbosity() >= (level)) Report(__VA_ARGS__); } while (0)



#define VPrintf(level, ...) do { if ((uptr)Verbosity() >= (level)) Printf(__VA_ARGS__); } while (0)





class ScopedErrorReportLock {
public:
ScopedErrorReportLock() ACQUIRE(mutex_) { Lock(); }
~ScopedErrorReportLock() RELEASE(mutex_) { Unlock(); }

static void Lock() ACQUIRE(mutex_);
static void Unlock() RELEASE(mutex_);
static void CheckLocked() CHECK_LOCKED(mutex_);

private:
static atomic_uintptr_t reporting_thread_;
static StaticSpinMutex mutex_;
};

extern uptr stoptheworld_tracer_pid;
extern uptr stoptheworld_tracer_ppid;

bool IsAccessibleMemoryRange(uptr beg, uptr size);


const char *StripPathPrefix(const char *filepath,
const char *strip_file_prefix);

const char *StripModuleName(const char *module);


uptr ReadBinaryName(char *buf, uptr buf_len);
uptr ReadBinaryNameCached(char *buf, uptr buf_len);
uptr ReadBinaryDir( char *buf, uptr buf_len);
uptr ReadLongProcessName( char *buf, uptr buf_len);
const char *GetProcessName();
void UpdateProcessName();
void CacheBinaryName();
void DisableCoreDumperIfNecessary();
void DumpProcessMap();
const char *GetEnv(const char *name);
bool SetEnv(const char *name, const char *value);

u32 GetUid();
void ReExec();
void CheckASLR();
void CheckMPROTECT();
char **GetArgv();
char **GetEnviron();
void PrintCmdline();
bool StackSizeIsUnlimited();
void SetStackSizeLimitInBytes(uptr limit);
bool AddressSpaceIsUnlimited();
void SetAddressSpaceUnlimited();
void AdjustStackSize(void *attr);
void PlatformPrepareForSandboxing(__sanitizer_sandbox_arguments *args);
void SetSandboxingCallback(void (*f)());

void InitializeCoverage(bool enabled, const char *coverage_dir);

void InitTlsSize();
uptr GetTlsSize();


void SleepForSeconds(unsigned seconds);
void SleepForMillis(unsigned millis);
u64 NanoTime();
u64 MonotonicNanoTime();
int Atexit(void (*function)(void));
bool TemplateMatch(const char *templ, const char *str);


void NORETURN Abort();
void NORETURN Die();
void NORETURN
CheckFailed(const char *file, int line, const char *cond, u64 v1, u64 v2);
void NORETURN ReportMmapFailureAndDie(uptr size, const char *mem_type,
const char *mmap_type, error_t err,
bool raw_report = false);



typedef void (*DieCallbackType)(void);





bool AddDieCallback(DieCallbackType callback);
bool RemoveDieCallback(DieCallbackType callback);

void SetUserDieCallback(DieCallbackType callback);

void SetCheckUnwindCallback(void (*callback)());





void SetSoftRssLimitExceededCallback(void (*Callback)(bool exceeded));


typedef void (*SignalHandlerType)(int, void *, void *);
HandleSignalMode GetHandleSignalMode(int signum);
void InstallDeadlySignalHandlers(SignalHandlerType handler);



typedef void (*UnwindSignalStackCallbackType)(const SignalContext &sig,
const void *callback_context,
BufferedStackTrace *stack);

void HandleDeadlySignal(void *siginfo, void *context, u32 tid,
UnwindSignalStackCallbackType unwind,
const void *unwind_context);


void StartReportDeadlySignal();

void ReportDeadlySignal(const SignalContext &sig, u32 tid,
UnwindSignalStackCallbackType unwind,
const void *unwind_context);


void SetAlternateSignalStack();
void UnsetAlternateSignalStack();





void ReportErrorSummary(const char *error_message,
const char *alt_tool_name = nullptr);


void ReportErrorSummary(const char *error_type, const AddressInfo &info,
const char *alt_tool_name = nullptr);

void ReportErrorSummary(const char *error_type, const StackTrace *trace,
const char *alt_tool_name = nullptr);

void ReportMmapWriteExec(int prot);


#if SANITIZER_WINDOWS && !defined(__clang__) && !defined(__GNUC__)
extern "C" {
unsigned char _BitScanForward(unsigned long *index, unsigned long mask);
unsigned char _BitScanReverse(unsigned long *index, unsigned long mask);
#if defined(_WIN64)
unsigned char _BitScanForward64(unsigned long *index, unsigned __int64 mask);
unsigned char _BitScanReverse64(unsigned long *index, unsigned __int64 mask);
#endif
}
#endif

inline uptr MostSignificantSetBitIndex(uptr x) {
CHECK_NE(x, 0U);
unsigned long up;
#if !SANITIZER_WINDOWS || defined(__clang__) || defined(__GNUC__)
#if defined(_WIN64)
up = SANITIZER_WORDSIZE - 1 - __builtin_clzll(x);
#else
up = SANITIZER_WORDSIZE - 1 - __builtin_clzl(x);
#endif
#elif defined(_WIN64)
_BitScanReverse64(&up, x);
#else
_BitScanReverse(&up, x);
#endif
return up;
}

inline uptr LeastSignificantSetBitIndex(uptr x) {
CHECK_NE(x, 0U);
unsigned long up;
#if !SANITIZER_WINDOWS || defined(__clang__) || defined(__GNUC__)
#if defined(_WIN64)
up = __builtin_ctzll(x);
#else
up = __builtin_ctzl(x);
#endif
#elif defined(_WIN64)
_BitScanForward64(&up, x);
#else
_BitScanForward(&up, x);
#endif
return up;
}

inline bool IsPowerOfTwo(uptr x) {
return (x & (x - 1)) == 0;
}

inline uptr RoundUpToPowerOfTwo(uptr size) {
CHECK(size);
if (IsPowerOfTwo(size)) return size;

uptr up = MostSignificantSetBitIndex(size);
CHECK_LT(size, (1ULL << (up + 1)));
CHECK_GT(size, (1ULL << up));
return 1ULL << (up + 1);
}

inline uptr RoundUpTo(uptr size, uptr boundary) {
RAW_CHECK(IsPowerOfTwo(boundary));
return (size + boundary - 1) & ~(boundary - 1);
}

inline uptr RoundDownTo(uptr x, uptr boundary) {
return x & ~(boundary - 1);
}

inline bool IsAligned(uptr a, uptr alignment) {
return (a & (alignment - 1)) == 0;
}

inline uptr Log2(uptr x) {
CHECK(IsPowerOfTwo(x));
return LeastSignificantSetBitIndex(x);
}



template <class T>
constexpr T Min(T a, T b) {
return a < b ? a : b;
}
template <class T>
constexpr T Max(T a, T b) {
return a > b ? a : b;
}
template<class T> void Swap(T& a, T& b) {
T tmp = a;
a = b;
b = tmp;
}


inline bool IsSpace(int c) {
return (c == ' ') || (c == '\n') || (c == '\t') ||
(c == '\f') || (c == '\r') || (c == '\v');
}
inline bool IsDigit(int c) {
return (c >= '0') && (c <= '9');
}
inline int ToLower(int c) {
return (c >= 'A' && c <= 'Z') ? (c + 'a' - 'A') : c;
}




template<typename T>
class InternalMmapVectorNoCtor {
public:
using value_type = T;
void Initialize(uptr initial_capacity) {
capacity_bytes_ = 0;
size_ = 0;
data_ = 0;
reserve(initial_capacity);
}
void Destroy() { UnmapOrDie(data_, capacity_bytes_); }
T &operator[](uptr i) {
CHECK_LT(i, size_);
return data_[i];
}
const T &operator[](uptr i) const {
CHECK_LT(i, size_);
return data_[i];
}
void push_back(const T &element) {
CHECK_LE(size_, capacity());
if (size_ == capacity()) {
uptr new_capacity = RoundUpToPowerOfTwo(size_ + 1);
Realloc(new_capacity);
}
internal_memcpy(&data_[size_++], &element, sizeof(T));
}
T &back() {
CHECK_GT(size_, 0);
return data_[size_ - 1];
}
void pop_back() {
CHECK_GT(size_, 0);
size_--;
}
uptr size() const {
return size_;
}
const T *data() const {
return data_;
}
T *data() {
return data_;
}
uptr capacity() const { return capacity_bytes_ / sizeof(T); }
void reserve(uptr new_size) {

if (new_size > capacity())
Realloc(new_size);
}
void resize(uptr new_size) {
if (new_size > size_) {
reserve(new_size);
internal_memset(&data_[size_], 0, sizeof(T) * (new_size - size_));
}
size_ = new_size;
}

void clear() { size_ = 0; }
bool empty() const { return size() == 0; }

const T *begin() const {
return data();
}
T *begin() {
return data();
}
const T *end() const {
return data() + size();
}
T *end() {
return data() + size();
}

void swap(InternalMmapVectorNoCtor &other) {
Swap(data_, other.data_);
Swap(capacity_bytes_, other.capacity_bytes_);
Swap(size_, other.size_);
}

private:
void Realloc(uptr new_capacity) {
CHECK_GT(new_capacity, 0);
CHECK_LE(size_, new_capacity);
uptr new_capacity_bytes =
RoundUpTo(new_capacity * sizeof(T), GetPageSizeCached());
T *new_data = (T *)MmapOrDie(new_capacity_bytes, "InternalMmapVector");
internal_memcpy(new_data, data_, size_ * sizeof(T));
UnmapOrDie(data_, capacity_bytes_);
data_ = new_data;
capacity_bytes_ = new_capacity_bytes;
}

T *data_;
uptr capacity_bytes_;
uptr size_;
};

template <typename T>
bool operator==(const InternalMmapVectorNoCtor<T> &lhs,
const InternalMmapVectorNoCtor<T> &rhs) {
if (lhs.size() != rhs.size()) return false;
return internal_memcmp(lhs.data(), rhs.data(), lhs.size() * sizeof(T)) == 0;
}

template <typename T>
bool operator!=(const InternalMmapVectorNoCtor<T> &lhs,
const InternalMmapVectorNoCtor<T> &rhs) {
return !(lhs == rhs);
}

template<typename T>
class InternalMmapVector : public InternalMmapVectorNoCtor<T> {
public:
InternalMmapVector() { InternalMmapVectorNoCtor<T>::Initialize(0); }
explicit InternalMmapVector(uptr cnt) {
InternalMmapVectorNoCtor<T>::Initialize(cnt);
this->resize(cnt);
}
~InternalMmapVector() { InternalMmapVectorNoCtor<T>::Destroy(); }

InternalMmapVector(const InternalMmapVector &) = delete;
InternalMmapVector &operator=(const InternalMmapVector &) = delete;
InternalMmapVector(InternalMmapVector &&) = delete;
InternalMmapVector &operator=(InternalMmapVector &&) = delete;
};

class InternalScopedString {
public:
InternalScopedString() : buffer_(1) { buffer_[0] = '\0'; }

uptr length() const { return buffer_.size() - 1; }
void clear() {
buffer_.resize(1);
buffer_[0] = '\0';
}
void append(const char *format, ...);
const char *data() const { return buffer_.data(); }
char *data() { return buffer_.data(); }

private:
InternalMmapVector<char> buffer_;
};

template <class T>
struct CompareLess {
bool operator()(const T &a, const T &b) const { return a < b; }
};


template <class T, class Compare = CompareLess<T>>
void Sort(T *v, uptr size, Compare comp = {}) {
if (size < 2)
return;

for (uptr i = 1; i < size; i++) {
uptr j, p;
for (j = i; j > 0; j = p) {
p = (j - 1) / 2;
if (comp(v[p], v[j]))
Swap(v[j], v[p]);
else
break;
}
}


for (uptr i = size - 1; i > 0; i--) {
Swap(v[0], v[i]);
uptr j, max_ind;
for (j = 0; j < i; j = max_ind) {
uptr left = 2 * j + 1;
uptr right = 2 * j + 2;
max_ind = j;
if (left < i && comp(v[max_ind], v[left]))
max_ind = left;
if (right < i && comp(v[max_ind], v[right]))
max_ind = right;
if (max_ind != j)
Swap(v[j], v[max_ind]);
else
break;
}
}
}



template <class Container,
class Compare = CompareLess<typename Container::value_type>>
uptr InternalLowerBound(const Container &v,
const typename Container::value_type &val,
Compare comp = {}) {
uptr first = 0;
uptr last = v.size();
while (last > first) {
uptr mid = (first + last) / 2;
if (comp(v[mid], val))
first = mid + 1;
else
last = mid;
}
return first;
}

enum ModuleArch {
kModuleArchUnknown,
kModuleArchI386,
kModuleArchX86_64,
kModuleArchX86_64H,
kModuleArchARMV6,
kModuleArchARMV7,
kModuleArchARMV7S,
kModuleArchARMV7K,
kModuleArchARM64,
kModuleArchRISCV64
};


template <class Container,
class Compare = CompareLess<typename Container::value_type>>
void SortAndDedup(Container &v, Compare comp = {}) {
Sort(v.data(), v.size(), comp);
uptr size = v.size();
if (size < 2)
return;
uptr last = 0;
for (uptr i = 1; i < size; ++i) {
if (comp(v[last], v[i])) {
++last;
if (last != i)
v[last] = v[i];
} else {
CHECK(!comp(v[i], v[last]));
}
}
v.resize(last + 1);
}




bool ReadFileToVector(const char *file_name,
InternalMmapVectorNoCtor<char> *buff,
uptr max_len = 1 << 26, error_t *errno_p = nullptr);









bool ReadFileToBuffer(const char *file_name, char **buff, uptr *buff_size,
uptr *read_len, uptr max_len = 1 << 26,
error_t *errno_p = nullptr);



inline const char *ModuleArchToString(ModuleArch arch) {
switch (arch) {
case kModuleArchUnknown:
return "";
case kModuleArchI386:
return "i386";
case kModuleArchX86_64:
return "x86_64";
case kModuleArchX86_64H:
return "x86_64h";
case kModuleArchARMV6:
return "armv6";
case kModuleArchARMV7:
return "armv7";
case kModuleArchARMV7S:
return "armv7s";
case kModuleArchARMV7K:
return "armv7k";
case kModuleArchARM64:
return "arm64";
case kModuleArchRISCV64:
return "riscv64";
}
CHECK(0 && "Invalid module arch");
return "";
}

const uptr kModuleUUIDSize = 16;
const uptr kMaxSegName = 16;



class LoadedModule {
public:
LoadedModule()
: full_name_(nullptr),
base_address_(0),
max_executable_address_(0),
arch_(kModuleArchUnknown),
instrumented_(false) {
internal_memset(uuid_, 0, kModuleUUIDSize);
ranges_.clear();
}
void set(const char *module_name, uptr base_address);
void set(const char *module_name, uptr base_address, ModuleArch arch,
u8 uuid[kModuleUUIDSize], bool instrumented);
void clear();
void addAddressRange(uptr beg, uptr end, bool executable, bool writable,
const char *name = nullptr);
bool containsAddress(uptr address) const;

const char *full_name() const { return full_name_; }
uptr base_address() const { return base_address_; }
uptr max_executable_address() const { return max_executable_address_; }
ModuleArch arch() const { return arch_; }
const u8 *uuid() const { return uuid_; }
bool instrumented() const { return instrumented_; }

struct AddressRange {
AddressRange *next;
uptr beg;
uptr end;
bool executable;
bool writable;
char name[kMaxSegName];

AddressRange(uptr beg, uptr end, bool executable, bool writable,
const char *name)
: next(nullptr),
beg(beg),
end(end),
executable(executable),
writable(writable) {
internal_strncpy(this->name, (name ? name : ""), ARRAY_SIZE(this->name));
}
};

const IntrusiveList<AddressRange> &ranges() const { return ranges_; }

private:
char *full_name_;
uptr base_address_;
uptr max_executable_address_;
ModuleArch arch_;
u8 uuid_[kModuleUUIDSize];
bool instrumented_;
IntrusiveList<AddressRange> ranges_;
};



class ListOfModules {
public:
ListOfModules() : initialized(false) {}
~ListOfModules() { clear(); }
void init();
void fallbackInit();
const LoadedModule *begin() const { return modules_.begin(); }
LoadedModule *begin() { return modules_.begin(); }
const LoadedModule *end() const { return modules_.end(); }
LoadedModule *end() { return modules_.end(); }
uptr size() const { return modules_.size(); }
const LoadedModule &operator[](uptr i) const {
CHECK_LT(i, modules_.size());
return modules_[i];
}

private:
void clear() {
for (auto &module : modules_) module.clear();
modules_.clear();
}
void clearOrInit() {
initialized ? clear() : modules_.Initialize(kInitialCapacity);
initialized = true;
}

InternalMmapVectorNoCtor<LoadedModule> modules_;

static const uptr kInitialCapacity = 1 << 14;
bool initialized;
};


typedef void (*RangeIteratorCallback)(uptr begin, uptr end, void *arg);

enum AndroidApiLevel {
ANDROID_NOT_ANDROID = 0,
ANDROID_KITKAT = 19,
ANDROID_LOLLIPOP_MR1 = 22,
ANDROID_POST_LOLLIPOP = 23
};

void WriteToSyslog(const char *buffer);

#if defined(SANITIZER_WINDOWS) && defined(_MSC_VER) && !defined(__clang__)
#define SANITIZER_WIN_TRACE 1
#else
#define SANITIZER_WIN_TRACE 0
#endif

#if SANITIZER_MAC || SANITIZER_WIN_TRACE
void LogFullErrorReport(const char *buffer);
#else
inline void LogFullErrorReport(const char *buffer) {}
#endif

#if SANITIZER_LINUX || SANITIZER_MAC
void WriteOneLineToSyslog(const char *s);
void LogMessageOnPrintf(const char *str);
#else
inline void WriteOneLineToSyslog(const char *s) {}
inline void LogMessageOnPrintf(const char *str) {}
#endif

#if SANITIZER_LINUX || SANITIZER_WIN_TRACE

void AndroidLogInit();
void SetAbortMessage(const char *);
#else
inline void AndroidLogInit() {}

inline void SetAbortMessage(const char *) {}
#endif

#if SANITIZER_ANDROID
void SanitizerInitializeUnwinder();
AndroidApiLevel AndroidGetApiLevel();
#else
inline void AndroidLogWrite(const char *buffer_unused) {}
inline void SanitizerInitializeUnwinder() {}
inline AndroidApiLevel AndroidGetApiLevel() { return ANDROID_NOT_ANDROID; }
#endif

inline uptr GetPthreadDestructorIterations() {
#if SANITIZER_ANDROID
return (AndroidGetApiLevel() == ANDROID_LOLLIPOP_MR1) ? 8 : 4;
#elif SANITIZER_POSIX
return 4;
#else

return 0;
#endif
}

void *internal_start_thread(void *(*func)(void*), void *arg);
void internal_join_thread(void *th);
void MaybeStartBackgroudThread();





static inline void SanitizerBreakOptimization(void *arg) {
#if defined(_MSC_VER) && !defined(__clang__)
_ReadWriteBarrier();
#else
__asm__ __volatile__("" : : "r" (arg) : "memory");
#endif
}

struct SignalContext {
void *siginfo;
void *context;
uptr addr;
uptr pc;
uptr sp;
uptr bp;
bool is_memory_access;
enum WriteFlag { UNKNOWN, READ, WRITE } write_flag;




bool is_true_faulting_addr;



SignalContext() = default;




SignalContext(void *siginfo, void *context)
: siginfo(siginfo),
context(context),
addr(GetAddress()),
is_memory_access(IsMemoryAccess()),
write_flag(GetWriteFlag()),
is_true_faulting_addr(IsTrueFaultingAddress()) {
InitPcSpBp();
}

static void DumpAllRegisters(void *context);


int GetType() const;


const char *Describe() const;


bool IsStackOverflow() const;

private:

void InitPcSpBp();
uptr GetAddress() const;
WriteFlag GetWriteFlag() const;
bool IsMemoryAccess() const;
bool IsTrueFaultingAddress() const;
};

void InitializePlatformEarly();
void MaybeReexec();

template <typename Fn>
class RunOnDestruction {
public:
explicit RunOnDestruction(Fn fn) : fn_(fn) {}
~RunOnDestruction() { fn_(); }

private:
Fn fn_;
};



template <typename Fn>
RunOnDestruction<Fn> at_scope_exit(Fn fn) {
return RunOnDestruction<Fn>(fn);
}





#if SANITIZER_LINUX && SANITIZER_S390_64
void AvoidCVE_2016_2143();
#else
inline void AvoidCVE_2016_2143() {}
#endif

struct StackDepotStats {
uptr n_uniq_ids;
uptr allocated;
};



const s32 kReleaseToOSIntervalNever = -1;

void CheckNoDeepBind(const char *filename, int flag);



bool GetRandom(void *buffer, uptr length, bool blocking = true);


u32 GetNumberOfCPUs();
extern u32 NumberOfCPUsCached;
inline u32 GetNumberOfCPUsCached() {
if (!NumberOfCPUsCached)
NumberOfCPUsCached = GetNumberOfCPUs();
return NumberOfCPUsCached;
}

template <typename T>
class ArrayRef {
public:
ArrayRef() {}
ArrayRef(T *begin, T *end) : begin_(begin), end_(end) {}

T *begin() { return begin_; }
T *end() { return end_; }

private:
T *begin_ = nullptr;
T *end_ = nullptr;
};

#define PRINTF_128(v) (*((u8 *)&v + 0)), (*((u8 *)&v + 1)), (*((u8 *)&v + 2)), (*((u8 *)&v + 3)), (*((u8 *)&v + 4)), (*((u8 *)&v + 5)), (*((u8 *)&v + 6)), (*((u8 *)&v + 7)), (*((u8 *)&v + 8)), (*((u8 *)&v + 9)), (*((u8 *)&v + 10)), (*((u8 *)&v + 11)), (*((u8 *)&v + 12)), (*((u8 *)&v + 13)), (*((u8 *)&v + 14)), (*((u8 *)&v + 15))






}

inline void *operator new(__sanitizer::operator_new_size_type size,
__sanitizer::LowLevelAllocator &alloc) {
return alloc.Allocate(size);
}

#endif
