; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSGS_ReconstructionDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "SGS_Reconstruction.h"

ClassCount=3
Class1=CSGS_ReconstructionApp
Class2=CSGS_ReconstructionDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_SGS_RECONSTRUCTION_DIALOG

[CLS:CSGS_ReconstructionApp]
Type=0
HeaderFile=SGS_Reconstruction.h
ImplementationFile=SGS_Reconstruction.cpp
Filter=N

[CLS:CSGS_ReconstructionDlg]
Type=0
HeaderFile=SGS_ReconstructionDlg.h
ImplementationFile=SGS_ReconstructionDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CSGS_ReconstructionDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=SGS_ReconstructionDlg.h
ImplementationFile=SGS_ReconstructionDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SGS_RECONSTRUCTION_DIALOG]
Type=1
Class=CSGS_ReconstructionDlg
ControlCount=3
Control1=IDC_PROGRESS1,msctls_progress32,1350565889
Control2=IDC_BUTTON1,button,1342242816
Control3=IDC_EDIT1,edit,1352730756

