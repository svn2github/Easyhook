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

#include <fusion.h>
#include "CLRAssemblyReferenceList.h"

CLRAssemblyReferenceList::CLRAssemblyReferenceList()
{
  m_pAssemblies = NULL;
}

CLRAssemblyReferenceList::CLRAssemblyReferenceList(list<AssemblyInfo> *assemblies)
{
  m_pAssemblies = assemblies;
  assert(m_pAssemblies);
}

CLRAssemblyReferenceList::~CLRAssemblyReferenceList()
{
}

//
// ICLRAssemblyReferenceList
//
HRESULT CLRAssemblyReferenceList::IsAssemblyReferenceInList(IUnknown *pName)
{
  // Return values:
  //  S_OK    - A matching value is NOT found in the list, the assembly will be loaded by the CLR
  //  S_FALSE - A matching value IS found in the list, the assembly will be loaded by our load manager.
  IAssemblyName* name = (IAssemblyName *) pName;
  LPWSTR displayName;
  HRESULT hr = name->GetDisplayName(NULL, NULL, ASM_DISPLAYF_FULL);
  assert(hr);
  for(list<AssemblyInfo>::iterator assembly = m_pAssemblies->begin(); assembly != m_pAssemblies->end(); ++assembly)
  {
    if (0== lstrcmpi(assembly->FullName, displayName))
      return S_FALSE;
  }
  return S_OK;
}

HRESULT CLRAssemblyReferenceList::IsStringAssemblyReferenceInList(LPCWSTR pwzAssemblyName)
{
  // Return values:
  //  S_OK    - A matching value is NOT found in the list, the assembly will be loaded by the CLR
  //  S_FALSE - A matching value IS found in the list, the assembly will be loaded by our load manager.
  for(list<AssemblyInfo>::iterator assembly = m_pAssemblies->begin(); assembly != m_pAssemblies->end(); ++assembly)
  {
    if (0 == lstrcmpi(assembly->FullName, pwzAssemblyName))
      return S_FALSE;
  }
  return S_OK;
}

//
// IUnknown
//
HRESULT STDMETHODCALLTYPE CLRAssemblyReferenceList::QueryInterface(const IID &iid,void **ppv)
{
	if (!ppv) return E_POINTER;
	*ppv=this;
	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE CLRAssemblyReferenceList::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

ULONG STDMETHODCALLTYPE CLRAssemblyReferenceList::Release()
{
	if(0 == InterlockedDecrement(&m_cRef)){
		delete this;
		return 0;
	}
	return m_cRef;
}