// TriData.cpp: implementation of the TriData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TriData.h"

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <math.h>
using namespace std;


	//三维数据空间使用初始化;
void TriData::TriDataInitialization(void)
{
	TriDataUsing=NULL;
}

//生成并初始化节点,节点号、坐标置零，链接指针置NULL;
TriDNodeDefine * TriData::CreatNode()
{
	TriDNodeDefine *node;
	node=new TriDNodeDefine;
	node->NoX=-1;
	node->NoY=-1;
	node->NoZ=-1;
	node->x=-1;
	node->y=-1;
	node->z=-1;
	for(int i=0;i<MaxNuDataInTriDNode;i++)
	node->data[i]=0;
	node->s=0;
	node->xx=0;
	node->yy=0;
	node->zz=0;
	node->Inf.Empty();
	node->NextX=NULL;
	node->LastX=NULL;
	node->NextY=NULL;
	node->LastY=NULL;
	node->NextZ=NULL;
	node->LastZ=NULL;
	node->Link=NULL;//特殊用途的链接;
	node->NuLink=0;
	return node;
}
//移动节点，移动一步,当前节点，移动方向，返回移动后的节点指针，方向1、2、3分别为x，y，z方向，负号表示负方向;
TriDNodeDefine * TriData::MoveNode(TriDNodeDefine *node,int direction)
{
	if(!node) return NULL;
	else if(direction==1) return node->NextX;//方向1为x正方向移动一步;
	else if(direction==-1) return node->LastX;//方向-1为x负方向移动一步;
	else if(direction==2) return node->NextY;//方向2为y正方向移动一步;
	else if(direction==-2) return node->LastY;//方向-2为y负方向移动一步;
	else if(direction==3) return node->NextZ;//方向3为z正方向移动一步;
	else if(direction==-3) return node->LastZ;//方向-3为z负方向移动一步;
	else return NULL;
}

//两个节点之间的链接，node1指向node2,direction=1 x方向，2 y方向，3 z方向;
void TriData::ConnectTwoNodes(TriDNodeDefine *node1,TriDNodeDefine *node2,int direction)
{
	if(!node1||!node2) return;
	if(direction==1) 
	{		node1->NextX=node2;node2->LastX=node1;	}
	else if(direction==2) 
	{		node1->NextY=node2;node2->LastY=node1;	}
	else 
	{		node1->NextZ=node2; node2->LastZ=node1;	}
}
//两行节点之间的链接，node1所在的行指向node2所在的行,direction=1 x方向，2 y方向，3 z方向;
void TriData::ConnectTwoLineNodes(TriDNodeDefine *node1,TriDNodeDefine *node2,int directionMove,int directionLink)
{
	while(node1||node2)
	{
		ConnectTwoNodes(node1,node2,directionLink);
		node1=MoveNode(node1,directionMove);
		node2=MoveNode(node2,directionMove);
	}	
}
//两面节点之间的链接，node1所在面指向node2所在面,direction=1 x方向，2 y方向，3 z方向;
void TriData::ConnectTwoFaceNodes(TriDNodeDefine *node1,TriDNodeDefine *node2,int directionLink)
{
	int direction1,direction2;
	if(directionLink==1) {direction1=2;direction2=3;}
	else if(directionLink==2) {direction1=1;direction2=3;}
	else {direction1=1;direction2=2;}
	while(node1||node2)
	{
		ConnectTwoLineNodes(node1,node2,direction2,directionLink);
		node1=MoveNode(node1,direction1);
		node2=MoveNode(node2,direction1);
	}	
}
//创建头节点;
TriDNodeHeadDefine * TriData::CreatNodeHead()
{
	TriDNodeHeadDefine *nodehead;
	CString CTemp;
	nodehead=new TriDNodeHeadDefine;
	nodehead->DimensionOfNodes=0;//维数，创建数据空间时根据个方向节点数是否大于1自动计算;
	nodehead->NuNodesInX=0;//x方向节点数，初始为零;
	nodehead->NuNodesInY=0;//y方向节点数，初始为零;
	nodehead->NuNodesInZ=0;//z方向节点数，初始为零;
	nodehead->NuNodesTotal=0;//总节点数，初始为零;
	nodehead->Title=_T("DataTemp.dat");//数据空间的名称，可修改;
	nodehead->FirstNode=NULL;//首节点;
	nodehead->EndNode=NULL;//尾节点;
	nodehead->UsingNode=NULL;//使用节点;
	nodehead->UsedNode=NULL;//上次使用节点;
	for(int i=0;i<MaxNuDataInTriDNode;i++)
	{
		nodehead->VARIABLESData[i]=i;//节点数据空间的横坐标，如果读数据说明是数据，自动识别成double类型;
		CTemp.Format(_T("VARIABLE%d"),i);
		nodehead->VARIABLES[i]=CTemp;//节点数据说明，随数据输出;
	}
	nodehead->TitleXYZ[0]=_T("x");//x坐标名称;
	nodehead->TitleXYZ[1]=_T("y");//y坐标名称;
	nodehead->TitleXYZ[2]=_T("z");//z坐标名称;
	nodehead->NuDataUsed=MaxNuDataInTriDNode;//数据空间使用数据的个数;
	nodehead->Inf=_T("No Remark");//空间的备注说明;
	nodehead->IsScientific=false;//数据输出时，是否科学计数法，不修改时即不采用;
	nodehead->IsOutputInf=false;//数据输出时，是否科学计数法，不修改时即不采用;
	nodehead->NuDecimal=8;//数据输出时，小数点位数，不修改为8位;
	return nodehead;
}
//将head1中的信息复制给head2，但是指针不复制;
void TriData::CopyHead(TriDNodeHeadDefine *nodehead1,TriDNodeHeadDefine *nodehead2)
{
	nodehead2->DimensionOfNodes=nodehead1->DimensionOfNodes;//维数，创建数据空间时根据个方向节点数是否大于1自动计算;
	nodehead2->NuNodesInX=nodehead1->NuNodesInX;//x方向节点数，初始为零;
	nodehead2->NuNodesInY=nodehead1->NuNodesInY;//y方向节点数，初始为零;
	nodehead2->NuNodesInZ=nodehead1->NuNodesInZ;//z方向节点数，初始为零;
	nodehead2->NuNodesTotal=nodehead1->NuNodesTotal;//总节点数，初始为零;
	nodehead2->NuNodesUsed=nodehead1->NuNodesUsed;//使用的总节点数，初始为零;
	nodehead2->Title=nodehead1->Title;//数据空间的名称，可修改;
	for(int i=0;i<MaxNuDataInTriDNode;i++)
	{
		nodehead2->VARIABLESData[i]=nodehead1->VARIABLESData[i];//节点数据空间的横坐标，如果读数据说明是数据，自动识别成double类型;
		nodehead2->VARIABLES[i]=nodehead1->VARIABLES[i];//节点数据说明，随数据输出;
	}
	nodehead2->TitleXYZ[0]=nodehead1->TitleXYZ[0];//x坐标名称;
	nodehead2->TitleXYZ[1]=nodehead1->TitleXYZ[1];//y坐标名称;
	nodehead2->TitleXYZ[2]=nodehead1->TitleXYZ[2];//z坐标名称;
	nodehead2->NuDataUsed=nodehead1->NuDataUsed;//数据空间使用数据的个数;
	nodehead2->Inf=nodehead1->Inf;//空间的备注说明;
	nodehead2->IsScientific=nodehead1->IsScientific;//数据输出时，是否科学计数法，不修改时即不采用;
	nodehead2->IsOutputInf=nodehead1->IsOutputInf;
	nodehead2->NuDecimal=nodehead1->NuDecimal;//数据输出时，小数点位数，不修改为8位;
}
//将head1中的信息复制给head2，但是节点数不变;
void TriData::CopyHeadInf(TriDNodeHeadDefine *nodehead1,TriDNodeHeadDefine *nodehead2)
{
	//nodehead2->DimensionOfNodes=nodehead1->DimensionOfNodes;//维数，创建数据空间时根据个方向节点数是否大于1自动计算;
	//nodehead2->Title=nodehead1->Title;//数据空间的名称，可修改;
	for(int i=0;i<MaxNuDataInTriDNode;i++)
	{
		nodehead2->VARIABLESData[i]=nodehead1->VARIABLESData[i];//节点数据空间的横坐标，如果读数据说明是数据，自动识别成double类型;
		nodehead2->VARIABLES[i]=nodehead1->VARIABLES[i];//节点数据说明，随数据输出;
	}
	nodehead2->TitleXYZ[0]=nodehead1->TitleXYZ[0];//x坐标名称;
	nodehead2->TitleXYZ[1]=nodehead1->TitleXYZ[1];//y坐标名称;
	nodehead2->TitleXYZ[2]=nodehead1->TitleXYZ[2];//z坐标名称;
	nodehead2->NuDataUsed=nodehead1->NuDataUsed;//数据空间使用数据的个数;
	nodehead2->Inf=nodehead1->Inf;//空间的备注说明;
	nodehead2->IsScientific=nodehead1->IsScientific;//数据输出时，是否科学计数法，不修改时即不采用;
	nodehead2->IsOutputInf=nodehead1->IsOutputInf;
	nodehead2->NuDecimal=nodehead1->NuDecimal;//数据输出时，小数点位数，不修改为8位;
}
//创建节点,x,y,z三个方向上的数量，初始数据都是零;
TriDNodeHeadDefine * TriData::CreatNodes(int NuX, int NuY, int NuZ)
{
	if(NuX>1000000||NuY>1000000||NuZ>1000000) { MessageBox(NULL,_T(" Too large nodes are demanded！"),_T("Warning"),MB_ICONWARNING | MB_OK); return NULL;}
	if(NuX<1||NuY<1||NuZ<1) { MessageBox(NULL,_T(" The number of nodes in one dimension should be larger than 1！"),_T("Warning"),MB_ICONWARNING | MB_OK); return NULL;}
	int i,j,k;
	TriDNodeDefine *node1,*node2,*nodeLine1,*nodeLine2,*nodeFace1,*nodeFace2;
	TriDNodeHeadDefine *nodehead;
	nodehead=CreatNodeHead();
	nodehead->NuNodesInX=NuX;
	nodehead->NuNodesInY=NuY;
	nodehead->NuNodesInZ=NuZ;
	nodehead->NuNodesTotal=NuX*NuY*NuZ;
	nodehead->NuNodesUsed=nodehead->NuNodesTotal;
	nodehead->DimensionOfNodes=3;
	if(NuX==1) nodehead->DimensionOfNodes--;
	if(NuY==1) nodehead->DimensionOfNodes--;
	if(NuZ==1) nodehead->DimensionOfNodes--;

	for(i=0;i<NuZ;i++)
	{
		for(j=0;j<NuY;j++)
		{
			for(k=0;k<NuX;k++)
			{
				node2=CreatNode();
				node2->NoX=k;
				node2->NoY=j;
				node2->NoZ=i;
				node2->x=node2->NoX;
				node2->y=node2->NoY;
				node2->z=node2->NoZ;
				if(i==0&&j==0&&k==0) nodehead->FirstNode=node2;
				if(k==0) nodeLine2=node2;
				else ConnectTwoNodes(node1,node2,1);
				node1=node2;
			}
			if(j==0) nodeFace2=nodeLine2;
			else ConnectTwoLineNodes(nodeLine1,nodeLine2,1,2);
			nodeLine1= nodeLine2;
		}
		if(i>0) ConnectTwoFaceNodes(nodeFace1,nodeFace2,3);
		nodeFace1=nodeFace2;
	}
	nodehead->EndNode=node2;
	return nodehead;
}
//创建节点无头结点,x,y,z三个方向上的数量，初始数据都是零;
TriDNodeDefine * TriData::CreatNodesWithoutHead(int NuX, int NuY, int NuZ)
{
	if(NuX>1000000||NuY>1000000||NuZ>1000000) { MessageBox(NULL,_T(" Too large nodes are demanded！"),_T("Warning"),MB_ICONWARNING | MB_OK); return NULL;}
	if(NuX<1||NuY<1||NuZ<1) { MessageBox(NULL,_T(" The number of nodes in one dimension should be larger than 1！"),_T("Warning"),MB_ICONWARNING | MB_OK); return NULL;}
	int i,j,k;
	TriDNodeDefine *node0,*node1,*node2,*nodeLine1,*nodeLine2,*nodeFace1,*nodeFace2;

	for(i=0;i<NuZ;i++)
	{
		for(j=0;j<NuY;j++)
		{
			for(k=0;k<NuX;k++)
			{
				node2=CreatNode();
				node2->NoX=k;
				node2->NoY=j;
				node2->NoZ=i;
				node2->x=node2->NoX;
				node2->y=node2->NoY;
				node2->z=node2->NoZ;
				if(i==0&&j==0&&k==0) node0=node2;
				if(k==0) nodeLine2=node2;
				else ConnectTwoNodes(node1,node2,1);
				node1=node2;
			}
			if(j==0) nodeFace2=nodeLine2;
			else ConnectTwoLineNodes(nodeLine1,nodeLine2,1,2);
			nodeLine1= nodeLine2;
		}
		if(i>0) ConnectTwoFaceNodes(nodeFace1,nodeFace2,3);
		nodeFace1=nodeFace2;
	}
	return node0;
}
//针对头指针删除节点;
void TriData::DeleteNodes(TriDNodeHeadDefine *nodehead)
{
	if(!nodehead) return;
	TriDNodeDefine *nodex,*nodey,*nodez,*node;
	nodez=nodehead->FirstNode;
	while(nodez)
	{////
		nodey=nodez;
		nodez=nodez->NextZ;
		while(nodey)
		{///
			nodex=nodey;
			nodey=nodey->NextY;
			while(nodex)
			{//
				node=nodex;
				nodex=nodex->NextX;
				delete node;
			}//
		}///
	}////
	delete nodehead;
}

