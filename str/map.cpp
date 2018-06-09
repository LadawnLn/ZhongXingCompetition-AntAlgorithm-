#include "map.h"
#include <string.h>
#include "set.h"
#include <list>
#include <string>
#include <sstream>
using std::list;
extern int includeNodeNum;
extern int edge_num;

/// 全局必经点集合
list<int> initInfluenceSet;		
/// 全局必经边集合
list<int> initMustEdge;		
/// 全局禁止边集合
list<int> initBanEdge;			
/// 全局限制节点
int limit;                             
/// 用于对节点进行计数,初始化为0
int bitFlagArray_1[1 + MAX_VERTEX_NUM / BITSPERWORD] = { 0 };
/// 用于对要求经过的点进行标记
int bitFlagArray_2[1 + MAX_DEMAND_NUM / BITSPERWORD] = { 0 };
/// 用于存放要求
int demandSet[60];
/// 寻路起始点
int start;
/// 寻路终点
int end;
/// 判断必经边是否含有终点的标志位
int Flag_EndVertex_Must=0;
/// 用于存储图的路径信息
int g_Distance[600][600];			
/// 对图做一定的修改的时候，会用到
int g_Distance_temp[600][600];	
/// including set中的点的集合
UnionFindSet includingSet;			
/// inf指代无穷大的数
const int inf = 100000000;

void set(int i,int a[])
{
	a[i >> SHIFT] |= (1 << (i&MASK));
}
void clr(int i,int a[])
{
	a[i >> SHIFT] &= ~(1 << (i&MASK));
}
int test(int i,int a[])
{
	return a[i >> SHIFT] & (1 << (i&MASK));
}

///		MakeNode函数

///		根据从文件读取的内容构造一个邻接链表的节点（即构造一条边）
///     @param linkid				构造的边的编号
///		@param destinationid		构造的边的指向节点
///		@param cost					构造的边的权重
///     @return							返回构造的边的指针
ArcNode * MakeNode(unsigned int linkid,unsigned int destinationid,unsigned int cost)
{
	ArcNode *pArcNode;
	if(( pArcNode = (ArcNode *) new ArcNode ) == NULL)
		return NULL;
	pArcNode->nextarc = NULL;
	pArcNode->adjvex = destinationid;		//该边指向的顶点
	(pArcNode->arcinfo).arcnum = linkid;	//该边的编号
	(pArcNode->arcinfo).weight = cost;		//该边的权重
	return pArcNode;
}

///初始化图的信息
void InitInfo()
{
	int i = 0;
	G.arcnum = edge_num;
	G.vexnum = 0;
	for (; i < MAX_VERTEX_NUM; i++)
	{
		G.vertices[i].adjlist.head = NULL;
		G.vertices[i].adjlist.tail = NULL;
		G.vertices[i].adjlist.len = 0;
		G.vertices[i].nodeinfo.inDegree = 0;
		G.vertices[i].nodeinfo.outDegree = 0;
		G.vertices[i].nodeinfo.distance = INT_MAX;
		G.vertices[i].nodeinfo.num = i;			//记录自己的节点号信息
	}
}

///从buffer中获取对应的linkid，sourceid，destinationid，cost
void GetValue(int value[],char buffer[])
{
	unsigned int i = 0, j = 0, k = 0;
	for (; j < 4; j++)
	{
		k = i;
		while (!(buffer[i] == ',' || buffer[i] == '|' || buffer[i] == '\n' || buffer[i] == '\0'))
			i++;
		buffer[i++] = '\0';
		value[j] = atoi(buffer + k);
	}
}

///		AddNode函数

