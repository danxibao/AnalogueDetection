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
	bool m_IsOnPause;//�Ƿ���ͣ
	bool m_IsOnCalculation;//�Ƿ����ڼ���
	int N_Progress;//����
};

class DecSetupDefine
{
public:
	CString DetectionType;//̽������
	int nDetectionType;
	CString NoOfWD;//����Ͱ���
	CString TypeOfWD;//����Ͱ����
	double GWDD;//����Ͱֱ��
	double GWDH;//����Ͱ�߶�
	double GHPGDX;//̽����������ʼλ��x���꣬cm
	double GHPGDY;//̽����������ʼλ��y���꣬cm
	double GHPGDZ;//̽����������ʼλ��z���꣬cm
	int DSNumY;//ƽ������
	int DSNumZ;//��������
	int DSNumA;//ת������
	double DSLY;//ƽ���������룬cm
	double DSLZ;//�����������룬cm
	double DSLA;//ת���������룬cm
	double DSigleT;//̽��ʱ�䣬min
	double VORotation;//ת��������min-1
	bool IsTransmission;//�Ƿ����͸�����
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
	//////////////////////////////////////////////////////////��ȡ̽������
	DecSetupDefine DecSetupData;
	bool ReadDetectionSetup(CString FullFileName);
	//������Ŀ
	CString CreatOneDetectionProject(DrumDefine drumdata);
	//////////////////////////////////////////////////////////�洢����
	bool SaveDetectionSetup(CString FullFileName);
	//SGS͸�����
	bool SGS_TransDetection(void);
	TriDNodeHeadDefine *TDHead;
	//Ǧ������˥��ϵ����cm2/g����������Χ0.1MeV��5MeV��Engergy��λΪMeV
	double CAttenuationPerMassOfPb(double Energy);
	//���ʲ��ϵ�����˥��ϵ����cm2/g����������Χ0.1MeV��5MeV��Engergy��λΪMeV
	double CAttenuationPerMass(double Energy); 
	//SGS�������
	bool SGS_EmisDetection();
	int m_NuDataUsed;//�ܹ����ٸ�����
	int m_NuSource;//Դ�ĸ���
	int m_NoData[200];
	double m_Energy[10];
	int m_NoEnergy[200];//ĳ��Դ�������ı��
	double m_Activity[200];//ĳ��Դ�����
	double m_Radius[200];//ĳ��Դ���뾶
	double m_Height[200];//ĳ��Դ���߶�
	bool m_IsEqualB;


	void SGS_EmisDetection_OnePositionOfDetection(TriDNodeDefine *data,int NoY,int ndata,int NDataInCali,double Activity,double density,double ROfSource,double HOfSource);
	TriDNodeHeadDefine *RL_E_WD[_NdY];
	//���趨��Դ�������ж�������Ϣ
	void IdentifySourceInf(double Energy,int NoSource);
	TriDNodeHeadDefine *EDHead;
	//�Է���Ͱ�����ܶȾ���
	TriDNodeHeadDefine *CreatDensityOfWD(int NuX,int NuY,int NuZ);

	void RandomError(TriDNodeHeadDefine * HeadTemp,double dtime);
};

#endif // !defined(AFX_ANALOGDETECTIONCALCULATION_H__6398358F_CFCD_438E_AA94_440B5C36CB20__INCLUDED_)