//将节点的数据全部置零;
void TriData::SetNodesData(TriDNodeHeadDefine *nodehead,double data)
{
	if(!nodehead) return;
	TriDNodeDefine *nodex,*nodey,*nodez;
	int i;
	nodez=nodehead->FirstNode;
	while(nodez)
	{
		nodey=nodez;		
		while(nodey)
		{
			nodex=nodey;
			while(nodex)
			{
				for(i=0;i<MaxNuDataInTriDNode&&i<nodehead->NuDataUsed;i++)
					nodex->data[i]=data;
					nodex=nodex->NextX;
			}
			nodey=nodey->NextY;
		}
		nodez=nodez->NextZ;
	}
}

//输出节点数据到文件（三维格式，即包括IJK，即使为1）,格式可被tecplot读入，变量为：头指针，全名（包括路径），数据存储的优先方向（如“XYZ”，“ZYX”等）;
void TriData::outputdata_3d(TriDNodeHeadDefine *nodehead,CString FullName,CString Direction)
{
		if(!nodehead) return;
		if(!nodehead->FirstNode) return;
		int i,l;
		int Direction1,Direction2,Direction3;
		CString CTemp;
		char IJK[3]={'I','J','K'};
		double Dxyz[3];
		int NuXYZ[3];
		NuXYZ[0]=nodehead->NuNodesInX;
		NuXYZ[1]=nodehead->NuNodesInY;
		NuXYZ[2]=nodehead->NuNodesInZ;
		if(Direction.Find(_T("XYZ"))>=0||Direction.Find(_T("xyz"))>=0) {Direction1=1;Direction2=2;Direction3=3;}
		else if(Direction.Find(_T("XZY"))>=0||Direction.Find(_T("xzy"))>=0) {Direction1=1;Direction2=3;Direction3=2;}
		else if(Direction.Find(_T("YXZ"))>=0||Direction.Find(_T("yxz"))>=0) {Direction1=2;Direction2=1;Direction3=3;}
		else if(Direction.Find(_T("YZX"))>=0||Direction.Find(_T("yzx"))>=0) {Direction1=2;Direction2=3;Direction3=1;}
		else if(Direction.Find(_T("ZXY"))>=0||Direction.Find(_T("zxy"))>=0) {Direction1=3;Direction2=1;Direction3=2;}
		else if(Direction.Find(_T("ZYX"))>=0||Direction.Find(_T("zyx"))>=0) {Direction1=3;Direction2=2;Direction3=1;}
		else {MessageBox(NULL,_T(" Direction selection is wrong in 3D output."),_T("Warning"),MB_ICONWARNING | MB_OK);return;}

		ofstream output_data;
		output_data.open(FullName,ios::trunc);
		if(nodehead->IsScientific) output_data.setf(ios::scientific);
		else output_data.setf(ios::fixed,ios::floatfield);
		output_data.precision(nodehead->NuDecimal);
		
		CTemp.Format(_T("TITLE=\"%s\""),nodehead->Title);	
		output_data<<(LPCTSTR)CTemp<<endl;
		CTemp.Empty();
		CTemp.Format(_T("VARIABLES= \"%s\" \"%s\" \"%s\" "),nodehead->TitleXYZ[0],nodehead->TitleXYZ[1],nodehead->TitleXYZ[2]);
		output_data<<(LPCTSTR)CTemp;
		for(i=0;i<nodehead->NuDataUsed&&i<MaxNuDataInTriDNode;i++)
		{
			CTemp.Format(_T("\"%s\" "),nodehead->VARIABLES[i]);	
			output_data<<(LPCTSTR)CTemp;		
		}
		if(nodehead->IsOutputInf) output_data<<(LPCTSTR)_T("\"Status\"")<<endl;
		else output_data<<endl;
		CTemp.Empty();CTemp.Format(_T("ZONE T=\"3D Data\" %c=%d %c=%d %c=%d F=POINT\n"),IJK[0],NuXYZ[0],IJK[1],NuXYZ[1],IJK[2],NuXYZ[2]);
		output_data<<(LPCTSTR)CTemp;

		TriDNodeDefine *nodex,*nodey,*nodez;
		nodez=nodehead->FirstNode;
		while(nodez)
		{
			nodey=nodez;
			while(nodey)
			{
				nodex=nodey;	
				while(nodex)
				{
					Dxyz[0]=nodex->x;Dxyz[1]=nodex->y;Dxyz[2]=nodex->z;
					output_data<<Dxyz[0]<<" "<<Dxyz[1]<<" "<<Dxyz[2]<<" ";
					for(l=0;l<nodehead->NuDataUsed&&l<MaxNuDataInTriDNode;l++)	
					{
						//CTemp.Format("%8e",nodex->data[l]);
						//output_data<<(LPCTSTR)CTemp<<" ";
						output_data<<nodex->data[l]<<(LPCTSTR)_T(" ");
					}
					if(nodehead->IsOutputInf) output_data<<(LPCTSTR)nodex->Inf<<endl;
					else output_data<<endl;
					 nodex=MoveNode(nodex,Direction1);
				}
				nodey=MoveNode(nodey,Direction2);
			}
			nodez=MoveNode(nodez,Direction3);
		}
		output_data<<(LPCTSTR)_T("END");

		output_data.close();
}

//输出节点数据到文件（三维格式，即包括IJK，即使为1）,格式可被tecplot读入，变量为：头指针，全名（包括路径），数据存储的优先方向（如“XYZ”，“ZYX”等）;
void TriData::outputdata_3dJZB(TriDNodeHeadDefine *nodehead,CString FullName)
{
	if(!nodehead) return;
	if(!nodehead->FirstNode) return;
	int i,l;
	CString CTemp;

	ofstream output_data;
	output_data.open(FullName,ios::trunc);
	if(nodehead->IsScientific) output_data.setf(ios::scientific);
	else output_data.setf(ios::fixed,ios::floatfield);
	output_data.precision(nodehead->NuDecimal);

	output_data<<"TITLE=\""<<nodehead->Title<<"\"\n";
	output_data<<"VARIABLES= \"x\" \"y\" \"z\" ";
	for(i=0;i<nodehead->NuDataUsed&&i<MaxNuDataInTriDNode;i++)
		output_data<<"\""<<nodehead->VARIABLES[i]<<"\" ";
	if(nodehead->IsOutputInf) output_data<<"\"Status\"\n";
	else output_data<<"\n";
	CTemp.Empty();CTemp.Format(_T("ZONE T=\"3D Data\" I=%d J=%d K=%d F=POINT\n"),nodehead->NuNodesInX,nodehead->NuNodesInY,nodehead->NuNodesInZ);
	output_data<<CTemp;

	TriDNodeDefine *nodex,*nodey,*nodez;
	nodez=nodehead->FirstNode;
	while(nodez)
	{
		nodey=nodez;
		while(nodey)
		{
			nodex=nodey;	
			while(nodex)
			{
				output_data<<nodex->y*cos(nodex->x*PI/180)<<" "<<nodex->y*sin(nodex->x*PI/180)<<" "<<nodex->z<<" ";
				for(l=0;l<nodehead->NuDataUsed&&l<MaxNuDataInTriDNode;l++)	
				{
					CTemp.Format("%8e",nodex->data[l]);
					output_data<<CTemp<<" ";
				}
				if(nodehead->IsOutputInf) output_data<<nodex->Inf<<"\n";
				else output_data<<"\n";
				nodex=nodex->NextX;
			}
			nodey=nodey->NextY;
		}
		nodez=nodez->NextZ;
	}
	output_data<<"END";
	output_data.close();
}

//将二维数据以平面的方式输出到文件（二维格式）,格式可被tecplot读入，变量为：头指针，全名（包括路径），平面（如“XY”，“ZY”等），第几平面,第几个数据;
void TriData::outputdata_2d(TriDNodeHeadDefine *nodehead,CString FullName,CString Plane,int NoPlane,int NoData)
{
	if(!nodehead) return;
	if(!nodehead->FirstNode) return;
	int i;
	int NuPlane;
	int MoveDirection1,MoveDirection2,MoveDirection3;
	CString CTemp;
	if(Plane.Find(_T("XY"))>=0||Plane.Find(_T("xy"))>=0||Plane.Find(_T("xY"))>=0||Plane.Find(_T("Xy"))>=0) {MoveDirection1=1;MoveDirection2=2;MoveDirection3=3;NuPlane=nodehead->NuNodesInZ;}
	else if(Plane.Find(_T("YX"))>=0||Plane.Find(_T("yx"))>=0||Plane.Find(_T("yX"))>=0||Plane.Find(_T("Yx"))>=0) {MoveDirection1=2;MoveDirection2=1;MoveDirection3=3;NuPlane=nodehead->NuNodesInZ;}
	else if(Plane.Find(_T("XZ"))>=0||Plane.Find(_T("xz"))>=0||Plane.Find(_T("xZ"))>=0||Plane.Find(_T("Xz"))>=0) {MoveDirection1=1;MoveDirection2=3;MoveDirection3=2;NuPlane=nodehead->NuNodesInY;}
	else if(Plane.Find(_T("ZX"))>=0||Plane.Find(_T("zx"))>=0||Plane.Find(_T("zX"))>=0||Plane.Find(_T("Zx"))>=0) {MoveDirection1=3;MoveDirection2=1;MoveDirection3=2;NuPlane=nodehead->NuNodesInY;}
	else if(Plane.Find(_T("YZ"))>=0||Plane.Find(_T("yz"))>=0||Plane.Find(_T("yZ"))>=0||Plane.Find(_T("Yz"))>=0) {MoveDirection1=2;MoveDirection2=3;MoveDirection3=1;NuPlane=nodehead->NuNodesInX;}
	else if(Plane.Find(_T("ZY"))>=0||Plane.Find(_T("zy"))>=0||Plane.Find(_T("zY"))>=0||Plane.Find(_T("Zy"))>=0) {MoveDirection1=3;MoveDirection2=2;MoveDirection3=1;NuPlane=nodehead->NuNodesInX;}
	else {MessageBox(NULL,_T(" Plane selection is wrong in 2D output."),_T("Warning"),MB_ICONWARNING | MB_OK);return;}

	if(NoPlane<0||NoPlane>=NuPlane) {MessageBox(NULL,_T(" Plane no. selection is wrong in 2D output."),_T("Warning"),MB_ICONWARNING | MB_OK);return;}

	ofstream output_data;
	output_data.open(FullName,ios::trunc);
	if(nodehead->IsScientific) output_data.setf(ios::scientific);
	else output_data.setf(ios::fixed,ios::floatfield);
	output_data.precision(nodehead->NuDecimal);

	TriDNodeDefine *nodex,*nodey,*nodez;
	nodex=nodehead->FirstNode;
	i=0;
	while(nodex)
	{
		if(i==NoPlane) break;
		nodex=MoveNode(nodex,MoveDirection3);i++;
	}
	nodez=nodex;
	while(nodez)
	{
		nodey=nodez;	
		while(nodey)
		{
			output_data<<nodey->data[NoData]<<" ";
			nodey=MoveNode(nodey,MoveDirection1);
		}
		nodez=MoveNode(nodez,MoveDirection2);
		output_data<<"\n";
	}
	output_data.close();
}

