
// DLLע����Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DLLע����.h"
#include "DLLע����Dlg.h"
#include "afxdialogex.h"
#include <windows.h>
#include <TlHelp32.h>
#include <afxpriv.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDLLע����Dlg �Ի���



CDLLע����Dlg::CDLLע����Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDLLע����Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDLLע����Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDLLע����Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CDLLע����Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDLLע����Dlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDLLע����Dlg ��Ϣ�������

BOOL CDLLע����Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDLLע����Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDLLע����Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDLLע����Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDLLע����Dlg::OnBnClickedOk()
{
	CString szDllName;
	CString szProcessName;
	DWORD dwpid = 0;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

//	char szDllName[MAX_PATH] = { 0 };
	//char *szProcessNameC;
	GetDlgItemText(IDC_EDIT1,szDllName);
	GetDlgItemText(IDC_EDIT2, szProcessName);

	
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, szProcessName, -1, NULL, NULL, 0, NULL);
	char *szProcessNameC = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, szProcessName, -1, szProcessNameC, dwNum, 0, NULL);
	
	DWORD dwNum1 = WideCharToMultiByte(CP_OEMCP, NULL, szDllName, -1, NULL, NULL, 0, NULL);
	char *szDllNameC = new char[dwNum1];
	WideCharToMultiByte(CP_OEMCP, NULL, szDllName, -1, szDllNameC, dwNum1, 0, NULL);
	//��ȡ����PID
	dwpid = GetProcId(szProcessNameC);


	//ע�����
	InjectDll(dwpid, szDllName);


}
DWORD CDLLע����Dlg::GetProcId(char *szProcessName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe))
	{
		MessageBox((LPWSTR)pe.dwSize);
		return 0;
	}

	while (Process32Next(hSnapshot, &pe))
	{
		CString a = pe.szExeFile;
		DWORD dwNum2 = WideCharToMultiByte(CP_OEMCP, NULL, a, -1, NULL, NULL, 0, NULL);
		char *b = new char[dwNum2];
		WideCharToMultiByte(CP_OEMCP, NULL, a, -1, b, dwNum2, 0, NULL);
		if (!strcmp(szProcessName, b))
		{
			return pe.th32ProcessID;
		}
	}
	return 0;
}
VOID CDLLע����Dlg::InjectDll(DWORD dwPid, CString szDllName)
{
	enableDebugPriv();
	if (dwPid == 0 || lstrlen(szDllName)==0)
	{
		return;
	}
	//��Ŀ�����
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwPid);
	if (hProcess ==NULL)
	{
		return;
	}
	//������ע��DLL�ļ�����·���ĳ���
	DWORD dwNum3 = WideCharToMultiByte(CP_OEMCP, NULL, szDllName, -1, NULL, NULL, 0, NULL);
	char *szProcessNameC = new char[dwNum3];
	WideCharToMultiByte(CP_OEMCP, NULL, szDllName, -1, szProcessNameC, dwNum3, 0, NULL);
	//int nDllLen = lstrlen(szDllName) + sizeof(CString);
	//��Ŀ���������һ�鳤��ΪnDllLen��С���ڴ�
	LPVOID pDllAddr = VirtualAllocEx(hProcess, NULL, strlen(szProcessNameC), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (pDllAddr == NULL)
	{
		CloseHandle(hProcess);
		return;
	}
	//DWORD dwWriteNum = 0;
	//����ע��DLL�ļ�������·��д����Ŀ�����������Ŀռ���
	WriteProcessMemory(hProcess, pDllAddr, szProcessNameC, strlen(szProcessNameC), NULL);
	//���loadlibraryA()�����ĵ�ַ
	HMODULE hModule = GetModuleHandle(L"kernel32.dll");
	LPVOID lpBaseAddress = (LPVOID)GetProcAddress(hModule, "LoadLibraryA");
	//FARPROC pFunAddr = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	//����Զ���߳�
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpBaseAddress, pDllAddr, NULL, NULL);
	WaitForSingleObject(hThread,INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
}
BOOL CDLLע����Dlg::enableDebugPriv()
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

void CDLLע����Dlg::OnBnClickedCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString szDllName;
	CString szProcessName;
	DWORD dwpid = 0;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	//	char szDllName[MAX_PATH] = { 0 };
	//char *szProcessNameC;
	GetDlgItemText(IDC_EDIT1, szDllName);
	GetDlgItemText(IDC_EDIT2, szProcessName);


	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, szProcessName, -1, NULL, NULL, 0, NULL);
	char *szProcessNameC = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, szProcessName, -1, szProcessNameC, dwNum, 0, NULL);


	//��ȡ����PID
	dwpid = GetProcId(szProcessNameC);
	UnInjectDll(dwpid, szDllName);
	CDialogEx::OnCancel();
}
VOID CDLLע����Dlg::UnInjectDll(DWORD dwpid, CString szDllName)
{
	CString a1 = szDllName;
	DWORD dwNum5 = WideCharToMultiByte(CP_OEMCP, NULL, a1, -1, NULL, NULL, 0, NULL);
	char *b1 = new char[dwNum5];
	WideCharToMultiByte(CP_OEMCP, NULL, a1, -1, b1, dwNum5, 0, NULL);
	if (dwpid == 0 || lstrlen(szDllName) == 0)
	{
		return;
	}
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwpid);
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(me32);
	//����ƥ��Ľ�������
	BOOL bRet = Module32First(hSnap,&me32);
	while (bRet)
	{
		CString a = me32.szExePath;
		DWORD dwNum4 = WideCharToMultiByte(CP_OEMCP, NULL, a, -1, NULL, NULL, 0, NULL);
		char *b = new char[dwNum4];
		WideCharToMultiByte(CP_OEMCP, NULL, a, -1, b, dwNum4, 0, NULL);
		if (!strcmp(b, b1))
		{
			break;
		}
		bRet = Module32Next(hSnap,&me32);
	}
	CloseHandle(hSnap);
	char * pFunName = "FreeLibrary";
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwpid);
	if (hProcess == NULL)
	{
		return;
	}
	FARPROC pFunAddr = GetProcAddress(GetModuleHandle(L"kernel32.dll"),pFunName);
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunAddr,me32.hModule,0,NULL);
	WaitForSingleObject(hThread,INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
}
