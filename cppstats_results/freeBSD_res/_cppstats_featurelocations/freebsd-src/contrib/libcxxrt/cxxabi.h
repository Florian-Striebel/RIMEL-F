





















#if !defined(__CXXABI_H_)
#define __CXXABI_H_
#include <stddef.h>
#include <stdint.h>
#include "unwind.h"
namespace std
{
class type_info;
}











#if defined(__cplusplus)
namespace __cxxabiv1 {
extern "C" {
#endif



typedef void (*unexpected_handler)();



typedef void (*terminate_handler)();






















struct __cxa_exception
{
#if __LP64__










uintptr_t referenceCount;
#endif

std::type_info *exceptionType;

void (*exceptionDestructor) (void *);

unexpected_handler unexpectedHandler;

terminate_handler terminateHandler;





__cxa_exception *nextException;






int handlerCount;
#if defined(__arm__) && !defined(__ARM_DWARF_EH__)





_Unwind_Exception *nextCleanup;



int cleanupCount;
#endif





int handlerSwitchValue;




const char *actionRecord;





const char *languageSpecificData;

void *catchTemp;





void *adjustedPtr;
#if !__LP64__










uintptr_t referenceCount;
#endif

_Unwind_Exception unwindHeader;
};









struct __cxa_eh_globals
{




__cxa_exception *caughtExceptions;



unsigned int uncaughtExceptions;
};



__cxa_eh_globals *__cxa_get_globals(void);




__cxa_eh_globals *__cxa_get_globals_fast(void);

std::type_info * __cxa_current_exception_type();





void __cxa_rethrow_primary_exception(void* thrown_exception);





void *__cxa_current_primary_exception(void);




void __cxa_increment_exception_refcount(void* thrown_exception);




void __cxa_decrement_exception_refcount(void* thrown_exception);













char* __cxa_demangle(const char* mangled_name,
char* buf,
size_t* n,
int* status);
#if defined(__cplusplus)
}
}

namespace abi = __cxxabiv1;

#endif
#endif
