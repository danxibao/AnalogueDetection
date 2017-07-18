// AnalogDetectionCalculation.cpp: implementation of the AnalogDetectionCalculation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SGS_Reconstruction.h"
#include "AnalogDetectionCalculation.h"
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <math.h>
using namespace std;
#include <time.h>
#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AnalogDetectionCalculation::AnalogDetectionCalculation()
{

}

AnalogDetectionCalculation::~AnalogDetectionCalculation()
{

}

bool AnalogDetectionCalculation::StartCalculation()
{
	if(!PathIsDirectory(m_Folder+_T("Detection\\")))
	{	
		CalThreadStatus.m_InfText+=_T("\r\n @_@ Detection文件夹不存在，失败");
		return false;
	}
	if(!ReadDetectionSetup(m_Folder+_T("Detection\\_DetectionSetupSample.txt")))
	{
		CalThreadStatus.m_InfText+=_T("\r\n @_@ 设置样品文件不存在，失败");
		return false;
	}

	CalThreadStatus.m_IsOnCalculation=true;
	CalThreadStatus.m_InfText+=_T("## 存储位置：")+m_Folder+_T("Detection\\");
	CString CTemp,FolderName;

	//读取10个偏心位置圆环的探测效率，用于发射测量
	int i;
	for(i=0;i<_NdY;i++)
		RL_E_WD[i]=NULL;
	RL_E_WD[0]=TriDataB.ReadData(m_Folder+_T("ESaved\\1RL_E_WD000.dat"));//0cm
	RL_E_WD[1]=TriDataB.ReadData(m_Folder+_T("ESaved\\1RL_E_WD035.dat"));//3.5cm
	RL_E_WD[2]=TriDataB.ReadData(m_Folder+_T("ESaved\\1RL_E_WD070.dat"));//7cm
	RL_E_WD[3]=TriDataB.ReadData(m_Folder+_T("ESaved\\1RL_E_WD105.dat"));//10.5cm
	RL_E_WD[4]=TriDataB.ReadData(m_Folder+_T("ESaved\\1RL_E_WD140.dat"));//14cm
	RL_E_WD[5]=TriDataB.ReadData(m_Folder+_T("ESaved\\1RL_E_WD175.dat"));//17.5cm
	RL_E_WD[6]=TriDataB.ReadData(m_Folder+_T("ESaved\\1RL_E_WD210.dat"));//21cm
	RL_E_WD[7]=TriDataB.ReadData(m_Folder+_T("ESaved\\1RL_E_WD245.dat"));//24.5cm
	RL_E_WD[8]=TriDataB.ReadData(m_Folder+_T("ESaved\\1RL_E_WD280.dat"));//28cm
	RL_E_WD[9]=TriDataB.ReadData(m_Folder+_T("ESaved\\1RL_E_WD315.dat"));//31.5cm
	for(i=0;i<10&&i<_NdY;i++)
	{
		CTemp.Format("1RL_E_WD%03d.dat",35*i);
		if(RL_E_WD[i]==NULL) 
		CalThreadStatus.m_InfText+=_T("\r\n 缺少文件：")+CTemp;
	}

	CalThreadStatus.m_InfText+=_T("\r\n == 开始模拟测量");

	//读取源信息
	ReadSourceB.ReadDetectionSources(m_Folder+_T("Detection\\_SourceDefine.txt"));
	if(ReadSourceB.NuDrums==0)
	{
		CalThreadStatus.m_InfText+=_T("\r\n == 未能读取源设置文件或存在错误");
		return false;
	}	

	//输出到探测列表
	ofstream output;
	output.open(m_Folder+_T("Detection\\ReconList.txt"),ios::app); 
	DrumDefine DrumData;
	//开始多废物桶模拟测量
	int NProjects=ReadSourceB.NuDrums;
	for(int nProject=0;nProject<NProjects;nProject++)
	{//for
		DrumData=ReadSourceB.DrumData[nProject];
		//创建文件夹
		m_IsEqualB=DrumData.IsEqualB;
		FolderName=CreatOneDetectionProject(DrumData);

		if(FolderName.GetLength()>0)
		{
			SaveDetectionSetup(m_Folder_Using+_T("_DetectionSetup.txt"));
			output<<(LPCTSTR)FolderName<<endl;
		}
		//创建透射测量
		if(DecSetupData.IsTransmission)	SGS_TransDetection();
		//创建发射测量
		m_NuDataUsed=0;//初值，不用设置

		m_NuSource=DrumData.NuSources;//源的个数
		for(int NoSource=0;	NoSource<m_NuSource;NoSource++)
		{
			m_NoEnergy[NoSource]=DrumData.SourceData[NoSource].NoEnergy;//某个源，能量的标号
			m_Activity[NoSource]=DrumData.SourceData[NoSource].Activity;//某个源，活度
			m_Radius[NoSource]=DrumData.SourceData[NoSource].Radius;//某个源，半径
			m_Height[NoSource]=DrumData.SourceData[NoSource].Height;//某个源，高度
			IdentifySourceInf(RL_E_WD[0]->VARIABLESData[m_NoEnergy[NoSource]],NoSource);//自动
		}

		//发射测量
		SGS_EmisDetection();

		CTemp.Format(_T("\r\n%d %s"),nProject,FolderName);
		CalThreadStatus.m_InfText+=CTemp;
		CalThreadStatus.N_Progress=int(100.0*(nProject+1)/NProjects);
		Sleep(1000);	
	}//for
	output.close();

	for(i=0;i<_NdY;i++)
	TriDataB.DeleteNodes(RL_E_WD[i]);

	CalThreadStatus.N_Progress=100;
	CalThreadStatus.m_IsOnCalculation=false;
	CalThreadStatus.m_InfText+=_T("\r\n ^_^ Finished");

	return true;
}
//对设定的源能量，判读次序信息
void AnalogDetectionCalculation::IdentifySourceInf(double Energy,int NoSource)
{
	int nEnergy = 0;
	for(;nEnergy<MaxNuDataInTriDNode && nEnergy<m_NuDataUsed;nEnergy++)
	{
		if(fabs(m_Energy[nEnergy]-Energy)<0.01)//能量相近
			break;	
	}
	if(nEnergy==m_NuDataUsed && m_NuDataUsed!=10) {m_Energy[m_NuDataUsed]=Energy;m_NoData[NoSource]=m_NuDataUsed;m_NuDataUsed++;}//新能量，添加
	if(nEnergy<m_NuDataUsed) m_NoData[NoSource]=nEnergy;
}

