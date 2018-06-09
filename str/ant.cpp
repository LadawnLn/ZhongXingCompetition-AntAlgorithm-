#include <cmath>
#include <iostream>
#include "ant.h"
#include "map.h"
#include "set.h"
#include "time.h"
#include "stack"
using std::stack;

extern int minDistance[600][600];
extern list<int> initInfluenceSet;
extern list<int> initMustEdge;
extern list<int> initBanEdge;
extern std::vector<int> mustedge_file;	
extern std::vector<int> ban_file;
extern int limit;
extern int Flag_EndVertex_Must;
extern int includeNodeNum;
/// 两两节点间信息素，就是环境信息素
double g_Trial[N_NODE_COUNT][N_NODE_COUNT]; 

///		rnd函数

///		返回指定范围内的随机浮点数
///     @param dbLow		范围起始数    
///     @param dbUpper  范围终止数
///     @return					返回的此范围内的随机数 
double rnd(double dbLow, double dbUpper)
{
	double dbTemp = rand() / ((double)RAND_MAX + 1.0);
	return dbLow + dbTemp*(dbUpper - dbLow);
}

///		Init函数

///		初始化蚂蚁信息
///     @note		  初始化蚂蚁的成员变量，包括起始点、终止点、必经点集合、必经边集合、禁止点集合等等  
void CAnt::Init()
{
	for (int i = 0; i<N_NODE_COUNT; i++)  
	{
		m_nAllowedNode[i] = 1;					   //设置全部节点为没有去过，即都可去
		m_nPath[i] = -1;								   //蚂蚁走的路径全部设置为-1
	}
	///在寻路的过程中，终点对于蚂蚁是不可见的。只有当蚂蚁走完全部必过节点后，才会手动将m_nAllowedNode[end]设置为1
	if(Flag_EndVertex_Must == 0)  //必经边包含终点
		m_nAllowedNode[end] = 0;
	else                                        //必经边不包含终点
		m_nAllowedNode[end] = 1;
	///蚂蚁走过的路径长度设置为0
	m_nPathLength = 0;
	///出发节点为起始点
	m_nCurNodeNo = start;
	///把出发节点保存入路径数组中
	m_nPath[0] = m_nCurNodeNo;
	///已经去过的节点数量设置为1，因为起点也算作去过的节点
	m_nMovedNodeCount = 1;
	///路径的所有节点包含m_nIncluding个including set中的节点
	m_nIncluding = 0;
	///完成路径搜索标志位为0
	m_finish = false;
	///用初始的吸引力集合初始化influenceSet
	influenceSet = initInfluenceSet;
	///用初始的必经边集合初始化mustEdgeSet
	mustEdgeSet = initMustEdge;
	///用初始的禁止边集合初始化banEdgeSet
	banEdgeSet = initBanEdge;
	///用初始的禁止边初始化limit_vertex
	limit_vertex = limit;
	///记录蚂蚁走过的禁止边的个数
	arcBanNum =0;
}

///		deleteElement函数

///		用于蚂蚁移动后，删除吸引力集合的元素
///     @param num   输入的必经点点编号    
///     @note		  蚂蚁经过一个必经点后，从吸引力集合中删除此节点，表明该必经点将不再起吸引的作用
void CAnt::deleteElement(int key)
{
	list<int>::iterator Itor;
	for (Itor = influenceSet.begin(); Itor != influenceSet.end();)
	{
		if (*Itor == key) 
		{
			Itor = influenceSet.erase(Itor);
		}
		else
		{
			Itor++;
		}
	}
}

///		deleteEdge函数

///		用于蚂蚁经过必经边后，删除必经边
///     @param linkid   输入的必经边的LinkID编号    
///     @note		  蚂蚁经过一个必经边后，从必经边集合中删除此边的编号，表明该必经边将不再起吸引的作用
void CAnt::deleteEdge(int linkid)
{
	list<int>::iterator Itor;
	for (Itor = mustEdgeSet.begin(); Itor != mustEdgeSet.end();)
	{
		if (*Itor == linkid) 
		{
			Itor = mustEdgeSet.erase(Itor);
		}
		else
		{
			Itor++;
		}
	}
}

