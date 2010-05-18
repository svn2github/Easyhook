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

#ifndef _CLRASSEMBLYLIST_H_
#define _CLRASSEMBLYLIST_H_

#include "CLRstdafx.h"
#include "AssemblyInfo.h"
#include <list>

//
// Implementation of ICLRAssemblyReferenceList
// - Manages a list of assemblies that are loaded by the common language runtime (CLR) and not by the host.
// - We don't have a list of those assemblies, we only know the assemblies that must be loaded by the host.
//
class CLRAssemblyReferenceList : public ICLRAssemblyReferenceList
{
public:
  //
  // ICLRAssemblyReferenceList
  //
  HRESULT STDMETHODCALLTYPE IsAssemblyReferenceInList(
  							IUnknown *pName);

  HRESULT STDMETHODCALLTYPE IsStringAssemblyReferenceInList(
  							LPCWSTR pwzAssemblyName);

  //
	// IUnknown 
  //
	virtual HRESULT STDMETHODCALLTYPE	QueryInterface(const IID &iid, void **ppv);
	virtual ULONG STDMETHODCALLTYPE		AddRef();
	virtual ULONG STDMETHODCALLTYPE		Release();

	CLRAssemblyReferenceList();
	CLRAssemblyReferenceList(list<AssemblyInfo>* assemblies);
	
	virtual ~CLRAssemblyReferenceList();

private:
  long m_cRef;
  list<AssemblyInfo> *m_pAssemblies;

};

#endif