//////////////////////////////////////////////////////////读取探测设置
bool AnalogDetectionCalculation::ReadDetectionSetup(CString FullFileName)
{
	//初值
	DecSetupData.DetectionType=_T("SGS");
	DecSetupData.nDetectionType=0;
	DecSetupData.TypeOfWD=_T("Steel200L");	//Type of the drum
	DecSetupData.GWDD=56;	//Diameter of the drum
	DecSetupData.GWDH=90;	//Height of the drum
	DecSetupData.GHPGDX=53;	//Initial x-coordinate of the inelt of HPG dector
	DecSetupData.GHPGDY=0;	//Initial y-coordinate of the inelt of HPG dector
	DecSetupData.GHPGDZ=5;	//Initial z-coordinate of the inelt of HPG dector
	DecSetupData.DSNumY=1;	//Times of transtation
	DecSetupData.DSNumZ=9;	//Number of segments
	DecSetupData.DSNumA=1;	//Times of rotation
	DecSetupData.DSLY=0;	//Step length of transtation
	DecSetupData.DSLZ=10;	//Step length of rise
	DecSetupData.DSLA=0;	//Step length of rotation
	DecSetupData.DSigleT=100;	//Period of single detection
	DecSetupData.VORotation=10;	//Velocity of rotation,min-1
	DecSetupData.IsTransmission=false;//Is transmission detection carried out
	DecSetupData.WeightOfDrum=222;//Weight of Waste drum
	DecSetupData.TypeOfIteration=1;
	DecSetupData.MaxIteration=500;
	DecSetupData.NuGridsX=1;
	DecSetupData.NuGridsY=1;
	DecSetupData.NuGridsZ=9;
	DecSetupData.IsEqualVolumeOfGrids=true;
	m_IsEqualB=false;
	//读取设置
		int i,j;
		CString CTemp,CSpe;
		char buf[300];
		char buft[300];
		int length_buf=300;
		int key=0;
		ifstream read(FullFileName);//打开文件
		if(read)
		{
			while(read)
			{///
			for(i=0;i<length_buf;i++) {buf[i]='\0';buft[i]='\0';}
			read.getline(buf,length_buf);//挨行读取文件
			CSpe=buf;
			if(CSpe.Find(_T("END"))>=0||CSpe.Find(_T("End"))>=0||CSpe.Find(_T("end"))>=0) break;//文件读取结束
			for(i=0,j=0,key=0;i<length_buf;i++) 
			{//
			if(buf[i]==';') break;
			else if(buf[i]==':'||buf[i]=='=') {key=1;}
			else if(key==1&&buf[i]!=' ') buft[j++]=buf[i];
			}//
			CTemp=buft;
			if(CSpe.Find(_T("DetectionType"))>=0) DecSetupData.DetectionType=CTemp;
			else if(CSpe.Find(_T("NoOfWD"))>=0) DecSetupData.NoOfWD=CTemp;
			else if(CSpe.Find(_T("TypeOfWD"))>=0) DecSetupData.TypeOfWD=CTemp;
			else if(CSpe.Find(_T("GWDD"))>=0) DecSetupData.GWDD=atof(CTemp);
			else if(CSpe.Find(_T("GWDH"))>=0) DecSetupData.GWDH=atof(CTemp);
			else if(CSpe.Find(_T("GHPGDX"))>=0) DecSetupData.GHPGDX=atof(CTemp);
			else if(CSpe.Find(_T("GHPGDY"))>=0) DecSetupData.GHPGDY=atof(CTemp);
			else if(CSpe.Find(_T("GHPGDZ"))>=0) DecSetupData.GHPGDZ=atof(CTemp);
			else if(CSpe.Find(_T("DSNumY"))>=0) DecSetupData.DSNumY=atoi(CTemp);
			else if(CSpe.Find(_T("DSNumZ"))>=0) DecSetupData.DSNumZ=atoi(CTemp);
			else if(CSpe.Find(_T("DSNumA"))>=0) DecSetupData.DSNumA=atoi(CTemp);
			else if(CSpe.Find(_T("DSLY"))>=0) DecSetupData.DSLY=atof(CTemp);
			else if(CSpe.Find(_T("DSLZ"))>=0) DecSetupData.DSLZ=atof(CTemp);
			else if(CSpe.Find(_T("DSLA"))>=0) DecSetupData.DSLA=atof(CTemp);
			else if(CSpe.Find(_T("DSigleT"))>=0) DecSetupData.DSigleT=atof(CTemp);
			else if(CSpe.Find(_T("VORotation"))>=0) DecSetupData.VORotation=atof(CTemp);
			else if(CSpe.Find(_T("IsTransmission"))>=0) {if(atoi(CTemp)>0) DecSetupData.IsTransmission=true; else DecSetupData.IsTransmission=false;}
			else if(CSpe.Find(_T("WeightOfDrum"))>=0) DecSetupData.WeightOfDrum=atof(CTemp);
			else if(CSpe.Find(_T("TypeOfIteration"))>=0) DecSetupData.TypeOfIteration=atoi(CTemp);
			else if(CSpe.Find(_T("MaxIteration"))>=0) DecSetupData.MaxIteration=atoi(CTemp);
			else if(CSpe.Find(_T("NuGridsX"))>=0) DecSetupData.NuGridsX=atoi(CTemp);
			else if(CSpe.Find(_T("NuGridsY"))>=0) DecSetupData.NuGridsY=atoi(CTemp);
			else if(CSpe.Find(_T("NuGridsZ"))>=0) DecSetupData.NuGridsZ=atoi(CTemp);
			else if(CSpe.Find(_T("IsEqualVolumeOfGrids"))>=0) {if(atoi(CTemp)>0) DecSetupData.IsEqualVolumeOfGrids=true; else DecSetupData.IsEqualVolumeOfGrids=false;}
			}///
			read.close();
			if(DecSetupData.DetectionType.Find(_T("DWGSTGS"))>=0) DecSetupData.nDetectionType=8;
			else if(DecSetupData.DetectionType.Find(_T("STGS8"))>=0) DecSetupData.nDetectionType=5;
			else if(DecSetupData.DetectionType.Find(_T("STGS4"))>=0) DecSetupData.nDetectionType=4;
			else if(DecSetupData.DetectionType.Find(_T("STGS2"))>=0) DecSetupData.nDetectionType=3;
			else if(DecSetupData.DetectionType.Find(_T("ISGS"))>=0) DecSetupData.nDetectionType=2;
			else if(DecSetupData.DetectionType.Find(_T("TGS"))>=0) DecSetupData.nDetectionType=1;
			else if(DecSetupData.DetectionType.Find(_T("SGS"))>=0) DecSetupData.nDetectionType=0;
			if(DecSetupData.nDetectionType>=3)
			{
				if(DecSetupData.DetectionType.Find(_T("DWGSTGS"))>=0) m_IsEqualB=true;
				else if(DecSetupData.DetectionType.Find(_T("EB"))>=0) m_IsEqualB=true;
				else m_IsEqualB=false;	
			}
		}
		else return false;
return true;
}
//////////////////////////////////////////////////////////存储设置
bool AnalogDetectionCalculation::SaveDetectionSetup(CString FullFileName)
{
	ofstream output;
	output.open(FullFileName,ios::trunc);
	CString CTemp;
	output<<(LPCTSTR)_T("==DetectionSetup==")<<endl;
	CTemp.Format(_T("DecSetupData.DetectionType:%s;"),DecSetupData.DetectionType);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.NoOfWD:%s;"),DecSetupData.NoOfWD);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.TypeOfWD:%s;"),DecSetupData.TypeOfWD);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.GWDD:%g;"),DecSetupData.GWDD);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.GWDH:%g;"),DecSetupData.GWDH);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.GHPGDX:%g;"),DecSetupData.GHPGDX);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.GHPGDY:%g;"),DecSetupData.GHPGDY);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.GHPGDZ:%g;"),DecSetupData.GHPGDZ);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.DSNumY:%d;"),DecSetupData.DSNumY);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.DSNumZ:%d;"),DecSetupData.DSNumZ);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.DSNumA:%d;"),DecSetupData.DSNumA);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.DSLY:%g;"),DecSetupData.DSLY);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.DSLZ:%g;"),DecSetupData.DSLZ);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.DSLA:%g;"),DecSetupData.DSLA);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.DSigleT:%g;"),DecSetupData.DSigleT);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.VORotation:%g;"),DecSetupData.VORotation);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.IsTransmission:%d;"),int(DecSetupData.IsTransmission));output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.WeightOfDrum:%g;"),DecSetupData.WeightOfDrum);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.TypeOfIteration:%d;"),DecSetupData.TypeOfIteration);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.MaxIteration:%d;"),DecSetupData.MaxIteration);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.NuGridsX:%d;"),DecSetupData.NuGridsX);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.NuGridsY:%d;"),DecSetupData.NuGridsY);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.NuGridsZ:%d;"),DecSetupData.NuGridsZ);output<<(LPCTSTR)CTemp<<endl;
	CTemp.Format(_T("DecSetupData.IsEqualVolumeOfGrids:%d;"),int(DecSetupData.IsEqualVolumeOfGrids));output<<(LPCTSTR)CTemp<<endl;
	output<<(LPCTSTR)_T("END");
	output.close();
