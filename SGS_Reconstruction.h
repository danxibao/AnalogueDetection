// SGS_Reconstruction.h : main header file for the SGS_RECONSTRUCTION application
//

#if !defined(AFX_SGS_RECONSTRUCTION_H__8B2D973C_00D8_41B6_ACD9_44818A340E13__INCLUDED_)
#define AFX_SGS_RECONSTRUCTION_H__8B2D973C_00D8_41B6_ACD9_44818A340E13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSGS_ReconstructionApp:
// See SGS_Reconstruction.cpp for the implementation of this class
//

class CSGS_ReconstructionApp : public CWinApp
{
public:
	CSGS_ReconstructionApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSGS_ReconstructionApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSGS_ReconstructionApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SGS_RECONSTRUCTION_H__8B2D973C_00D8_41B6_ACD9_44818A340E13__INCLUDED_)
