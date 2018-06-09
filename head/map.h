#ifndef _MAP_H_
#define _MAP_H_
#include "set.h"
#include <vector>
#include <iostream>
#define  BITSPERWORD 32
#define  SHIFT 5
#define  MASK 0x1F
/// 最大节点数
#define MAX_VERTEX_NUM 600  
/// Demands集合最大点数
#define  MAX_DEMAND_NUM 50 


///		边的信息域

///		定义了边的权重和编号信息
typedef struct
{
	unsigned char weight;	///< 边的权重
	unsigned int arcnum;	///< 边的编号
}ArcInfoType;

///		边的定义

///		定义了边的顶点位置、指向下一条边的指针以及该边的信息域
typedef struct ArcNode
{
	int adjvex;							///< 该弧指向的顶点的位置				
	struct ArcNode *nextarc;		///< 下一条弧的指针	
	ArcInfoType arcinfo;			///< 该弧的相关信息	
}*pArcNode,ArcNode;

///		每个节点的邻接链表的定义

///		定义了这个链的首尾指针、该链的长度
typedef struct
{
	ArcNode * head;///< 该链的头指针
	ArcNode * tail;	///< 该链的尾指针
	int len;				///< 该链的长度
}LinkList;

///		节点的信息域

///		定义了节点的入度出度、头结点的距离、序号
typedef struct
{
	int distance;						///< 与源节点的距离					
	unsigned char inDegree;		///< 节点的入度	
	unsigned char outDegree;	///< 节点的出度
	unsigned int num;				///< 节点的序号
}NodeInfoType;

///		图的节点的定义

///		定义了图节点的相关信息域、邻接链表
typedef struct VNode
{
	
	NodeInfoType nodeinfo;	///< 节点相关信息域
	LinkList adjlist;			///< 节点的邻接链表		
}VNode,AdjList[MAX_VERTEX_NUM],*pVNode;

///		图的定义

///		定义了图的顶点数目、边数目、存储的邻接链表数组
typedef struct
{
	int vexnum;	///< 图的顶点数目
	int arcnum;	///< 图的边的数目
	AdjList vertices;///< 存储图的边链表
}Graph;

///		MakeNode函数

///		根据从文件读取的内容构造一个邻接链表的节点（即构造一条边）
///     @param linkid				构造的边的编号
///		@param destinationid		构造的边的指向节点
///		@param cost					构造的边的权重
///     @return							返回构造的边的指针
ArcNode * MakeNode(unsigned int linkid, unsigned int destinationid, unsigned int cost);

///		AddNode函数

///		在节点 i 的邻接链表尾部添加一条边
///     @param i						节点
///		@param pArcNode		指向要添加的边的指针
void AddNode(unsigned int i, ArcNode * pArcNode);

///		InitMap函数

///		根据文件初始化图的信息
///     @param topo[]				含有图信息的数组指针
void InitMap(char *topo[]);

///		InitDemandSet函数

///		初始化必经点和吸引力集合
///     @param buf				存储全局必经点的集合
///		@param bufsize			必经点的个数
///		@param StartVertex	寻路的起点
///     @param EndVertex	寻路的终点
///     @return						必经点的个数
int InitDemandSet(std::vector<int> buf,int bufsize,int StartVertex,int EndVertex);

///		InitMustEdge函数

///		/初始化必经点的集合
///     @param buf				存储全局必经边的集合
///		@param bufsize			必经边的个数
///     @return						必经边的个数
int InitMustEdge(std::vector<int> buf,int bufsize);

///		InitBanEdge函数

///		初始化禁止边的集合、限制节点的最大值
///     @param buf				存储全局禁止边的集合
///		@param bufsize			禁止边的个数
///     @param num				限制节点的最大值
///     @return						禁止边的个数
int InitBanEdge(std::vector<int> buf,int bufsize,int num);

///		InputDemand函数

///		输入首尾节点、必经点的信息
///     @param demand				指向全局必经点的引用
///		@param VertexStart			指向起点序号的指针
///     @param VertexEnd			指向终点序号的指针
///     @return								必经点的个数
int InputDemand( std::vector<int> & demand,int *VertexStart,int * VertexEnd);

///		InputMustEdge函数

///		输入必经边的信息
///     @param Mustedge				指向全局必经边的引用
///     @return									必经边的个数
int InputMustEdge(std::vector<int>  & Mustedge);

///		InputBanEdge函数

///		输入禁止边、限制点的信息
///     @param ban_file				指向全局禁止边的引用
///     @param num						指向限制节点最大值的指针
///     @return								禁止边的个数
int InputBanEdge(std::vector<int> & ban_file,int *num);

///		GetLinkID函数

///		取得任意两相连节点的linkID值
///     @param v1						节点1
///     @param v2						节点2
///     @return								两节点相连边的LinkID值，即边的编号
int GetLinkID(int v1,int v2);

///		DestroyMap函数

///		释放存储图的堆内存
///     @param pgraph					指向存储图的结构体指针
void DestroyMap(Graph *pgraph);

///		FloydWarshall函数

///		弗洛伊德算法，寻找给定的图中多源点之间最短路径
///     @param mindist	t[][600]						mindist[][600] 表示的是 i 到 j 的最短路径
///     @param pre[][600]							pre[][600] 记录 i 到 j 的最短路径的父节点
void FloydWarshall(int mindist[][600], int pre[][600]);

///		Dijkstra函数

///		迪杰斯特拉算法，寻找满足所有要求后的节点到终点的最短路径
///     @param n						图的节点数
///     @param v						计算从点v开始的单源最短路径
///     @param dist					返回的最短路径
///     @param prev				返回的路径信息
///     @param c						图的信息
void Dijkstra(int n, int v, int *dist, int *prev, int c[][600]);

//topo图
extern Graph G;
//表示蚂蚁的起始出发点
extern int start;
//表示蚂蚁要到的终点
extern int end;
//表示蚂蚁权重的二维数组
extern int g_Distance[600][600];
//对图做一定的修改的时候，会用到
extern int g_Distance_temp[600][600];
//表示蚂蚁的必经点
extern int demandSet[60];
//表示蚂蚁的必经点集合
extern UnionFindSet includingSet;

#endif // !_MAP_H_