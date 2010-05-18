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

#ifndef _ASMSTORE_H_
#define _ASMSTORE_H_

#include "CLRstdafx.h"
#include "AssemblyInfo.h"

//
// Implementation of IHostAssemblyStore
// - Provides methods that allow a host to load assemblies and modules independently of the common language runtime (CLR).
//
class AssemblyStore : public IHostAssemblyStore
{
public:
	//
	// IHostAssemblyStore
  //
	HRESULT STDMETHODCALLTYPE ProvideAssembly(
								AssemblyBindInfo *pBindInfo,
								UINT64           *pAssemblyId,
								UINT64           *pContext,
								IStream          **ppStmAssemblyImage,
								IStream          **ppStmPDB);
        

	HRESULT STDMETHODCALLTYPE ProvideModule(
								ModuleBindInfo *pBindInfo,
								DWORD          *pdwModuleId,
								IStream        **ppStmModuleImage,
								IStream        **ppStmPDB);

  //
	// IUnknown
  //
	virtual HRESULT STDMETHODCALLTYPE	QueryInterface(const IID &iid, void **ppv);
	virtual ULONG STDMETHODCALLTYPE		AddRef();
	virtual ULONG STDMETHODCALLTYPE		Release();

	AssemblyStore();
	AssemblyStore(list<AssemblyInfo> *assemblies);
	virtual ~AssemblyStore();

private:
	long m_cRef;
	list<AssemblyInfo> *m_pAssemblies;
  HRESULT LoadFile(LPCWSTR fileName, IStream* pResult);
};

#endif