
// PCServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PCServer.h"
#include "PCServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned int Speed = 1,Direction = 1,Mode=1;
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


// CPCServerDlg 对话框




CPCServerDlg::CPCServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPCServerDlg::IDD, pParent)
{
	Videorev_flag = false;
	Datarev_flag = false;
	Phydatarev_flag = false;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_mode = _T("");
	m_direction = _T("");
	m_velocity = _T("");
	m_ClientSocket =NULL;
	m_sensor1 = 0.0;
	m_sensor2 = 0.0;
	m_sensor3 = 0.0;
	m_sensor4 = 0.0;
}

void CPCServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_BMP, m_bmp);
	DDX_Text(pDX, IDC_MODE, m_mode);
	DDX_Text(pDX, IDC_DIRECTION, m_direction);
	DDX_Text(pDX, IDC_VELOCITY, m_velocity);
	DDX_Text(pDX, IDC_SENSOR1, m_sensor1);
	DDX_Text(pDX, IDC_SENSOR2, m_sensor2);
	DDX_Text(pDX, IDC_SENSOR3, m_sensor3);
	DDX_Text(pDX, IDC_SENSOR4, m_sensor4);
}

BEGIN_MESSAGE_MAP(CPCServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_VIDEOSOCKET,OnVideosocket)
	ON_MESSAGE(WM_DATASOCKET,OnDatasocket)
	ON_MESSAGE(WM_PHYDATASOCKET,OnPhyDatasocket)
//	ON_BN_CLICKED(IDCANCEL, &CPCServerDlg::OnBnClickedCancel)
    ON_MESSAGE(MY_MESSAGE,OnUpdate)
	ON_BN_CLICKED(IDC_DATAREV, &CPCServerDlg::OnBnClickedDatarev)
	ON_BN_CLICKED(IDC_VIDEOREV, &CPCServerDlg::OnBnClickedVideorev)	
	ON_BN_CLICKED(IDC_REMOTECONTROL, &CPCServerDlg::OnBnClickedRemotecontrol)
END_MESSAGE_MAP()


// CPCServerDlg 消息处理程序

BOOL CPCServerDlg::OnInitDialog()
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
	SocketInit();

	CComboBox *speedcontrol = (CComboBox*)GetDlgItem(IDC_VELCONTROL);
	speedcontrol->InsertString(0,_T("速度1"));
	speedcontrol->InsertString(1,_T("速度2"));
	speedcontrol->InsertString(2,_T("速度3"));
	speedcontrol->InsertString(3,_T("速度4"));
	speedcontrol->InsertString(4,_T("速度5"));
	speedcontrol->InsertString(5,_T("速度6"));
	speedcontrol->SetCurSel(0);

	CComboBox *directioncontrol = (CComboBox*)GetDlgItem(IDC_DIRECTIONCONTROL);
	directioncontrol->InsertString(0,_T("前进"));
	directioncontrol->InsertString(1,_T("后退"));
	directioncontrol->InsertString(2,_T("停止"));
	directioncontrol->SetCurSel(0);

	CComboBox *modecontrol = (CComboBox*)GetDlgItem(IDC_MODECONTROL);
	modecontrol->InsertString(0,_T("主动模式"));
	modecontrol->InsertString(1,_T("面板控制"));
	modecontrol->InsertString(2,_T("被动模式"));
	modecontrol->SetCurSel(0);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPCServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPCServerDlg::OnPaint()
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
HCURSOR CPCServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//void CPCServerDlg::OnBnClickedCancel()
//{
	// TODO: 在此添加控件通知处理程序代码
//	CDialogEx::OnCancel();
//}


void CPCServerDlg::OnBnClickedDatarev()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!Datarev_flag)
	{	
		Datarev_flag=true;
		GetDlgItem(IDC_DATAREV)->SetWindowText(_T("关闭数据接收"));
		AfxBeginThread(ListenDataProc,this);		
	}
	else
	{	
		shutdown(m_DataListenSocket,2);	
		closesocket(m_AcceptDataSocket);
		closesocket(m_DataListenSocket);
		GetDlgItem(IDC_DATAREV)->SetWindowText(_T("开启数据接收"));
		Datarev_flag=false;
	}
}

