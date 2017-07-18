
#pragma once

#define MaxNuDataInTriDNode 20//节点存储数据的个数;
#define LineChar_MaxLength 3000//字符辨认时的最大个数;
#define PI 3.14159265358979323846

class CartesianCoordinate
{
public:
	double x;
	double y;
	double z;
};

class TriDNodeDefine
{
public:
	int NoX;
	int NoY;
	int NoZ;
	double x;
	double y;
	double z;
	double data[MaxNuDataInTriDNode];
	double s;
	double xx;
	double yy;
	double zz;
	double SumS;
	CString Inf;
	TriDNodeDefine *NextX;
	TriDNodeDefine *LastX;
	TriDNodeDefine *NextY;
	TriDNodeDefine *LastY;
	TriDNodeDefine *NextZ;
	TriDNodeDefine *LastZ;
	TriDNodeDefine *Link;
	int NuLink;
};

class TriDNodeHeadDefine
{
protected:

public:
	int DimensionOfNodes;//维数，一维值为1，2维为2，3维为3;
	int NuNodesInX;//X方向节点个数;
	int NuNodesInY;//Y方向节点个数;
	int NuNodesInZ;//Z方向节点个数;
	int NuNodesTotal;//节点总数;
	CString Title;//数据空间的名称;
	CString Inf;//该数据空间的注释;
	CString VARIABLES[MaxNuDataInTriDNode];//double数组内各个物理量的名称;
	CString TitleXYZ[3];
	double VARIABLESData[MaxNuDataInTriDNode];//每个节点的数据说明;
	double MaxData[MaxNuDataInTriDNode];
	double MinData[MaxNuDataInTriDNode];
	TriDNodeDefine *FirstNode;
	TriDNodeDefine *EndNode;
	TriDNodeDefine *UsingNode;
	TriDNodeDefine *UsedNode;
	int NuDataUsed;//data数组内数组空间的使用个数，应小于MaxNuDataInTriDNode;
	int NuNodesUsed;//使用节点的总数;
	int NuDecimal;//小数点位数;
	bool IsScientific;//是否科学计数法;
	bool IsOutputInf;//是否科学计数法;
};

class TriData  
{
public:
	int KeyDirection[3];
	int NuIJK;//读取data文件时,IJK存在的个数，都存在为3;
	int IsExistI;//读取data文件时，是否存在I=,不存在为-1，存在为IJK排列次序编号，零开始;
	int IsExistJ;//读取data文件时，是否存在I=,不存在为-1，存在为IJK排列次序编号，零开始;
	int IsExistK;//读取data文件时，是否存在I=,不存在为-1，存在为IJK排列次序编号，零开始;
	TriDNodeHeadDefine *TriDataUsing;
	TriDNodeHeadDefine *TriDataUsingTemp;
	double m_RelaxFactor;
	//三维数据空间使用初始化;
	void TriDataInitialization(void);
	//创建节点,x,y,z三个方向上的数量，初始数据都是零;
	TriDNodeHeadDefine * CreatNodes(int NuX, int NuY, int NuZ);
	//创建头节点;
	TriDNodeHeadDefine * CreatNodeHead();
	//复制head节点;
	void CopyHead(TriDNodeHeadDefine *nodehead1,TriDNodeHeadDefine *nodehead2);
	//将head1中的信息复制给head2，但是节点数不变;
	void CopyHeadInf(TriDNodeHeadDefine *nodehead1,TriDNodeHeadDefine *nodehead2);
	//生成并初始化节点,节点号、坐标置零，链接指针置NULL;
	TriDNodeDefine * CreatNode();
	//创建节点无头结点,x,y,z三个方向上的数量，初始数据都是零;
	TriDNodeDefine * CreatNodesWithoutHead(int NuX, int NuY, int NuZ);
	//移动节点，移动一步,当前节点，移动方向，返回移动后的节点指针，方向1、2、3分别为x，y，z方向，负号表示负方向;
	TriDNodeDefine * MoveNode(TriDNodeDefine *node,int direction);
	//两个节点之间的链接，node1指向node2,direction=1 x方向，2 y方向，3 z方向;
	void ConnectTwoNodes(TriDNodeDefine *node1,TriDNodeDefine *node2,int direction);
	//两行节点之间的链接，node1所在的行指向node2所在的行,direction=1 x方向，2 y方向，3 z方向;
	void ConnectTwoLineNodes(TriDNodeDefine *node1,TriDNodeDefine *node2,int directionMove,int directionLink);
	//两面节点之间的链接，node1所在面指向node2所在面,direction=1 x方向，2 y方向，3 z方向;
	void ConnectTwoFaceNodes(TriDNodeDefine *node1,TriDNodeDefine *node2,int directionLink);
	//针对头指针删除节点;
	void DeleteNodes(TriDNodeHeadDefine *nodehead);
	//将节点的数据全部置零;
	void SetNodesData(TriDNodeHeadDefine *nodehead,double data);
	//输出节点数据到文件（三维格式，即包括IJK，即使为1）,格式可被tecplot读入，变量为：头指针，全名（包括路径），数据存储的优先方向（如“XYZ”，“ZYX”等）;
	void outputdata_3d(TriDNodeHeadDefine *nodehead,CString FullName,CString Direction);
	void outputdata_3dJZB(TriDNodeHeadDefine *nodehead,CString FullName);//极坐标情况下，计算卡尔迪坐标输出(不能读入);
	//输出节点数据到文件（二维格式）,格式可被tecplot读入，变量为：头指针，全名（包括路径），平面（如“XY”，“ZY”等），第几平面;
	void outputdata_2d(TriDNodeHeadDefine *nodehead,CString FullName,CString Plane,int NoPlane);
	//输出节点数据到文件（一维格式）,格式可被tecplot读入，变量为：头指针，全名（包括路径），xyz方向的序号(从零开始)，其中一项小于0时，该列全输出;
	void outputdata_1d(TriDNodeHeadDefine *nodehead,CString FullName,int NoX,int NoY,int NoZ);
	//将二维数据以平面的方式输出到文件（二维格式）,格式可被tecplot读入，变量为：头指针，全名（包括路径），平面（如“XY”，“ZY”等），第几平面,第几个数据;
	void outputdata_2d(TriDNodeHeadDefine *nodehead,CString FullName,CString Plane,int NoPlane,int NoData);
	//以下三项为一维输出，沿输出方向输入字符，如“full”;
	void outputdata_1d(TriDNodeHeadDefine *nodehead,CString FullName,CString DirectionTemp,int NoY,int NoZ);
	void outputdata_1d(TriDNodeHeadDefine *nodehead,CString FullName,int NoX,CString DirectionTemp,int NoZ);
	void outputdata_1d(TriDNodeHeadDefine *nodehead,CString FullName,int NoX,int NoY,CString DirectionTemp);
	void outputdata_Array(CString FullName,double x[],double y[],int N);
	void outputdata_Array(CString FullName,double y[],int N);

