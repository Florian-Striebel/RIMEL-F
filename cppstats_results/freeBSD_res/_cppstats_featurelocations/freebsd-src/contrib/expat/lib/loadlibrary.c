































#if defined(_WIN32)

#include <windows.h>
#include <tchar.h>


HMODULE _Expat_LoadLibrary(LPCTSTR filename);


#if !defined(LOAD_WITH_ALTERED_SEARCH_PATH)
#define LOAD_WITH_ALTERED_SEARCH_PATH 0x00000008
#endif

#if !defined(LOAD_LIBRARY_SEARCH_SYSTEM32)
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif


typedef HMODULE (APIENTRY *LOADLIBRARYEX_FN)(LPCTSTR, HANDLE, DWORD);


#if defined(UNICODE)
#if defined(_WIN32_WCE)
#define LOADLIBARYEX L"LoadLibraryExW"
#else
#define LOADLIBARYEX "LoadLibraryExW"
#endif
#else
#define LOADLIBARYEX "LoadLibraryExA"
#endif
















HMODULE _Expat_LoadLibrary(LPCTSTR filename)
{
HMODULE hModule = NULL;
LOADLIBRARYEX_FN pLoadLibraryEx = NULL;


HMODULE hKernel32 = GetModuleHandle(TEXT("kernel32"));
if(!hKernel32)
return NULL;



pLoadLibraryEx = (LOADLIBRARYEX_FN) GetProcAddress(hKernel32, LOADLIBARYEX);





if(_tcspbrk(filename, TEXT("\\/"))) {

hModule = pLoadLibraryEx ?
pLoadLibraryEx(filename, NULL, LOAD_WITH_ALTERED_SEARCH_PATH) :
LoadLibrary(filename);
}



else if(pLoadLibraryEx && GetProcAddress(hKernel32, "AddDllDirectory")) {

hModule = pLoadLibraryEx(filename, NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
}
else {

UINT systemdirlen = GetSystemDirectory(NULL, 0);
if(systemdirlen) {


size_t filenamelen = _tcslen(filename);
TCHAR *path = malloc(sizeof(TCHAR) * (systemdirlen + 1 + filenamelen));
if(path && GetSystemDirectory(path, systemdirlen)) {

_tcscpy(path + _tcslen(path), TEXT("\\"));
_tcscpy(path + _tcslen(path), filename);



hModule = pLoadLibraryEx ?
pLoadLibraryEx(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH) :
LoadLibrary(path);

}
free(path);
}
}

return hModule;
}

#else



typedef int _TRANSLATION_UNIT_LOAD_LIBRARY_C_NOT_EMTPY;

#endif
