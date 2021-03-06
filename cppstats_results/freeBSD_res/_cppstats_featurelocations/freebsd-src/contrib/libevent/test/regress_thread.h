

























#if !defined(REGRESS_THREAD_H_INCLUDED_)
#define REGRESS_THREAD_H_INCLUDED_

#if defined(EVENT__HAVE_PTHREADS)
#define THREAD_T pthread_t
#define THREAD_FN void *
#define THREAD_RETURN() return (NULL)
#define THREAD_START(threadvar, fn, arg) pthread_create(&(threadvar), NULL, fn, arg)

#define THREAD_JOIN(th) pthread_join(th, NULL)
#else
#define THREAD_T HANDLE
#define THREAD_FN unsigned __stdcall
#define THREAD_RETURN() return (0)
#define THREAD_START(threadvar, fn, arg) do { uintptr_t threadhandle = _beginthreadex(NULL,0,fn,(arg),0,NULL); (threadvar) = (HANDLE) threadhandle; } while (0)



#define THREAD_JOIN(th) WaitForSingleObject(th, INFINITE)
#endif

#endif