	void CreatData(int NuX, int NuY, int NuZ);
	void DeleteData();
	void ReadDataFromFile(CString FileFullName);
	//从文件FullName读取数据，该文件格式同tecplot读取格式;
	TriDNodeHeadDefine * ReadData(CString FullName);
	//从字符数组linetext分解double数据至数组datatemp，返回识别数据的个数;
	//对VARIABLES存储的字符进行数值辨认;
	void ATOF_VARIABLES(TriDNodeHeadDefine *nodehead);
	int IdentifyData(char linetext[LineChar_MaxLength],double datatemp[]);
	//数据文件第一行处理，即提取文件名;
	CString Process1stLine(char linetext[LineChar_MaxLength]);
	//数据文件第二行处理，提取变量名;
	int Process2ndLine(char linetext[LineChar_MaxLength],CString VARIABLESTemp[],CString VARIABLESDataTemp[]);
	//数据文件第三行处理，提取IJK信息;
	bool Process3rdLine(char linetext[LineChar_MaxLength]);
	//读取数据时，根据新读取节点与上一节点判别存储的xyz方向;
	int ProcessDirection(TriDNodeDefine *nodenew,TriDNodeDefine *nodeold);
	//将node1中data的数据及坐标拷贝给node2;
	bool CopyDataNode1toNode2(TriDNodeDefine *node1,TriDNodeDefine *node2);
	//寻找节点，IJK为节点编号，从零开始;
	TriDNodeDefine * FindNode(TriDNodeHeadDefine *nodehead,int I, int J, int K);
	//根据坐标对节点定位，XYZ为三个方向上的坐标，找到该点周围的8个顶点，条件坐标单调递增;
	void LocationNode(TriDNodeHeadDefine *nodehead,double X, double Y, double Z,TriDNodeDefine *node[8]);
	//两个节点之间线性插值入一个节点,变量：第一个节点，第二个节点，插值横坐标，方向（1,2,3代表xyz），data使用数据的个数(若不知，尽量写大，如999);
	TriDNodeDefine * InsertOneNodeBetweenTwoNodes(TriDNodeDefine *VertexNode1,TriDNodeDefine *VertexNode2,double xAxis,int Direction,int NuData);
	//根据x,y,z坐标，在三维数据结构内对相邻的8个节点进行线性插值，获取数据，变量：8个节点指针，xyz坐标，插值后的数据储存空间，数据个数;
	void InterpolationFrom8Nodes(TriDNodeDefine *VertexNode[8],double x, double y, double z,double data[],int NuData);
	//根据x,y,z坐标，在三维数据空间内寻找相邻节点，并进行线性插值，获取数据，储存在data数组中;
	bool InterpolationFromNodes(TriDNodeHeadDefine *nodehead,double x, double y, double z,double data[]);
	//从一单调数组中，找到x所在的位置，如介于i与i+1之间，返回i，不合规定返回-2，最左边返回-1,最右边返回NYT-1;
	int LocationInLineArray(double xAxis[],int NYT,double x);
	//线性插值,样本数组x，样本数组y，样本总数，插值x数组，插值存储的空间y，插值个数;
	double LineInterpolation(double xAxis[], double yAxis[], int NYT, double x);
	//线性插值,样本数组x，样本数组y，样本总数，插值x，返回插值数据;
	bool LineInterpolation(double xAxis[], double yAxis[], int NYT, double x[],double y[],int Nyt);
	//线性插值,样本x1,样本y1，样本x2,样本y2，样本总数,插值x数组，插值存储的空间y数组，插值个数;
	void LineInterpolation(double xAxis1,double xAxis2, double yAxis1, double yAxis2,double x[],double y[],int Nyt);
	//线性插值,样本x1,样本y1，样本x2,样本y2，插值x，返回插值数据;
	double LineInterpolation(double xAxis1,double xAxis2, double yAxis1, double yAxis2,double x);
	//样条插值,样本数组x，样本数组y，样本总数，插值x数组，插值存储的空间y，插值个数;
	bool SplineInterpolation(double xAxis[], double yAxis[], int NYT, double x[],double y[],int Nyt);
	//样条插值,样本数组x，样本数组y，样本总数，插值x数组，插值存储的空间y，插值个数，是否y值指数变换;
	bool  SplineInterpolation(double xAxis[], double yAxis[], int NYT, double x[],double y[],int Nyt,bool IsExponent);
	//拉格朗日插值,样本数组x，样本数组y，样本总数，插值x数组，插值存储的空间y，插值个数;
	bool LagrangeInterpolation(double xAxis[], double yAxis[], int NYT, double x[],double y[],int Nyt);
	
