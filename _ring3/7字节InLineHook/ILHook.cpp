#include "stdafx.h"
#include "ILHook.h"





CILHook::CILHook(){
	m_pfnOrig = NULL;
	ZeroMemory(m_bOldBytes,7);
	ZeroMemory(bJmpCode, 7);
}

CILHook::~CILHook(){
	UnHook();
	m_pfnOrig = NULL;
	ZeroMemory(m_bOldBytes,7);
	ZeroMemory(bJmpCode, 7);
}

BOOL CILHook::Hook(LPCWSTR pszModuleName, LPSTR pszFuncName, PROC pfnHookFunc)
{
	BOOL bRet = FALSE;
	//��ȡָ��ģ���еĺ�����ַ
	m_pfnOrig = (PROC)GetProcAddress(GetModuleHandle(pszModuleName),pszFuncName);
	if (m_pfnOrig != NULL)
	{
		//����õ�ַ��5�ֽڵ�����
		DWORD dwNum = 0;
		ReadProcessMemory(GetCurrentProcess(),m_pfnOrig,m_bOldBytes,7,0);
		//����jmpָ��
		//bJmpCode[7] = {'\xb8','\0','\0','\0','\0','\xFF','\xE0'};
		byte byteData[4]; // byte���飬ʾ��
		DWORD dwData = (DWORD)pfnHookFunc; // ������ַ
		//ͨ��λ��������ȡ��Ŀ�ĵ�ַ����λ
		byteData[0] = (dwData & 0xFF000000) >> 24; 
		byteData[1] = (dwData & 0x00FF0000) >> 16; 
		byteData[2] = (dwData & 0x0000FF00) >> 8; 
		byteData[3] = (dwData & 0x000000FF); 
		bJmpCode[0] = '\xb8';
		bJmpCode[1] = byteData[3];
		bJmpCode[2] = byteData[2];
		bJmpCode[3] = byteData[1];
		bJmpCode[4] = byteData[0];
		bJmpCode[5] = '\xFF';
		bJmpCode[6] = '\xE0';
		
		WriteProcessMemory(GetCurrentProcess(), m_pfnOrig, bJmpCode, 7, 0);
		bRet = TRUE;
	}
	return bRet;
}VOID CILHook::UnHook()
{
	if (m_pfnOrig != 0)
	{
		DWORD dwNum = 0;
		WriteProcessMemory(GetCurrentProcess(), m_pfnOrig, m_bOldBytes, 7, 0);
	}
}

/*
�������ƣ�ReHook
�������ܣ����¶Ժ������йҹ�
*/
BOOL CILHook::ReHook()
{
	BOOL bRet = FALSE;

	if (m_pfnOrig != 0)
	{
		DWORD dwNum = 0;
		WriteProcessMemory(GetCurrentProcess(), m_pfnOrig, bJmpCode, 7, 0);

		bRet = TRUE;
	}

	return bRet;
}