///		在节点 i 的邻接链表尾部添加一条边
///     @param i						节点
///		@param pArcNode		指向要添加的边的指针
void AddNode(unsigned int i, ArcNode * pArcNode)
{
	
	if (G.vertices[i].adjlist.len == 0)		//判断这个将要添加的顶点是否是某个顶点的邻接链表的第一个节点
		G.vertices[i].adjlist.head = pArcNode;
	else 
		(G.vertices[i].adjlist.tail)->nextarc = pArcNode;
	G.vertices[i].adjlist.tail = pArcNode;
	G.vertices[i].adjlist.len++;
	//以下两个 if 统计节点个数
	if (!test(i,bitFlagArray_1))
	{
		G.vexnum++;
		set(i,bitFlagArray_1);
	}
	if (!test(pArcNode->adjvex,bitFlagArray_1))
	{
		G.vexnum++;
		set(pArcNode->adjvex,bitFlagArray_1);
	}
	G.vertices[i].nodeinfo.outDegree += 1;  //节点 i 出度+1
	G.vertices[pArcNode->adjvex].nodeinfo.inDegree += 1;  //新加入的节点入度+1
}

///		InitMap函数

///		根据文件初始化图的信息
///     @param topo[]				含有图信息的数组指针
void InitMap(char *topo[])
{
	ArcNode * pArcNode;
	int value[4];		//分别存放linkid,sourceid,destinationid,cost
	InitInfo();
	for (int i = 0; i < edge_num; i++)
	{
		GetValue(value, topo[i]);
		g_Distance[value[1]][value[2]] = value[3];		//矩阵的形式存储图的信息
		g_Distance_temp[value[1]][value[2]] = value[3];
		if ((pArcNode = MakeNode(value[0], value[2], value[3])) == NULL) 
		{
			printf("构造邻接链表节点失败！\n");
			return;
		}
		//将构建的这个弧添加到节点号为sourceid的邻接链表上去
		AddNode(value[1], pArcNode);
	}
	/*将图中不存在的边的权值设为inf */
	for (int i = 0; i < G.vexnum; i++)
	{
		for (int j = 0; j < G.vexnum; j++)
		{
			if (g_Distance[i][j] == 0)
			{
				g_Distance[i][j] = inf;
				g_Distance_temp[i][j] = inf;
			}
		}
	}
	return;
}

///		InitDemandSet函数

///		初始化必经点和吸引力集合
///     @param buf				存储全局必经点的集合
///		@param bufsize			必经点的个数
///		@param StartVertex	寻路的起点
///     @param EndVertex	寻路的终点
///     @return						必经点的个数
int InitDemandSet(std::vector<int> buf,int bufsize,int StartVertex,int EndVertex)
{
	for(int i=0;i<bufsize;i++)
	{
		demandSet[i] = buf[i];
	}
	start = StartVertex;
	end = EndVertex;
	/*初始化including set集合*/
	InitUnionSet(&includingSet, G.vexnum);			
	/*创建including set的集合*/
	for (int i = 0; i < bufsize; i++)
	{
		includingSet.UnionSet(demandSet[i], demandSet[0], &includingSet);        //demandSet[0]作为父节点。
		initInfluenceSet.push_back(demandSet[i]);			//初始化能对蚂蚁产生吸引力的节点集合。
	}
	return bufsize;
}

///		InitMustEdge函数

///		/初始化必经点的集合
///     @param buf				存储全局必经边的集合
///		@param bufsize			必经边的个数
///     @return						必经边的个数
int InitMustEdge(std::vector<int> buf,int bufsize)
{
	for(int i=0;i<bufsize;i++)
	{
		initMustEdge.push_back(buf[i]);
	}
	return initMustEdge.size();
}

///		InitBanEdge函数

///		初始化禁止边的集合、限制节点的最大值
///     @param buf				存储全局禁止边的集合
///		@param bufsize			禁止边的个数
///     @param num				限制节点的最大值
///     @return						禁止边的个数
int InitBanEdge(std::vector<int> buf,int bufsize,int num)
{
	for(int i=0;i<bufsize;i++)
	{
		initBanEdge.push_back(buf[i]);
	}
	limit = num;
	return initBanEdge.size();
}

///		InputDemand函数

