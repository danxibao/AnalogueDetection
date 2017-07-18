// SGS_ReconstructionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SGS_Reconstruction.h"
#include "SGS_ReconstructionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSGS_ReconstructionDlg dialog

CSGS_ReconstructionDlg::CSGS_ReconstructionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSGS_ReconstructionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSGS_ReconstructionDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSGS_ReconstructionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSGS_ReconstructionDlg)
	DDX_Control(pDX, IDC_BUTTON1, m_Button1B);
	DDX_Control(pDX, IDC_EDIT1, m_InfB);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressB);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSGS_ReconstructionDlg, CDialog)
	//{{AFX_MSG_MAP(CSGS_ReconstructionDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSGS_ReconstructionDlg message handlers

BOOL CSGS_ReconstructionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	INIProcess();//��ʼ����

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSGS_ReconstructionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if((nID   &   0xfff0)   ==   SC_CLOSE)   //����;
	{
		Exit();//���ùرմ��ں���
	} 
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSGS_ReconstructionDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSGS_ReconstructionDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//��ʼ������ʼ����
void CSGS_ReconstructionDlg::INIProcess() 
{
	//��ѯ��ǰ·��
	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL,path,MAX_PATH);
	CString p(path);
	CString subp;
	int nPos = p.ReverseFind('\\');
	m_PresentFolder=p.Left(nPos+1);

	//��ʱ��;
	SetTimer(1,1000,0);
	//�ؼ�����;
	CalThreadStatus.m_IsOnCalculation=false;
	m_ProgressB.SetRange(0,100);
	CalThreadStatus.N_Progress=0;
	CalThreadStatus.m_InfText.Empty();
	m_LOfInf=0;
}

void CSGS_ReconstructionDlg::OnButton1() //����
{
	m_Button1B.EnableWindow(false);
	CalThreadB.m_Folder=m_PresentFolder;
	CalThreadB.CalThreadStatus=CalThreadStatus;
	Calculating_ThreadData.CalculatingC=&CalThreadB;//
	AfxBeginThread(AD_ThreadFunc,&Calculating_ThreadData);//
}

//�߳�1�������ؽ�����
UINT AD_ThreadFunc(LPVOID lpParam)
{
	Calculating_ThreadInf * pInf=(Calculating_ThreadInf *)lpParam;
	pInf->CalculatingC->StartCalculation();//������ڳ���
	return 1;
}

void CSGS_ReconstructionDlg::Exit() //�˳�
{
	if(CalThreadStatus.m_IsOnCalculation)//����������У�ѯ���Ƿ��˳�
	if(MessageBox(_T("ȷ��Ҫ�˳����㣿"),_T("�˳�����"), MB_ICONQUESTION|MB_OK|MB_OKCANCEL)==IDCANCEL)
	{
		return;//���ȡ�����ͷ���
	}
	CalThreadStatus.m_IsOnCalculation=false;
	Sleep(1000);
	CDialog::OnCancel();//�˳�����	
}

//Timer��ÿ��һ�θ���
void CSGS_ReconstructionDlg::OnTimer(UINT nIDEvent) 
{
	CalThreadStatus=CalThreadB.CalThreadStatus;
	//�����Ϣ�ַ������ȷ����ı䣬���
	if(m_LOfInf!=CalThreadStatus.m_InfText.GetLength())
	{
		m_InfB.SetWindowText(CalThreadStatus.m_InfText);//ʵʱ��ʾ��Ϣ	
		m_LOfInf=CalThreadStatus.m_InfText.GetLength();
		m_InfB.LineScroll(m_LOfInf,0);
	}
//	if(CalThreadStatus.m_IsOnCalculation) m_Button1B.EnableWindow(false);
//	else m_Button1B.EnableWindow(true); 

		m_ProgressB.SetPos(CalThreadStatus.N_Progress);//ʵʱ��ʾ����

		CDialog::OnTimer(nIDEvent);
}
