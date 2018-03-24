// InLineHookMessageBoxA.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include<windows.h>
#include "ILHook.h"

CILHook MsgHook;

int WINAPI MyMessageBoxA(HWND hWnd,LPCSTR lpText,LPCSTR lpCaption,UINT uType)
{
	MsgHook.UnHook();
	MessageBoxA(hWnd,"Hook����",lpCaption,uType);
	MessageBoxA(hWnd, lpText, lpCaption, uType);
	MsgHook.ReHook();
	return 0;
}
int main(int argc, char* argv[])
{
	MessageBoxA(NULL,"��������1","test",MB_OK);
	MsgHook.Hook(L"User32.dll","MessageBoxA",(PROC)MyMessageBoxA);
	MessageBoxA(NULL,"��HOOK��1","test",MB_OK);
	MessageBoxA(NULL,"��HOOK��2","test",MB_OK);
	MsgHook.UnHook();
	MessageBoxA(NULL, "��������2", "test", MB_OK);
	return 0;
}

