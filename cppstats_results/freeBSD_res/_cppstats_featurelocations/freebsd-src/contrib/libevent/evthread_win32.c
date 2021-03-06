
























#include "event2/event-config.h"
#include "evconfig-private.h"

#if defined(_WIN32)
#if !defined(_WIN32_WINNT)

#define _WIN32_WINNT 0x0403
#endif
#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <sys/locking.h>
#endif

struct event_base;
#include "event2/thread.h"

#include "mm-internal.h"
#include "evthread-internal.h"
#include "time-internal.h"

#define SPIN_COUNT 2000

static void *
evthread_win32_lock_create(unsigned locktype)
{
CRITICAL_SECTION *lock = mm_malloc(sizeof(CRITICAL_SECTION));
if (!lock)
return NULL;
if (InitializeCriticalSectionAndSpinCount(lock, SPIN_COUNT) == 0) {
mm_free(lock);
return NULL;
}
return lock;
}

static void
evthread_win32_lock_free(void *lock_, unsigned locktype)
{
CRITICAL_SECTION *lock = lock_;
DeleteCriticalSection(lock);
mm_free(lock);
}

static int
evthread_win32_lock(unsigned mode, void *lock_)
{
CRITICAL_SECTION *lock = lock_;
if ((mode & EVTHREAD_TRY)) {
return ! TryEnterCriticalSection(lock);
} else {
EnterCriticalSection(lock);
return 0;
}
}

static int
evthread_win32_unlock(unsigned mode, void *lock_)
{
CRITICAL_SECTION *lock = lock_;
LeaveCriticalSection(lock);
return 0;
}

static unsigned long
evthread_win32_get_id(void)
{
return (unsigned long) GetCurrentThreadId();
}

#if defined(WIN32_HAVE_CONDITION_VARIABLES)
static void WINAPI (*InitializeConditionVariable_fn)(PCONDITION_VARIABLE)
= NULL;
static BOOL WINAPI (*SleepConditionVariableCS_fn)(
PCONDITION_VARIABLE, PCRITICAL_SECTION, DWORD) = NULL;
static void WINAPI (*WakeAllConditionVariable_fn)(PCONDITION_VARIABLE) = NULL;
static void WINAPI (*WakeConditionVariable_fn)(PCONDITION_VARIABLE) = NULL;

static int
evthread_win32_condvar_init(void)
{
HANDLE lib;

lib = GetModuleHandle(TEXT("kernel32.dll"));
if (lib == NULL)
return 0;

#define LOAD(name) name##_fn = GetProcAddress(lib, #name)

LOAD(InitializeConditionVariable);
LOAD(SleepConditionVariableCS);
LOAD(WakeAllConditionVariable);
LOAD(WakeConditionVariable);

return InitializeConditionVariable_fn && SleepConditionVariableCS_fn &&
WakeAllConditionVariable_fn && WakeConditionVariable_fn;
}



static void *
evthread_win32_condvar_alloc(unsigned condflags)
{
CONDITION_VARIABLE *cond = mm_malloc(sizeof(CONDITION_VARIABLE));
if (!cond)
return NULL;
InitializeConditionVariable_fn(cond);
return cond;
}

static void
evthread_win32_condvar_free(void *cond_)
{
CONDITION_VARIABLE *cond = cond_;

mm_free(cond);
}

static int
evthread_win32_condvar_signal(void *cond, int broadcast)
{
CONDITION_VARIABLE *cond = cond_;
if (broadcast)
WakeAllConditionVariable_fn(cond);
else
WakeConditionVariable_fn(cond);
return 0;
}

static int
evthread_win32_condvar_wait(void *cond_, void *lock_, const struct timeval *tv)
{
CONDITION_VARIABLE *cond = cond_;
CRITICAL_SECTION *lock = lock_;
DWORD ms, err;
BOOL result;

if (tv)
ms = evutil_tv_to_msec_(tv);
else
ms = INFINITE;
result = SleepConditionVariableCS_fn(cond, lock, ms);
if (result) {
if (GetLastError() == WAIT_TIMEOUT)
return 1;
else
return -1;
} else {
return 0;
}
}
#endif

struct evthread_win32_cond {
HANDLE event;

CRITICAL_SECTION lock;
int n_waiting;
int n_to_wake;
int generation;
};