///		输入首尾节点、必经点的信息
///     @param demand				指向全局必经点的引用
///		@param VertexStart			指向起点序号的指针
///     @param VertexEnd			指向终点序号的指针
///     @return								必经点的个数
int InputDemand( std::vector<int> & demand,int *VertexStart,int * VertexEnd)
{
	/*输入起点和终点*/
	int i=0;
	std::string line;
	std::string word;
	std::cout<<"输入起点和终点 (空格间断，回车结束)：  \n ";
	getline(std::cin,line);
	std::istringstream record(line);
	std::string tmp;
	while(record >> word)
	{
		tmp = word;
		if(0 == i)
			*VertexStart = atoi(tmp.c_str());
		else if(1 == i)
			*VertexEnd = atoi(tmp.c_str());
		else
		{
			std::cerr<<"输入有误！\n";
			exit(1);
		}
		++i;
	}
	std::cin.clear();

	/*输入必经点*/
	std::string line_1;
	std::string word_1;
	std::cout<<"输入必经点 (空格间断，回车结束)： \n ";
	getline(std::cin,line_1);
	std::istringstream rtc(line_1);
	while(rtc >> word_1)
	{
		demand.push_back((atoi( word_1.c_str() ) ));
	}
	std::cin.clear();

	return demand.size();
}

///		InputMustEdge函数

///		输入必经边的信息
///     @param Mustedge				指向全局必经边的引用
///     @return									必经边的个数
int InputMustEdge(std::vector<int>  & Mustedge)
{
	std::string buf;
	std::string vertex;
	int first=0;
	int v[2];
	int tmp;
	std::cout<<"输入必经边（例如：输入2 4 11 12，表示2和4为一条必经边，11和12为另一条必经边）：\n ";
	getline(std::cin,buf);
	std::istringstream rtc(buf); //文件流绑定
	while(rtc >> vertex)		   //输入流可以读取流信息，以空格为间隔
	{
		 tmp = atoi(vertex.c_str());
		 v[first] = tmp;
		 if(++first >1)
		 {
			 if(v[0] == end || v[1] == end)
				Flag_EndVertex_Must = 1;
			 int linkID = GetLinkID(v[0],v[1]);
	//		 std::cout<<"linkId:"<<linkID<<std::endl;
			 Mustedge.push_back(linkID);
			 v[0] = v[1] = 0;
			 first = 0;
		 }
	}
	std::cin.clear();
	return Mustedge.size();
}

///		InputBanEdge函数

///		输入禁止边、限制点的信息
///     @param ban_file				指向全局禁止边的引用
///     @param num						指向限制节点最大值的指针
///     @return								禁止边的个数
int InputBanEdge(std::vector<int> & ban_file,int *num)
 {
	 /*输入禁止边*/
	 std::string buf;
	 std::string vertex;
	 int first=0;
	 int v[2];
	 int tmp;
	 std::cout<<"输入禁止边（例如：输入2 4 11 12，表示2和4为一条禁止边，11和12为另一条禁止边）：\n ";
	 getline(std::cin,buf);
	 std::istringstream rtc(buf); //文件流绑定
	 while(rtc >> vertex)		//输入流可以读取流信息，以空格为间隔
	 {
		 tmp = atoi(vertex.c_str());
		 v[first] = tmp;
		 if(++first >1)
		 {
			 int linkID = GetLinkID(v[0],v[1]);
			 ban_file.push_back(linkID);
			 v[0] = v[1] = 0;
			 first = 0;
		 }
	 }
	 std::cin.clear();

	 /*输入限制节点个数*/
	 std::string numstring;
	 std::string temp;
	 std::cout<<"输入限制节点个数的最大值： \n ";
	 getline(std::cin,numstring);
	 std::istringstream record(numstring);
	 record >> temp;
	 *num = atoi(temp.c_str());
	 std::cin.clear();
	 
	 return ban_file.size();
 }

///		DestroyMap函数

///		释放存储图的堆内存
///     @param pgraph					指向存储图的结构体指针
void DestroyMap(Graph *pgraph)
{
	int i = 0;
	pArcNode parc = NULL;
	pArcNode preParc = NULL;
	for (; i < pgraph->vexnum; i++)
	{
		parc = pgraph->vertices[i].adjlist.head;
		while (parc != NULL)
		{
			preParc = parc;
			parc = parc->nextarc;
			delete preParc;
		}
	}
}

