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

#include <Shlwapi.h>
#include "AssemblyStore.h"

AssemblyStore::AssemblyStore()
{
	m_cRef = 0;
    m_pAssemblies = NULL;
}

AssemblyStore::AssemblyStore(list<AssemblyInfo>* assemblies)
{
	m_cRef=0;
    m_pAssemblies = assemblies;
	assert(m_pAssemblies);
}

AssemblyStore::~AssemblyStore()
{
}

//
// IHostAssemblyStore
//
HRESULT STDMETHODCALLTYPE AssemblyStore::ProvideAssembly(
								AssemblyBindInfo *pBindInfo,
								UINT64           *pAssemblyId,
								UINT64           *pContext,
								IStream          **ppStmAssemblyImage,
								IStream          **ppStmPDB)
{
	assert(m_pAssemblies);
	
	*pContext = 0;   // We don't use pContext for any host-specific data - set it to 0.
    UINT64 id = 0;   // Will be assigned to pAssemblyId
    for(list<AssemblyInfo>::iterator assembly = m_pAssemblies->begin(); assembly != m_pAssemblies->end(); ++assembly)
    {
      ++id;
      if (!IsSameAssembly(assembly->FullName, pBindInfo->lpPostPolicyIdentity))
          continue;
      if (NULL == assembly->AssemblyLoadPath)
          return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);  // Unable to load the assembly without a known location.
      HRESULT hr = ReadFileToStream(assembly->AssemblyLoadPath, *ppStmAssemblyImage);
      if (S_OK != hr)
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);    // Failed to create an IStream for the assembly.
      if (NULL != assembly->AssemblyDebugInfoPath)
        hr = ReadFileToStream(assembly->AssemblyDebugInfoPath, *ppStmPDB);
      if (S_OK != hr)
        *ppStmPDB = NULL;                                   // Failed to create an IStream for the assembly's debug info.
      /**
        Assigning pAssemblyId enables the CLR to increase performance by preventing multiple loads of the same assembly.
        If multiple calls to ProvideAssembly result in the same number being returned in *pAssemblyId, the CLR assumes the assemblies
        are the same and reuses the bytes and data structures it already has instead of mapping the contents of the stream again.
        The CLR treats the unique number assigned by the host as an opaque identity and never interprets the number in any way.
      */
      *pAssemblyId = id;
      return S_OK;
    }
    return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);  // Force CLR to fall back to it's default load manager
}

        
HRESULT STDMETHODCALLTYPE AssemblyStore::ProvideModule(
								ModuleBindInfo *pBindInfo,
								DWORD          *pdwModuleId,
								IStream        **ppStmModuleImage,
								IStream        **ppStmPDB)
{
    /**
      Providing assembly modules is not supported.
      Probably, for assembly modules, an IStream can be acquired by using IStorage.
    */
    return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);  // Force CLR to fall back to it's default load manager
}

HRESULT AssemblyStore::ReadFileToStream(LPCWSTR fileName, IStream* pStream) 
{
    HRESULT hr = SHCreateStreamOnFileEx
                    (fileName,
                     STGM_READ | STGM_SHARE_DENY_WRITE | STGM_FAILIFTHERE,
                     0, FALSE, NULL,
                     &pStream);
    return hr;
}

BOOL AssemblyStore::IsSameAssembly(LPCWSTR assemblyName, LPCWSTR otherAssemblyName)
{
    return (0 == lstrcmpi(assemblyName, otherAssemblyName));
}

//
// IUnknown
//
HRESULT STDMETHODCALLTYPE AssemblyStore::QueryInterface(const IID &iid,void **ppv)
{
	if (!ppv) return E_POINTER;
	*ppv=this;
	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE AssemblyStore::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

ULONG STDMETHODCALLTYPE AssemblyStore::Release()
{
	if(0 == InterlockedDecrement(&m_cRef))
    {
		delete this;
		return 0;
	}
	return m_cRef;
}