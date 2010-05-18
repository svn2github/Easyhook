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

#ifndef _ASMMANAGER_H_
#define _ASMMANAGER_H_

#include "CLRstdafx.h"
#include "AssemblyStore.h"
#include "CLRAssemblyReferenceList.h"

//
// Implementation of IHostAssemblyManager
// - Provides methods that allow a host to specify sets of assemblies that should be loaded by the common language runtime (CLR) or by the host.
//
class AssemblyManager : public IHostAssemblyManager
{
public:
	//
	// IHostAssemblyManager
  //
  HRESULT STDMETHODCALLTYPE GetNonHostStoreAssemblies( 
								ICLRAssemblyReferenceList **ppReferenceList);
        
	HRESULT STDMETHODCALLTYPE GetAssemblyStore( 
								IHostAssemblyStore **ppAssemblyStore);
	    
	HRESULT STDMETHODCALLTYPE GetHostApplicationPolicy(
								DWORD dwPolicy,
								DWORD dwAppDomainId,
								DWORD *pcbBufferSize,
								BYTE  *pbBuffer);
  //
	// IUnknown 
	//
  virtual HRESULT STDMETHODCALLTYPE	QueryInterface(const IID &iid, void **ppv);
	virtual ULONG STDMETHODCALLTYPE		AddRef();
	virtual ULONG STDMETHODCALLTYPE		Release();

	AssemblyManager();
	AssemblyManager(list<AssemblyInfo>* assemblies);
	
	virtual ~AssemblyManager();

private:
	long					m_cRef;
	list<AssemblyInfo>  *m_pAssemblies;
	AssemblyStore	*m_pAssemblyStore;
};

#endif