static void *
evthread_win32_cond_alloc(unsigned flags)
{
struct evthread_win32_cond *cond;
if (!(cond = mm_malloc(sizeof(struct evthread_win32_cond))))
return NULL;
if (InitializeCriticalSectionAndSpinCount(&cond->lock, SPIN_COUNT)==0) {
mm_free(cond);
return NULL;
}
if ((cond->event = CreateEvent(NULL,TRUE,FALSE,NULL)) == NULL) {
DeleteCriticalSection(&cond->lock);
mm_free(cond);
return NULL;
}
cond->n_waiting = cond->n_to_wake = cond->generation = 0;
return cond;
}

static void
evthread_win32_cond_free(void *cond_)
{
struct evthread_win32_cond *cond = cond_;
DeleteCriticalSection(&cond->lock);
CloseHandle(cond->event);
mm_free(cond);
}

static int
evthread_win32_cond_signal(void *cond_, int broadcast)
{
struct evthread_win32_cond *cond = cond_;
EnterCriticalSection(&cond->lock);
if (broadcast)
cond->n_to_wake = cond->n_waiting;
else
++cond->n_to_wake;
cond->generation++;
SetEvent(cond->event);
LeaveCriticalSection(&cond->lock);
return 0;
}

static int
evthread_win32_cond_wait(void *cond_, void *lock_, const struct timeval *tv)
{
struct evthread_win32_cond *cond = cond_;
CRITICAL_SECTION *lock = lock_;
int generation_at_start;
int waiting = 1;
int result = -1;
DWORD ms = INFINITE, ms_orig = INFINITE, startTime, endTime;
if (tv)
ms_orig = ms = evutil_tv_to_msec_(tv);

EnterCriticalSection(&cond->lock);
++cond->n_waiting;
generation_at_start = cond->generation;
LeaveCriticalSection(&cond->lock);

LeaveCriticalSection(lock);

startTime = GetTickCount();
do {
DWORD res;
res = WaitForSingleObject(cond->event, ms);
EnterCriticalSection(&cond->lock);
if (cond->n_to_wake &&
cond->generation != generation_at_start) {
--cond->n_to_wake;
--cond->n_waiting;
result = 0;
waiting = 0;
goto out;
} else if (res != WAIT_OBJECT_0) {
result = (res==WAIT_TIMEOUT) ? 1 : -1;
--cond->n_waiting;
waiting = 0;
goto out;
} else if (ms != INFINITE) {
endTime = GetTickCount();
if (startTime + ms_orig <= endTime) {
result = 1;
--cond->n_waiting;
waiting = 0;
goto out;
} else {
ms = startTime + ms_orig - endTime;
}
}

if (cond->n_to_wake == 0) {


ResetEvent(cond->event);
}
out:
LeaveCriticalSection(&cond->lock);
} while (waiting);

EnterCriticalSection(lock);

EnterCriticalSection(&cond->lock);
if (!cond->n_waiting)
ResetEvent(cond->event);
LeaveCriticalSection(&cond->lock);

return result;
}

int
evthread_use_windows_threads(void)
{
struct evthread_lock_callbacks cbs = {
EVTHREAD_LOCK_API_VERSION,
EVTHREAD_LOCKTYPE_RECURSIVE,
evthread_win32_lock_create,
evthread_win32_lock_free,
evthread_win32_lock,
evthread_win32_unlock
};


struct evthread_condition_callbacks cond_cbs = {
EVTHREAD_CONDITION_API_VERSION,
evthread_win32_cond_alloc,
evthread_win32_cond_free,
evthread_win32_cond_signal,
evthread_win32_cond_wait
};
#if defined(WIN32_HAVE_CONDITION_VARIABLES)
struct evthread_condition_callbacks condvar_cbs = {
EVTHREAD_CONDITION_API_VERSION,
evthread_win32_condvar_alloc,
evthread_win32_condvar_free,
evthread_win32_condvar_signal,
evthread_win32_condvar_wait
};
#endif

evthread_set_lock_callbacks(&cbs);
evthread_set_id_callback(evthread_win32_get_id);
#if defined(WIN32_HAVE_CONDITION_VARIABLES)
if (evthread_win32_condvar_init()) {
evthread_set_condition_callbacks(&condvar_cbs);
return 0;
}
#endif
evthread_set_condition_callbacks(&cond_cbs);

return 0;
}