///		getInfluence函数

///		取得指定编号的节点到所有吸引力节点的最小距离
///     @param num 输入的节点编号    
///     @return      到吸引力点的最小路径值 
int CAnt::getInfluence(int num) 
{
	list<int>::iterator Itor;
	int val = 100000000;
	for (Itor = influenceSet.begin(); Itor != influenceSet.end();Itor++) 
	{
		if (val > minDistance[num][*Itor])
		{
			val = minDistance[num][*Itor];
		}
	}
	return val;
}

///		ChooseNextNode函数

///		蚂蚁选择下一个节点
///     @return      蚂蚁选择的下一个路径节点的编号
///     @note		  蚂蚁根据到各个吸引力点的最小路径计算出转移概率，随后用轮盘随机的方法选择下一个要走的路径节点 
int CAnt::ChooseNextNode()
{
	//此刻蚂蚁还没有走完特定点
	int nSelectedNode = -1; //返回结果，先暂时把其设置为-1
	double dbTotal = 0.0;
	double prob[8] = {-1,-1,-1,-1,-1,-1,-1,-1};		///保存蚂蚁从当前节点到它的邻接点去的概率（转移概率）
	int pos[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };		    ///与prob数组的概率对应，表示相应概率对应的节点number
	int i = 0;
	pArcNode parc = G.vertices[m_nCurNodeNo].adjlist.head;//当前节点的邻接链表的头指针
	//计算当前节点的相连节点的转移概率
	for (; parc != NULL; parc = parc->nextarc)
	{
		if (m_nAllowedNode[parc->adjvex] == 1)  ///没去过 ( 用来限制蚂蚁不直接去终点，而是经过所有必经点后才去 )
		{
			double flu = getInfluence(parc->adjvex) + parc->arcinfo.weight;	///得到这条边的吸引力
			prob[i] = pow(g_Trial[m_nCurNodeNo][parc->adjvex],  ALPHA) * pow(1.0 / flu,  BETA);
			//检测该条边是不是必经边
			if(!mustEdgeSet.empty())
			{
				for(auto itor=mustEdgeSet.begin();itor!=mustEdgeSet.end();itor++)
				{
					if(*itor == parc->arcinfo.arcnum)
					{
						prob[i] = 10;
						break;
					}
				}
			}
			//检测该条边是不是禁止边
			if(!banEdgeSet.empty())
			{
				for(auto itor=banEdgeSet.begin();itor!=banEdgeSet.end();itor++)
				{
					if(*itor == parc->arcinfo.arcnum)
					{
						prob[i] = 0;
					}
				}
			}
			pos[i] = parc->adjvex;
			dbTotal = dbTotal + prob[i];
			i++;
		}
	}	//最后，i为当前节点下一步可以走的路径的数目
	if (i == 0)
	{
		return -1;//表示当前节点下一步无路可走，返回-1.（i=0表明当前节点的相邻节点都已经去过）
	}
	int nAvailable = i;//表示当前节点下一步可以走的路径的数目

	double dbTemp = 0.0;
	if (dbTotal > 0.0) 
	{
		dbTemp = rnd(0.0, dbTotal);
		for (int i = 0; i < nAvailable; i++) 
		{
			dbTemp = dbTemp - prob[i];
			if (dbTemp < 0.0) 
			{
				nSelectedNode = pos[i];
				break;
			}
		}
	}
	if (nSelectedNode == -1)
	{
		nSelectedNode = pos[0];
	}

	return nSelectedNode;	//返回结果，就是节点的编号
}
///		Move函数

