// ReadSource.cpp: implementation of the ReadSource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SGS_Reconstruction.h"
#include "ReadSource.h"
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <math.h>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ReadSource::ReadSource()
{
	NuDrums=0;
}

ReadSource::~ReadSource()
{

}

bool ReadSource::ReadDetectionSources(CString FullFileName)
{

	NuDrums=0;
	//��ȡ����
		int i,j;
		CString CTemp,CSpe;
		char buf[300];
		char buft[300];
		int length_buf=300;
		int key=0;
		ifstream read(FullFileName);//���ļ�
		int nSource;	
		bool keyreadsource=true;
		if(read)
		{
			while(read)
			{///
				for(i=0;i<length_buf;i++) {buf[i]='\0';buft[i]='\0';}
				read.getline(buf,length_buf);//���ж�ȡ�ļ�
				CSpe=buf;
				if(CSpe.Find(_T("END"))>=0||CSpe.Find(_T("End"))>=0||CSpe.Find(_T("end"))>=0) break;//�ļ���ȡ����
				else if(CSpe.Find(_T("$$Drum"))>=0)//������ָ��ַ�����ʾ�·���Ͱ��Ͱ����1��Դ������
				{
					NuDrums++;
					if(NuDrums>MaxNuDrums) break;
					DrumData[NuDrums-1].NoOfDrum=NuDrums;//����Ͱ���
					DrumData[NuDrums-1].NuSources=0;//ע�⣬Ͱ���Ǵ�1��ʼ���
					nSource=0;
				}
				else if(CSpe.Find(_T("##Source"))>=0)//������ָ��ַ�����ʾ��Դ��Դ����1
				{
					if(DrumData[NuDrums-1].NuSources<MaxNuSources) 
					{
						DrumData[NuDrums-1].NuSources++;//ע�⣬Դ���Ǵ�1��ʼ���	
						nSource++;
						keyreadsource=true;
					}
					else keyreadsource=false;				
				}				
				else if(CSpe.Find(_T("Detection"))>=0) //̽������
				{
					if(CSpe.Find(_T("DWGSTGS"))>=0) DrumData[NuDrums-1].TypeOfDetection=8;
					else if(CSpe.Find(_T("ISGS"))>=0) DrumData[NuDrums-1].TypeOfDetection=2;
					else if(CSpe.Find(_T("STGS2"))>=0) DrumData[NuDrums-1].TypeOfDetection=3;
					else if(CSpe.Find(_T("STGS4"))>=0) DrumData[NuDrums-1].TypeOfDetection=4;
					else if(CSpe.Find(_T("STGS8"))>=0) DrumData[NuDrums-1].TypeOfDetection=5;
					else if(CSpe.Find(_T("SGS"))>=0) DrumData[NuDrums-1].TypeOfDetection=0;
					else if(CSpe.Find(_T("TGS"))>=0) DrumData[NuDrums-1].TypeOfDetection=1;	
					DrumData[NuDrums-1].IsEqualB=false;
					if(CSpe.Find(_T("EB"))>=0) DrumData[NuDrums-1].IsEqualB=true;
				}
				else if(CSpe.GetLength()>5&&keyreadsource)	//������ַ�����������ж�ȡԴ��Ϣ
				{
					for(i=0,j=0,key=0;i<length_buf;i++) 
					{//
					if(buf[i]==';') break;
					else if(buf[i]==':'||buf[i]=='=') {key=1;}
					else if(key==1&&buf[i]!=' ') buft[j++]=buf[i];
					}//
					CTemp=buft;
					if(CSpe.Find(_T("Density"))>=0) DrumData[NuDrums-1].Density=atof(CTemp);
					else if(CSpe.Find(_T("Radius"))>=0) 
						DrumData[NuDrums-1].SourceData[nSource-1].Radius=atof(CTemp);
					else if(CSpe.Find(_T("Angle"))>=0) 
						DrumData[NuDrums-1].SourceData[nSource-1].Angle=atof(CTemp);
					else if(CSpe.Find(_T("Height"))>=0) 
						DrumData[NuDrums-1].SourceData[nSource-1].Height=atof(CTemp);
					else if(CSpe.Find(_T("NoEnergy"))>=0) 
						DrumData[NuDrums-1].SourceData[nSource-1].NoEnergy=atoi(CTemp);
					else if(CSpe.Find(_T("Activity"))>=0) 
						DrumData[NuDrums-1].SourceData[nSource-1].Activity=atof(CTemp);
				}		
			}///
			read.close();	
		}
		else return false;
		return true;
}
