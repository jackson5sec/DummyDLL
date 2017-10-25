//===============================================================================================//
// This is a stub for the actuall functionality of the DLL.
//===============================================================================================//
#include "ReflectiveLoader.h"
#include "strsafe.h"

// Note: REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR and REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN are
// defined in the project properties (Properties->C++->Preprocessor) so as we can specify our own 
// DllMain and use the LoadRemoteLibraryR() API to inject this DLL.

extern "C" __declspec(dllexport) DWORD CallFunc(char *lpParam)
{
	MessageBoxA(NULL, lpParam, "Reflective Dll Injection", MB_OK | MB_SYSTEMMODAL);
	return 1;
}

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
	TCHAR path[100];
	TCHAR user[100];
	DWORD size_user = sizeof(user);
	TCHAR msg[400];
	BOOL bReturnValue = TRUE;
	switch (dwReason)
	{
	case DLL_QUERY_HMODULE:
		if (lpReserved != NULL)
			*(HMODULE *)lpReserved = hAppInstance;
		break;
	case DLL_PROCESS_ATTACH:
		hAppInstance = hinstDLL;
		pid = GetCurrentProcessId();
		GetModuleFileName(NULL, path, 1000);
		GetUserName(user, &size_user);
		StringCbPrintf(msg, 400, TEXT("%s %u %s %s %s %s"), TEXT("PID:"), pid, TEXT("\nProcess:"), path, TEXT("\nUser: "), user);

		BOOL isAdmin;
		isAdmin = IsElevated();
		if (isAdmin) {
			MessageBox(NULL, msg, TEXT("Elevated*"), MB_OK | MB_SYSTEMMODAL);
			break;
		}
		else {
			MessageBox(NULL, msg, TEXT("User"), MB_OK | MB_SYSTEMMODAL);
			break;
		}
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return bReturnValue;
}