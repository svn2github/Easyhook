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

#include "AssemblyManager.h"

AssemblyManager::AssemblyManager()
{
	m_cRef=0;
	m_pAssemblies = NULL;
	m_pAssemblyStore = NULL;
}

AssemblyManager::AssemblyManager(list<AssemblyInfo>* assemblies)
{
	m_cRef=0;
  m_pAssemblies = assemblies;
	m_pAssemblyStore = NULL;
	assert(m_pAssemblies);
}

AssemblyManager::~AssemblyManager()
{
}


//
// IHostAssemblyManager
//
        
HRESULT STDMETHODCALLTYPE AssemblyManager::GetNonHostStoreAssemblies( 
										ICLRAssemblyReferenceList **ppReferenceList)
{
  *ppReferenceList = new CLRAssemblyReferenceList(m_pAssemblies);
	return S_OK;
}

        
HRESULT STDMETHODCALLTYPE AssemblyManager::GetAssemblyStore( 
								IHostAssemblyStore **ppAssemblyStore)
{
  assert(m_pAssemblies);
	if(!m_pAssemblyStore)
	  m_pAssemblyStore = new AssemblyStore(m_pAssemblies);
	*ppAssemblyStore = (IHostAssemblyStore *)m_pAssemblyStore;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AssemblyManager::GetHostApplicationPolicy(
								DWORD           dwPolicy,
								DWORD           dwAppDomainId,
								DWORD           *pcbBufferSize,
								BYTE            *pbBuffer)
{
    *pcbBufferSize = 0;
    return S_OK;
}

//
// IUnknown
//
HRESULT STDMETHODCALLTYPE AssemblyManager::QueryInterface(const IID &iid,void **ppv)
{
	if (!ppv) return E_POINTER;
	*ppv=this;
	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE AssemblyManager::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

ULONG STDMETHODCALLTYPE AssemblyManager::Release()
{
	if(0 == InterlockedDecrement(&m_cRef)){
		delete this;
		return 0;
	}
	return m_cRef;
}