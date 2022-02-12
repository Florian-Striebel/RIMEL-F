



























#pragma once



#if !defined(E_NOTFOUND)
#define E_NOTFOUND HRESULT_FROM_WIN32(ERROR_NOT_FOUND)
#endif

#if !defined(E_FILENOTFOUND)
#define E_FILENOTFOUND HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)
#endif






enum InstanceState : unsigned {



eNone = 0,




eLocal = 1,




eRegistered = 2,




eNoRebootRequired = 4,




eComplete = MAXUINT,
};



#if !defined(__ISetupInstance_FWD_DEFINED__)
#define __ISetupInstance_FWD_DEFINED__
typedef struct ISetupInstance ISetupInstance;
#endif

#if !defined(__ISetupInstance2_FWD_DEFINED__)
#define __ISetupInstance2_FWD_DEFINED__
typedef struct ISetupInstance2 ISetupInstance2;
#endif

#if !defined(__IEnumSetupInstances_FWD_DEFINED__)
#define __IEnumSetupInstances_FWD_DEFINED__
typedef struct IEnumSetupInstances IEnumSetupInstances;
#endif

#if !defined(__ISetupConfiguration_FWD_DEFINED__)
#define __ISetupConfiguration_FWD_DEFINED__
typedef struct ISetupConfiguration ISetupConfiguration;
#endif

#if !defined(__ISetupConfiguration2_FWD_DEFINED__)
#define __ISetupConfiguration2_FWD_DEFINED__
typedef struct ISetupConfiguration2 ISetupConfiguration2;
#endif

#if !defined(__ISetupPackageReference_FWD_DEFINED__)
#define __ISetupPackageReference_FWD_DEFINED__
typedef struct ISetupPackageReference ISetupPackageReference;
#endif

#if !defined(__ISetupHelper_FWD_DEFINED__)
#define __ISetupHelper_FWD_DEFINED__
typedef struct ISetupHelper ISetupHelper;
#endif



#if !defined(__SetupConfiguration_FWD_DEFINED__)
#define __SetupConfiguration_FWD_DEFINED__

#if defined(__cplusplus)
typedef class SetupConfiguration SetupConfiguration;
#endif

#endif

