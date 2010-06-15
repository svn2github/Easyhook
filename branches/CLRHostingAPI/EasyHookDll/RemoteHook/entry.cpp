/*
    EasyHook - The reinvention of Windows API hooking
 
    Copyright (C) 2009 EasyHook

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    Please visit http://www.codeplex.com/easyhook for more information
    about the project and latest updates.
*/
#include ".\CLR Load Manager\CLRstdafx.h"
#include ".\CLR Load Manager\HostControl.h"
#include ".\CLR Load Manager\AssemblyInfo.h"
#include ".\CLR Load Manager\AssemblyStore.h"
#include ".\CLR Load Manager\AssemblyManager.h"
#include "stdafx.h"
#include <mscoree.h>
#ifdef _NET4
#include <metahost.h>
#endif

// prevent static NET binding...
typedef HRESULT __stdcall PROC_CorBindToRuntime(
                    LPCWSTR pwszVersion, 
                    LPCWSTR pwszBuildFlavor, 
                    REFCLSID rclsid, 
                    REFIID riid, 
                    LPVOID FAR *ppv);

#ifdef _NET4
typedef HRESULT __stdcall PROC_CLRCreateInstance(
                    REFCLSID  clsid,
                    REFIID riid,
                    LPVOID * ppInterface);
#endif

// a macro to compress error information...
#define UNMANAGED_ERROR(code) {ErrorCode = ((code) & 0xFF) | 0xF0000000; goto ABORT_ERROR;}

EASYHOOK_NT_INTERNAL CompleteUnmanagedInjection(LPREMOTE_INFO InInfo)
{
/*
Description:

    Loads the user library and finally executes the user entry point.

*/
    ULONG		            ErrorCode = 0;
    HMODULE                 hUserLib = LoadLibraryW(InInfo->UserLibrary);
    REMOTE_ENTRY_INFO       EntryInfo;
    REMOTE_ENTRY_POINT*     EntryProc = (REMOTE_ENTRY_POINT*)GetProcAddress(
                                hUserLib, 
#ifdef _M_X64
                                "NativeInjectionEntryPoint");
#else
								"_NativeInjectionEntryPoint@4");
#endif

    if(hUserLib == NULL)
        UNMANAGED_ERROR(20);

    if(EntryProc == NULL)
        UNMANAGED_ERROR(21);

    // set and close event
	if(!SetEvent(InInfo->hRemoteSignal))
		UNMANAGED_ERROR(22);

    // invoke user defined entry point
    EntryInfo.HostPID = InInfo->HostProcess;
    EntryInfo.UserData = (InInfo->UserDataSize) ? InInfo->UserData : NULL;
    EntryInfo.UserDataSize = InInfo->UserDataSize;

    EntryProc(&EntryInfo);

    return ERROR_SUCCESS;

ABORT_ERROR:

    return ErrorCode;
}

EASYHOOK_NT_EXPORT AssociateCustomCLRHost(ICLRRuntimeHost* pClr, RhAssemblyInfo* pRhAssemblies, long assembliesCount)
{
    ULONG               ErrorCode = ERROR_SUCCESS;
    AssemblyManager*    pAsmManager = NULL;
    HostControl*        pHostControl = NULL;

    // Copy pRhAssemblies to a list<AssemblyInfo>
    list<AssemblyInfo>* pAssemblies = new list<AssemblyInfo>();
    for (ULONG assemblyIndex = 0; assemblyIndex < assembliesCount; assemblyIndex++)
    {
        RhAssemblyInfo rhAssembly = pRhAssemblies[assemblyIndex];
        AssemblyInfo* assembly = new AssemblyInfo(rhAssembly.FullName,
                                                  rhAssembly.AssemblyLoadPath,
                                                  rhAssembly.AssemblyDebugInfoPath);
        pAssemblies->push_back(*assembly);
    }

    // Create an instance of the custom CLR Host, with the custom CLR Assembly Loader.
	pAsmManager = new AssemblyManager(pAssemblies);
	if (NULL == pAsmManager)
        UNMANAGED_ERROR(14);
	pHostControl = new HostControl((IHostAssemblyManager *)pAsmManager);
	if (NULL == pHostControl)
        UNMANAGED_ERROR(14);

    // Associate the custom CLR Host with the CLR.
    if (!RTL_SUCCESS(pClr->SetHostControl((IHostControl *)pHostControl)))
        UNMANAGED_ERROR(14);

ABORT_ERROR:

    // Release Resources
    if (NULL != pAsmManager)
        pAsmManager->Release();
    if (NULL != pHostControl)
        pHostControl->Release();

    return ErrorCode;
}