return true;
}
//创建项目
CString AnalogDetectionCalculation::CreatOneDetectionProject(DrumDefine drumdata)
{
	//以时间来命名
	CTime time = CTime::GetCurrentTime();//获取当前时间
	CString sCurDate=time.Format("%Y年%m月%d日%H:%M 星期%w");
	CString sCurDate2=time.Format("%Y年%m月%d日%H:%M 星期%w %H:%M:%S");
	CString NameTemp=time.Format("Det%Y%m%d_%H%M%S_");//按照时间对文件夹命名
	CString CTemp;
	CTemp.Format(_T("WD_%03d"),drumdata.NoOfDrum);
	DecSetupData.NoOfWD=CTemp;

	double density=drumdata.Density;//88888
	DecSetupData.WeightOfDrum=density*PI*pow(DecSetupData.GWDD/2,2)*DecSetupData.GWDH/1000;
	DecSetupData.IsTransmission=false;
	DecSetupData.nDetectionType=drumdata.TypeOfDetection;//*****
	if(DecSetupData.nDetectionType==0) 
	{
		CTemp.Format(_T("%sSGS"),NameTemp);DecSetupData.DetectionType=_T("SGS");
		DecSetupData.DSNumY=1;	//Times of transtation
	//	DecSetupData.DSNumZ=3;	//Number of segments
	//	DecSetupData.DSNumA=1;	//Times of rotation
	//	DecSetupData.DSLY=0;	//Step length of transtation
	//	DecSetupData.DSLZ=10;	//Step length of rise
	//	DecSetupData.DSLA=0;	//Step length of rotation
		DecSetupData.NuGridsX=DecSetupData.DSNumA;
		DecSetupData.NuGridsY=DecSetupData.DSNumY;
		DecSetupData.NuGridsZ=DecSetupData.DSNumZ;
	}
	else if(DecSetupData.nDetectionType==1) //TGS
	{
		CTemp.Format(_T("%sTGS"),NameTemp);DecSetupData.DetectionType=_T("TGS");
		DecSetupData.DSNumY=4;	//Times of transtation
	//	DecSetupData.DSNumZ=11;	//Number of segments
	//	DecSetupData.DSNumA=24;	//Times of rotation
	//	DecSetupData.DSLY=7;	//Step length of transtation
	//	DecSetupData.DSLZ=10;	//Step length of rise
	//	DecSetupData.DSLA=15;	//Step length of rotation
	}
	else if(DecSetupData.nDetectionType==2)//ISGS
	{
		CTemp.Format(_T("%sISGS"),NameTemp);DecSetupData.DetectionType=_T("ISGS");
		DecSetupData.DSNumY=2;	//Times of transtation
	//	DecSetupData.DSNumZ=9;	//Number of segments
		DecSetupData.DSNumA=1;	//Times of rotation
	//	DecSetupData.DSLY=17.5;	//Step length of transtation
	//	DecSetupData.DSLZ=10;	//Step length of rise
	//	DecSetupData.DSLA=0;	//Step length of rotation
	//	DecSetupData.NuGridsX=1;
	//	DecSetupData.NuGridsY=1;
	//	DecSetupData.NuGridsZ=9;
		DecSetupData.MaxIteration=10000;
	}
	else if(DecSetupData.nDetectionType==3)//STGS2
	{
		CTemp.Format(_T("%sSTGS"),NameTemp);DecSetupData.DetectionType=_T("STGS");
		DecSetupData.DSNumY=2;	//Times of transtation***
	//	DecSetupData.DSNumZ=11;	//Number of segments
		DecSetupData.DSNumA=1;	//Times of rotation
	//	DecSetupData.DSLY=21;	//Step length of transtation****
	//	DecSetupData.DSLZ=10;	//Step length of rise
	//	DecSetupData.DSLA=0;	//Step length of rotation
		DecSetupData.NuGridsX=DecSetupData.DSNumA;
		DecSetupData.NuGridsY=DecSetupData.DSNumY;
		DecSetupData.NuGridsZ=DecSetupData.DSNumZ;
		DecSetupData.MaxIteration=500;
		DecSetupData.IsEqualVolumeOfGrids=m_IsEqualB;//****
	}
	else if(DecSetupData.nDetectionType==4)//STGS4
	{
		CTemp.Format(_T("%sSTGS"),NameTemp);DecSetupData.DetectionType=_T("STGS");
		DecSetupData.DSNumY=4;	//Times of transtation***
	//	DecSetupData.DSNumZ=11;	//Number of segments
		DecSetupData.DSNumA=1;	//Times of rotation
	//	DecSetupData.DSLY=10.5;	//Step length of transtation****
	//	DecSetupData.DSLZ=10;	//Step length of rise
	//	DecSetupData.DSLA=0;	//Step length of rotation
		DecSetupData.NuGridsX=DecSetupData.DSNumA;
		DecSetupData.NuGridsY=DecSetupData.DSNumY;
		DecSetupData.NuGridsZ=DecSetupData.DSNumZ;
		DecSetupData.MaxIteration=500;
		DecSetupData.IsEqualVolumeOfGrids=m_IsEqualB;//****
	}
	else if(DecSetupData.nDetectionType==5)//STGS8
	{
		CTemp.Format(_T("%sSTGS"),NameTemp);DecSetupData.DetectionType=_T("STGS");
		DecSetupData.DSNumY=8;	//Times of transtation***
		//DecSetupData.DSNumZ=11;	//Number of segments
		DecSetupData.DSNumA=1;	//Times of rotation
	//	DecSetupData.DSLY=3.5;	//Step length of transtation****
	//	DecSetupData.DSLZ=10;	//Step length of rise
	//	DecSetupData.DSLA=0;	//Step length of rotation
		DecSetupData.NuGridsX=DecSetupData.DSNumA;
		DecSetupData.NuGridsY=DecSetupData.DSNumY;
		DecSetupData.NuGridsZ=DecSetupData.DSNumZ;
		DecSetupData.MaxIteration=2000;
		DecSetupData.IsEqualVolumeOfGrids=m_IsEqualB;//****
	}
	else if(DecSetupData.nDetectionType==8)//STGS,动网格
	{
		CTemp.Format(_T("%sDWGSTGS"),NameTemp);DecSetupData.DetectionType=_T("DWGSTGS");
		DecSetupData.DSNumY=4;	//Times of transtation***
		//DecSetupData.DSNumZ=11;	//Number of segments
		DecSetupData.DSNumA=1;	//Times of rotation
	//	DecSetupData.DSLY=10.5;	//Step length of transtation****
	//	DecSetupData.DSLZ=10;	//Step length of rise
	//	DecSetupData.DSLA=0;	//Step length of rotation
		DecSetupData.NuGridsX=DecSetupData.DSNumA;
		DecSetupData.NuGridsY=DecSetupData.DSNumY;
		DecSetupData.NuGridsZ=DecSetupData.DSNumZ;
		DecSetupData.MaxIteration=500;
		DecSetupData.IsEqualVolumeOfGrids=m_IsEqualB;//****
	}
	m_Folder_Using.Format(_T("%sDetection\\%s\\"),m_Folder,CTemp);
	//创建文件夹
	CreateDirectory(m_Folder_Using,NULL);

	if(PathIsDirectory(m_Folder_Using) )
	{	
		ofstream output;
		output.open(m_Folder_Using+_T("_DetInformation.txt"),ios::trunc);
		output<<(LPCTSTR)_T("=2=建立文件夹时间：")<<(LPCTSTR)sCurDate2<<endl;
		output.close();
	}
	return CTemp;
}

