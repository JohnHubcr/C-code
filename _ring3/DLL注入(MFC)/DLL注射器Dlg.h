
// DLLע����Dlg.h : ͷ�ļ�
//

#pragma once


// CDLLע����Dlg �Ի���
class CDLLע����Dlg : public CDialogEx
{
// ����
public:
	CDLLע����Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DLL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedOk();
	DWORD GetProcId(char *szProcessName);
	VOID InjectDll(DWORD dwPid, CString szDllName);
	BOOL enableDebugPriv();
	afx_msg void OnBnClickedCancel();
	VOID UnInjectDll(DWORD dwpid, CString szDllName);

};