void CPCServerDlg::SocketInit()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1,1);
	
	err = WSAStartup(wVersionRequested, &wsaData);
	if(err != 0)
	{
		MessageBox(_T("failed to WSAStart up!\n"));
        return;
	}
	if(LOBYTE(wsaData.wVersion)!= 1 || HIBYTE(wsaData.wVersion)!= 1)
	{
		WSACleanup();
		return;
	}
		return ;
}


void CPCServerDlg::OnBnClickedVideorev()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!Videorev_flag)
	{
		Videorev_flag=true;
		GetDlgItem(IDC_VIDEOREV)->SetWindowText(_T("关闭视频接收"));
		AfxBeginThread(ListenVideoProc,this);

	}
	else
	{
		shutdown(m_ListenSocket,2);
		closesocket(m_AcceptVideoSocket);
		closesocket(m_ListenSocket);
		GetDlgItem(IDC_VIDEOREV)->SetWindowText(_T("开启视频接收"));
		Videorev_flag=false;
	}

}

UINT CPCServerDlg::ListenVideoProc(LPVOID lParam)
{
	CPCServerDlg *pServerDlg = (CPCServerDlg *)lParam;
	//pServerDlg->m_ListenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	pServerDlg->m_ListenSocket = socket(AF_INET,SOCK_STREAM,0);
	if(pServerDlg->m_ListenSocket == INVALID_SOCKET)
	{
		return 3;
	}
   SOCKADDR_IN addrSrv;
   addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
   addrSrv.sin_family = AF_INET;
   addrSrv.sin_port=htons(8080);//视频端口设置为6666

   if(::bind(pServerDlg->m_ListenSocket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR))==SOCKET_ERROR)
   {
	 //  AfxMessageBox("failed to bind");
	   return 4;
   }
   
 //WSAAsyncSelect(pServerDlg->m_ListenSocket,pServerDlg->m_hWnd,WM_VIDEOSOCKET,FD_ACCEPT|FD_CLOSE);

   listen(pServerDlg->m_ListenSocket,3);
 //  pServerDlg->Videorev_flag = true;
 //  pServerDlg->GetDlgItem(IDC_VIDEOREV)->SetWindowText(_T("关闭视频接收"));
 //  return 0; 

   SOCKADDR_IN addrClient;
   int len = sizeof(SOCKADDR);
   int i = 0;
   while(pServerDlg->Videorev_flag)
  {
	 pServerDlg->m_AcceptVideoSocket= accept(pServerDlg->m_ListenSocket,(SOCKADDR*)&addrClient,&len);
	 char recvBuf[10240];
	 HANDLE hf = CreateFile(_T("data.jpg"), GENERIC_WRITE, 
	      FILE_SHARE_WRITE , NULL, CREATE_ALWAYS, 0, NULL);
              if (hf == INVALID_HANDLE_VALUE)
			  {
               return 0;
			  }	
			   DWORD dwWritten = 0;
	 while((i=recv(pServerDlg->m_AcceptVideoSocket,recvBuf,10000,0))>0)
	{         
	 		 Sleep(2);
		//	 Sleep(200);	 		
             WriteFile( hf,recvBuf,i, &dwWritten, NULL );             
           
	}
	        CloseHandle(hf);	
	 	    CRect rect;
			pServerDlg->GetDlgItem(IDC_PICTURE)->GetWindowRect(rect);
			pServerDlg->ScreenToClient(rect);
		    if(! pServerDlg->image.IsNull())
			 pServerDlg->image.Destroy();
		     pServerDlg->image.Load(_T("data.jpg"));		
		    if(! pServerDlg->image.IsNull())
			 pServerDlg->image.Draw(pServerDlg->GetDC()->m_hDC,rect.left,rect.top);
 }  

		
   return 0;
}

