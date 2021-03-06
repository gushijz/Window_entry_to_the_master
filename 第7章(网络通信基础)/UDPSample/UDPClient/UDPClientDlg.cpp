// UDPClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UDPClient.h"
#include "UDPClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include <winsock2.h>
#pragma comment ( lib, "ws2_32.lib" )

#define DEF_BUF_SIZE 1024

inline VOID OutputMessage ( LPWSTR lpMessage, DWORD dwErrorCode )
{
	WCHAR szMsg[DEF_BUF_SIZE] = {0} ;
	wsprintf ( szMsg, L"%s, ErrorCode=%d", lpMessage, dwErrorCode ) ;
	::MessageBox ( 0, szMsg, 0, 0 ) ;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CUDPClientDlg 对话框




CUDPClientDlg::CUDPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUDPClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUDPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUDPClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CUDPClientDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// CUDPClientDlg 消息处理程序

BOOL CUDPClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
		
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData)  ;

	in_addr addr ;
	char szHostName[DEF_BUF_SIZE] = {0} ;
	gethostname ( szHostName, DEF_BUF_SIZE ) ;
	hostent* pHost = gethostbyname ( szHostName ) ;
	if ( pHost )
	{
		memcpy ( &addr, pHost->h_addr_list[0], pHost->h_length ) ;
		::SetDlgItemTextA ( this->m_hWnd, IDC_SRV_IP, inet_ntoa ( addr ) ) ;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUDPClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUDPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CUDPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUDPClientDlg::OnBnClickedSend()
{	
	// 初始化WinSock
	DWORD dwErrorCode = 0 ;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)
	{
		OutputMessage ( L"初始化失败!", GetLastError() ) ;
		return ;
	}

	// 创建套接字
	SOCKET LocalSock = socket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ;
	if ( LocalSock == INVALID_SOCKET )
	{
		OutputMessage ( L"创建socket失败!", GetLastError() ) ;
		return ;
	}
	
	// 取得服务地址
	char szSrvIp[DEF_BUF_SIZE] = {0} ;
	::GetDlgItemTextA ( this->m_hWnd, IDC_SRV_IP, szSrvIp, DEF_BUF_SIZE ) ;
	sockaddr_in SrvAddr;
	SrvAddr.sin_family = AF_INET;
	SrvAddr.sin_port = htons(34567);
	SrvAddr.sin_addr.s_addr = inet_addr ( szSrvIp ) ;

	// 发送数据（最多512字节）
	char szData[DEF_BUF_SIZE] = {0} ;
	::GetDlgItemTextA ( this->m_hWnd, IDC_DATA, szData, DEF_BUF_SIZE ) ;
	int nSendSize = sendto ( LocalSock, szData, DEF_BUF_SIZE, 0, (sockaddr*)&SrvAddr, sizeof(sockaddr) ) ;
	if ( nSendSize <= 0 )
	{
		OutputMessage ( L"发送数据错误！", GetLastError() ) ;
		return ;
	}

	// 关闭套接字句柄
	closesocket ( LocalSock ) ;
	WSACleanup();
}