	//求B=AX;
	TriDNodeHeadDefine *BCalculation(TriDNodeHeadDefine *headA,CString DirectionA,TriDNodeHeadDefine *headX,CString DirectionX);
	//求A向量与B向量的内积;
	double ABVectorsProduct(TriDNodeDefine *nodeA,int directionA,TriDNodeDefine *nodeB,int directionB,int NoData);
	//向量A的平方和平均;
	double VectorsMeanSquare(TriDNodeDefine *nodeA,int directionA,int NoData);
	//求方程残差,RMS(B-AX),系数矩阵A的头节点，方程右边B头节点，未知数X头节点，计算残差的第NoData个数据;
	bool ResidualOfABX(TriDNodeHeadDefine *headA,int directionAI,int directionAJ,TriDNodeHeadDefine *headB,int directionB,TriDNodeHeadDefine *headX,int directionX,double Resid[]);
	//迭代计算,Sirt,系数矩阵A，矩阵坐标平面（如“XY”指X为纵向，Y为横向），结果数据B，方向，未知数数据空间X，方向,存储残差的数组;
	bool MLEM_Iteration(TriDNodeHeadDefine *headA,CString DirectionA,TriDNodeHeadDefine *headB,CString DirectionB,TriDNodeHeadDefine *headX,CString DirectionX,double Resid[]);
	//迭代计算,ART,系数矩阵A，矩阵坐标平面（如“XY”指X为纵向，Y为横向），结果数据B，方向，未知数数据空间X，方向,存储残差的数组;
	bool ART_Iteration(TriDNodeHeadDefine *headA,CString DirectionA,TriDNodeHeadDefine *headB,CString DirectionB,TriDNodeHeadDefine *headX,CString DirectionX,double Resid[]);
	//X赋初值,X=AT*B,系数矩阵A，矩阵坐标平面（如“XY”指X为纵向，Y为横向），结果数据B，方向，未知数数据空间X，方向;
	bool InitializationX(TriDNodeHeadDefine *headA,CString DirectionA,TriDNodeHeadDefine *headB,CString DirectionB,TriDNodeHeadDefine *headX,CString DirectionX);
	//求A空间内的数据除与B空间内数据的比值（A/B），返回比值的数据空间;
	TriDNodeHeadDefine *RatioOfAToB(TriDNodeHeadDefine *headA,TriDNodeHeadDefine *headB);

};


