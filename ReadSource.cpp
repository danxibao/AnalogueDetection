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
	//读取设置
		int i,j;
		CString CTemp,CSpe;
		char buf[300];
		char buft[300];
		int length_buf=300;
		int key=0;
		ifstream read(FullFileName);//打开文件
		int nSource;	
		bool keyreadsource=true;
		if(read)
		{
			while(read)
			{///
				for(i=0;i<length_buf;i++) {buf[i]='\0';buft[i]='\0';}
				read.getline(buf,length_buf);//挨行读取文件
				CSpe=buf;
				if(CSpe.Find(_T("END"))>=0||CSpe.Find(_T("End"))>=0||CSpe.Find(_T("end"))>=0) break;//文件读取结束
				else if(CSpe.Find(_T("$$Drum"))>=0)//如果发现该字符，表示新废物桶，桶数加1，源数清零
				{
					NuDrums++;
					if(NuDrums>MaxNuDrums) break;
					DrumData[NuDrums-1].NoOfDrum=NuDrums;//废物桶编号
					DrumData[NuDrums-1].NuSources=0;//注意，桶数是从1开始编号
					nSource=0;
				}
				else if(CSpe.Find(_T("##Source"))>=0)//如果发现该字符，表示新源，源数加1
				{
					if(DrumData[NuDrums-1].NuSources<MaxNuSources) 
					{
						DrumData[NuDrums-1].NuSources++;//注意，源数是从1开始编号	
						nSource++;
						keyreadsource=true;
					}
					else keyreadsource=false;				
				}				
				else if(CSpe.Find(_T("Detection"))>=0) //探测类型
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
				else if(CSpe.GetLength()>5&&keyreadsource)	//如果有字符，且允许进行读取源信息
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