UINT CPCServerDlg::ListenDataProc(LPVOID lParam)
{
	CPCServerDlg *pServerDlg = (CPCServerDlg *)lParam;
	pServerDlg->m_DataListenSocket = socket(AF_INET,SOCK_STREAM,0);
	if(pServerDlg->m_DataListenSocket == INVALID_SOCKET)
	{	
		return 3;
	}
   SOCKADDR_IN addrSrv;
   addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
   addrSrv.sin_family = AF_INET;
   addrSrv.sin_port=htons(5555);//数据端口设置为5555

   if(::bind(pServerDlg->m_DataListenSocket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR))==SOCKET_ERROR)
   {	 
	   return 4;
   }
   
 //  WSAAsyncSelect(pServerDlg->m_DataListenSocket,pServerDlg->m_hWnd,WM_DATASOCKET,FD_ACCEPT|FD_CLOSE);
   listen(pServerDlg->m_DataListenSocket,3);   
 // pServerDlg->Datarev_flag = true;
 // pServerDlg->GetDlgItem(IDC_DATAREV)->SetWindowText(_T("关闭数据接收"));  
   
   SOCKADDR_IN addrClient;
   int len = sizeof(SOCKADDR);
   int buflen = 0;
   unsigned char recvBuf[5000];
    while(pServerDlg->Datarev_flag)		
		{
			pServerDlg->m_AcceptDataSocket= accept(pServerDlg->m_DataListenSocket,(SOCKADDR*)&addrClient,&len);	
			
			pServerDlg->m_ClientSocket=pServerDlg->m_AcceptDataSocket;//保存客户端套接字
		    pServerDlg->ClientIP=addrClient.sin_addr.S_un.S_addr;//保护客户端ip

		    while((buflen=recv(pServerDlg->m_AcceptDataSocket,(char *)recvBuf,5000,0))>0)
	      {         
	 		          
               if(recvBuf[0]==170 && recvBuf[1]==170)			
				 {  
					if(recvBuf[2]==232 && recvBuf[3] ==162)
					 {
						  pServerDlg->m_mode="被动模式";
					  }
					 else if(recvBuf[2]==228 && recvBuf[3] ==184)
					 {
						 pServerDlg-> m_mode="主动模式";
					  }
					 else
					 {
                         pServerDlg-> m_mode="面板控制";
					 }
					 if(recvBuf[14]==229 &&recvBuf[15]==129)
					 {
                          pServerDlg->m_direction="停止";
					 }
					 else if(recvBuf[14]==229 && recvBuf[15]==137)
					 {
						 pServerDlg->m_direction="前进";
					 }
					  else
					 {
					  pServerDlg->m_direction ="后退";
					  }
					  if(recvBuf[26]==49)
					 {
                          pServerDlg->m_velocity="速度1";
					  }
					  else if(recvBuf[26]==50)
					  {
						  pServerDlg-> m_velocity="速度2";
					  }
					  else if(recvBuf[26]==51)
					  {
						   pServerDlg->m_velocity="速度3";
					  }
					   else if(recvBuf[26]==52)
					  {
						   pServerDlg->m_velocity="速度4";
					  }
					  else if(recvBuf[26]==53)
					  {
						   pServerDlg->m_velocity="速度5";
					  }
					  else
					  {
						  pServerDlg->m_velocity="速度6";
					  }
					
				//	 pServerDlg->UpdateData(false); 
					  pServerDlg->PostMessageW(MY_MESSAGE);
	            }	
			    else if (recvBuf[0]==187 && recvBuf[1]==187)	
			   {
				   pServerDlg->m_sensor1=recvBuf[2]-256;
				   pServerDlg->m_sensor2=recvBuf[3]-256;
				   if(recvBuf[4]>127)
					   pServerDlg->m_sensor3=recvBuf[4]-256;
				   else
				   {
					   pServerDlg->m_sensor3=recvBuf[4];
				   }
				   pServerDlg->m_sensor4=recvBuf[5]-256;
				   pServerDlg->PostMessageW(MY_MESSAGE);
			   }
          }  						
				
       }         
	    	 		 		 
 
   return 0; 
}
long CPCServerDlg::OnUpdate(WPARAM wParam,LPARAM lParam)
{
	UpdateData(false);
     return 0;
}
long CPCServerDlg::OnVideosocket(WPARAM wParam,LPARAM lParam)
{
	SOCKET currentsocket = wParam;
    int buflen = 0;
   unsigned char recvBuf[100000];
	if(WSAGETSELECTERROR(lParam))
	{
		return 1;
	}
	switch(WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
		{
          SOCKADDR_IN addrClient;
		  memset(&addrClient,0,sizeof(addrClient));
		  int len = sizeof(SOCKADDR);         
		  SOCKET acceptsocket;
		  acceptsocket = ::accept(currentsocket,(SOCKADDR*)&addrClient,&len);
		  ::WSAAsyncSelect(acceptsocket,m_hWnd,WM_VIDEOSOCKET,FD_READ|FD_CLOSE);    
		  break;
		} 
			
	case FD_READ:
		{      		
			  HANDLE hf = CreateFile(_T("data.jpg"), GENERIC_WRITE, 
	    	   FILE_SHARE_WRITE , NULL, CREATE_ALWAYS, 0, NULL);
              if (hf == INVALID_HANDLE_VALUE)
			  {
               return 0;
			  }	
			   DWORD dwWritten = 0;
          while(Videorev_flag && (buflen =::recv(currentsocket,(char *)recvBuf,10000,0))>0 )
			{	
			 
	 		 Sleep(200);
			 Sleep(200);
             WriteFile( hf,recvBuf,buflen, &dwWritten, NULL );             
           
	    	 } 
		    CloseHandle(hf);	

			CRect rect;
			GetDlgItem(IDC_PICTURE)->GetWindowRect(rect);
			ScreenToClient(rect);
		    if(!image.IsNull())
			image.Destroy();
		    image.Load(_T("data.jpg"));
		
		  if(!image.IsNull())
	    	image.Draw(GetDC()->m_hDC,rect.left,rect.top);
         }
	      break;			
	case FD_CLOSE:
		closesocket(currentsocket);
        break;
	default:
		break;
	}
	return 0;


}
long CPCServerDlg::OnDatasocket(WPARAM wParam,LPARAM lParam)
{
	SOCKET datasocket = wParam;
    int buflen = 0;
    unsigned char recvBuf[100];
	if(WSAGETSELECTERROR(lParam))
	{
		return 1;
	}
	switch(WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
		{
          SOCKADDR_IN addrClient;
		  memset(&addrClient,0,sizeof(addrClient));
		  int len = sizeof(SOCKADDR);         
		  SOCKET acceptsocket;
		  acceptsocket = ::accept(datasocket,(SOCKADDR*)&addrClient,&len);
		//  m_ClientSocket=acceptsocket;//保存客户端套接字
		//  ClientIP=addrClient.sin_addr.S_un.S_addr;//保护客户端ip
		  ::WSAAsyncSelect(acceptsocket,m_hWnd,WM_DATASOCKET,FD_READ|FD_CLOSE);    
		  break;
		} 			
	case FD_READ:
		{  
		     while(Datarev_flag && (buflen =::recv(datasocket,(char *)recvBuf,100,0))>0 )
			{
				int a =0;				
				if(recvBuf[0]==170 && recvBuf[1]==170)			
				{  
					if(recvBuf[2]==231 && recvBuf[3] ==168)
					 {
						  m_mode="程序控制";
					  }
					 else if(recvBuf[2]==228 && recvBuf[3] ==184)
					 {
						  m_mode="主动运动";
					  }
					 else
					 {
                          m_mode="按键被动";
					 }
					 if(recvBuf[14]==229 &&recvBuf[15]==129)
					 {
                          m_direction="停止";
					 }
					 else if(recvBuf[14]==229 && recvBuf[15]==137)
					 {
						 m_direction="前进";
					 }
					 else
					 {
					 m_direction ="后退";
					 }
					  if(recvBuf[26]==49)
					 {
                          m_velocity="速度1";
					 }
					  else if(recvBuf[26]==50)
					  {
						   m_velocity="速度2";
					  }
					  else if(recvBuf[26]==51)
					  {
						   m_velocity="速度3";
					  }
					   else if(recvBuf[26]==52)
					  {
						   m_velocity="速度4";
					  }
					  else if(recvBuf[26]==53)
					  {
						   m_velocity="速度5";
					  }
					  else
					  {
						  m_velocity="速度6";
					  }
					
					 UpdateData(false);
				 }
           
	    	 }  		 		 
         }
	      break;			
	case FD_CLOSE:
		closesocket(datasocket);
        break;
	default:
		break;
	}
	return 0;


}

