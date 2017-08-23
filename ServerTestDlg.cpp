
// ServerTestDlg.cpp : ʵ���ļ�
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


// CServerTestDlg �Ի���




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


// CServerTestDlg ��Ϣ�������

BOOL CServerTestDlg::OnInitDialog()
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	WSADATA wsd;										//����WSADATA����
	WSAStartup(MAKEWORD(2,2),&wsd);		
	//�����׽���
	m_local = socket(AF_INET, SOCK_STREAM, 0);
	unsigned long nCmd;
	int nState = ioctlsocket(m_local, FIONBIO, &nCmd);		//���÷�����ģʽ
	if (nState != 0)												//�����׽��ַ�����ģʽʧ��
	{
		TRACE("�����׽��ַ�����ģʽʧ��");
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
		TRACE("����WSAAsyncSelectģ��ʧ��");
	}
	m_state.SetWindowText(_T("����δ����״̬"));
	socket_type=-1;
	//m_length.SetWindowText(_T("0"));
	if((SocketGetBuff=(char*)malloc(SOCKETSIZE*4))==NULL)
		puts("SocketGetBuff Gotanullpointer");
	else
		puts("SocketGetBuff Gotavalidpointer");

	memset(SocketGetBuff,0,SOCKETSIZE*4);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CServerTestDlg::OnPaint()
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
HCURSOR CServerTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CServerTestDlg::OnBnClickedBind()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	unsigned short num=1234;//�˿ں�ͳһ���ó�1234
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
		MessageBox(_T("��ַ�Ͷ˿ڰ󶨳ɹ�!"),_T("��ʾ"));
	}
	else
	{
		MessageBox(_T("��ַ��ʧ��!"), _T("��ʾ"));
	}
}


void CServerTestDlg::OnBnClickedConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_state.SetWindowText(_T("���ӽ�����....."));
}

bool GetFlag=false;
bool ConnectFlag=false;
ULONGLONG total_recv=0;

UINT SEND(LPVOID pThreadParam)//�̺߳���
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
		sz_text.Format(_T("��ʼ����"));
		dlg->UpdateList(sz_text);
	}
	len=0;
	a=clock();
	while(SendFlag)//ֱ��ȫ���������
	{
		//ReadDataFromFpga(hDevice,buffer,BufferSrc,nRead);//��ȡ115������
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
	ss.Format(_T("���ͽ����������ٶ�=%lf KB/s %lldKB"),total/1024/c,total/1024);  
	dlg->UpdateList(ss);
	//MessageBox(dlg->m_hWnd,ss,_T("��ʾ"),MB_OK);
		 
    DWORD exit=0;
 
	BOOL ret=GetExitCodeThread(dlg->p_thread->m_hThread,&exit);//��ȡ�߳��˳�����
	if(exit==STILL_ACTIVE) //����������ڽ���
	{
		dlg->p_thread->ExitInstance();//�˳�����
		dlg->p_thread=NULL;
	}
	free(buffer);
	
	dlg->m_SendButton.EnableWindow(true);
	return 0;
}

void CServerTestDlg::OnBnClickedSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(SendFlag){
		UpdateList(_T("���ڷ������磡"));
		return;
	}
	m_SendButton.EnableWindow(false);
	p_thread=AfxBeginThread(SEND,this,0,0,0,NULL);//����һ���߳�
}


void CServerTestDlg::OnBnClickedStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SendFlag=false;
}


void CServerTestDlg::UpdateList(CString str)
{
	CString string;
	CTime time = CTime::GetCurrentTime();// ��ȡϵͳ��ǰʱ��
	str += _T("\r\n");// ���ڻ�����ʾ��־
	string = time.Format(_T("%H:%M:%S ")) + str;// ��ʽ����ǰʱ��
	m_list.InsertString(0,string);
}


ULONG CurrentClock;
ULONG LastClock=0;
ULONGLONG rLastSize=0;
afx_msg LRESULT CServerTestDlg::OnSocket(WPARAM wParam, LPARAM lParam)
{
	int nError = WSAGETSELECTERROR(lParam);		//��ȡ�������
	int nEvent = WSAGETSELECTEVENT(lParam);		//��ȡ�����¼�
	SOCKET sock = wParam;
	CString sz_text;
	ULONG len_recv=0;
	char a[1];
	switch (nEvent) 
	{
	case FD_ACCEPT:
		{
			//���տͻ��˵�����
			closesocket(m_accept);
			sockaddr_in sockAddr;
			int nAddrSize = sizeof(sockaddr_in);
			m_accept = accept(sock, (sockaddr*)&sockAddr, &nAddrSize);
			WSAAsyncSelect(m_accept, m_hWnd, WM_SOCKET, FD_CLOSE|FD_READ|FD_OOB);
			socket_type=0; //��ʾ��Ϊ������
			m_state.SetWindowText(_T("��������״̬"));
			m_connect.EnableWindow(FALSE);//���ڱ����ӣ���Ҫ�����ӹ���ɥʧ
			ConnectFlag=true;
#if 1
			UpdateList(_T("��ʼ�������ݣ�"));
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
	case FD_READ:	//��������
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
			UpdateList(_T("�����������ݣ�"));
			//f.Close();
			m_acceptsocket.SetWindowText(_T("0KB"));
			m_Receivespeed.SetWindowText(_T("0KB/s"));
			total_recv=0;
			GetFlag=false;
#endif
			m_state.SetWindowText(_T("����δ����״̬"));
			m_acceptsocket.SetWindowText(_T("0KB"));
			m_Receivespeed.SetWindowText(_T("0KB/s"));
			socket_type=-1;
			m_connect.EnableWindow(TRUE);//�����ӹ��ָܻ�����
			break;
		}
	case FD_CONNECT:							//���������¼�
		{
			if(nError == 0)						//���ӳɹ�
			{
				ConnectFlag=true;
				m_state.SetWindowText(_T("��������״̬"));
				socket_type=1;//��ʾ��Ϊ�ͻ���
				MessageBox(_T("���ӳɹ�"),_T("��ʾ"));
			}
			break;
		}

	case FD_OOB:
		{
			len_recv=recv(sock,a,1,MSG_OOB);
			if(len_recv==1){
				if(a[0]==1)
				{
					UpdateList(_T("��ʼ�������ݣ�"));
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
					UpdateList(_T("�����������ݣ�"));
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