//SGS透射测量
bool AnalogDetectionCalculation::SGS_TransDetection(void)
{
	TriDNodeHeadDefine *DensitySample;
	DensitySample=TriDataB.ReadData(m_Folder+_T("Detection\\DensitySample.dat"));
	if(!DensitySample) return false;

	//创建指针
	int NuX=1;//DecSetupData.DSNumA;
	int NuY=1;//DecSetupData.DSNumY;
	int NuZ=DecSetupData.DSNumZ;

	TriDNodeHeadDefine * HeadTemp=TriDataB.CreatNodes(NuX,NuY,NuZ);
	if(!HeadTemp) {TriDataB.DeleteNodes(DensitySample);return false;}
	HeadTemp->DimensionOfNodes=3;
	HeadTemp->Title=_T("_TransDect.dat");
	int ndata;
	for(ndata=0;ndata<MaxNuDataInTriDNode&&ndata<DensitySample->NuDataUsed;ndata++)
	{
		HeadTemp->VARIABLESData[ndata]=DensitySample->VARIABLESData[ndata];//节点数据空间的横坐标，如果读数据说明是数据，自动识别成double类型;
		HeadTemp->VARIABLES[ndata]=DensitySample->VARIABLES[ndata];//节点数据说明，随数据输出;
	}
	HeadTemp->IsScientific=true;
	HeadTemp->TitleXYZ[0]=_T("Angle(°)");//x坐标名称;
	HeadTemp->TitleXYZ[1]=_T("dY(cm)");//y坐标名称;
	HeadTemp->TitleXYZ[2]=_T("Height(cm)");//z坐标名称;
	HeadTemp->NuDataUsed=DensitySample->NuDataUsed;//数据空间使用数据的个数;
	int i;	
	TriDNodeDefine *dataz=HeadTemp->FirstNode;
	TriDNodeDefine *dataDz=DensitySample->FirstNode;
	double ud[MaxNuDataInTriDNode];
	for(ndata=0;ndata<MaxNuDataInTriDNode&&ndata<HeadTemp->NuDataUsed;ndata++)
	{
		ud[ndata]=CAttenuationPerMass(HeadTemp->VARIABLESData[ndata]) ;
	}
	for(i=0;i<NuZ&&dataz;i++,dataz=dataz->NextZ,dataDz=dataDz->NextZ)
	{
		dataz->x=0;
		dataz->y=DecSetupData.GWDD*0.5;
		dataz->z=i*DecSetupData.DSLZ+DecSetupData.GHPGDZ;
		for(ndata=0;ndata<MaxNuDataInTriDNode&&ndata<HeadTemp->NuDataUsed;ndata++)
		dataz->data[ndata]=exp(-1.0*ud[ndata]*dataDz->data[ndata]*DecSetupData.GWDD);			
	}
	TriDataB.outputdata_3d(HeadTemp,m_Folder_Using+HeadTemp->Title,_T("xyz"));//输出计算结果
	TriDataB.DeleteNodes(HeadTemp);//删除节点
	TriDataB.DeleteNodes(DensitySample);
	return true;
}
//铅的质量衰减系数（cm2/g），能量范围0.1MeV至5MeV，Engergy单位为MeV
double AnalogDetectionCalculation::CAttenuationPerMassOfPb(double Energy) 
{
	return 10*exp(2.2003*pow(log10(Energy),2.0)-2.1953*pow(log10(Energy),1.0)-4.9528);
}
//轻质材料的质量衰减系数（cm2/g），能量范围0.1MeV至5MeV，Engergy单位为MeV
double AnalogDetectionCalculation::CAttenuationPerMass(double Energy) 
{
	return 10*exp(-1.0643*log10(Energy)-5.0829);
}