//输出节点数据到文件（二维格式）,格式可被tecplot读入，变量为：头指针，全名（包括路径），平面（如“XY”，“ZY”等），第几平面;
void TriData::outputdata_2d(TriDNodeHeadDefine *nodehead,CString FullName,CString Plane,int NoPlane)
{
		if(!nodehead) return;
		if(!nodehead->FirstNode) return;
		int i,l;
		int NuPlane;
		int MoveDirection1,MoveDirection2,MoveDirection3;
		CString CTemp;
		if(Plane.Find(_T("XY"))>=0||Plane.Find(_T("xy"))>=0||Plane.Find(_T("xY"))>=0||Plane.Find(_T("Xy"))>=0) {MoveDirection1=1;MoveDirection2=2;MoveDirection3=3;NuPlane=nodehead->NuNodesInZ;}
		else if(Plane.Find(_T("YX"))>=0||Plane.Find(_T("yx"))>=0||Plane.Find(_T("yX"))>=0||Plane.Find(_T("Yx"))>=0) {MoveDirection1=2;MoveDirection2=1;MoveDirection3=3;NuPlane=nodehead->NuNodesInZ;}
		else if(Plane.Find(_T("XZ"))>=0||Plane.Find(_T("xz"))>=0||Plane.Find(_T("xZ"))>=0||Plane.Find(_T("Xz"))>=0) {MoveDirection1=1;MoveDirection2=3;MoveDirection3=2;NuPlane=nodehead->NuNodesInY;}
		else if(Plane.Find(_T("ZX"))>=0||Plane.Find(_T("zx"))>=0||Plane.Find(_T("zX"))>=0||Plane.Find(_T("Zx"))>=0) {MoveDirection1=3;MoveDirection2=1;MoveDirection3=2;NuPlane=nodehead->NuNodesInY;}
		else if(Plane.Find(_T("YZ"))>=0||Plane.Find(_T("yz"))>=0||Plane.Find(_T("yZ"))>=0||Plane.Find(_T("Yz"))>=0) {MoveDirection1=2;MoveDirection2=3;MoveDirection3=1;NuPlane=nodehead->NuNodesInX;}
		else if(Plane.Find(_T("ZY"))>=0||Plane.Find(_T("zy"))>=0||Plane.Find(_T("zY"))>=0||Plane.Find(_T("Zy"))>=0) {MoveDirection1=3;MoveDirection2=2;MoveDirection3=1;NuPlane=nodehead->NuNodesInX;}
		else {MessageBox(NULL,_T(" Plane selection is wrong in 2D output."),_T("Warning"),MB_ICONWARNING | MB_OK);return;}

		if(NoPlane<0||NoPlane>=NuPlane) {MessageBox(NULL,_T(" Plane no. selection is wrong in 2D output."),_T("Warning"),MB_ICONWARNING | MB_OK);return;}

		ofstream output_data;
		output_data.open(FullName,ios::trunc);
		if(nodehead->IsScientific) output_data.setf(ios::scientific);
		else output_data.setf(ios::fixed,ios::floatfield);
		output_data.precision(nodehead->NuDecimal);

		output_data<<"TITLE=\""<<nodehead->Title<<"\"\n";	
		CTemp.Empty();
			CTemp.Format(_T("VARIABLES= \"%s\" \"%s\" "),nodehead->TitleXYZ[MoveDirection1-1],nodehead->TitleXYZ[MoveDirection2-1]);
		output_data<<CTemp;
		for(i=0;i<nodehead->NuDataUsed&&i<MaxNuDataInTriDNode;i++)
		output_data<<"\""<<nodehead->VARIABLES[i]<<"\" ";
	if(nodehead->IsOutputInf) output_data<<"\"Status\"\n";
	else output_data<<"\n";

		if(MoveDirection1==1&&MoveDirection2==2) CTemp.Format(_T("ZONE T=\"2D Data in xy plane where NoZ=%d\" I=%d J=%d F=POINT\n"),NoPlane,nodehead->NuNodesInX,nodehead->NuNodesInY);
		else if(MoveDirection1==2&&MoveDirection2==1) CTemp.Format(_T("ZONE T=\"2D Data in yx plane where NoZ=%d\" J=%d I=%d F=POINT\n"),NoPlane,nodehead->NuNodesInY,nodehead->NuNodesInX);
		else if(MoveDirection1==1&&MoveDirection2==3) CTemp.Format(_T("ZONE T=\"2D Data in xz plane where NoY=%d\" I=%d K=%d F=POINT\n"),NoPlane,nodehead->NuNodesInX,nodehead->NuNodesInZ);
		else if(MoveDirection1==3&&MoveDirection2==1) CTemp.Format(_T("ZONE T=\"2D Data in zx plane where NoY=%d\" K=%d I=%d F=POINT\n"),NoPlane,nodehead->NuNodesInZ,nodehead->NuNodesInX);
		else if(MoveDirection1==2&&MoveDirection2==3) CTemp.Format(_T("ZONE T=\"2D Data in yz plane where NoX=%d\" J=%d K=%d F=POINT\n"),NoPlane,nodehead->NuNodesInY,nodehead->NuNodesInZ);
		else if(MoveDirection1==3&&MoveDirection2==2) CTemp.Format(_T("ZONE T=\"2D Data in zy plane where NoX=%d\" K=%d J=%d F=POINT\n"),NoPlane,nodehead->NuNodesInZ,nodehead->NuNodesInY);
		output_data<<CTemp;

		TriDNodeDefine *nodex,*nodey,*nodez;
		nodex=nodehead->FirstNode;
		i=0;
		while(nodex)
		{
			if(i==NoPlane) break;
			nodex=MoveNode(nodex,MoveDirection3);i++;
		}
		nodez=nodex;
		while(nodez)
		{
				nodey=nodez;	
				while(nodey)
				{
					if(MoveDirection1==1&&MoveDirection2==2) output_data<<nodey->x<<" "<<nodey->y<<" ";
					else if(MoveDirection1==2&&MoveDirection2==1) output_data<<nodey->y<<" "<<nodey->x<<" ";
					else if(MoveDirection1==1&&MoveDirection2==3) output_data<<nodey->x<<" "<<nodey->z<<" ";
					else if(MoveDirection1==3&&MoveDirection2==1) output_data<<nodey->z<<" "<<nodey->x<<" ";
					else if(MoveDirection1==2&&MoveDirection2==3) output_data<<nodey->y<<" "<<nodey->z<<" ";
					else if(MoveDirection1==3&&MoveDirection2==2) output_data<<nodey->z<<" "<<nodey->y<<" ";

					for(l=0;l<nodehead->NuDataUsed&&l<MaxNuDataInTriDNode;l++)	
						output_data<<nodey->data[l]<<" ";
					if(nodehead->IsOutputInf) output_data<<nodex->Inf<<"\n";
					else output_data<<"\n";
					nodey=MoveNode(nodey,MoveDirection1);
				}
				nodez=MoveNode(nodez,MoveDirection2);
		}

		output_data<<"END";
		output_data.close();
}

//输出节点数据到文件（一维格式）,格式可被tecplot读入，变量为：头指针，全名（包括路径），xyz方向的序号(从零开始)，其中一项小于0时，该列全输出;
void TriData::outputdata_1d(TriDNodeHeadDefine *nodehead,CString FullName,int NoX,int NoY,int NoZ)
{
		if(!nodehead) return;
		if(!nodehead->FirstNode) return;
		int i,l;
		int Nu1,Nu2;
		int MoveDirection1,MoveDirection2,MoveDirection3;
		CString CTemp;
		
		if(NoX>=nodehead->NuNodesInX||NoY>=nodehead->NuNodesInY||NoZ>=nodehead->NuNodesInZ) 		
		{MessageBox(NULL,_T(" Node no. selection is wrong in 1D output."),_T("Warning"),MB_ICONWARNING | MB_OK);return;}
		if((NoX<0&&NoY<0)||(NoX<0&&NoZ<0)||(NoY<0&&NoZ<0))	
		{MessageBox(NULL,_T(" Node no. selection is wrong in 1D output."),_T("Warning"),MB_ICONWARNING | MB_OK);return;}

		if(NoX<0) { MoveDirection1=1;MoveDirection2=2;MoveDirection3=3;Nu1=NoY;Nu2=NoZ;}
		else if(NoY<0) { MoveDirection1=2;MoveDirection2=1;MoveDirection3=3;Nu1=NoX;Nu2=NoZ;}
		else { MoveDirection1=3;MoveDirection2=1;MoveDirection3=2;Nu1=NoX;Nu2=NoY;}

		ofstream output_data;
		output_data.open(FullName,ios::trunc);
		if(nodehead->IsScientific) output_data.setf(ios::scientific);
		else output_data.setf(ios::fixed,ios::floatfield);
		output_data.precision(nodehead->NuDecimal);

		CTemp.Format(_T("TITLE=\"%s\""),nodehead->Title);
		 output_data<<(LPCTSTR)CTemp<<endl;	
		CTemp.Empty();
			if(NoX<0) CTemp.Format(_T("VARIABLES= \"%s\" "),nodehead->TitleXYZ[0]);
			else if(NoY<0) CTemp.Format(_T("VARIABLES= \"%s\" "),nodehead->TitleXYZ[1]);
			else CTemp.Format(_T("VARIABLES= \"%s\" "),nodehead->TitleXYZ[2]);
		output_data<<(LPCTSTR)CTemp;
		for(i=0;i<nodehead->NuDataUsed&&i<MaxNuDataInTriDNode;i++)
			CTemp.Format(_T("\"%s\" "),nodehead->VARIABLES[i]);
		output_data<<(LPCTSTR)CTemp;
	if(nodehead->IsOutputInf) output_data<<(LPCTSTR)_T("\"Status\"")<<endl;
	else output_data<<endl;

		if(NoX<0) CTemp.Format(_T("ZONE T=\"1D Data in x direction where NoY=%d NoZ=%d\" I=%d F=POINT\n"),NoY,NoZ,nodehead->NuNodesInX);
		else if(NoY<0) CTemp.Format(_T("ZONE T=\"1D Data in y direction where NoX=%d NoZ=%d\" J=%d F=POINT\n"),NoX,NoZ,nodehead->NuNodesInY);
		else  CTemp.Format(_T("ZONE T=\"1D Data in z direction where NoX=%d NoY=%d\" K=%d F=POINT\n"),NoX,NoY,nodehead->NuNodesInZ);
		output_data<<(LPCTSTR)CTemp;

		TriDNodeDefine *node;
		node=nodehead->FirstNode;
		i=0;
		while(node)
		{
			if(i==Nu1) break;
			node=MoveNode(node,MoveDirection2);i++;
		}
		i=0;
		while(node)
		{
			if(i==Nu2) break;
			node=MoveNode(node,MoveDirection3);i++;
		}
		while(node)
		{
			if(MoveDirection1==1) output_data<<node->x<<" ";
			else if(MoveDirection1==2) output_data<<node->y<<" ";
			else output_data<<node->z<<" ";
					for(l=0;l<nodehead->NuDataUsed&&l<MaxNuDataInTriDNode;l++)	
						output_data<<node->data[l]<<(LPCTSTR)_T(" ");
					if(nodehead->IsOutputInf) output_data<<(LPCTSTR)node->Inf<<endl;
					else output_data<<endl;
					node=MoveNode(node,MoveDirection1);
		}
		output_data<<(LPCTSTR)_T("END");
		output_data.close();
}

