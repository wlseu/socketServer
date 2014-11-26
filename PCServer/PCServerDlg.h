
// PCServerDlg.h : 头文件
//
#include <atlimage.h>
#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")

#define WM_VIDEOSOCKET WM_USER+2
#define WM_DATASOCKET WM_USER+3
#define WM_PHYDATASOCKET WM_USER+4
#define MY_MESSAGE WM_USER+5

#pragma once


// CPCServerDlg 对话框
class CPCServerDlg : public CDialogEx
{
// 构造
public:
	CPCServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PCSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	//视频传输
	static UINT ListenVideoProc(LPVOID lParam);
	static UINT ListenDataProc(LPVOID lParam);
	static UINT RemoteControlProc(LPVOID lParam);

	long OnVideosocket(WPARAM wParam,LPARAM lParam);
	long OnDatasocket(WPARAM wParam,LPARAM lParam);
	long OnPhyDatasocket(WPARAM wParam,LPARAM lParam);
	long OnUpdate(WPARAM wParam,LPARAM lParam);
	void SocketInit();
	bool Videorev_flag;
	bool Datarev_flag;
	bool Phydatarev_flag;
	unsigned long ClientIP;
	SOCKET m_ListenSocket;
	SOCKET m_DataListenSocket;
	SOCKET m_AcceptVideoSocket;
	SOCKET m_AcceptDataSocket;
	SOCKET m_ClientSocket;
	SOCKET m_RemoteConSocket;
	CImage image;
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
//	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedDatarev();
	afx_msg void OnBnClickedVideorev();
//	CStatic m_bmp;
	CString m_mode;
	CString m_direction;
	CString m_velocity;	
	afx_msg void OnBnClickedRemotecontrol();
	double m_sensor1;
	double m_sensor2;
	double m_sensor3;
	double m_sensor4;
};
