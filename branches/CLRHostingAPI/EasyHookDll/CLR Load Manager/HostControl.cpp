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

#include "HostControl.h"

HostControl::HostControl()
{
	m_cRef=0;
	m_pHostAssemblyManager = NULL;
}

HostControl::HostControl(IHostAssemblyManager *pHostAssemblyManager)
{
	m_pHostAssemblyManager = pHostAssemblyManager;
    assert(m_pHostAssemblyManager);
	if (m_pHostAssemblyManager)
        m_pHostAssemblyManager->AddRef();
}

HostControl::~HostControl()
{
	if (m_pHostAssemblyManager)
        m_pHostAssemblyManager->Release();
}

//
// IHostControl
//
HRESULT STDMETHODCALLTYPE HostControl::GetHostManager(REFIID riid,void **ppv)
{
	if ((riid == IID_IHostAssemblyManager) && (m_pHostAssemblyManager))
	{
		printf("HostControl returning a custom Assembly Manager\n");
		*ppv = m_pHostAssemblyManager;
		return S_OK;
	}
  // No implementations for any other type of host manager.
  // CLR will fall back to its default implementation.
	return E_NOINTERFACE;
}

        
HRESULT STDMETHODCALLTYPE HostControl::SetAppDomainManager( 
											DWORD dwAppDomainID,
											IUnknown *pUnkAppDomainManager)
{
  // Not completely sure if something needs to be done here.
	return S_OK;
}

//
// IUnknown
//
HRESULT STDMETHODCALLTYPE HostControl::QueryInterface(const IID &iid,void **ppv)
{
	if (!ppv) return E_POINTER;
	*ppv=this;
	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE HostControl::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

ULONG STDMETHODCALLTYPE HostControl::Release()
{
	if(0 == InterlockedDecrement(&m_cRef)){
		delete this;
		return 0;
	}
	return m_cRef;
}