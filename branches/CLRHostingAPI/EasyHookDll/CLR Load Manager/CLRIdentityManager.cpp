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


#include "CLRstdafx.h"
#include "CLRIdentityManager.h"

CLRIdentityManager::CLRIdentityManager()
{
	// use GetRealProcAddress to get a pointer to GetCLRIdentityManager
	HRESULT hr = GetRealProcAddress("GetCLRIdentityManager", (void **)&m_pIdentityManagerProc);
}

CLRIdentityManager::~CLRIdentityManager()
{
}

ICLRAssemblyIdentityManager *CLRIdentityManager::GetCLRIdentityManager()
{
	assert(m_pIdentityManagerProc);
	ICLRAssemblyIdentityManager *pIdentityManager = NULL;

	// call GetCLRIdentityManager to get a pointer to ICLRAssemblyIdentityManager
	HRESULT hr = (m_pIdentityManagerProc)(IID_ICLRAssemblyIdentityManager, (IUnknown **)&pIdentityManager);
	assert(pIdentityManager);
	assert(SUCCEEDED(hr));

	return pIdentityManager;
}
