
// ServerTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerTest.h"
#include "ServerTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  SOCKETSIZE    0x20000

int socket_type=-1;

char* SocketGetBuff;

bool SendFlag=false;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CServerTestDlg 对话框




CServerTestDlg::CServerTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OWNIP, m_ownip);
	DDX_Control(pDX, IDC_OTHERIP, m_otherip);
	DDX_Control(pDX, IDC_STATE, m_state);
	DDX_Control(pDX, IDC_BIND, m_bind);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_CONNECT, m_connect);
	DDX_Control(pDX, IDC_EDIT3, m_acceptsocket);
	DDX_Control(pDX, IDC_EDIT4, m_sendsocket);

	m_acceptsocket.SetWindowText(_T("0KB"));
	m_sendsocket.SetWindowText(_T("0KB"));
	DDX_Control(pDX, IDC_EDIT5, m_Sendspeed);
	DDX_Control(pDX, IDC_EDIT6, m_Receivespeed);
	m_Sendspeed.SetWindowText(_T("0KB/s"));
	m_Receivespeed.SetWindowText(_T("0KB/s"));
	DDX_Control(pDX, IDC_SEND, m_SendButton);
}

BEGIN_MESSAGE_MAP(CServerTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BIND, &CServerTestDlg::OnBnClickedBind)
	ON_BN_CLICKED(IDC_CONNECT, &CServerTestDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_SEND, &CServerTestDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_STOP, &CServerTestDlg::OnBnClickedStop)
	ON_MESSAGE(WM_SOCKET, &CServerTestDlg::OnSocket)
END_MESSAGE_MAP()


// CServerTestDlg 消息处理程序

BOOL CServerTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	WSADATA wsd;										//定义WSADATA对象
	WSAStartup(MAKEWORD(2,2),&wsd);		
	//创建套接字
	m_local = socket(AF_INET, SOCK_STREAM, 0);
	unsigned long nCmd;
	int nState = ioctlsocket(m_local, FIONBIO, &nCmd);		//设置非阻塞模式
	if (nState != 0)												//设置套接字非阻塞模式失败
	{
		TRACE("设置套接字非阻塞模式失败");
	}	
	char	szHostName[128] = {0};
	gethostname(szHostName, 128);
	hostent* phostent = gethostbyname(szHostName);
	CString s = (CString)inet_ntoa(*(in_addr*)&phostent->h_addr_list[2]);
	m_ownip.SetWindowText(s);
	UpdateData(false);
	int nRet = WSAAsyncSelect(m_local, m_hWnd, WM_SOCKET, FD_ACCEPT|FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE|FD_OOB);
	if (nRet != 0)
	{
		TRACE("设置WSAAsyncSelect模型失败");
	}
	m_state.SetWindowText(_T("网络未连接状态"));
	socket_type=-1;
	//m_length.SetWindowText(_T("0"));
	if((SocketGetBuff=(char*)malloc(SOCKETSIZE*4))==NULL)
		puts("SocketGetBuff Gotanullpointer");
	else
		puts("SocketGetBuff Gotavalidpointer");

	memset(SocketGetBuff,0,SOCKETSIZE*4);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServerTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServerTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CServerTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CServerTestDlg::OnBnClickedBind()
{
	// TODO: 在此添加控件通知处理程序代码
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	unsigned short num=1234;//端口号统一设置成1234
	sockAddr.sin_port = htons(num);
	CString t;
    m_ownip.GetWindowText(t);
	char strAddr[30];
	wcstombs(strAddr,t,t.GetLength()*2);
	sockAddr.sin_addr.S_un.S_addr = inet_addr(strAddr);
	if(bind(m_local, (sockaddr*)&sockAddr, sizeof(sockAddr)) == 0
		&& listen(m_local, 5) == 0)
	{
		m_bind.EnableWindow(FALSE);
		MessageBox(_T("地址和端口绑定成功!"),_T("提示"));
	}
	else
	{
		MessageBox(_T("地址绑定失败!"), _T("提示"));
	}
}


void CServerTestDlg::OnBnClickedConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	closesocket(m_local);
	m_local = socket(AF_INET, SOCK_STREAM, 0);
	int nRet = WSAAsyncSelect(m_local, m_hWnd, WM_SOCKET, 
		FD_ACCEPT|FD_CLOSE|FD_READ|FD_WRITE|FD_CONNECT|FD_OOB);
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	unsigned short num=1234;
	sockAddr.sin_port = htons(num);
	CString ss;
	m_otherip.GetWindowText(ss);
	char strAddr[30];
	wcstombs(strAddr,ss,ss.GetLength()*2);
	sockAddr.sin_addr.S_un.S_addr = inet_addr(strAddr);
	connect(m_local, (sockaddr*)&sockAddr, sizeof(sockAddr));
	m_state.SetWindowText(_T("连接进行中....."));
}