long CPCServerDlg::OnPhyDatasocket(WPARAM wParam,LPARAM lParam)
{
	SOCKET phydatasocket = wParam;
    int buflen = 0;
    unsigned char recvBuf[100000];
	if(WSAGETSELECTERROR(lParam))
	{
		return 1;
	}
	switch(WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
		{
          SOCKADDR_IN addrClient;
		  memset(&addrClient,0,sizeof(addrClient));
		  int len = sizeof(SOCKADDR);         
		  SOCKET acceptsocket;
		  acceptsocket = ::accept(phydatasocket,(SOCKADDR*)&addrClient,&len);
		  ::WSAAsyncSelect(acceptsocket,m_hWnd,WM_PHYDATASOCKET,FD_READ|FD_CLOSE);    
		  break;
		} 
			
	case FD_READ:
		{   
		     
		  DWORD dwWritten = 0;
          while(Videorev_flag && (buflen =::recv(phydatasocket,(char *)recvBuf,10000,0))>0 )
			{ 
	 	      
             	 } 
			  
         }
	      break;			
	case FD_CLOSE:
		closesocket(phydatasocket);
        break;
	default:
		break;
	}
	return 0;


}

void CPCServerDlg::OnBnClickedRemotecontrol()
{
	// TODO: 在此添加控件通知处理程序代码
	if(Datarev_flag)
	{		
		if(m_ClientSocket !=NULL)
		{
		AfxBeginThread(RemoteControlProc,this);	
		}
	
	}
	else
	{
		MessageBox(_T("请先开启设备数据接收"));
	}

}
UINT CPCServerDlg::RemoteControlProc(LPVOID lParam)
{
	CPCServerDlg *pServerDlg = (CPCServerDlg *)lParam;
	pServerDlg->m_RemoteConSocket = socket(AF_INET,SOCK_STREAM,0);
	if(pServerDlg->m_RemoteConSocket == INVALID_SOCKET)
	{	
		return 1;
	}
   SOCKADDR_IN addrSrv;   
   addrSrv.sin_addr.S_un.S_addr=pServerDlg->ClientIP;
   addrSrv.sin_family = AF_INET;
   addrSrv.sin_port=htons(3333);//远程控制端口设置为3333
    
    Speed = ((CComboBox *)pServerDlg->GetDlgItem(IDC_VELCONTROL))->GetCurSel()+1;
	Direction = ((CComboBox *)pServerDlg->GetDlgItem(IDC_DIRECTIONCONTROL))->GetCurSel()+1;
	Mode = ((CComboBox *)pServerDlg->GetDlgItem(IDC_MODECONTROL))->GetCurSel()+1;
	unsigned char buffer[5];
	buffer[0]=0xaa;
	buffer[1]=0xaa;
	buffer[2]=Mode;
	buffer[3]=Direction;
	buffer[4]=Speed;
	connect(pServerDlg->m_RemoteConSocket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
    send(pServerDlg->m_RemoteConSocket,(const char*)buffer,5,0);  
    closesocket(pServerDlg->m_RemoteConSocket);
	return 0;
}