void TriData::outputdata_Array(CString FullName,double x[],double y[],int N)
{
	ofstream output_data;
	output_data.open(FullName,ios::trunc);
	for(int n=0;n<N;n++)
		output_data<<x[n]<<_T(" ")<<y[n]<<endl;
	output_data.close();
}
void TriData::outputdata_Array(CString FullName,double y[],int N)
{
	ofstream output_data;
	output_data.open(FullName,ios::trunc);
	for(int n=0;n<N;n++)
		output_data<<n<<_T(" ")<<y[n]<<endl;
	output_data.close();
}
//重载函数，输出方向为字母;
void TriData::outputdata_1d(TriDNodeHeadDefine *nodehead,CString FullName,CString DirectionTemp,int NoY,int NoZ)
{
	outputdata_1d(nodehead,FullName,-1,NoY,NoZ);
}
//重载函数，输出方向为字母;
void TriData::outputdata_1d(TriDNodeHeadDefine *nodehead,CString FullName,int NoX,CString DirectionTemp,int NoZ)
{
	outputdata_1d(nodehead,FullName,NoX,-1,NoZ);
}
//重载函数，输出方向为字母;
void TriData::outputdata_1d(TriDNodeHeadDefine *nodehead,CString FullName,int NoX,int NoY,CString DirectionTemp)
{
	outputdata_1d(nodehead,FullName,NoX,NoY,-1);
}
//创建一个数据空间，赋给类中的变量TriData[NoHead]，NoHead为编号，最大为10;
void TriData::CreatData(int NuX, int NuY, int NuZ)
{
	TriDataUsing=CreatNodes(NuX,NuY, NuZ);
}
void TriData::ReadDataFromFile(CString FileFullName)
{
	TriDataUsing=ReadData(FileFullName);
}
void TriData::DeleteData()
{
	DeleteNodes(TriDataUsing);
	TriDataUsing=NULL;
}


//从字符数组linetext分解double数据至数组datatemp，返回识别数据的个数;
int TriData::IdentifyData(char linetext[LineChar_MaxLength],double datatemp[])
{
	char CharData[50];
	CString CData[1000];
	int NuCData=0;
	int i,j;

	for(j=0;j<50;j++) CharData[j]='\0';
	for(i=0,j=0;i<LineChar_MaxLength;i++)
	{
		if((linetext[i]>='0'&&linetext[i]<='9')||linetext[i]=='e'||linetext[i]=='E'||linetext[i]=='+'||linetext[i]=='-'||linetext[i]=='.')
		{CharData[j++]=linetext[i];CharData[j]='\0';}
		else
		{
			if(j>0) CData[NuCData++]=CharData;
			for(j=0;j<50;j++) CharData[j]='\0';
			j=0;
		}
	}
	//CString CTemp;
	if(NuCData>0)
	{
		for(i=0;i<NuCData;i++)
		{
			datatemp[i]=atof(CData[i]);
		}
	}
	return NuCData;
}
//数据文件第一行处理，即提取文件名;
CString TriData::Process1stLine(char linetext[LineChar_MaxLength])
{
	char chartemp[100]={'\0'};
	int i,j;
	int key=0;
	for(i=0,j=0;i<LineChar_MaxLength;i++)
	{
		if(linetext[i]=='"') key++;
		else if(key==1&&linetext[i]!=' ') {chartemp[j++]=linetext[i];chartemp[j]='\0';}
		if(key==2) break;
	}
	return chartemp;
}
//数据文件第二行处理，提取变量名;
int TriData::Process2ndLine(char linetext[LineChar_MaxLength],CString VARIABLESTemp[],CString VARIABLESDataTemp[])
{
	char chartemp[50]={'\0'};
	char chartemp2[50]={'\0'};
	int i,j,jj,k;
	int key=0;
	for(i=0,j=0,jj=0,k=0;i<LineChar_MaxLength;i++)
	{
		if(linetext[i]=='"') key++;
		else if(key>0&&(linetext[i]!=' '||linetext[i]!='\\t'))
		{
			chartemp[j++]=linetext[i];chartemp[j]='\0';
			if((linetext[i]>='0'&&linetext[i]<='9')||linetext[i]=='.'||linetext[i]=='+'||linetext[i]=='-')
			{chartemp2[jj++]=linetext[i];chartemp2[jj]='\0';}
		}
		if(key==2)
		{			
			if(k<MaxNuDataInTriDNode+3) {VARIABLESTemp[k]=chartemp;VARIABLESDataTemp[k++]=chartemp2;}
			key=0; 
			for(j=0;j<50;j++) {chartemp[j]='\0';chartemp2[j]='\0';}
			j=0;jj=0;
		}
	}
	return k;
}
//数据文件第三行处理，提取IJK信息;
bool TriData::Process3rdLine(char linetext[LineChar_MaxLength])
{
	char chartemp[32]={'\0'};
	char chartemp2[500]={'\0'};
	CString CTemp,CTemp2;
	int i,j;
	int IData=0,JData=0,KData=0;
	int key;
	int DimensionIJK=0;

		for(i=0,j=0;i<LineChar_MaxLength;i++)
		{
			if(linetext[i]=='I') {chartemp2[j++]=linetext[i];chartemp2[j]='\0';}
			if(linetext[i]=='J') {chartemp2[j++]=linetext[i];chartemp2[j]='\0';}
			if(linetext[i]=='K') {chartemp2[j++]=linetext[i];chartemp2[j]='\0';}
			if(linetext[i]=='=') {chartemp2[j++]=linetext[i];chartemp2[j]='\0';}
			if(linetext[i]>='0'&&linetext[i]<='9') {chartemp2[j++]=linetext[i];chartemp2[j]='\0';}
		}

	CTemp=chartemp2;
	key=0;
	IsExistI=-1;	IsExistJ=-1;	IsExistK=-1;
	for(i=0;i<499&&i<=CTemp.GetLength();i++)
	{
		if(chartemp2[i]=='I'&&chartemp2[i+1]=='=') {IData=i;IsExistI=DimensionIJK;DimensionIJK++;}
		if(chartemp2[i]=='J'&&chartemp2[i+1]=='=') {JData=i;IsExistJ=DimensionIJK;DimensionIJK++;}
		if(chartemp2[i]=='K'&&chartemp2[i+1]=='=') {KData=i;IsExistK=DimensionIJK;DimensionIJK++;}
	}
	if(DimensionIJK==0) return false;
	if(DimensionIJK>3) return false;
	NuIJK=DimensionIJK;//IJK寸在的个数;

	if(IData>0)
	{
		for(i=0;i<32;i++)
			chartemp[i]='\0';
		for(i=IData+2,j=0,key=0;i<499&&i<=CTemp.GetLength();i++)
		{
			if(chartemp2[i]>='0'&&chartemp2[i]<='9'){chartemp[j++]=chartemp2[i];chartemp[j]='\0';}
			else break;
		}
		CTemp2.Empty();CTemp2=chartemp;IData=_ttoi(CTemp2);
	}
	if(JData>0)
	{
		for(i=0;i<32;i++)
			chartemp[i]='\0';
		for(i=JData+2,j=0,key=0;i<499&&i<=CTemp.GetLength();i++)
		{
			if(chartemp2[i]>='0'&&chartemp2[i]<='9'){chartemp[j++]=chartemp2[i];chartemp[j]='\0';}
			else break;
		}
		CTemp2.Empty();CTemp2=chartemp;JData=_ttoi(CTemp2);
	}
	if(KData>0)
	{
		for(i=0;i<32;i++)
			chartemp[i]='\0';
		for(i=KData+2,j=0,key=0;i<499&&i<=CTemp.GetLength();i++)
		{
			if(chartemp2[i]>='0'&&chartemp2[i]<='9'){chartemp[j++]=chartemp2[i];chartemp[j]='\0';}
			else break;
		}
		CTemp2.Empty();CTemp2=chartemp;KData=_ttoi(CTemp2);
	}

	if(IData<1) IData=1;
	if(JData<1) JData=1;	
	if(KData<1) KData=1;
	TriDataUsingTemp=CreatNodes(IData,JData, KData);
	return true;
}
//读取数据时，根据新读取节点与上一节点判别存储的xyz方向;
int TriData::ProcessDirection(TriDNodeDefine *nodenew,TriDNodeDefine *nodeold)
{
	if(!nodenew||!nodeold) return -1;
	double a,b,c;
	double key=0.00000001;
	a=fabs(nodenew->x-nodeold->x);
	b=fabs(nodenew->y-nodeold->y);
	c=fabs(nodenew->z-nodeold->z);
// 	CString CTEMP;
// 	CTEMP.Format(_T("a=%g b=%g c=%g"),a,b,c);
// 	MessageBox(NULL,CTEMP,_T("Information"),MB_ICONWARNING | MB_OK);

	if(KeyDirection[0]==0)//确定KeyDirection0的移动方向
	{
		if((a>key&&b>key)||(a>key&&c>key)||(c>key&&b>key)) return 0;
		if(a>key) KeyDirection[0]=1;
		else if(b>key) KeyDirection[0]=2;
		else KeyDirection[0]=3;
	}
	if(KeyDirection[1]==0)//确定KeyDirection1与KeyDirection2的移动方向
	{
		if(a>key&&b>key&&c>key) return 0;
		if(a>key&&b>key) 
		{
			if(KeyDirection[0]==1) {KeyDirection[1]=2;KeyDirection[2]=3;}
			else if(KeyDirection[0]==2) {KeyDirection[1]=1;KeyDirection[2]=3;}
			else return 0;
		}
		if(a>key&&c>key) 
		{
			if(KeyDirection[0]==1) {KeyDirection[1]=3;KeyDirection[2]=2;}
			else if(KeyDirection[0]==3) {KeyDirection[1]=1;KeyDirection[2]=2;}
			else return 0;
		}
		if(b>key&&c>key) 
		{
			if(KeyDirection[0]==2) {KeyDirection[1]=3;KeyDirection[2]=1;}
			else if(KeyDirection[0]==3) {KeyDirection[1]=2;KeyDirection[2]=1;}
			else return 0;
		}
	}
	//一个变动;
	if(a>key&&b<key&&c<key)
	{
		if(KeyDirection[0]!=1) return 0;
		else return 1;
	}
	if(a<key&&b>key&&c<key)
	{
		if(KeyDirection[0]!=2) return 0;
		else return 1;
	}
	if(a<key&&b<key&&c>key)
	{
		if(KeyDirection[0]!=3) return 0;
		else return 1;
	}
	//两个变动;
	if(a>key&&b>key&&c<key)
	{
		if(KeyDirection[0]==1&&KeyDirection[1]==2) return 2;
		if(KeyDirection[0]==2&&KeyDirection[1]==1) return 2;
		else return 0;
	}
	if(a>key&&b<key&&c>key)
	{
		if(KeyDirection[0]==1&&KeyDirection[1]==3) return 2;
		if(KeyDirection[0]==3&&KeyDirection[1]==1) return 2;
		else return 0;
	}
	if(a<key&&b>key&&c>key)
	{
		if(KeyDirection[0]==2&&KeyDirection[1]==3) return 2;
		if(KeyDirection[0]==3&&KeyDirection[1]==2) return 2;
		else return 0;
	}
	//三个变动;
	if(a>key&&b>key&&c>key) return 3;
	return 0;	
}
//将node1中data的数据及坐标拷贝给node2;
bool TriData::CopyDataNode1toNode2(TriDNodeDefine *node1,TriDNodeDefine *node2)
{
	if(!node1||!node2) return false;
	node2->x=node1->x;
	node2->y=node1->y;
	node2->z=node1->z;
	for(int i=0;i<MaxNuDataInTriDNode;i++)
		node2->data[i]=node1->data[i];
	return true;
}
//从文件FullName读取数据，该文件格式同tecplot读取格式;
TriDNodeHeadDefine * TriData::ReadData(CString FullName)
{
		ifstream read(FullName);  //打开文件;
		if(!read) {return NULL;}
		int i,j;
		int NuLines=0;
		int NuData;
		char buf[LineChar_MaxLength];
		double datatemp[MaxNuDataInTriDNode+3];
		CString VARIABLESTemp[MaxNuDataInTriDNode+3];
		CString VARIABLESDataTemp[MaxNuDataInTriDNode+3];
		TriDNodeHeadDefine * head=NULL;
		CString Name;
		CString CTemp;
		int NuDataTotal=0;
		int keyDirection;
		TriDNodeDefine *node1,*node2,*node3,*nodenew,*nodeold;
		nodenew=new TriDNodeDefine;
		KeyDirection[0]=0;KeyDirection[1]=0;KeyDirection[2]=0;
		while(read)
		{///
			for(i=0;i<LineChar_MaxLength;i++) buf[i]='\0';
			read.getline(buf,LineChar_MaxLength);//挨行读取文件;
			CTemp.Empty();CTemp=buf;
			if(NuLines==0)
			{
				Name=Process1stLine(buf);
			}
			else if(NuLines==1)
			{
				NuData=Process2ndLine(buf,VARIABLESTemp,VARIABLESDataTemp);
				if(NuData==0||NuData>MaxNuDataInTriDNode+3) {MessageBox(NULL,_T(" Can not read the file: ")+FullName,_T("Warning"),MB_ICONWARNING | MB_OK);break;}
			}
			else if(NuLines==2)
			{
				if(!Process3rdLine(buf)) {MessageBox(NULL,_T(" Can not read the file: ")+FullName,_T("Warning"),MB_ICONWARNING | MB_OK);break;}
				if(!TriDataUsingTemp) {MessageBox(NULL,_T(" Can not read the file: ")+FullName,_T("Warning"),MB_ICONWARNING | MB_OK);break;}
				TriDataUsingTemp->Title=Name;
				TriDataUsingTemp->NuDataUsed=NuData-NuIJK;//包含坐标项的总个数除去坐标IJK的个数;

				for(i=NuIJK,j=0;i<NuData;i++,j++)
				{
					TriDataUsingTemp->VARIABLES[j]=VARIABLESTemp[i];//坐标名称没有复制;
					TriDataUsingTemp->VARIABLESData[j]=atof(VARIABLESDataTemp[i]);
				}
				if(IsExistI>=0) TriDataUsingTemp->TitleXYZ[0]=VARIABLESTemp[IsExistI];
				if(IsExistJ>=0) TriDataUsingTemp->TitleXYZ[1]=VARIABLESTemp[IsExistJ];
				if(IsExistK>=0) TriDataUsingTemp->TitleXYZ[2]=VARIABLESTemp[IsExistK];
				head=TriDataUsingTemp;
				node1=head->FirstNode;
				node2=node1;node3=node1;
			}
			else if(CTemp.Find("END")>=0||CTemp.Find("end")>=0||CTemp.Find("End")>=0) break;
			else 
			{
				NuData=IdentifyData(buf,datatemp);
				if(TriDataUsingTemp->NuDataUsed!=NuData-NuIJK) break;
				NuDataTotal++;
				for(i=NuIJK,j=0;i<NuData;i++,j++)
					nodenew->data[j]=datatemp[i];
				if(IsExistI<0) nodenew->x=0;
				else nodenew->x=datatemp[IsExistI];
				if(IsExistJ<0) nodenew->y=0;
				else nodenew->y=datatemp[IsExistJ];
				if(IsExistK<0) nodenew->z=0;
				else nodenew->z=datatemp[IsExistK];

				if(NuDataTotal==1) CopyDataNode1toNode2(nodenew,node1);
				else
				{
					keyDirection=ProcessDirection(nodenew,nodeold);
					if(keyDirection<=0) {MessageBox(NULL,_T(" Problem appear in reading the file: ")+FullName,_T("Warning"),MB_ICONWARNING | MB_OK);DeleteNodes(head);head=NULL;break;}
					if(keyDirection==1) node1=MoveNode(node1,KeyDirection[0]);
					else if(keyDirection==2) {node2=MoveNode(node2,KeyDirection[1]);node1=node2;}			
					else {node3=MoveNode(node3,KeyDirection[2]);node1=node3;node2=node3;}
				
					if(!CopyDataNode1toNode2(nodenew,node1))
						{MessageBox(NULL,_T(" Problem appear in reading the file: ")+FullName,_T("Warning"),MB_ICONWARNING | MB_OK);DeleteNodes(head);head=NULL;break;}
				}
				nodeold=node1;
			}
			NuLines++;
		}///
		read.close();
		delete nodenew;
		if(head&&NuDataTotal!=head->NuNodesTotal) {MessageBox(NULL,_T(" Failure in reading the data from the file: ")+FullName,_T("Warning"),MB_ICONWARNING | MB_OK);DeleteNodes(head);head=NULL;}
		return head;
}

