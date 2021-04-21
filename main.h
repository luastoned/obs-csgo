#pragma once

#include <Windows.h>
#include <iostream>
#include <stdint.h>
#include <psapi.h>
#include <stdio.h>
#include <intrin.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <codecvt>

namespace NPath
{
	const static std::wstring kwstHookPath = L"C:\\Windows\\obs-studio-hook\\";
	const static std::wstring kwstHookPathX86 = L"C:\\Windows\\obs-studio-hook\\graphics-hook32.dll";
	const static std::wstring kwstHookPathX64 = L"C:\\Windows\\obs-studio-hook\\graphics-hook64.dll";
}

namespace NOBS
{
	typedef size_t(*OsWcsToUtf8Ptr_t)(const wchar_t*, size_t, char**);
	OsWcsToUtf8Ptr_t pOsWcsToUtf8Ptr = 0;

	typedef char*(*GetHookPath_t)(bool);
	GetHookPath_t pGetHookPath = 0;
	uintptr_t uiGetHookPath = 0;

	char *Hook_GetHookPath(bool bIsX64)
	{
		char *chpReturn = 0;
		pOsWcsToUtf8Ptr((bIsX64 ? NPath::kwstHookPathX64.c_str() : NPath::kwstHookPathX86.c_str()), 0, &chpReturn);

		return chpReturn;
	}
}

namespace NModule
{
	template<typename T>
	inline bool bCaptureModule(T *pAddress, MODULEINFO *pModuleInfo, const std::wstring &kwstrModule)
	{
		if (!pAddress || !pModuleInfo || kwstrModule.empty())
		{
			return false;
		}

		HMODULE hModule;

		while (true)
		{
			hModule = GetModuleHandle(kwstrModule.c_str());
			if (hModule)
			{
				break;
			}

			Sleep(15);
		}

		*pAddress = (T)hModule;

		if (!GetModuleInformation(GetCurrentProcess(), hModule, pModuleInfo, sizeof(MODULEINFO)))
		{
			return false;
		}

		return true;
	}
}

namespace NPattern
{
	inline bool bCompare(const uint8_t *kuchpData, const uint8_t *kuchpSignature, const char *kchpMask)
	{
		for (; *kchpMask; ++kchpMask, ++kuchpData, ++kuchpSignature)
		{
			if ((*kchpMask == 'x') && (*kuchpData != *kuchpSignature))
			{
				return false;
			}
		}

		return ((*kchpMask) == 0);
	}

	inline uintptr_t uiFindPattern(uintptr_t uiAddress, uintptr_t uiLen, const uint8_t *kuchpSignature, const char *kchpMask)
	{
		for (uintptr_t i = 0; i < uiLen; i++)
		{
			if (bCompare((uint8_t *)(uiAddress + i), kuchpSignature, kchpMask))
			{
				return (uintptr_t)(uiAddress + i);
			}
		}

		return 0;
	}
}

namespace NString
{
	inline std::wstring wstStringToWString(const std::string &kstrInput)
	{
		std::wstring_convert< std::codecvt_utf8_utf16< wchar_t > > Convert;
		return Convert.from_bytes(kstrInput);
	}

	inline std::string stWStringToString(const std::wstring &kwstrInput)
	{
		std::wstring_convert< std::codecvt_utf8_utf16< wchar_t > > Convert;
		return Convert.to_bytes(kwstrInput);
	}
}

namespace NFile
{
	inline bool bDirectory(const char *kchpPath)
	{
		struct stat qInfo = { 0 };
		return ((stat(kchpPath, &qInfo) == 0) && (qInfo.st_mode & S_IFDIR));
	}

	inline bool bReadFile(const std::wstring &kwstrPath, std::vector< uint8_t > &vruchBytes, uint32_t *uipLastError)
	{
		bool bReturn = false;

		HANDLE pFile = CreateFile(kwstrPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (pFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwFileSizeHigh = 0x0;
			DWORD dwFileSize = GetFileSize(pFile, &dwFileSizeHigh);
			if (dwFileSize != INVALID_FILE_SIZE)
			{
				vruchBytes.resize(dwFileSize);

				DWORD dwNumberOfBytesRead = 0x0;
				if (ReadFile(pFile, (void*)&vruchBytes[0], dwFileSize, &dwNumberOfBytesRead, NULL))
				{
					bReturn = true;
				}
				else
				{
					vruchBytes.clear();
				}
			}

			CloseHandle(pFile);
		}

		if (uipLastError)
		{
			*uipLastError = GetLastError();
		}

		return bReturn;
	}
}

namespace NDetour
{
	inline void *pDetourFunction(void *pOrigin, void *pTarget, size_t ullCommandLength)
	{
		uint8_t *uchpTramp = (uint8_t*)malloc(ullCommandLength + 12);
		if (!uchpTramp)
		{
			return 0;
		}

		DWORD dwOldProtect;
		VirtualProtect(uchpTramp, (ullCommandLength + 12), PAGE_EXECUTE_READWRITE, &dwOldProtect);

		VirtualProtect(pOrigin, ullCommandLength, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		memcpy(uchpTramp, pOrigin, ullCommandLength);

		uchpTramp += ullCommandLength;

		uchpTramp[0] = 0x48; // v
		uchpTramp[1] = 0xB8; // mov rax, ...
		uchpTramp[0xA] = 0xFF; // v
		uchpTramp[0xB] = 0xE0; // jmp rax
		*(uintptr_t*)(uchpTramp + 2) = (uintptr_t)pOrigin + ullCommandLength;

		uint8_t *uchpOriginBytePtr = (uint8_t *)pOrigin;

		uchpOriginBytePtr[0] = 0x48; // v
		uchpOriginBytePtr[1] = 0xB8; // mov rax, ...
		uchpOriginBytePtr[0xA] = 0xFF; // v
		uchpOriginBytePtr[0xB] = 0xE0; // jmp rax
		*(uintptr_t*)(uchpOriginBytePtr + 2) = (uintptr_t)pTarget;

		for (uint8_t i = 12; i < ullCommandLength; i++)
		{
			uchpOriginBytePtr[i] = 0x90;
		}

		VirtualProtect(pOrigin, ullCommandLength, dwOldProtect, &dwOldProtect);

		return (uchpTramp - ullCommandLength);
	}

	inline void RemoveDetourFunction(void *pOrigin, void *pTrampoline, uint8_t uchCommandLength)
	{
		DWORD dwOldProtect;
		VirtualProtect(pOrigin, uchCommandLength, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		memcpy(pOrigin, pTrampoline, uchCommandLength);
		VirtualProtect(pOrigin, uchCommandLength, dwOldProtect, &dwOldProtect);

		free(pTrampoline);
	}
}