EASYHOOK_NT_INTERNAL CompleteManagedInjection(LPREMOTE_INFO InInfo)
{
    /// Region: Temporary workaround for InInfo->Assemblies not being received correctly
    InInfo->AssembliesCount = 3;
    InInfo->Assemblies = new RhAssemblyInfo[InInfo->AssembliesCount];
    InInfo->Assemblies[0].FullName = L"ProcMonInject, Version=1.0.0.0, Culture=neutral, PublicKeyToken=d34a061f079be347, ProcessorArchitecture=MSIL";
    InInfo->Assemblies[0].AssemblyLoadPath = L"D:/Documenten/Documenten/Projects/EasyHook/EasyHook - CLRHostingAPI - vs2008/Debug/x64/ProcMonInject.dll";
    InInfo->Assemblies[0].AssemblyDebugInfoPath = NULL;
    InInfo->Assemblies[1].FullName = L"ProcessMonitor, Version=1.0.0.0, Culture=neutral, PublicKeyToken=d34a061f079be347, ProcessorArchitecture=MSIL";
    InInfo->Assemblies[1].AssemblyLoadPath = L"D:/Documenten/Documenten/Projects/EasyHook/EasyHook - CLRHostingAPI - vs2008/Debug/x64/ProcessMonitor.exe";
    InInfo->Assemblies[1].AssemblyDebugInfoPath = NULL;
    InInfo->Assemblies[2].FullName = L"EasyHook, Version=2.5.0.0, Culture=neutral, PublicKeyToken=4b580fca19d0b0c5, ProcessorArchitecture=MSIL";
    InInfo->Assemblies[2].AssemblyLoadPath = L"D:/Documenten/Documenten/Projects/EasyHook/EasyHook - CLRHostingAPI - vs2008/Debug/x64/EasyHook.dll";
    InInfo->Assemblies[2].AssemblyDebugInfoPath = NULL;
    /// Endregion

/*
Description:

    Loads the NET runtime into the calling process and invokes the
    managed injection entry point.
*/

#ifdef _NET4
    ICLRMetaHost*	        pMetaClrHost = NULL;
    ICLRRuntimeInfo*        pRuntimeInfo = NULL;
#endif
    REMOTE_ENTRY_INFO       EntryInfo;
	ICLRRuntimeHost*	    pClr = NULL;
    WCHAR                   ParamString[MAX_PATH];
    DWORD                   ErrorCode = 0;
    HMODULE                 hMsCorEE = LoadLibraryA("mscoree.dll");
    PROC_CorBindToRuntime*  CorBindToRuntime = (PROC_CorBindToRuntime*)GetProcAddress(hMsCorEE, "CorBindToRuntime");

    if(CorBindToRuntime == NULL)
        UNMANAGED_ERROR(10);

    // Invoke user defined entry point
    EntryInfo.HostPID = InInfo->HostProcess;
    EntryInfo.UserData = InInfo->UserData;
    EntryInfo.UserDataSize = InInfo->UserDataSize;

    RtlLongLongToUnicodeHex((LONGLONG)&EntryInfo, ParamString);

    // Load the Common Language Runtime
    if(!RTL_SUCCESS(CorBindToRuntime(NULL, NULL, CLSID_CLRRuntimeHost, IID_ICLRRuntimeHost, (void**)&pClr)))
        UNMANAGED_ERROR(11);
    if (!RTL_SUCCESS(AssociateCustomCLRHost(pClr, InInfo->Assemblies, InInfo->AssembliesCount)))
        UNMANAGED_ERROR(14);
	pClr->Start();

    /*
		Test library code.
		This is because if once we have set the remote signal, there is no way to
		notify the host about errors. If the following call succeeds, then it will
		also do so some lines later... If not, then we are still able to report an error.

        The EasyHook managed injection loader will establish a connection to the
        host, so that further error reporting is still possible after we set the event!
	*/
    RtlLongLongToUnicodeHex((LONGLONG)&EntryInfo, ParamString);

    HRESULT result = pClr->ExecuteInDefaultAppDomain(
                            InInfo->UserLibrary, 
                            L"EasyHook.InjectionLoader", L"Main", 
                            ParamString, &ErrorCode);
	if(!RTL_SUCCESS(result))
	{
        // Test failed!
#ifdef _NET4
        // Target CLR might be .NET 4.0, try to load this CLR instead.
		if(NULL != pClr)
	        pClr->Release();
		if(FAILED(CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (void**)&pMetaClrHost)))
			UNMANAGED_ERROR(11);
		if(FAILED(pMetaClrHost->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, (void**)&pRuntimeInfo)))
			UNMANAGED_ERROR(11);
		if(FAILED(pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_ICLRRuntimeHost, (void**) &pClr)))
			UNMANAGED_ERROR(11);
        AssociateCustomCLRHost(pClr);
		pClr->Start();
		RtlLongLongToUnicodeHex((LONGLONG)&EntryInfo, ParamString);
		if(!RTL_SUCCESS(RuntimeClrHost->ExecuteInDefaultAppDomain(InInfo->UserLibrary, L"EasyHook.InjectionLoader", L"Main", ParamString, &RetVal)))
