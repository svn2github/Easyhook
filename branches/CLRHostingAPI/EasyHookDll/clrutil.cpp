/*
    EasyHook - The reinvention of Windows API hooking
 
    Copyright (C) 2010 EasyHook

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

typedef struct _INTERNAL_CONTEXT_{
    list<AssemblyInfo>             *assemblies;
}INTERNAL_CONTEXT, *LPINTERNAL_CONTEXT;

extern "C" __declspec(dllexport) LPINTERNAL_CONTEXT __stdcall CLRCreateContext()
{
    LPINTERNAL_CONTEXT context
        = (LPINTERNAL_CONTEXT)RtlAllocateMemory(TRUE, sizeof(INTERNAL_CONTEXT));
	if(NULL == context)
		return NULL;
    memset(context, 0, sizeof(INTERNAL_CONTEXT));
    return context;
}

extern "C" __declspec(dllexport) void __stdcall CLRReleaseContext(LPINTERNAL_CONTEXT *clrContext)
{
    if (NULL == *clrContext)
        return;
    LPINTERNAL_CONTEXT context = *clrContext;
    delete(context->assemblies);
    memset(context, 0, sizeof(INTERNAL_CONTEXT));
    RtlFreeMemory(context);
    *clrContext = NULL;
}

extern "C" __declspec(dllexport) BOOL __stdcall CLRAddAssemblyToContext(LPINTERNAL_CONTEXT *clrContext,
                                                                     LPCWSTR fullAssemblyName,
                                                                     LPCWSTR assemblyLoadPath,
                                                                     LPCWSTR assemblyDebugInfoPath)
{
    if (NULL == *clrContext
        || NULL == fullAssemblyName
        || NULL == assemblyLoadPath)
        return S_FALSE;
    LPINTERNAL_CONTEXT context = *clrContext;
    AssemblyInfo info(fullAssemblyName, assemblyLoadPath, assemblyDebugInfoPath);
    context->assemblies->push_back(info);
    return S_OK;
}

extern "C" __declspec(dllexport) HRESULT __stdcall CLRRegisterToLoadManager(
    LPINTERNAL_CONTEXT *clrContext)
{
    LPINTERNAL_CONTEXT context = *clrContext;

    // Create an instance of AssemblyManager.  This class contains our
	// implementation of the Assembly Loading Manager, specifically the
	// IHostAssemblyStore interface.
	AssemblyManager *pAsmManager = new AssemblyManager(context->assemblies);
	assert(pAsmManager);

	// Create a host control object that takes the new assembly loading manager.  The
	// HostControl class implements IHostControl which the CLR calls at startup
	// to determine which managers we support.  In this case, we support just the
	// assembly loading manager.
	HostControl *pHostControl = new HostControl((IHostAssemblyManager *)pAsmManager);
    assert(pHostControl);
    
    // Get the CLR
	ICLRRuntimeHost *pCLR = NULL;
    HRESULT hr = CorBindToRuntimeEx(
      NULL,              // Load the latest CLR version available. BUG?! ENSURE THAT VERSION MATCHES THE CORRECT CLR VERSION
	  L"wks",            // Workstation GC, this value is either "wks" or "svr" (Server GC)
      STARTUP_CONCURRENT_GC, 
      CLSID_CLRRuntimeHost, 
      IID_ICLRRuntimeHost, 
      (PVOID*) &pCLR);
    assert(SUCCEEDED(hr));

	// Tell the CLR about our Host Control object.
    // Remember that we must do this before calling ICLRRuntimeHost::Start
    hr = pCLR->SetHostControl((IHostControl *)pHostControl);
	assert(SUCCEEDED(hr));

	// Start the CLR
	hr = pCLR->Start();

    // ToDo:
    //  Now we are ready to invoke the entry method of the managed code side.
    //  When that code attempts to load an assembly matching any AssemblyInfo specified in the assemblies parameter, this hosting API will be used.
    //  Otherwise, the default CLR Hosting API is used to load the assembly.

    // When the entry method returns, the custom CLR Hosting API is not needed anymore.
	pHostControl->Release();
	return S_OK;
}