#include "main.h"

void Main(void)
{
	HMODULE hObs;
	MODULEINFO qObs = { 0 };
	if (!NModule::bCaptureModule< HMODULE >(&hObs, &qObs, L"obs.dll"))
	{
		return;
	}

	NOBS::pOsWcsToUtf8Ptr = (NOBS::OsWcsToUtf8Ptr_t)GetProcAddress(hObs, "os_wcs_to_utf8_ptr");
	if (!NOBS::pOsWcsToUtf8Ptr)
	{
		return;
	}

	uintptr_t uiWinCapture;
	MODULEINFO qWinCapture = { 0 };
	if (!NModule::bCaptureModule< uintptr_t >(&uiWinCapture, &qWinCapture, L"win-capture.dll"))
	{
		return;
	}

	const uint8_t kuchSignature[34] = { 0x48, 0x89, 0x5C, 0x24, 0x0, 0x57, 0x48, 0x81, 0xEC, 0x0, 0x0, 0x0, 0x0, 0x48, 0x8B, 0x05, 0x0, 0x0, 0x0, 0x0, 0x48, 0x33, 0xC4, 0x48, 0x89, 0x84, 0x24, 0x0, 0x0, 0x0, 0x0, 0x45, 0x33, 0xC9 };
	const char kchMask[35] = "xxxx?xxxx????xxx????xxxxxxx????xxx";

	NOBS::uiGetHookPath = NPattern::uiFindPattern(uiWinCapture, qWinCapture.SizeOfImage, kuchSignature, kchMask);
	if (!NOBS::uiGetHookPath)
	{
		return;
	}

	char *chpHookPathX86 = ((NOBS::GetHookPath_t)NOBS::uiGetHookPath)(false);
	if (!chpHookPathX86)
	{
		return;
	}

	std::wstring wstHookPathX86 = NString::wstStringToWString(std::string(chpHookPathX86, strlen(chpHookPathX86) + 1));

	char *chpHookPathX64 = ((NOBS::GetHookPath_t)NOBS::uiGetHookPath)(true);
	if (!chpHookPathX64)
	{
		return;
	}

	std::wstring wstHookPathX64 = NString::wstStringToWString(std::string(chpHookPathX64, strlen(chpHookPathX64) + 1));

	if (!NFile::bDirectory(NString::stWStringToString(NPath::kwstHookPath).c_str()))
	{
		if (!CreateDirectory(NPath::kwstHookPath.c_str(), 0))
		{
			int32_t iMessage = MessageBox(GetActiveWindow(), L"Please run OBS as administrator to create the directory", L"Information", MB_OK);
			if (iMessage == IDOK)
			{
				ExitProcess(0);
			}
		}
	}

	std::vector< uint8_t > vuchBytesX86[2];
	std::vector< uint8_t > vuchBytesX64[2];

	uint32_t uiLastError;

	if (NFile::bReadFile(wstHookPathX86, vuchBytesX86[0], &uiLastError) && NFile::bReadFile(wstHookPathX64, vuchBytesX64[0], &uiLastError))
	{
		if ((NFile::bReadFile(NPath::kwstHookPathX86, vuchBytesX86[1], &uiLastError) || uiLastError == ERROR_FILE_NOT_FOUND) &&
			((vuchBytesX86[0].size() != vuchBytesX86[1].size()) || (memcmp((void*)&vuchBytesX86[0][0], (void*)&vuchBytesX86[1][0], vuchBytesX86[0].size()) != 0)))
		{
			if (!CopyFile(wstHookPathX86.c_str(), NPath::kwstHookPathX86.c_str(), FALSE))
			{
				int32_t iMessage = MessageBox(GetActiveWindow(), L"Please run OBS as administrator to copy the files", L"Information", MB_OK);
				if (iMessage == IDOK)
				{
					ExitProcess(0);
				}
			}
		}

		if ((NFile::bReadFile(NPath::kwstHookPathX64, vuchBytesX64[1], &uiLastError) || uiLastError == ERROR_FILE_NOT_FOUND) &&
			((vuchBytesX64[0].size() != vuchBytesX64[1].size()) || (memcmp((void*)&vuchBytesX64[0][0], (void*)&vuchBytesX64[1][0], vuchBytesX64[0].size()) != 0)))
		{
			if (!CopyFile(wstHookPathX64.c_str(), NPath::kwstHookPathX64.c_str(), FALSE))
			{
				int32_t iMessage = MessageBox(GetActiveWindow(), L"Please run OBS as administrator to copy the files", L"Information", MB_OK);
				if (iMessage == IDOK)
				{
					ExitProcess(0);
				}
			}
		}
	}
	else
	{
		///...
	}

	NOBS::pGetHookPath = (NOBS::GetHookPath_t)NDetour::pDetourFunction((void*)NOBS::uiGetHookPath, (void*)NOBS::Hook_GetHookPath, 13);
	if (!NOBS::pGetHookPath)
	{
		return;
	}

	return;
}

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Main, 0, 0, 0);
		if (!hThread)
		{
			return FALSE;
		}

		CloseHandle(hThread);
	}

	return TRUE;
}

extern "C" __declspec(dllexport) bool LoadPlugin(void)
{
	return true;
}

extern "C" __declspec(dllexport) void UnloadPlugin(void)
{
	if (NOBS::pGetHookPath)
	{
		NDetour::RemoveDetourFunction((void*)NOBS::uiGetHookPath, (void*)NOBS::pGetHookPath, 13);
	}
}

extern "C" __declspec(dllexport) const wchar_t *GetPluginName(void)
{
	return L"OBS-CS:GO";
}

extern "C" __declspec(dllexport) const wchar_t *GetPluginDescription(void)
{
	return L"OBS-CS:GO";
}