///		蚂蚁在节点间移动
///     @return      若蚂蚁可以走到下一个节点则返回1，反之蚂蚁无路可走则返回2
///     @note		  无路可走说明：必过点没有走完(因为走完了就不会进入这个Move函数)，同时又没有下一条可走的路
int CAnt::Move()
{
	int nNodeNo = ChooseNextNode();		//选择下一个节点
	if (nNodeNo != -1)							    //说明还有节点可以走
	{
		m_nPath[m_nMovedNodeCount] = nNodeNo;				//保存蚂蚁走的路径
		int CurLinkID = GetLinkID(m_nCurNodeNo,nNodeNo);  //取出当前节点和下一次要过的节点之间的LinkID。
		m_nCurNodeNo = nNodeNo;										//改变当前所在节点为选择的节点
		m_nMovedNodeCount++;											//已经去过的节点数量加1

		/*检测这次是否要走 还没有走的必经点*/
		if (includingSet.FindSet(nNodeNo, &includingSet) == demandSet[0])
		{
			m_nIncluding++;
			deleteElement(nNodeNo);	//如果选择的节点是influence set里面的点，将其从influence set中删除
			if(m_nIncluding == includeNodeNum)
				m_nAllowedNode[end] = 1;
		}
		/*检测这次是否要走 还没有走的必经边*/
		if(!mustEdgeSet.empty())
		{
			for(auto itor = mustedge_file.begin();itor != mustedge_file.end();itor++)
			{
				if(*itor == CurLinkID)
				{
					deleteEdge(CurLinkID);			//如果选择的边是mustEdgeSet里面的点，将其从mustEdgeSet中删除
				}
			}
		}
		/*检测这次是否走了 禁止边*/
		for (auto itor = ban_file.begin(); itor != ban_file.end(); itor++)
		{
			if (*itor == CurLinkID)
			{
				arcBanNum++;
			}
		}
		return 1;
	}
	else											 //说明无路可走了，此只蚂蚁走到头了
		return 2;
}

///		Search函数

///		蚂蚁搜索路径
///     @note		  蚂蚁进行路径地搜索，若满足搜索要求则退出搜索并进行路径长度计算，反之则一直处于Move寻路状态
void CAnt::Search() 
{
	///蚂蚁初始化
	Init();			
	///只要蚂蚁走过的路径中包含的特定点还没有达到要求的数目，就让蚂蚁一直move
	while ( !(influenceSet.empty() && mustEdgeSet.empty() ) && 1==Move() ); 
	///完成搜索，计算路径长度
	if(m_nIncluding == includeNodeNum)		
		CalPathLength();		
}

///		CalPathLength函数

///		计算蚂蚁走过的路径长度
///     @note		  用Dijkstra算法进行与终止点的最短距离相连，并且计算总的路径长度
void CAnt::CalPathLength()
{
	int m = 0;
	int n = 0;
	int i = 1;
	int vnum = G.vexnum;
	m_nPathLength = 0;
	int dist[600], pres[600];
	Dijkstra(vnum, m_nPath[m_nMovedNodeCount - 1], dist, pres, g_Distance);
	if (	dist[end] < 100000000) 
	{
		m_nPathLength += dist[end];
		m_finish = true;//说明完成了一次路径搜索
	}
	else 
	{
		//std::cout<<"Error\n";
		return;
	}
	while (m_nPath[i] != -1)
	{
		m = m_nPath[i];
		n = m_nPath[i - 1];
		m_nPathLength = m_nPathLength + g_Distance[n][m];
		i++;
	}
	
	stack<int> sta;
	int temp = end;
	while (pres[temp] != m_nPath[m_nMovedNodeCount - 1])
	{
		sta.push(temp);
		temp = pres[temp];
	}
	sta.push(temp);
	while (!sta.empty())
	{
		int temp = sta.top();
		m_nPath[m_nMovedNodeCount] = temp;
		sta.pop();
		m_nMovedNodeCount++;
	}
}