//对废物桶创建密度矩阵
TriDNodeHeadDefine *AnalogDetectionCalculation::CreatDensityOfWD(int NuX,int NuY,int NuZ)
{
	TriDNodeHeadDefine * m_DensityOfWD=TriDataB.CreatNodes(NuX,NuY,NuZ);
	if(!m_DensityOfWD) return false;
	m_DensityOfWD->Title=_T("Results_Density.dat");//数据空间的名称，可修改;
	int i,j,k,ndata,NData=1;
	m_DensityOfWD->TitleXYZ[0]=_T("Angle(°)");//x坐标名称;
	m_DensityOfWD->TitleXYZ[1]=_T("Radius(cm)");//y坐标名称;
	m_DensityOfWD->TitleXYZ[2]=_T("Height(cm)");//z坐标名称;
	m_DensityOfWD->NuDataUsed=NData;//数据空间使用数据的个数;	
	m_DensityOfWD->IsScientific=true;
	double MeanDensity=DecSetupData.WeightOfDrum*1000/(PI*pow(DecSetupData.GWDD*0.5,2.0)*DecSetupData.GWDH);//单位：g/cm3
	TriDNodeDefine *dataz=m_DensityOfWD->FirstNode,*datax,*datay;
	//赋坐标值
	for(i=0;i<NuZ && dataz;i++,dataz=dataz->NextZ)
	{
		for(j=0,datay=dataz;j<NuY && datay;j++,datay=datay->NextY)
		{
			for(k=0,datax=datay;k<NuX && datax;k++,datax=datax->NextX)
			{
				datax->x=360.0*k/NuX;
				datax->y=DecSetupData.GWDD*(j+0.5)/NuY;
				datax->z=DecSetupData.GWDH*(i+0.5)/NuZ;
				for(ndata=0;ndata<MaxNuDataInTriDNode&&ndata<NData;ndata++)
					datax->data[ndata]=MeanDensity;	//平均密度
			}		
		}
	}
	return m_DensityOfWD;
}

