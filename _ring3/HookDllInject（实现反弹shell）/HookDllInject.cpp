// HookDllInject.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <tlhelp32.h>
//ͨ������ID��ȡ�߳�ID
DWORD getThreadID(DWORD pid)
{
	puts("��ȡ�߳�ID");
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h != INVALID_HANDLE_VALUE)
	{
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		if (Thread32First(h, &te))
		{
			do
			{
				if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
				{
					if (te.th32OwnerProcessID == pid)
					{
						HANDLE hThread = OpenThread(READ_CONTROL, FALSE, te.th32ThreadID);
						if (!hThread)
						{
							puts("���ܵõ��߳̾��");
						}
						else
						{

							return te.th32ThreadID;
						}
					}
				}
			} while (Thread32Next(h, &te));
		}
	}
	CloseHandle(h);
	return (DWORD)0;
}

//ע�뺯��
int processInject(int pid)
{
	DWORD processID = (DWORD)pid;

	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	//��ָ������
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
		}
	}

	_tprintf(TEXT("��ʼע�뵽���� %s PID: %u\n"), szProcessName, processID);
	//��ȡ�߳�ID
	DWORD threadID = getThreadID(processID);

	printf("ʹ���߳� ID %u\n", threadID);

	if (threadID == (DWORD)0)
	{
		puts("�Ҳ����߳�");
		return -1;
	}
	//�����������ڷ������ӵ�DLL
	HMODULE dll = LoadLibrary(L"inject2.dll");
	if (dll == NULL)
	{
		puts("�Ҳ���DLL");
		return -1;
	}
	//��ȡdll��inject���������ڷ������ӣ�
	HOOKPROC addr = (HOOKPROC)GetProcAddress(dll, "inject");
	if (addr == NULL)
	{
		puts("�Ҳ���DLL�еĺ���");
		return -1;
	}
	//����ȫ�ֹ��ӣ�������������Ϣʱ����inject����
	HHOOK handle = SetWindowsHookEx(WH_KEYBOARD, addr, dll, threadID);

	if (handle == NULL)
	{
		puts("����HOOK������Ϣ");
	}
	getchar();
	getchar();
	getchar();
	UnhookWindowsHookEx(handle);
	return 0;
}

int main(int argc, char* argv)
{

	int pid;
	puts("��Ҫע���̵߳�ID��?");
	scanf_s("%u", &pid);
	printf("����PID: %u\n", pid);
	int result = processInject(pid);
	if (result == -1)
	{
		puts("���ܱ�ע��");
	}
	else
	{
		puts("ע��ɹ�");
	}
	getchar();

}