bool GetFlag=false;
bool ConnectFlag=false;
ULONGLONG total_recv=0;

UINT SEND(LPVOID pThreadParam)//线程函数
{
	ULONGLONG total=0;
	ULONGLONG sLastSize=0;
	ULONG nRead;
	UCHAR BufferSrc=1;
	char *buffer=(char*)malloc(sizeof(char)*4*1024*1024);
	memset(buffer,0,4*1024*1024);
	int len=0;
	int count=0;
	ULONGLONG SendCount=0;
	CString sz_text;
	double a,b,c;
	ULONG Send_Current_Clock,Send_Last_Clock=0;
	char Tflag[1];
	SendFlag=true;
	CServerTestDlg *dlg=(CServerTestDlg *)pThreadParam;


	Tflag[0]=1;
	while(len!=1 && SendFlag)
	{
		if(socket_type==0){
			len=send(dlg->m_accept, Tflag, 1, MSG_OOB);
		}
		if(socket_type==1){
			len=send(dlg->m_local, Tflag, 1, MSG_OOB);
		}
		sz_text.Format(_T("开始传输"));
		dlg->UpdateList(sz_text);
	}
	len=0;
	a=clock();
	while(SendFlag)//直到全部发送完成
	{
		//ReadDataFromFpga(hDevice,buffer,BufferSrc,nRead);//读取115的数据
		if(BufferSrc!=0){
			while(count<(0x400000/SOCKETSIZE) && SendFlag){
				if(socket_type==0)
				{
					while(len!=SOCKETSIZE && SendFlag)
					{
						len=send(dlg->m_accept, buffer+count*SOCKETSIZE,SOCKETSIZE, 0);
					}
					len=0;
				}
				if(socket_type==1)
				{
					while(len!=SOCKETSIZE && SendFlag)
					{
						len=send(dlg->m_local, buffer+count*SOCKETSIZE,SOCKETSIZE, 0);
					}
					len=0;
				}
				count++;
				total=total+SOCKETSIZE;
				Send_Current_Clock=clock();
				if((Send_Current_Clock-Send_Last_Clock)>=1000)
				{
					sz_text.Format(_T("%dKB"),total/1024);
					dlg->m_sendsocket.SetWindowText(sz_text);
					//Send_Last_Clock=Send_Current_Clock;
					if(Send_Last_Clock!=0)
					{
						sz_text.Format(_T("%lfKB/s"),((double)(total-sLastSize))/(Send_Current_Clock-Send_Last_Clock)*1000/1024);
						dlg->m_Sendspeed.SetWindowText(sz_text);
					}
					sLastSize=total;
					Send_Last_Clock=Send_Current_Clock;
				}
			}
			count=0;
		}
	}
	b=clock();
	Tflag[0]=2;
	Sleep(500);
	if(socket_type==0 && ConnectFlag){
		len=send(dlg->m_accept, Tflag, 1, MSG_OOB);
	}
	if(socket_type==1 && ConnectFlag){
		len=send(dlg->m_local, Tflag, 1, MSG_OOB);
	}
	len=0;
	dlg->m_sendsocket.SetWindowText(_T("0KB"));
	dlg->m_Sendspeed.SetWindowText(_T("0KB/s"));

	c=(b-a)/CLOCKS_PER_SEC;
	CString ss;
	ss.Format(_T("发送结束：网络速度=%lf KB/s %lldKB"),total/1024/c,total/1024);  
	dlg->UpdateList(ss);
	//MessageBox(dlg->m_hWnd,ss,_T("提示"),MB_OK);
		 
    DWORD exit=0;
 
	BOOL ret=GetExitCodeThread(dlg->p_thread->m_hThread,&exit);//获取线程退出代码
	if(exit==STILL_ACTIVE) //如果进程仍在进行
	{
		dlg->p_thread->ExitInstance();//退出进程
		dlg->p_thread=NULL;
	}
	free(buffer);
	
	dlg->m_SendButton.EnableWindow(true);
	return 0;
}

void CServerTestDlg::OnBnClickedSend()
{
	// TODO: 在此添加控件通知处理程序代码
	if(SendFlag){
		UpdateList(_T("正在发送网络！"));
		return;
	}
	m_SendButton.EnableWindow(false);
	p_thread=AfxBeginThread(SEND,this,0,0,0,NULL);//开启一个线程
}


void CServerTestDlg::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
	SendFlag=false;
}