//对VARIABLES存储的字符进行数值辨认;
void TriData::ATOF_VARIABLES(TriDNodeHeadDefine *nodehead)
{
	for(int i=0;i<nodehead->NuDataUsed&&i<MaxNuDataInTriDNode;i++)
		nodehead->VARIABLESData[i]=atof(nodehead->VARIABLES[i]);	
}

//寻找节点，IJK为节点编号，从零开始;
TriDNodeDefine * TriData::FindNode(TriDNodeHeadDefine *nodehead,int I, int J, int K)
{
	if(!nodehead) 	return NULL;
	if(!nodehead->FirstNode) 	return NULL;
	if(I>=nodehead->NuNodesInX||J>=nodehead->NuNodesInY||K>=nodehead->NuNodesInZ)  	return NULL;
	TriDNodeDefine *node;
	node=nodehead->FirstNode;
	int i;
	for(i=0;i<I&&node;i++)
		node=node->NextX;
	for(i=0;i<J&&node;i++)
		node=node->NextY;
	for(i=0;i<K&&node;i++)
		node=node->NextZ;
	return node;
}
//根据坐标对节点定位，XYZ为三个方向上的坐标，找到该点周围的8个顶点，条件坐标单调递增;
void TriData::LocationNode(TriDNodeHeadDefine *nodehead,double X, double Y, double Z,TriDNodeDefine *VertexNode[8])
{
	int i;
	for(i=0;i<8;i++) VertexNode[i]=NULL;
	if(!nodehead) 	return;
	if(!nodehead->FirstNode) return;
	TriDNodeDefine *NodeTemp;
	NodeTemp=nodehead->FirstNode;
	while(NodeTemp)
	{
		if(X<NodeTemp->x) {VertexNode[0]=NULL;VertexNode[1]=NodeTemp;break;}
		if(NodeTemp->NextX==NULL) {VertexNode[0]=NodeTemp;VertexNode[1]=NULL;break;}
		if(X>=NodeTemp->x&&X<NodeTemp->NextX->x) {VertexNode[0]=NodeTemp;VertexNode[1]=NodeTemp->NextX;break;}
		NodeTemp=NodeTemp->NextX;
	}
	NodeTemp=nodehead->FirstNode;
	while(NodeTemp)
	{
		if(Y<NodeTemp->y) {VertexNode[2]=VertexNode[0];VertexNode[3]=VertexNode[1];VertexNode[0]=NULL;VertexNode[1]=NULL;break;}
		if(NodeTemp->NextY==NULL) {VertexNode[2]=NULL;VertexNode[3]=NULL;break;}
		if(Y>=NodeTemp->y&&Y<NodeTemp->NextY->y)
		{
			if(VertexNode[0]) VertexNode[2]=VertexNode[0]->NextY;
			if(VertexNode[1]) VertexNode[3]=VertexNode[1]->NextY;
			break;
		}
		NodeTemp=NodeTemp->NextY;
		if(VertexNode[0]) VertexNode[0]=VertexNode[0]->NextY;
		if(VertexNode[1]) VertexNode[1]=VertexNode[1]->NextY;
	}
	NodeTemp=nodehead->FirstNode;
	while(NodeTemp)
	{
		if(Z<NodeTemp->z)
		{
			VertexNode[4]=VertexNode[0];
			VertexNode[5]=VertexNode[1];
			VertexNode[6]=VertexNode[2];
			VertexNode[7]=VertexNode[3];
			VertexNode[0]=NULL;
			VertexNode[1]=NULL;
			VertexNode[2]=NULL;
			VertexNode[3]=NULL;
			break;
		}
		if(NodeTemp->NextZ==NULL)
		{
			VertexNode[4]=NULL;
			VertexNode[5]=NULL;
			VertexNode[6]=NULL;
			VertexNode[7]=NULL;
			break;
		}
		if(Z>=NodeTemp->z&&Z<NodeTemp->NextZ->z)
		{
			if(VertexNode[0]) VertexNode[4]=VertexNode[0]->NextZ;
			if(VertexNode[1]) VertexNode[5]=VertexNode[1]->NextZ;
			if(VertexNode[2]) VertexNode[6]=VertexNode[2]->NextZ;
			if(VertexNode[3]) VertexNode[7]=VertexNode[3]->NextZ;
			break;
		}
		NodeTemp=NodeTemp->NextZ;
		if(VertexNode[0]) VertexNode[0]=VertexNode[0]->NextZ;
		if(VertexNode[1]) VertexNode[1]=VertexNode[1]->NextZ;
		if(VertexNode[2]) VertexNode[2]=VertexNode[2]->NextZ;
		if(VertexNode[3]) VertexNode[3]=VertexNode[3]->NextZ;
	}
}

//两个节点之间线性插值入一个节点,变量：第一个节点，第二个节点，插值横坐标，方向（1,2,3代表xyz），data使用数据的个数(若不知，尽量写大，如999);
TriDNodeDefine * TriData::InsertOneNodeBetweenTwoNodes(TriDNodeDefine *VertexNode1,TriDNodeDefine *VertexNode2,double xAxis,int Direction,int NuData)
{
	if(VertexNode1==NULL&&VertexNode2==NULL) return NULL;
	TriDNodeDefine * nodetemp;
	nodetemp=CreatNode();
	if(VertexNode1==NULL)
	{
		nodetemp->x=VertexNode2->x;nodetemp->y=VertexNode2->y;nodetemp->z=VertexNode2->z;
		for(int i=0;i<MaxNuDataInTriDNode&&i<NuData;i++)
			nodetemp->data[i]=VertexNode2->data[i];
	}	
	else if(VertexNode2==NULL)
	{
		nodetemp->x=VertexNode1->x;nodetemp->y=VertexNode1->y;nodetemp->z=VertexNode1->z;
		for(int i=0;i<MaxNuDataInTriDNode&&i<NuData;i++)
			nodetemp->data[i]=VertexNode1->data[i];
	}	
	else
	{
	if(Direction==1)
	{
		nodetemp->x=xAxis;nodetemp->y=VertexNode1->y;nodetemp->z=VertexNode1->z;
		for(int i=0;i<MaxNuDataInTriDNode&&i<NuData;i++)
			nodetemp->data[i]=LineInterpolation(VertexNode1->x,VertexNode2->x, VertexNode1->data[i], VertexNode2->data[i],xAxis);
	}
	else if(Direction==2)
	{
		nodetemp->x=VertexNode1->x;nodetemp->y=xAxis;nodetemp->z=VertexNode1->z;
		for(int i=0;i<MaxNuDataInTriDNode&&i<NuData;i++)
			nodetemp->data[i]=LineInterpolation(VertexNode1->y,VertexNode2->y, VertexNode1->data[i], VertexNode2->data[i],xAxis);
	}
	else if(Direction==3)
	{
		nodetemp->x=VertexNode1->x;nodetemp->y=VertexNode1->y;nodetemp->z=xAxis;
		for(int i=0;i<MaxNuDataInTriDNode&&i<NuData;i++)
			nodetemp->data[i]=LineInterpolation(VertexNode1->z,VertexNode2->z, VertexNode1->data[i], VertexNode2->data[i],xAxis);
	}	
	else return NULL;
	}
	return nodetemp;
}

