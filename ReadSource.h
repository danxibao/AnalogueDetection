// ReadSource.h: interface for the ReadSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_READSOURCE_H__D74EAD64_EBD4_4F52_AD59_8DCB2B9727DB__INCLUDED_)
#define AFX_READSOURCE_H__D74EAD64_EBD4_4F52_AD59_8DCB2B9727DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MaxNuSources 200
#define MaxNuDrums 100
struct SourceDefine
{
	int no;
	double Radius;
	double Angle;
	double Height;
	int NoEnergy;
	double Activity;
};
struct DrumDefine
{
	int NoOfDrum;
	int NuSources;
	int TypeOfDetection;
	bool IsEqualB;
	double Density;
	SourceDefine SourceData[MaxNuSources];
};
class ReadSource  
{
public:
	ReadSource();
	virtual ~ReadSource();

	int NuDrums;
	DrumDefine DrumData[MaxNuDrums];
	bool ReadDetectionSources(CString FullFileName);


};

#endif // !defined(AFX_READSOURCE_H__D74EAD64_EBD4_4F52_AD59_8DCB2B9727DB__INCLUDED_)
