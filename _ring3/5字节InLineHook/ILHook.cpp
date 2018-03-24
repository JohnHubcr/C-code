#include "stdafx.h"
#include "ILHook.h"





CILHook::CILHook(){
	m_pfnOrig = NULL;
	ZeroMemory(m_bOldBytes,5);
	ZeroMemory(m_bNewBytes,5);
}

CILHook::~CILHook(){
	UnHook();
	m_pfnOrig = NULL;
	ZeroMemory(m_bOldBytes,5);
	ZeroMemory(m_bNewBytes,5);
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
		ReadProcessMemory(GetCurrentProcess(),m_pfnOrig,m_bOldBytes,5,0);
		//����jmpָ��
		m_bNewBytes[0] = '\xe9';
		//jmp����ĵ�λ���㹫ʽΪ��Ŀ���ַ-ԭ��ַ-5
		*(DWORD *)(m_bNewBytes + 1) = (DWORD)pfnHookFunc - (DWORD)m_pfnOrig - 5;

		WriteProcessMemory(GetCurrentProcess(),m_pfnOrig,m_bNewBytes,5,0);
		bRet = TRUE;
	}
	return bRet;
}VOID CILHook::UnHook()
{
	if (m_pfnOrig != 0)
	{
		DWORD dwNum = 0;
		WriteProcessMemory(GetCurrentProcess(), m_pfnOrig, m_bOldBytes, 5, 0);
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
		WriteProcessMemory(GetCurrentProcess(), m_pfnOrig, m_bNewBytes, 5, 0);

		bRet = TRUE;
	}

	return bRet;
}