#if defined(__cplusplus)
extern "C" {
#endif



EXTERN_C const IID IID_ISetupInstance;

#if defined(__cplusplus) && !defined(CINTERFACE)



struct DECLSPEC_UUID("B41463C3-8866-43B5-BC33-2B0676F7F42E")
DECLSPEC_NOVTABLE ISetupInstance : public IUnknown {







STDMETHOD(GetInstanceId)(_Out_ BSTR *pbstrInstanceId) = 0;










STDMETHOD(GetInstallDate)(_Out_ LPFILETIME pInstallDate) = 0;











STDMETHOD(GetInstallationName)(_Out_ BSTR *pbstrInstallationName) = 0;









STDMETHOD(GetInstallationPath)(_Out_ BSTR *pbstrInstallationPath) = 0;









STDMETHOD(GetInstallationVersion)(_Out_ BSTR *pbstrInstallationVersion) = 0;










STDMETHOD(GetDisplayName)(_In_ LCID lcid, _Out_ BSTR *pbstrDisplayName) = 0;










STDMETHOD(GetDescription)(_In_ LCID lcid, _Out_ BSTR *pbstrDescription) = 0;












STDMETHOD(ResolvePath)
(_In_opt_z_ LPCOLESTR pwszRelativePath, _Out_ BSTR *pbstrAbsolutePath) = 0;
};
#endif

EXTERN_C const IID IID_ISetupInstance2;

#if defined(__cplusplus) && !defined(CINTERFACE)



struct DECLSPEC_UUID("89143C9A-05AF-49B0-B717-72E218A2185C")
DECLSPEC_NOVTABLE ISetupInstance2 : public ISetupInstance {






STDMETHOD(GetState)(_Out_ InstanceState *pState) = 0;









STDMETHOD(GetPackages)(_Out_ LPSAFEARRAY *ppsaPackages) = 0;











STDMETHOD(GetProduct)
(_Outptr_result_maybenull_ ISetupPackageReference **ppPackage) = 0;








STDMETHOD(GetProductPath)
(_Outptr_result_maybenull_ BSTR *pbstrProductPath) = 0;
};
#endif

EXTERN_C const IID IID_IEnumSetupInstances;

#if defined(__cplusplus) && !defined(CINTERFACE)



struct DECLSPEC_UUID("6380BCFF-41D3-4B2E-8B2E-BF8A6810C848")
DECLSPEC_NOVTABLE IEnumSetupInstances : public IUnknown {












STDMETHOD(Next)
(_In_ ULONG celt, _Out_writes_to_(celt, *pceltFetched) ISetupInstance **rgelt,
_Out_opt_ _Deref_out_range_(0, celt) ULONG *pceltFetched) = 0;







STDMETHOD(Skip)(_In_ ULONG celt) = 0;





STDMETHOD(Reset)(void) = 0;










STDMETHOD(Clone)(_Deref_out_opt_ IEnumSetupInstances **ppenum) = 0;
};
#endif

EXTERN_C const IID IID_ISetupConfiguration;

#if defined(__cplusplus) && !defined(CINTERFACE)



struct DECLSPEC_UUID("42843719-DB4C-46C2-8E7C-64F1816EFD5B")
DECLSPEC_NOVTABLE ISetupConfiguration : public IUnknown {






STDMETHOD(EnumInstances)(_Out_ IEnumSetupInstances **ppEnumInstances) = 0;








STDMETHOD(GetInstanceForCurrentProcess)
(_Out_ ISetupInstance **ppInstance) = 0;







STDMETHOD(GetInstanceForPath)
(_In_z_ LPCWSTR wzPath, _Out_ ISetupInstance **ppInstance) = 0;
};
#endif

EXTERN_C const IID IID_ISetupConfiguration2;

#if defined(__cplusplus) && !defined(CINTERFACE)



struct DECLSPEC_UUID("26AAB78C-4A60-49D6-AF3B-3C35BC93365D")
DECLSPEC_NOVTABLE ISetupConfiguration2 : public ISetupConfiguration {






STDMETHOD(EnumAllInstances)(_Out_ IEnumSetupInstances **ppEnumInstances) = 0;
};
#endif

EXTERN_C const IID IID_ISetupPackageReference;

#if defined(__cplusplus) && !defined(CINTERFACE)



struct DECLSPEC_UUID("da8d8a16-b2b6-4487-a2f1-594ccccd6bf5")
DECLSPEC_NOVTABLE ISetupPackageReference : public IUnknown {





STDMETHOD(GetId)(_Out_ BSTR *pbstrId) = 0;






STDMETHOD(GetVersion)(_Out_ BSTR *pbstrVersion) = 0;







STDMETHOD(GetChip)(_Out_ BSTR *pbstrChip) = 0;







STDMETHOD(GetLanguage)(_Out_ BSTR *pbstrLanguage) = 0;






STDMETHOD(GetBranch)(_Out_ BSTR *pbstrBranch) = 0;






STDMETHOD(GetType)(_Out_ BSTR *pbstrType) = 0;








STDMETHOD(GetUniqueId)(_Out_ BSTR *pbstrUniqueId) = 0;
};
#endif

EXTERN_C const IID IID_ISetupHelper;

#if defined(__cplusplus) && !defined(CINTERFACE)







struct DECLSPEC_UUID("42b21b78-6192-463e-87bf-d577838f1d5c")
DECLSPEC_NOVTABLE ISetupHelper : public IUnknown {








STDMETHOD(ParseVersion)
(_In_ LPCOLESTR pwszVersion, _Out_ PULONGLONG pullVersion) = 0;













STDMETHOD(ParseVersionRange)
(_In_ LPCOLESTR pwszVersionRange, _Out_ PULONGLONG pullMinVersion,
_Out_ PULONGLONG pullMaxVersion) = 0;
};
#endif



EXTERN_C const CLSID CLSID_SetupConfiguration;

#if defined(__cplusplus)




class DECLSPEC_UUID("177F0C4A-1CD3-4DE7-A32C-71DBBB9FA36D") SetupConfiguration;
#endif












STDMETHODIMP GetSetupConfiguration(_Out_ ISetupConfiguration **ppConfiguration,
_Reserved_ LPVOID pReserved);

#if defined(__cplusplus)
}
#endif
