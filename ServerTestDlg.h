
// ServerTestDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#define WM_SOCKET (WM_USER+1009)


// CServerTestDlg 对话框
class CServerTestDlg : public CDialogEx
{
// 构造
public:
	CServerTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERVERTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ownip;
	CEdit m_otherip;
	CWinThread *p_thread;//多线程的句柄
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