void CServerTestDlg::UpdateList(CString str)
{
	CString string;
	CTime time = CTime::GetCurrentTime();// 获取系统当前时间
	str += _T("\r\n");// 用于换行显示日志
	string = time.Format(_T("%H:%M:%S ")) + str;// 格式化当前时间
	m_list.InsertString(0,string);
}


ULONG CurrentClock;
ULONG LastClock=0;
ULONGLONG rLastSize=0;
afx_msg LRESULT CServerTestDlg::OnSocket(WPARAM wParam, LPARAM lParam)
{
	int nError = WSAGETSELECTERROR(lParam);		//读取错误代码
	int nEvent = WSAGETSELECTEVENT(lParam);		//读取网络事件
	SOCKET sock = wParam;
	CString sz_text;
	ULONG len_recv=0;
	char a[1];
	switch (nEvent) 
	{
	case FD_ACCEPT:
		{
			//接收客户端的连接
			closesocket(m_accept);
			sockaddr_in sockAddr;
			int nAddrSize = sizeof(sockaddr_in);
			m_accept = accept(sock, (sockaddr*)&sockAddr, &nAddrSize);
			WSAAsyncSelect(m_accept, m_hWnd, WM_SOCKET, FD_CLOSE|FD_READ|FD_OOB);
			socket_type=0; //表示作为服务器
			m_state.SetWindowText(_T("网络连接状态"));
			m_connect.EnableWindow(FALSE);//由于被连接，需要是连接功能丧失
			ConnectFlag=true;
#if 1
			UpdateList(_T("开始接收数据！"));
			//f.Open(_T("E:\\f.bin"),CFile::modeCreate|CFile::modeWrite);
			m_acceptsocket.SetWindowText(_T("0KB"));
			m_Receivespeed.SetWindowText(_T("0KB/s"));
			total_recv=0;
			LastClock=0;
			rLastSize=0;
			GetFlag=true;
#endif
			break;
		}
	case FD_READ:	//接收数据
		{
			len_recv=recv(sock,SocketGetBuff,SOCKETSIZE,0);
			if(GetFlag==true && len_recv!=0){
				//f.Write(SocketGetBuff,len_recv);
				total_recv=len_recv+total_recv;
				CurrentClock=clock();
				if(CurrentClock-LastClock>=1000)
				{
					sz_text.Format(_T("%dKB"),(total_recv/1024));
					m_acceptsocket.SetWindowText(sz_text);
					if(LastClock!=0)
					{
						sz_text.Format(_T("%lfKB/s"),((double)(total_recv-rLastSize))/(CurrentClock-LastClock)*1000/1024);
						m_Receivespeed.SetWindowText(sz_text);
					}
					rLastSize=total_recv;
					LastClock=CurrentClock;
				}
			}
			len_recv=0;
			break;
		}
	case FD_CLOSE:
		{
			if(socket_type==0)
				closesocket(m_accept);
			ConnectFlag=false;
			SendFlag=false;
#if 1
			UpdateList(_T("结束接收数据！"));
			//f.Close();
			m_acceptsocket.SetWindowText(_T("0KB"));
			m_Receivespeed.SetWindowText(_T("0KB/s"));
			total_recv=0;
			GetFlag=false;
#endif
			m_state.SetWindowText(_T("网络未连接状态"));
			m_acceptsocket.SetWindowText(_T("0KB"));
			m_Receivespeed.SetWindowText(_T("0KB/s"));
			socket_type=-1;
			m_connect.EnableWindow(TRUE);//是连接功能恢复正常
			break;
		}
	case FD_CONNECT:							//连接网络事件
		{
			if(nError == 0)						//连接成功
			{
				ConnectFlag=true;
				m_state.SetWindowText(_T("网络连接状态"));
				socket_type=1;//表示作为客户端
				MessageBox(_T("连接成功"),_T("提示"));
			}
			break;
		}

	case FD_OOB:
		{
			len_recv=recv(sock,a,1,MSG_OOB);
			if(len_recv==1){
				if(a[0]==1)
				{
					UpdateList(_T("开始接收数据！"));
					//f.Open(_T("E:\\f.bin"),CFile::modeCreate|CFile::modeWrite);
					m_acceptsocket.SetWindowText(_T("0KB"));
					m_Receivespeed.SetWindowText(_T("0KB/s"));
					total_recv=0;
					LastClock=0;
					rLastSize=0;
					GetFlag=true;
				}
				if(a[0]==2)
				{
					UpdateList(_T("结束接收数据！"));
					//f.Close();
					m_acceptsocket.SetWindowText(_T("0KB"));
					m_Receivespeed.SetWindowText(_T("0KB/s"));
					total_recv=0;
					GetFlag=false;
				}
			}
			break;
		}
		
	}
	return 0;
}
