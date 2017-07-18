// SGS_ReconstructionDlg.h : header file
//

#if !defined(AFX_SGS_RECONSTRUCTIONDLG_H__94294D1A_0A01_452B_BE25_4AD158A29068__INCLUDED_)
#define AFX_SGS_RECONSTRUCTIONDLG_H__94294D1A_0A01_452B_BE25_4AD158A29068__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSGS_ReconstructionDlg dialog
#include "AnalogDetectionCalculation.h"

struct Calculating_ThreadInf
{
public:
	AnalogDetectionCalculation *CalculatingC;
	bool IsThreadFinishedSuccessful;
};
UINT AD_ThreadFunc(LPVOID lpParam);

class CSGS_ReconstructionDlg : public CDialog
{
// Construction
public:
	CSGS_ReconstructionDlg(CWnd* pParent = NULL);	// standard constructor
	Calculating_ThreadInf Calculating_ThreadData;

// Dialog Data
	//{{AFX_DATA(CSGS_ReconstructionDlg)
	enum { IDD = IDD_SGS_RECONSTRUCTION_DIALOG };
	CButton	m_Button1B;
	CEdit	m_InfB;
	CProgressCtrl	m_ProgressB;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSGS_ReconstructionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
//变量
	CString	m_PresentFolder;//可执行文件所在文件夹
	AnalogDetectionCalculation CalThreadB;//*CalculatingC
	CalculationStatus CalThreadStatus;
	int m_LOfInf;
//函数
	void INIProcess();
	void Exit();

	// Generated message map functions
	//{{AFX_MSG(CSGS_ReconstructionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButton1();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SGS_RECONSTRUCTIONDLG_H__94294D1A_0A01_452B_BE25_4AD158A29068__INCLUDED_)