//根据x,y,z坐标，在三维数据结构内对相邻的8个节点进行线性插值，获取数据，变量：8个节点指针，xyz坐标，插值后的数据储存空间，数据个数;
void TriData::InterpolationFrom8Nodes(TriDNodeDefine *VertexNode[8],double x, double y, double z,double data[],int NuData)
{
	int i;
	TriDNodeDefine *VertexNodeX[4],*VertexNodeY[2],*VertexNodeZ;
	VertexNodeX[0]=InsertOneNodeBetweenTwoNodes(VertexNode[0],VertexNode[1],x,1,NuData);
	VertexNodeX[1]=InsertOneNodeBetweenTwoNodes(VertexNode[2],VertexNode[3],x,1,NuData);
	VertexNodeX[2]=InsertOneNodeBetweenTwoNodes(VertexNode[4],VertexNode[5],x,1,NuData);
	VertexNodeX[3]=InsertOneNodeBetweenTwoNodes(VertexNode[6],VertexNode[7],x,1,NuData);
	VertexNodeY[0]=InsertOneNodeBetweenTwoNodes(VertexNodeX[0],VertexNodeX[1],y,2,NuData);
	VertexNodeY[1]=InsertOneNodeBetweenTwoNodes(VertexNodeX[2],VertexNodeX[3],y,2,NuData);
	VertexNodeZ=InsertOneNodeBetweenTwoNodes(VertexNodeY[0],VertexNodeY[1],z,3,NuData);
	for(i=0;i<MaxNuDataInTriDNode&&i<NuData;i++) data[i]=VertexNodeZ->data[i];
	delete VertexNodeZ;
	for(i=0;i<4;i++) delete VertexNodeX[i];
	for(i=0;i<2;i++) delete VertexNodeY[i];
	//
}

//根据x,y,z坐标，在三维数据空间内寻找相邻节点，并进行线性插值，获取数据，储存在data数组中;
bool TriData::InterpolationFromNodes(TriDNodeHeadDefine *nodehead,double x, double y, double z,double data[])
{
	int i;
	for(i=0;i<nodehead->NuDataUsed&&i<MaxNuDataInTriDNode;i++) data[i]=0;
	if(!nodehead) 	return false;
	if(!nodehead->FirstNode) 	return false;
	TriDNodeDefine *VertexNode[8];
	LocationNode(nodehead,x,y,z,VertexNode);
	InterpolationFrom8Nodes(VertexNode,x, y, z,data,nodehead->NuDataUsed);	
	return true;
}
//从一单调数组中，找到x所在的位置，如介于i与i+1之间，返回i，不合规定返回-2，最左边返回-1,最右边返回NYT-1;
int TriData::LocationInLineArray(double xAxis[],int NYT,double x)
{
	int MaxNYT=300;
	if(NYT>MaxNYT||NYT<=1) return -2;
	int i;
	i=int(NYT*0.5)-1;
	if(i<0) i=0;
	if(xAxis[i]<xAxis[i+1])//递增;
	{
		while(i>=0&&i<NYT-1)
		{
			if(x>=xAxis[i]&&x<xAxis[i+1]) break;
			else if(x<xAxis[i]) i--;
			else if(x>=xAxis[i+1]) i++;
		}
	}
	else//递减;
	{
		while(i>=0&&i<NYT-1)
		{
			if(x<=xAxis[i]&&x>xAxis[i+1]) break;
			else if(x>xAxis[i]) i--;
			else if(x<=xAxis[i+1]) i++;
		}
	}
		return i;
}
//线性插值,样本数组x，样本数组y，样本总数，插值x数组，插值存储的空间y，插值个数;
bool  TriData::LineInterpolation(double xAxis[], double yAxis[], int NYT, double x[],double y[],int Nyt)
{
	int i,j;	
	for(i=0;i<Nyt;i++) y[i]=0;
	if(NYT==0) return false;
	for(j=0;j<Nyt;j++)
	{
		i=LocationInLineArray(xAxis,NYT,x[j]);
		if(i<0) y[j]=yAxis[0];
		else if(i>=NYT-1) y[j]=yAxis[NYT-1];
		else y[j]=(yAxis[i+1]-yAxis[i])*(x[j]-xAxis[i])/(xAxis[i+1]-xAxis[i])+yAxis[i];
	}
	return true;
}

	//线性插值,样本数组x，样本数组y，样本总数，插值x，返回插值数据;
double  TriData::LineInterpolation(double xAxis[], double yAxis[], int NYT, double x)
{
	int i;	
	if(NYT==0) return -999;
		i=LocationInLineArray(xAxis,NYT,x);
		if(i<0) return yAxis[0];
		else if(i>=NYT-1) return yAxis[NYT-1];
		else return (yAxis[i+1]-yAxis[i])*(x-xAxis[i])/(xAxis[i+1]-xAxis[i])+yAxis[i];
}

//线性插值,样本x1,样本y1，样本x2,样本y2，样本总数,插值x数组，插值存储的空间y数组，插值个数;
void  TriData::LineInterpolation(double xAxis1,double xAxis2, double yAxis1, double yAxis2,double x[],double y[],int Nyt)
{
	int j;
	for(j=0;j<Nyt;j++)
	{
		if(fabs(xAxis1-xAxis2)<0.000001) y[j]=(yAxis1+yAxis2)*0.5;
		else y[j]=(yAxis2-yAxis1)*(x[j]-xAxis1)/(xAxis2-xAxis1)+yAxis1;
	}
}

//线性插值,样本x1,样本y1，样本x2,样本y2，插值x，返回插值数据;
double  TriData::LineInterpolation(double xAxis1,double xAxis2, double yAxis1, double yAxis2,double x)
{
		if(fabs(xAxis1-xAxis2)<0.000001) return (yAxis1+yAxis2)*0.5;
		else return (yAxis2-yAxis1)*(x-xAxis1)/(xAxis2-xAxis1)+yAxis1;
}
//样条插值,样本数组x，样本数组y，样本总数，插值x数组，插值存储的空间y，插值个数;
bool  TriData::SplineInterpolation(double xAxis[], double yAxis[], int NYT, double x[],double y[],int Nyt)
{
	int i,j,k;	
	for(i=0;i<Nyt;i++) y[i]=0;
	int MaxNYT=300;
	if(NYT>MaxNYT) return false;
	if(NYT==0) return false; 
	double aaa,sig,p,bbb,ccc;
	double y2[300];
	double u[300];

	if(NYT==1)//如果样本只有一个，所有的值与他相同;

	for (i = 0; i<Nyt; i++)
		y[i]=yAxis[0];
	y2[0] = 0;
	u[0] = 0;
	for (i = 1; i<NYT-1; i++)
	{
		sig = (xAxis[i] - xAxis[i - 1]) / (xAxis[i + 1] - xAxis[i - 1]);
		p = sig * y2[i - 1] + 2.0;
		y2[i] = (sig - 1.0) / p;
		aaa = (yAxis[i + 1] - yAxis[i]) / (xAxis[i + 1] - xAxis[i]);
		bbb = (yAxis[i] - yAxis[i - 1]) / (xAxis[i] - xAxis[i - 1]);
		ccc = xAxis[i + 1] - xAxis[i - 1];
		u[i] = (6.0 * (aaa - bbb) / ccc - sig * u[i - 1]) / p;
	}
	double qn = 0.0;
	double un = 0.0;
	y2[NYT-1] = 0;
	for (k = NYT - 2;k>=1;k--)
		y2[k] = y2[k] * y2[k + 1] + u[k];

	int klo,khi;
	double h,a,b;
//如果插值的x小于被插最小横坐标，取y边值，不允许外插;
//如果插值的x大于被插最大横坐标，取y边值，不允许外插;
	for(j=0;j<Nyt;j++)
	{
// 		if(x[j]<=xAxis[0]) y[j]=yAxis[0];
// 		else if(x[j]>=xAxis[NYT-1]) y[j]=yAxis[NYT-1];
// 		else 
		{
		klo=0;khi= NYT-1;
		while (khi - klo > 1 )
		{
			k = (khi + klo) / 2;
			if (xAxis[k] > x[j]) 	khi = k;
			else klo = k;
		}
		h = xAxis[khi] - xAxis[klo];
		if (fabs(h) <1e-10 ) return false;
		a = (xAxis[khi] - x[j]) / h;
		b = (x[j] - xAxis[klo]) / h;
		aaa = a * yAxis[klo] + b * yAxis[khi];
		bbb = (a*a*a - a) * y2[klo] + (b*b*b - b) * y2[khi];
		y[j]=aaa + bbb * h*h /6.0;
		}
	}
	return true;
}
//样条插值,样本数组x，样本数组y，样本总数，插值x数组，插值存储的空间y，插值个数，是否y值指数变换;
bool  TriData::SplineInterpolation(double xAxis[], double yAxis[], int NYT, double x[],double y[],int Nyt,bool IsExponent)
{
	double MinData=1e-12;
	int i;
	double yAxis2[300];
	bool key;
	if(IsExponent)
	{
		for(i=0;i<NYT;i++)
		{
			if(yAxis[i]<=0) yAxis2[i]=log(MinData);
			else yAxis2[i]=log(yAxis[i]);
		}
		key=SplineInterpolation(xAxis, yAxis2, NYT, x, y, Nyt);
		for(i=0;i<Nyt;i++)
			y[i]=exp(y[i]);
	}
	else key=SplineInterpolation(xAxis, yAxis, NYT, x, y, Nyt);
	return key;
}

