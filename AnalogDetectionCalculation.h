// AnalogDetectionCalculation.h: interface for the AnalogDetectionCalculation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALOGDETECTIONCALCULATION_H__6398358F_CFCD_438E_AA94_440B5C36CB20__INCLUDED_)
#define AFX_ANALOGDETECTIONCALCULATION_H__6398358F_CFCD_438E_AA94_440B5C36CB20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "math.h"
#include "TriData.h"
#include "ReadSource.h"
#define _NdY 16
#include <fstream>
using namespace std;
struct CalculationStatus
{
public:
	CString m_InfText;
	bool m_IsOnPause;//是否暂停
	bool m_IsOnCalculation;//是否正在计算
	int N_Progress;//进度
};

class DecSetupDefine
{
public:
	CString DetectionType;//探测类型
	int nDetectionType;
	CString NoOfWD;//废物桶编号
	CString TypeOfWD;//废物桶类型
	double GWDD;//废物桶直径
	double GWDH;//废物桶高度
	double GHPGDX;//探测器测量初始位置x坐标，cm
	double GHPGDY;//探测器测量初始位置y坐标，cm
	double GHPGDZ;//探测器测量初始位置z坐标，cm
	int DSNumY;//平动次数
	int DSNumZ;//升降次数
	int DSNumA;//转动次数
	double DSLY;//平动步进距离，cm
	double DSLZ;//升降步进距离，cm
	double DSLA;//转动步进距离，cm
	double DSigleT;//探测时间，min
	double VORotation;//转动次数，min-1
	bool IsTransmission;//是否进行透射测量
	double WeightOfDrum;
	int TypeOfIteration;
	int MaxIteration;
	int NuGridsX;
	int NuGridsY;
	int NuGridsZ;
	bool IsEqualVolumeOfGrids;
};

class AnalogDetectionCalculation  
{
public:
	AnalogDetectionCalculation();
	virtual ~AnalogDetectionCalculation();

	CString m_Folder;
	CString m_Folder_Using;
	CalculationStatus CalThreadStatus;
	bool StartCalculation();
	TriData TriDataB;
	ReadSource ReadSourceB;
	//////////////////////////////////////////////////////////读取探测设置
	DecSetupDefine DecSetupData;
	bool ReadDetectionSetup(CString FullFileName);
	//创建项目
	CString CreatOneDetectionProject(DrumDefine drumdata);
	//////////////////////////////////////////////////////////存储设置
	bool SaveDetectionSetup(CString FullFileName);
	//SGS透射测量
	bool SGS_TransDetection(void);
	TriDNodeHeadDefine *TDHead;
	//铅的质量衰减系数（cm2/g），能量范围0.1MeV至5MeV，Engergy单位为MeV
	double CAttenuationPerMassOfPb(double Energy);
	//轻质材料的质量衰减系数（cm2/g），能量范围0.1MeV至5MeV，Engergy单位为MeV
	double CAttenuationPerMass(double Energy); 
	//SGS发射测量
	bool SGS_EmisDetection();
	int m_NuDataUsed;//总共多少个能量
	int m_NuSource;//源的个数
	int m_NoData[200];
	double m_Energy[10];
	int m_NoEnergy[200];//某个源，能量的标号
	double m_Activity[200];//某个源，活度
	double m_Radius[200];//某个源，半径
	double m_Height[200];//某个源，高度
	bool m_IsEqualB;


	void SGS_EmisDetection_OnePositionOfDetection(TriDNodeDefine *data,int NoY,int ndata,int NDataInCali,double Activity,double density,double ROfSource,double HOfSource);
	TriDNodeHeadDefine *RL_E_WD[_NdY];
	//对设定的源能量，判读次序信息
	void IdentifySourceInf(double Energy,int NoSource);
	TriDNodeHeadDefine *EDHead;
	//对废物桶创建密度矩阵
	TriDNodeHeadDefine *CreatDensityOfWD(int NuX,int NuY,int NuZ);

	void RandomError(TriDNodeHeadDefine * HeadTemp,double dtime);
};

#endif // !defined(AFX_ANALOGDETECTIONCALCULATION_H__6398358F_CFCD_438E_AA94_440B5C36CB20__INCLUDED_)
