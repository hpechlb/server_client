
// ServerTestDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#define WM_SOCKET (WM_USER+1009)


// CServerTestDlg �Ի���
class CServerTestDlg : public CDialogEx
{
// ����
public:
	CServerTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SERVERTEST_DIALOG };

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
	CEdit m_ownip;
	CEdit m_otherip;
	CWinThread *p_thread;//���̵߳ľ��
	afx_msg void OnBnClickedBind();
	CStatic m_state;
	SOCKET m_local;
	SOCKET m_accept;
	afx_msg void OnBnClickedConnect();
	CButton m_bind;
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedStop();
	CListBox m_list;
	void UpdateList(CString str);
protected:
	afx_msg LRESULT OnSocket(WPARAM wParam, LPARAM lParam);
public:
	CButton m_connect;
	CEdit m_acceptsocket;
	CEdit m_sendsocket;
	CEdit m_Sendspeed;
	CEdit m_Receivespeed;
	CButton m_SendButton;
};