//拉格朗日插值,样本数组x，样本数组y，样本总数，插值x数组，插值存储的空间y，插值个数;
bool  TriData::LagrangeInterpolation(double xAxis[], double yAxis[], int NYT, double x[],double y[],int Nyt)
{
	int i,j,k;	
	for(i=0;i<Nyt;i++) y[i]=0;
	int MaxNYT=100;
	if(NYT>MaxNYT) return false;
	if(NYT==0) return false; 
	double data,FS;
	for(i=0;i<Nyt;i++)
	{
		data=0;
		for(j=0;j<NYT;j++)
		{
			FS=yAxis[j];
			for(k=0;k<NYT;k++)
				if(k!=j) FS*=((x[i]-xAxis[k])/(xAxis[j]-xAxis[k]));
			data+=FS;
		}
		y[i]=data;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//以下为迭代计算//
/////////////////////////////
//求B=AX;
TriDNodeHeadDefine * TriData::BCalculation(TriDNodeHeadDefine *headA,CString DirectionA,TriDNodeHeadDefine *headX,CString DirectionX)
{
	if(!headA||!headX) return NULL;
	if(headA->NuDataUsed==0||headA->NuDataUsed>MaxNuDataInTriDNode) return NULL;
	if(headA->NuDataUsed!=headX->NuDataUsed) return false;
	int directionAI,directionAJ,directionB,directionX;
	int NuI;
	if(DirectionA.Find(_T("XY"))>=0||DirectionA.Find(_T("xy"))>=0||DirectionA.Find(_T("Xy"))>=0||DirectionA.Find(_T("xY"))>=0) {directionAI=1;directionAJ=2;NuI=headA->NuNodesInX;}
	else if(DirectionA.Find(_T("YX"))>=0||DirectionA.Find(_T("yx"))>=0||DirectionA.Find(_T("Yx"))>=0||DirectionA.Find(_T("yX"))>=0) {directionAI=2;directionAJ=1;NuI=headA->NuNodesInY;}
	else if(DirectionA.Find(_T("XZ"))>=0||DirectionA.Find(_T("xz"))>=0||DirectionA.Find(_T("Xz"))>=0||DirectionA.Find(_T("xZ"))>=0) {directionAI=1;directionAJ=3;NuI=headA->NuNodesInX;}
	else if(DirectionA.Find(_T("ZX"))>=0||DirectionA.Find(_T("zx"))>=0||DirectionA.Find(_T("Zx"))>=0||DirectionA.Find(_T("zX"))>=0) {directionAI=3;directionAJ=1;NuI=headA->NuNodesInZ;}
	else if(DirectionA.Find(_T("YZ"))>=0||DirectionA.Find(_T("yz"))>=0||DirectionA.Find(_T("Yz"))>=0||DirectionA.Find(_T("yZ"))>=0) {directionAI=2;directionAJ=3;NuI=headA->NuNodesInY;}
	else if(DirectionA.Find(_T("ZY"))>=0||DirectionA.Find(_T("zy"))>=0||DirectionA.Find(_T("Zy"))>=0||DirectionA.Find(_T("zY"))>=0) {directionAI=3;directionAJ=2;NuI=headA->NuNodesInZ;}
	else return NULL;
	if(DirectionX.Find(_T("X"))>=0||DirectionX.Find(_T("x"))>=0) directionX=1;
	else if(DirectionX.Find(_T("Y"))>=0||DirectionX.Find(_T("y"))>=0) directionX=2;
	else if(DirectionX.Find(_T("Z"))>=0||DirectionX.Find(_T("z"))>=0) directionX=3;
	else return NULL;

	TriDNodeHeadDefine *headB;
	headB=CreatNodes(1,headX->NuNodesInY,headX->NuNodesInZ);
	if(!headB) return NULL;
	headB->Title=_T("BTemp.dat");
	headB->TitleXYZ[0]=_T("None");
	headB->TitleXYZ[1]=_T("None");
	headB->TitleXYZ[2]=_T("Height(cm)");
	for(int i=0;i<headA->NuDataUsed&&i<MaxNuDataInTriDNode;i++)
	{
		headB->VARIABLES[i]=headA->VARIABLES[i];
		headB->VARIABLESData[i]=headA->VARIABLESData[i];
	}
	headB->NuDataUsed=headA->NuDataUsed;
	directionB=3;

	TriDNodeDefine *nodeATemp;
	TriDNodeDefine *nodeBTemp;
	int noData;
	nodeATemp=headA->FirstNode;
	nodeBTemp=headB->FirstNode;
	while(nodeATemp&&nodeBTemp)
	{//
		for(noData=0;noData<headA->NuDataUsed&&noData<MaxNuDataInTriDNode;noData++)
		{
			nodeBTemp->data[noData]=ABVectorsProduct(nodeATemp,directionAJ,headX->FirstNode,directionX,noData);
		}
		nodeBTemp->z=nodeATemp->z;
		nodeBTemp=MoveNode(nodeBTemp,directionB);
		nodeATemp=MoveNode(nodeATemp,directionAI);//纵向向下;
	}//
	return headB;
}

/////////////////////////////
//最大似然法
bool TriData::MLEM_Iteration(TriDNodeHeadDefine *headA,CString DirectionA,TriDNodeHeadDefine *headB,CString DirectionB,TriDNodeHeadDefine *headX,CString DirectionX,double  Resid[])
{
	if(!headA||!headB||!headX) return false;
	if(headA->NuDataUsed==0||headA->NuDataUsed>MaxNuDataInTriDNode) return false;
	if(headA->NuDataUsed!=headB->NuDataUsed||headA->NuDataUsed!=headX->NuDataUsed) return false;
	int directionAI=0,directionAJ=0,directionB=0,directionX=0;
	CString CTemp;
	if(DirectionA.Find(_T("XY"))>=0||DirectionA.Find(_T("xy"))>=0||DirectionA.Find(_T("Xy"))>=0||DirectionA.Find(_T("xY"))>=0) {directionAI=1;directionAJ=2;}
	else if(DirectionA.Find(_T("YX"))>=0||DirectionA.Find(_T("yx"))>=0||DirectionA.Find(_T("Yx"))>=0||DirectionA.Find(_T("yX"))>=0) {directionAI=2;directionAJ=1;}
	else if(DirectionA.Find(_T("XZ"))>=0||DirectionA.Find(_T("xz"))>=0||DirectionA.Find(_T("Xz"))>=0||DirectionA.Find(_T("xZ"))>=0) {directionAI=1;directionAJ=3;}
	else if(DirectionA.Find(_T("ZX"))>=0||DirectionA.Find(_T("zx"))>=0||DirectionA.Find(_T("Zx"))>=0||DirectionA.Find(_T("zX"))>=0) {directionAI=3;directionAJ=1;}
	else if(DirectionA.Find(_T("YZ"))>=0||DirectionA.Find(_T("yz"))>=0||DirectionA.Find(_T("Yz"))>=0||DirectionA.Find(_T("yZ"))>=0) {directionAI=2;directionAJ=3;}
	else if(DirectionA.Find(_T("ZY"))>=0||DirectionA.Find(_T("zy"))>=0||DirectionA.Find(_T("Zy"))>=0||DirectionA.Find(_T("zY"))>=0)	{directionAI=3;directionAJ=2;}
	else return false;
	if(DirectionB.Find(_T("X"))>=0||DirectionB.Find(_T("x"))>=0) directionB=1;
	else if(DirectionB.Find(_T("Y"))>=0||DirectionB.Find(_T("y"))>=0) directionB=2;
	else if(DirectionB.Find(_T("Z"))>=0||DirectionB.Find(_T("z"))>=0) directionB=3;
	else return false;
	if(DirectionX.Find(_T("X"))>=0||DirectionX.Find(_T("x"))>=0) directionX=1;
	else if(DirectionX.Find(_T("Y"))>=0||DirectionX.Find(_T("y"))>=0) directionX=2;
	else if(DirectionX.Find(_T("Z"))>=0||DirectionX.Find(_T("z"))>=0) directionX=3;
	else return false;
//CTemp.Format(_T("%d %d"),directionAI,directionAJ);::MessageBox(NULL,CTemp,_T("Test"),MB_ICONWARNING | MB_OK);}

	TriDNodeDefine *nodeATemp,*nodeATemp2,*nodeALine;
	TriDNodeDefine *nodeBTemp;
	TriDNodeDefine *nodeXTemp;
	double Cancha[MaxNuDataInTriDNode],Neiji,Length[MaxNuDataInTriDNode];
	int noData;
	nodeATemp=headA->FirstNode;
	nodeXTemp=headX->FirstNode;
	while(nodeATemp&&nodeXTemp)
	{//
		nodeALine=headA->FirstNode;
		nodeATemp2=nodeATemp;
		nodeBTemp=headB->FirstNode;
		for(noData=0;noData<headA->NuDataUsed&&noData<MaxNuDataInTriDNode;noData++)
		{Cancha[noData]=0;Length[noData]=0;}		
		while(nodeATemp2&&nodeALine&&nodeBTemp)
		{//
			for(noData=0;noData<headA->NuDataUsed&&noData<MaxNuDataInTriDNode;noData++)
			{
				Neiji=ABVectorsProduct(nodeALine,directionAJ,headX->FirstNode,directionX,noData);
				if(Neiji>0.00000001) Cancha[noData]+=nodeATemp2->data[noData]*nodeBTemp->data[noData]/Neiji;
				Length[noData]+=nodeATemp2->data[noData];
			}
			nodeALine=MoveNode(nodeALine,directionAI);//纵向向下;
			nodeATemp2=MoveNode(nodeATemp2,directionAI);//纵向向下;
			nodeBTemp=MoveNode(nodeBTemp,directionB);//纵向向下;
		}//
		for(noData=0;noData<headA->NuDataUsed&&noData<MaxNuDataInTriDNode;noData++)
		{
			if(fabs(Length[noData])>0.00000001) nodeXTemp->data[noData]*=Cancha[noData]/Length[noData];
		}
		nodeXTemp=MoveNode(nodeXTemp,directionX);
		nodeATemp=MoveNode(nodeATemp,directionAJ);
	}//
	//求残差;
	ResidualOfABX(headA,directionAI,directionAJ,headB,directionB,headX,directionX,Resid);
	return true;
}

//ART迭代，注意松弛因子;
bool TriData::ART_Iteration(TriDNodeHeadDefine *headA,CString DirectionA,TriDNodeHeadDefine *headB,CString DirectionB,TriDNodeHeadDefine *headX,CString DirectionX,double  Resid[])
{
	if(!headA||!headB||!headX) return false;
	if(headA->NuDataUsed==0||headA->NuDataUsed>MaxNuDataInTriDNode) return false;
	if(headA->NuDataUsed!=headB->NuDataUsed||headA->NuDataUsed!=headX->NuDataUsed) return false;
	int directionAI,directionAJ,directionB,directionX;
	if(DirectionA.Find(_T("XY"))>=0||DirectionA.Find(_T("xy"))>=0||DirectionA.Find(_T("Xy"))>=0||DirectionA.Find(_T("xY"))>=0) {directionAI=1;directionAJ=2;}
	else if(DirectionA.Find(_T("YX"))>=0||DirectionA.Find(_T("yx"))>=0||DirectionA.Find(_T("Yx"))>=0||DirectionA.Find(_T("yX"))>=0) {directionAI=2;directionAJ=1;}
	else if(DirectionA.Find(_T("XZ"))>=0||DirectionA.Find(_T("xz"))>=0||DirectionA.Find(_T("Xz"))>=0||DirectionA.Find(_T("xZ"))>=0) {directionAI=1;directionAJ=3;}
	else if(DirectionA.Find(_T("ZX"))>=0||DirectionA.Find(_T("zx"))>=0||DirectionA.Find(_T("Zx"))>=0||DirectionA.Find(_T("zX"))>=0) {directionAI=3;directionAJ=1;}
	else if(DirectionA.Find(_T("YZ"))>=0||DirectionA.Find(_T("yz"))>=0||DirectionA.Find(_T("Yz"))>=0||DirectionA.Find(_T("yZ"))>=0) {directionAI=2;directionAJ=3;}
	else if(DirectionA.Find(_T("ZY"))>=0||DirectionA.Find(_T("zy"))>=0||DirectionA.Find(_T("Zy"))>=0||DirectionA.Find(_T("zY"))>=0) {directionAI=3;directionAJ=2;}
	else return false;
	if(DirectionB.Find(_T("X"))>=0||DirectionB.Find(_T("x"))>=0) directionB=1;
	else if(DirectionB.Find(_T("Y"))>=0||DirectionB.Find(_T("y"))>=0) directionB=2;
	else if(DirectionB.Find(_T("Z"))>=0||DirectionB.Find(_T("z"))>=0) directionB=3;
	else return false;
	if(DirectionX.Find(_T("X"))>=0||DirectionX.Find(_T("x"))>=0) directionX=1;
	else if(DirectionX.Find(_T("Y"))>=0||DirectionX.Find(_T("y"))>=0) directionX=2;
	else if(DirectionX.Find(_T("Z"))>=0||DirectionX.Find(_T("z"))>=0) directionX=3;
	else return false;

	TriDNodeDefine *nodeATemp,*nodeATemp2;
	TriDNodeDefine *nodeBTemp;
	TriDNodeDefine *nodeXTemp;
	int noData;
	double ax,cancha,lineMS;
	m_RelaxFactor=0.1;
	for(noData=0;noData<headA->NuDataUsed&&noData<MaxNuDataInTriDNode;noData++)
	{//5
			nodeATemp=headA->FirstNode;
			nodeBTemp=headB->FirstNode;
		while(nodeATemp&&nodeBTemp)
		{//4
			lineMS=VectorsMeanSquare(nodeATemp,directionAJ,noData);
			if(lineMS>1e-15)
			{///
				ax=ABVectorsProduct(nodeATemp,directionAJ,headX->FirstNode,directionX,noData);
				cancha=m_RelaxFactor*(nodeBTemp->data[noData]-ax)/lineMS;
				nodeATemp2=nodeATemp;
				nodeXTemp=headX->FirstNode;
				while(nodeATemp2&&nodeXTemp)
				{//
					nodeXTemp->data[noData]+=cancha*nodeATemp2->data[noData];
					nodeATemp2=MoveNode(nodeATemp2,directionAJ);
					nodeXTemp=MoveNode(nodeXTemp,directionX);
				}//
			}///
			nodeATemp=MoveNode(nodeATemp,directionAI);
			nodeBTemp=MoveNode(nodeBTemp,directionB);
		}//4
		//求残差;
	}//5
	ResidualOfABX(headA,directionAI,directionAJ,headB,directionB,headX,directionX,Resid);
	return true;
}

//向量A与B做内积;
double TriData::ABVectorsProduct(TriDNodeDefine *nodeA,int directionA,TriDNodeDefine *nodeB,int directionB,int NoData)
{
	TriDNodeDefine *nodeATemp=nodeA;
	TriDNodeDefine *nodeBTemp=nodeB;
	double neiji=0;
	if(NoData<0||NoData>=MaxNuDataInTriDNode) return -1e-8;	
	while(nodeATemp&&nodeBTemp)
	{
		neiji+=nodeATemp->data[NoData]*nodeBTemp->data[NoData];
		nodeATemp=MoveNode(nodeATemp,directionA);
		nodeBTemp=MoveNode(nodeBTemp,directionB);
	}
	return neiji;
}

//向量A的平方和平均;
double TriData::VectorsMeanSquare(TriDNodeDefine *nodeA,int directionA,int NoData)
{
	TriDNodeDefine *nodeATemp=nodeA;
	double neiji=0;
	int n=0;
	if(NoData<0||NoData>=MaxNuDataInTriDNode) return -1e-8;		
	while(nodeATemp)
	{
		neiji+=nodeATemp->data[NoData]*nodeATemp->data[NoData];
		nodeATemp=MoveNode(nodeATemp,directionA);
		n++;
	}
	if(n>0) return neiji/n;
	else return -1e-8;	
}

//求方程残差,RMS(B-AX),系数矩阵A的头节点，方程右边B头节点，未知数X头节点，计算残差的第NoData个数据;
bool TriData::ResidualOfABX(TriDNodeHeadDefine *headA,int directionAI,int directionAJ,TriDNodeHeadDefine *headB,int directionB,TriDNodeHeadDefine *headX,int directionX,double Resid[])
{
	if(!headA||!headB||!headX) return false;
	TriDNodeDefine *nodeATemp;
	TriDNodeDefine *nodeBTemp;
	double cancha,neiji;
	int NLine;
	int noData;
	for(noData=0;noData<headA->NuDataUsed&&noData<MaxNuDataInTriDNode;noData++)
	{
		nodeATemp=headA->FirstNode;
		nodeBTemp=headB->FirstNode;
		NLine=0;
		cancha=0;
	while(nodeATemp&&nodeBTemp)
	{
		neiji=ABVectorsProduct(nodeATemp,directionAJ,headX->FirstNode,directionX,noData);
		cancha+=pow(nodeBTemp->data[noData]-neiji,2.0);
		nodeATemp=MoveNode(nodeATemp,directionAI);
		nodeBTemp=MoveNode(nodeBTemp,directionB);
		NLine++;
	}
		if(NLine>0) Resid[noData]=sqrt(cancha/NLine);
		else Resid[noData]=-999999;
	}
	return true;
}

//X赋初值,X=AT*B,系数矩阵A，矩阵坐标平面（如“XY”指X为纵向，Y为横向），结果数据B，方向，未知数数据空间X，方向;
bool TriData::InitializationX(TriDNodeHeadDefine *headA,CString DirectionA,TriDNodeHeadDefine *headB,CString DirectionB,TriDNodeHeadDefine *headX,CString DirectionX)
{
	if(!headA||!headB||!headX) return false;
	if(headA->NuDataUsed==0||headA->NuDataUsed>MaxNuDataInTriDNode) return false;
	if(headA->NuDataUsed!=headB->NuDataUsed||headA->NuDataUsed!=headX->NuDataUsed) return false;
	int directionAI,directionAJ,directionB,directionX;
	if(DirectionA.Find(_T("XY"))>=0||DirectionA.Find(_T("xy"))>=0||DirectionA.Find(_T("Xy"))>=0||DirectionA.Find(_T("xY"))>=0) {directionAI=1;directionAJ=2;}
	else if(DirectionA.Find(_T("YX"))>=0||DirectionA.Find(_T("yx"))>=0||DirectionA.Find(_T("Yx"))>=0||DirectionA.Find(_T("yX"))>=0) {directionAI=2;directionAJ=1;}
	else if(DirectionA.Find(_T("XZ"))>=0||DirectionA.Find(_T("xz"))>=0||DirectionA.Find(_T("Xz"))>=0||DirectionA.Find(_T("xZ"))>=0) {directionAI=1;directionAJ=3;}
	else if(DirectionA.Find(_T("ZX"))>=0||DirectionA.Find(_T("zx"))>=0||DirectionA.Find(_T("Zx"))>=0||DirectionA.Find(_T("zX"))>=0) {directionAI=3;directionAJ=1;}
	else if(DirectionA.Find(_T("YZ"))>=0||DirectionA.Find(_T("yz"))>=0||DirectionA.Find(_T("Yz"))>=0||DirectionA.Find(_T("yZ"))>=0) {directionAI=2;directionAJ=3;}
	else if(DirectionA.Find(_T("ZY"))>=0||DirectionA.Find(_T("zy"))>=0||DirectionA.Find(_T("Zy"))>=0||DirectionA.Find(_T("zY"))>=0) {directionAI=3;directionAJ=2;}
	else return false;
	if(DirectionB.Find(_T("X"))>=0||DirectionB.Find(_T("x"))>=0) directionB=1;
	else if(DirectionB.Find(_T("Y"))>=0||DirectionB.Find(_T("y"))>=0) directionB=2;
	else if(DirectionB.Find(_T("Z"))>=0||DirectionB.Find(_T("z"))>=0) directionB=3;
	else return false;
	if(DirectionX.Find(_T("X"))>=0||DirectionX.Find(_T("x"))>=0) directionX=1;
	else if(DirectionX.Find(_T("Y"))>=0||DirectionX.Find(_T("y"))>=0) directionX=2;
	else if(DirectionX.Find(_T("Z"))>=0||DirectionX.Find(_T("z"))>=0) directionX=3;
	else return false;

	TriDNodeDefine *nodeATemp,*nodeATemp2,*nodeALine;
	TriDNodeDefine *nodeBTemp;
	TriDNodeDefine *nodeXTemp;
	double Length,lineMS;
	int noData;
	for(noData=0;noData<headA->NuDataUsed&&noData<MaxNuDataInTriDNode;noData++)
	{
	nodeATemp=headA->FirstNode;
	nodeXTemp=headX->FirstNode;
	while(nodeATemp)
	{
	nodeBTemp=headB->FirstNode;
	nodeALine=headA->FirstNode;
	nodeATemp2=nodeATemp;
	Length=0;
	nodeXTemp->data[noData]=0;
	while(nodeATemp2)
	{
		lineMS=VectorsMeanSquare(nodeALine,directionAJ,noData);
		nodeXTemp->data[noData]+=pow(nodeATemp2->data[noData],2.0)*nodeBTemp->data[noData]/lineMS;
		Length+=nodeATemp2->data[noData];
		nodeALine=MoveNode(nodeALine,directionAI);//纵向向下;
		nodeATemp2=MoveNode(nodeATemp2,directionAI);//纵向向下;
		nodeBTemp=MoveNode(nodeBTemp,directionB);//纵向向下;
	}
	if(fabs(Length)>0.000001) nodeXTemp->data[noData]/=Length;
	nodeXTemp=MoveNode(nodeXTemp,directionX);
	nodeATemp=MoveNode(nodeATemp,directionAJ);
	}
	}
	return true;
}
//求A空间内的数据除与B空间内数据的比值（A/B），返回比值的数据空间;
TriDNodeHeadDefine *TriData::RatioOfAToB(TriDNodeHeadDefine *headA,TriDNodeHeadDefine *headB)
{
	if(!headA) return NULL;
	if(!headB) return NULL;
	if(headA->NuNodesInX!=headB->NuNodesInX||headA->NuNodesInY!=headB->NuNodesInY||headA->NuNodesInZ!=headB->NuNodesInZ) return NULL;
	if(headA->NuDataUsed!=headB->NuDataUsed) return NULL;

	TriDNodeDefine *nodeATemp1,*nodeATemp2,*nodeATemp3=headA->FirstNode;
	TriDNodeDefine *nodeBTemp1,*nodeBTemp2,*nodeBTemp3=headB->FirstNode;
	TriDNodeHeadDefine *headRatio;
	headRatio=CreatNodes(headA->NuNodesInX,headA->NuNodesInY,headA->NuNodesInZ);
	if(!headRatio) return NULL;
	headRatio->Title=_T("RatioOfAToB.txt");
	for(int i=0;i<headA->NuDataUsed&&i<MaxNuDataInTriDNode;i++)
	{
		headRatio->VARIABLES[i]=headA->VARIABLES[i];
		headRatio->VARIABLESData[i]=headA->VARIABLESData[i];
	}
	headRatio->NuDataUsed=headA->NuDataUsed;
	headRatio->TitleXYZ[0]=headA->TitleXYZ[0];
	headRatio->TitleXYZ[1]=headA->TitleXYZ[1];
	headRatio->TitleXYZ[2]=headA->TitleXYZ[2];
	TriDNodeDefine *nodeRTemp1,*nodeRTemp2,*nodeRTemp3=headRatio->FirstNode;
	int noData;
	while(nodeATemp3&&nodeBTemp3&&nodeRTemp3)
	{
		nodeATemp2=nodeATemp3;
		nodeBTemp2=nodeBTemp3;
		nodeRTemp2=nodeRTemp3;
		while(nodeATemp2&&nodeBTemp2&&nodeRTemp2)
		{
			nodeATemp1=nodeATemp2;
			nodeBTemp1=nodeBTemp2;
			nodeRTemp1=nodeRTemp2;
			while(nodeATemp1&&nodeBTemp1&&nodeRTemp1)
			{
				nodeRTemp1->x=nodeATemp1->x;
				nodeRTemp1->y=nodeATemp1->y;
				nodeRTemp1->z=nodeATemp1->z;
				for(noData=0;noData<headA->NuDataUsed&&noData<MaxNuDataInTriDNode;noData++)
					nodeRTemp1->data[noData]=nodeATemp1->data[noData]/nodeBTemp1->data[noData];
				nodeATemp1=nodeATemp1->NextZ;
				nodeBTemp1=nodeBTemp1->NextZ;
				nodeRTemp1=nodeRTemp1->NextZ;
			}
			nodeATemp2=nodeATemp2->NextY;
			nodeBTemp2=nodeBTemp2->NextY;
			nodeRTemp2=nodeRTemp2->NextY;
		}
		nodeATemp3=nodeATemp3->NextX;
		nodeBTemp3=nodeBTemp3->NextX;
		nodeRTemp3=nodeRTemp3->NextX;
	}
	return headRatio;
}