///		GetLinkID函数

///		取得任意两相连节点的linkID值
///     @param v1						节点1
///     @param v2						节点2
///     @return								两节点相连边的LinkID值，即边的编号
int GetLinkID(int v1,int v2)
{
	int i=0;
	int LinkValue = -1;
	auto head = G.vertices[v1].adjlist.head;
	while(head != NULL)
	{
		if(head->adjvex == v2)
		{
			LinkValue = head->arcinfo.arcnum;
			//	std::cout<<"link:"<<LinkValue<<std::endl;
			break;
		}
		head = head->nextarc;
	}
	return LinkValue;
}

///		FloydWarshall函数

///		弗洛伊德算法，寻找给定的图中多源点之间最短路径
///     @param mindist	t[][600]						mindist[][600] 表示的是 i 到 j 的最短路径
///     @param pre[][600]							    pre[][600] 记录 i 到 j 的最短路径的父节点
void FloydWarshall(int mindist[][600], int pre[][600])
{

	int vnum = G.vexnum;
	/*去掉start和end节点相关的边信息（设置为Inf）*/
	for (int i = 0; i <vnum ; i++) 
	{
		g_Distance_temp[start][i] = inf;
		g_Distance_temp[end][i] = inf;
		g_Distance_temp[i][start] = inf;
		g_Distance_temp[i][end] = inf;
	}
	int i, j, k;
	/*二重循环初始化*/
	for (i = 0; i < vnum; i++)
	{
		for (j = 0; j < vnum; j++)
		{
			mindist[i][j] = g_Distance_temp[i][j];
			pre[i][j] = (i == j) ? -1 : i;
		}
	}
	/*三重循环权值修正*/
	for (k = 0; k < vnum; k++)
	{
		for (i = 0; i < vnum; i++)
		{
			for (j = 0; j < vnum; j++)
			{
				if (mindist[i][k] + mindist[k][j] < mindist[i][j])
					mindist[i][j] = mindist[i][k] + mindist[k][j];
					pre[i][j] = pre[k][j];
			}
		}
	}
	/*将include set里面的点到自己的最短距离设置为0,这是为了计算吸引力的时候方便*/
	for (i = 0; i < includeNodeNum; i++) 
	{
		mindist[demandSet[i]][demandSet[i]] = 0;
	}
}

///		Dijkstra函数

///		迪杰斯特拉算法，寻找满足所有要求后的节点到终点的最短路径
///     @param n						图的节点数
///     @param v						计算从点v开始的单源最短路径
///     @param dist					返回的最短路径
///     @param prev				返回的路径信息
///     @param c						图的信息
void Dijkstra(int n, int v, int *dist, int *prev, int c[][600])
{
	bool s[600];    // 判断是否已存入该点到S集合中
	for (int i = 0; i < n; ++i)
	{
		dist[i] = c[v][i];
		s[i] = 0;     // 初始都未用过该点
		if (dist[i] == INT_MAX)
			prev[i] = 0;
		else
			prev[i] = v;
	}
	dist[v] = 0;
	s[v] = 1;
	for (int i = 1; i < n; ++i)
	{
		int tmp = INT_MAX;
		int u = v;
		/* 找出当前未使用的点j的dist[j]最小值*/
		for (int j = 0; j < n; ++j)
		{
			if ((!s[j]) && dist[j]<tmp)
			{
				u = j;              // u保存当前邻接点中距离最小的点的号码
				tmp = dist[j];
			}
		}
		s[u] = 1;    // 表示u点已存入S集合中
		/* 更新dist*/
		for (int j = 0; j < n; ++j)
		{
			if ((!s[j]) && c[u][j]<INT_MAX)
			{
				int newdist = dist[u] + c[u][j];
				if (newdist < dist[j])
				{
					dist[j] = newdist;
					prev[j] = u;  //u是前向驱动
				}
			}
		}
	}
}