//SGS发射测量
//一个探测器位置的探测
double AReal[5];
bool AnalogDetectionCalculation::SGS_EmisDetection()
{
	int i,j,k;
	for(i=0;i<10&&i<_NdY;i++)
	if(!RL_E_WD[i]) return false;
	TriDNodeHeadDefine *DensitySample;
	if(DecSetupData.IsTransmission) DensitySample=TriDataB.ReadData(m_Folder+_T("Detection\\DensitySample.dat"));
	else DensitySample=CreatDensityOfWD(1,1,DecSetupData.DSNumZ);
	if(!DensitySample) return false;
	//创建指针
	int NuX=1;//DecSetupData.DSNumA;
	int NuY=DecSetupData.DSNumY;//DecSetupData.DSNumY;
	int NuZ=DecSetupData.DSNumZ;//SGS
	TriDNodeHeadDefine * HeadTemp=TriDataB.CreatNodes(NuX,NuY,NuZ);
	if(!HeadTemp) return false;
	//AfxMessageBox("3");
	HeadTemp->DimensionOfNodes=3;
	HeadTemp->Title=_T("_EmisDect.dat");
	int ndata;
	CString CTemp;
	for(ndata=0;ndata<MaxNuDataInTriDNode && ndata<m_NuDataUsed;ndata++)
	{
		HeadTemp->VARIABLESData[ndata]=m_Energy[ndata];//节点数据空间的横坐标，如果读数据说明是数据，自动识别成double类型;
		CTemp.Format(_T("%g"),m_Energy[ndata]);
		HeadTemp->VARIABLES[ndata]=CTemp;//节点数据说明，随数据输出;
	}
	HeadTemp->IsScientific=true;
	HeadTemp->TitleXYZ[0]=_T("Angle(°)");//x坐标名称;
	HeadTemp->TitleXYZ[1]=_T("dY(cm)");//y坐标名称;
	HeadTemp->TitleXYZ[2]=_T("Height(cm)");//z坐标名称;
	HeadTemp->NuDataUsed=m_NuDataUsed;//数据空间使用数据的个数;

	ofstream output;
	output.open(m_Folder_Using+"AReal.txt",ios::trunc); 
	output<<"NoY\tNDataInCali\tActivity\tdensity\tROfSource\tHOfSource\tAReal[0]\tAReal[1]\tAReal[2]\tAReal[3]\tAReal[4]"<<endl;

	TriDNodeDefine *dataz,*datay=HeadTemp->FirstNode;
	TriDNodeDefine *dataDz;
	int NoY;
	double y;
	for(j=0;j<NuY&&datay;j++,datay=datay->NextY)//偏心方向测量
	{
		y=j*DecSetupData.DSLY+DecSetupData.GHPGDY;
		NoY=int(floor(y/3.5+0.5));//探测器所在偏心位置的序号，即3.5cm的倍数
		for(i=0,dataDz=DensitySample->FirstNode,dataz=datay;i<NuZ&&dataz&&dataDz;i++,dataz=dataz->NextZ,dataDz=dataDz->NextZ)//垂直方向测量
		{
				dataz->x=0;
				dataz->y=y;
				dataz->z=i*DecSetupData.DSLZ+DecSetupData.GHPGDZ;
				for(k=0;k<m_NuSource;k++)//不同点源
				{
					if((i!=0&&-0.5*DecSetupData.DSLZ<(m_Height[k]-dataz->z)&&(m_Height[k]-dataz->z)<=0.5*DecSetupData.DSLZ)||(i==0&&fabs(m_Height[k]-dataz->z)<=0.5*DecSetupData.DSLZ))//如果源在探测器的当前层，则进行测量
					{
						SGS_EmisDetection_OnePositionOfDetection(dataz,NoY,m_NoData[k],m_NoEnergy[k],m_Activity[k],dataDz->data[0],m_Radius[k],m_Height[k]-dataz->z);
						output<<NoY<<"\t"<<m_NoEnergy[k]<<"\t"<<m_Activity[k]<<"\t"<<dataDz->data[0]<<"\t"<<m_Radius[k]<<"\t"<<m_Height[k]-dataz->z<<"\t"<<AReal[0]<<"\t"<<AReal[1]<<"\t"<<AReal[2]<<"\t"<<AReal[3]<<"\t"<<AReal[4]<<endl;
					}
				}
		}		
	}
	output.close();
	RandomError(HeadTemp,DecSetupData.DSigleT);//添加误差，100s
	TriDataB.outputdata_3d(HeadTemp,m_Folder_Using+HeadTemp->Title,_T("xyz"));//输出计算结果
	TriDataB.DeleteNodes(HeadTemp);//删除节点
	TriDataB.DeleteNodes(DensitySample);

	return true;
}
//一个探测器位置的探测,各参数意义：data探测节点，NoY偏心序号，ndata探测指针里的数据序号，NDataInCali刻度文件里能量序号,Activity源的活度,density密度,ROfSource源的半径,HOfSource源的高度（相对探测器的高度）
int key=0;
void AnalogDetectionCalculation::SGS_EmisDetection_OnePositionOfDetection(TriDNodeDefine *data,int NoY, int ndata,int NDataInCali,double Activity,double density,double ROfSource,double HOfSource)
{
	TriDNodeDefine *dataWDx,*dataWDy,*dataWDz;
	double XR[50],YR[50],XH[50],YH[50],XD[50],YD[50],xdata[1],ydata[1];
	int NR,NH,ND;
	CString CTemp;

	for(int i=0;i<5;i++)
		AReal[i]=0;
	//for(int ndata=0;ndata<MaxNuDataInTriDNode&&ndata<m_NuDataUsed;ndata++)//能量
	{
		for(NH=0,dataWDz=RL_E_WD[NoY]->FirstNode;NH<RL_E_WD[NoY]->NuNodesInZ&&dataWDz;NH++,dataWDz=dataWDz->NextZ)//高度方向插值
		{
			for(NR=0,dataWDy=dataWDz;NR<RL_E_WD[NoY]->NuNodesInY&&dataWDy;NR++,dataWDy=dataWDy->NextY)//半径方向插值，注意半径由大到小排列
			{
				for(ND=0,dataWDx=dataWDy;ND<RL_E_WD[NoY]->NuNodesInX&&dataWDx;ND++,dataWDx=dataWDx->NextX)//密度插值
				{
					XD[ND]=dataWDx->x;
					YD[ND]=dataWDx->data[NDataInCali];
				}
				xdata[0]=density;
				TriDataB.SplineInterpolation(XD, YD, ND, xdata,ydata,1,true);//密度插值
				XR[RL_E_WD[NoY]->NuNodesInY-1-NR]=dataWDy->y;
				YR[RL_E_WD[NoY]->NuNodesInY-1-NR]=ydata[0];
			}
			xdata[0]=ROfSource;
			TriDataB.SplineInterpolation(XR, YR, NR, xdata,ydata,1,true);//半径插值
			XH[NH]=dataWDz->z;
			YH[NH]=ydata[0];
				
		}	
		//当前层
		xdata[0]=fabs(HOfSource-DecSetupData.DSLZ*0);
		TriDataB.SplineInterpolation(XH, YH, NH, xdata, ydata,1,true);//高度插值，考虑串层影响，5层
		data->data[ndata]+=Activity*ydata[0];
		AReal[2]=ydata[0];
	//	if(key++==0){CTemp.Format(_T("%d %f %f"),ndata,xdata[0],data->data[ndata]);AfxMessageBox(CTemp);}
		//下一层
		if(data->LastZ)
		{
			xdata[0]=fabs(HOfSource+DecSetupData.DSLZ*1);
			TriDataB.SplineInterpolation(XH, YH, NH, xdata, ydata,1,true);//高度插值，考虑串层影响，5层
			data->LastZ->data[ndata]+=Activity*ydata[0];
			AReal[1]=ydata[0];
		}
		//上一层
		if(data->NextZ)
		{
			xdata[0]=fabs(HOfSource-DecSetupData.DSLZ*1);
			TriDataB.SplineInterpolation(XH, YH, NH, xdata, ydata,1,true);//高度插值，考虑串层影响，5层
			data->NextZ->data[ndata]+=Activity*ydata[0];
			AReal[3]=ydata[0];
		}
	
		//下二层
		if(data->LastZ&&data->LastZ->LastZ)
		{
			xdata[0]=fabs(HOfSource+DecSetupData.DSLZ*2);
			TriDataB.SplineInterpolation(XH, YH, NH, xdata, ydata,1,true);//高度插值，考虑串层影响，5层
			data->LastZ->LastZ->data[ndata]+=Activity*ydata[0];
			AReal[0]=ydata[0];
		}

		//上二层
		if(data->NextZ&&data->NextZ->NextZ)
		{
			xdata[0]=fabs(HOfSource-DecSetupData.DSLZ*2);
			TriDataB.SplineInterpolation(XH, YH, NH, xdata, ydata,1,true);//高度插值，考虑串层影响，5层
			data->NextZ->NextZ->data[ndata]+=Activity*ydata[0];
			AReal[4]=ydata[0];
		}		
	}
}
//对探测器的计数率添加误差
void AnalogDetectionCalculation::RandomError(TriDNodeHeadDefine * HeadTemp,double dtime)
{
	if(!HeadTemp) return;
	if(dtime<=0) return;
	if(dtime<=1) dtime=1;
	int ndata;
	double dot_Error;
	int plusormius;
	TriDNodeDefine *dataz=HeadTemp->FirstNode,*datay,*datax;
	while(dataz)
	{
		datay=dataz;
		while(datay)
		{
			datax=datay;
			while(datax)
			{
				for(ndata=0;ndata<MaxNuDataInTriDNode&&ndata<HeadTemp->NuDataUsed;ndata++)
				{
					if((rand()%10000)%2==0) plusormius=1;//随机正负
					else plusormius=-1;
					dot_Error=plusormius*sqrt(datax->data[ndata]*dtime)/dtime;
					datax->data[ndata]+=dot_Error;//计数的误差
				}
				datax=datax->NextX;	
			}
			datay=datay->NextY;	
		}
		dataz=dataz->NextZ;	
	}
}