#endif
			UNMANAGED_ERROR(12);
	}

	if(!ErrorCode)
        UNMANAGED_ERROR(13);

	// set and close event
	if(!SetEvent(InInfo->hRemoteSignal))
		UNMANAGED_ERROR(22);
	CloseHandle(InInfo->hRemoteSignal);

	InInfo->hRemoteSignal = NULL;

	// Execute library code (no way for error reporting, so we dont need to check)
	pClr->ExecuteInDefaultAppDomain(
        InInfo->UserLibrary, 
        L"EasyHook.InjectionLoader", 
        L"Main", 
        ParamString, 
        &ErrorCode);

ABORT_ERROR:

	// Release Resources
	if(NULL != pClr)
		pClr->Release();

#ifdef _NET4
    if (NULL != pMetaClrHost)
        pMetaClrHost->Release();
    if (NULL != pRuntimeInfo)
        pRuntimeInfo->Release();
#endif

	return ErrorCode;
}

EASYHOOK_NT_EXPORT HookCompleteInjection(LPREMOTE_INFO InInfo)
{
/*
Description:

    This method is directly called from assembler code. It will update
    the symbols with the ones of the current process, adjust the PATH
    variable and invoke one of two above injection completions.

*/

    WCHAR*	    	PATH = NULL;
	ULONG		    ErrorCode = 0;
	HMODULE         hMod = GetModuleHandleA("kernel32.dll");
    ULONG           DirSize;
    ULONG           EnvSize;

    /*
		To increase stability we will now update all symbols with the
		real local ones...
	*/
	InInfo->LoadLibraryW = GetProcAddress(hMod, "LoadLibraryW");
	InInfo->FreeLibrary = GetProcAddress(hMod, "FreeLibrary");
	InInfo->GetProcAddress = GetProcAddress(hMod, "GetProcAddress");
	InInfo->VirtualFree = GetProcAddress(hMod, "VirtualFree");
	InInfo->VirtualProtect = GetProcAddress(hMod, "VirtualProtect");
	InInfo->ExitThread = GetProcAddress(hMod, "ExitThread");
	InInfo->GetLastError = GetProcAddress(hMod, "GetLastError");

    

	/* 
        Make directory of user library path available to current process...
	    This is to find dependencies without copying them into a global
	    directory which might cause trouble.
    */

	DirSize = RtlUnicodeLength(InInfo->PATH);
	EnvSize = GetEnvironmentVariableW(L"PATH", NULL, 0) + DirSize;

	if((PATH = (wchar_t*)RtlAllocateMemory(TRUE, EnvSize * 2 + 10)) == NULL)
		UNMANAGED_ERROR(1);

	GetEnvironmentVariableW(L"PATH", PATH, EnvSize);

	// add library path to environment variable
	if(!RtlMoveMemory(PATH + DirSize, PATH, (EnvSize - DirSize) * 2))
        UNMANAGED_ERROR(1);

	RtlCopyMemory(PATH, InInfo->PATH, DirSize * 2);

	if(!SetEnvironmentVariableW(L"PATH", PATH))
		UNMANAGED_ERROR(2);

    if(!RTL_SUCCESS(RhSetWakeUpThreadID(InInfo->WakeUpThreadID)))
        UNMANAGED_ERROR(3);

	// load and execute user library...
	if(InInfo->IsManaged)
        ErrorCode = CompleteManagedInjection(InInfo);
    else
        ErrorCode = CompleteUnmanagedInjection(InInfo);

    return ErrorCode;

ABORT_ERROR:

    // release resources
	if(PATH != NULL)
		RtlFreeMemory(PATH);

	if(InInfo->hRemoteSignal != NULL)
		CloseHandle(InInfo->hRemoteSignal);

	return ErrorCode;
}
