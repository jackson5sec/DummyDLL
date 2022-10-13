//===============================================================================================//
// This is a stub for the actuall functionality of the DLL.
//===============================================================================================//
#include "ReflectiveLoader.h"
#include "strsafe.h"
#include <stdlib.h>
#include <string>
#include <fstream>
// Note: REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR and REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN are
// defined in the project properties (Properties->C++->Preprocessor) so as we can specify our own 
// DllMain and use the LoadRemoteLibraryR() API to inject this DLL.
// You can use this value as a pseudo hinstDLL value (defined and set via ReflectiveLoader.c)
extern "C" HINSTANCE hAppInstance;

//Check if in Elevated Token Context (Admin vs User)
BOOL IsElevated() {
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return fRet;
}
//===============================================================================================//
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	DWORD pid = 0;
	TCHAR path[MAX_PATH];
	TCHAR user[MAX_PATH];
	TCHAR context[MAX_PATH];
	DWORD size_user = sizeof(user);
	TCHAR msg[400];
	BOOL bReturnValue = TRUE;
	std::wstring logpath;
	std::wofstream outfile;
	wchar_t* publicfolder;
	LPCWSTR Path;

	switch (dwReason)
	{
	case DLL_QUERY_HMODULE:
		if (lpReserved != NULL)
			*(HMODULE *)lpReserved = hAppInstance;
		break;
	case DLL_PROCESS_ATTACH:
		hAppInstance = hinstDLL;

		// Public folder (C:\\Users\\Public)
		publicfolder = _wgetenv(L"PUBLIC");
		logpath = publicfolder;
		logpath += std::wstring(L"\\dummy.log");
		Path = logpath.c_str();

		pid = GetCurrentProcessId();
		GetModuleFileName(NULL, path, MAX_PATH);
		GetUserName(user, &size_user);
		BOOL isAdmin;
		isAdmin = IsElevated();
		if (isAdmin) {
			wcscpy(context, TEXT("Elevated*"));
		}
		else {
			wcscpy(context, TEXT("User"));
		}

		// Write log %PUBLIC%\dummy.log
		outfile.open(Path, std::ios_base::app);
		outfile << TEXT("PID:") << pid << std::endl;
		outfile << TEXT("Process:") << path << std::endl;
		outfile << TEXT("User:") << user << std::endl;
		outfile << TEXT("Context:") << context << std::endl << std::endl;
		outfile.close();


	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return bReturnValue;
}