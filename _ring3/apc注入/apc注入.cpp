// apcע��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "stdafx.h"
#include <windows.h>
#include <Tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>





typedef struct _THREADLIST
{
	DWORD dwThreadId;
	_THREADLIST *pNext;
}THREADLIST;
int q = 0;
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
//ͨ���������ֻ�ȡ����ID
DWORD GetProcessID(const char *szProcessName)
{
	//PROCESSENTRY32�������<Tlhelp32.h>��
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	//�����߳̿���
	HANDLE SnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (SnapshotHandle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if (!Process32First(SnapshotHandle, &pe32))
	{
		return 0;
	}

	do
	{
		if (!_strnicmp(szProcessName, pe32.szExeFile, strlen(szProcessName)))
		{
			printf("%s��PID��:%d\n", pe32.szExeFile, pe32.th32ProcessID);
			return pe32.th32ProcessID;
		}
		//Process32Next��һ�����̻�ȡ���������������ú���CreateToolhelp32Snapshot()��õ�ǰ���н��̵Ŀ��պ�, ���ǿ�������Process32Next�����������һ�����̵ľ��
	} while (Process32Next(SnapshotHandle, &pe32));

	return 0;
}
//����Ĳ������
THREADLIST* InsertThreadId(THREADLIST *pdwTidListHead, DWORD dwTid)
{
	THREADLIST *pCurrent = NULL;
	THREADLIST *pNewMember = NULL;

	if (pdwTidListHead == NULL)
	{
		return NULL;
	}
	pCurrent = pdwTidListHead;

	while (pCurrent != NULL)
	{

		if (pCurrent->pNext == NULL)
		{
			// ��λ���������һ��Ԫ��
			pNewMember = (THREADLIST *)malloc(sizeof(THREADLIST));

			if (pNewMember != NULL)
			{
				pNewMember->dwThreadId = dwTid;
				pNewMember->pNext = NULL;
				pCurrent->pNext = pNewMember;
				return pNewMember;
			}
			else
			{
				return NULL;
			}
		}
		pCurrent = pCurrent->pNext;
	}

	return NULL;
}
//ö�ٽ����������߳�ID��������������
int EnumThreadID(DWORD dwPID, THREADLIST * pdwTidList)
{
	int i = 0;

	THREADENTRY32 te32 = { 0 };
	te32.dwSize = sizeof(THREADENTRY32);

	HANDLE SnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwPID);

	if (SnapshotHandle != INVALID_HANDLE_VALUE)
	{
		if (Thread32First(SnapshotHandle, &te32))
		{
			do
			{
				if (te32.th32OwnerProcessID == dwPID)
				{
					if (pdwTidList->dwThreadId == 0)
					{
						pdwTidList->dwThreadId = te32.th32ThreadID;
					}
					else
					{
						if (NULL == InsertThreadId(pdwTidList, te32.th32ThreadID))
						{
							printf("����ʧ��!\n");
							return 0;
						}
					}

				}
			} while (Thread32Next(SnapshotHandle, &te32));
		}
	}

	return 0;
}

//ע�뺯��
DWORD Inject(HANDLE hProcess, THREADLIST *pThreadIdList)
{
	THREADLIST *pCurrentThreadId = pThreadIdList;

	const char szInjectModName[] = "C:\\mydll.dll";
	DWORD dwLen = strlen(szInjectModName) + 1;

	PVOID param = VirtualAllocEx(hProcess,
		NULL, dwLen, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	UINT_PTR LoadLibraryAAddress = (UINT_PTR)GetProcAddress(GetModuleHandle("Kernel32.dll"), "LoadLibraryA");

	if (param != NULL)
	{
		SIZE_T dwRet;
		if (WriteProcessMemory(hProcess, param, (LPVOID)szInjectModName, dwLen, &dwRet))
		{
			while (pCurrentThreadId)
			{
				HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, pCurrentThreadId->dwThreadId);

				if (hThread != NULL)
				{
					//ע��DLL��ָ������
					QueueUserAPC((PAPCFUNC)LoadLibraryAAddress, hThread, (ULONG_PTR)param);
					q++;

				}
				pCurrentThreadId = pCurrentThreadId->pNext;
			}
		}
	}
	return 0;
}


int main()
{
	enableDebugPriv();
	THREADLIST *pThreadIdHead = NULL;
	pThreadIdHead = (THREADLIST *)malloc(sizeof(THREADLIST));
	if (pThreadIdHead == NULL)
	{
		printf("����ʧ��");
		return 0;
	}

	ZeroMemory(pThreadIdHead, sizeof(THREADLIST));

	DWORD dwProcessID = 0;

	if ((dwProcessID = GetProcessID("notepad.exe")) == 0)
	{
		printf("����ID��ȡʧ��!\n");
		return 0;
	}

	EnumThreadID(dwProcessID, pThreadIdHead);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);

	if (hProcess == NULL)
	{
		printf("�򿪽���ʧ��");
		return 1;
	}

	Inject(hProcess, pThreadIdHead);
	return 0;
}