#include "ShortestPath.h"
#include "string.h"
#include "map.h"
#include <iostream>
#include <queue>
#include <random>
#include <time.h>
using std::queue;
extern int includeNodeNum;
extern list<int> initMustEdge;
/// 路径节点数是否在限制节点数之内		1*/
bool if_nodeNum; 
/// 必经点==要求数		2*/
bool if_nodeNeed;
///	必经边==要求数        3*/
bool if_arcNeed;
///	是否走了禁止边		    4*/
bool  if_arcBan;
///	是否进行花费少或者节点数少的选择
bool if_56 = false; 
/// 保存被去掉的边ID
list<int > arcObsolete;
/// 随机数产生器
std::default_random_engine eng((unsigned)time(NULL));

///		deleteArc函数

///		删除并返回删除的必经边
///     @param MustEdge		指向必经点的集合的引用
///     @param arcObsolete		指向保存禁止边的集合的引用
///     @return							保存已经删除的禁止边的集合
list<int> deleteArc(list<int >&MustEdge, list<int >&arcObsolete) 
{
	std::uniform_int_distribution<int> randInt(1, MustEdge.size()); 
	int deleteArcAt = randInt(eng);
	int j = 1;
	if(MustEdge.size()!=0)
	for (auto i = MustEdge.begin(); i != MustEdge.end(); i++, j++)
	{
		if (j == deleteArcAt)
		{
			arcObsolete.push_back(*i);
			MustEdge.erase(i);
			break;
		}
	}
	return arcObsolete;
}

///		InitData函数

///		初始化蚂蚁的信息素分布
///     @note		  初始化topo图中所有相连边的信息素大小为10000
void CShortestPath::InitData()
{
	const int inf = 100000000;
	m_cGlobalBestAnt.m_nPathLength = inf;//初始化全局最优蚂蚁路径长度最大
	pArcNode parc = NULL;
	/*初始化所有边的信息素为10000.0,两个点如果不存在边的话，信息素为0*/
	for (int i = 0; i < G.vexnum; i++)    //初始化每条边的信息素为10000
	{
		parc = G.vertices[i].adjlist.head;
		while (parc != NULL)   
		{
			g_Trial[i][parc->adjvex] = 10000.0;//初始化图中的每条路径的信息素为10000.0
			parc = parc->nextarc;
		}
	}
	/*第一次迭代时，还没有全局最优解，所有计算不出最大值和最小值，先设置为0.0*/
	Qmax = 0.0;
	Qmin = 0.0;
	m_dbRate = 15;//最大信息素和最小信息素的比值为15倍
}

///		UpdateTrial函数

///		更新路径的信息素分布
///     @param flag  表示是否使用全局最优解，1表示使用全局最优解，0表示使用迭代最优解 
///     @note			 使用全局最优和迭代最优交替更新的策略，每5次迭代更新一次信息素
void CShortestPath::UpdateTrial(int nFlag)
{
	if (nFlag == 1)			//使用全局最优解
	{
		if (m_cGlobalBestAnt.m_nPathLength == 100000000)
		{
			return;		//在这次迭代后，全局最优蚂蚁还不存在
		}
		m_cTempAnt = m_cGlobalBestAnt;
	}
	else							//使用迭代最优解
	{
		if (m_cIterationBestAnt.m_nPathLength == 100000000)
		{
			return;		//这次迭代没有产生一个解
		}
		m_cTempAnt = m_cIterationBestAnt;
	}
	double dbTempAry[N_NODE_COUNT][N_NODE_COUNT];
	memset(dbTempAry, 0, sizeof(dbTempAry));
	int m = 0;
	int n = 0;

	/*只用全局最优或者某次迭代的最优蚂蚁去更新信息素*/
	double dbTemp = 1.0 / m_cTempAnt.m_nPathLength;
	for (int j = 1; j <= m_cTempAnt.m_nMovedNodeCount; j++)
	{
		m = m_cTempAnt.m_nPath[j];
		n = m_cTempAnt.m_nPath[j - 1];
		dbTempAry[n][m] = dbTempAry[n][m] + dbTemp;
	}

	/*更新环境信息素，需要参考最大信息素和最小信息素*/
	Qmax = 1.0 / (m_cGlobalBestAnt.m_nPathLength*(1.0 - ROU));
	Qmin = Qmax / m_dbRate;
	/*------------------------------------------------------------*/
	pArcNode parc = NULL;
	for (int i = 0; i < G.vexnum; i++)
	{
		parc = G.vertices[i].adjlist.head;
		while (parc != NULL)
		{
			int x = parc->adjvex;
			g_Trial[i][x] = g_Trial[i][x] * ROU + dbTempAry[i][x];
			if (g_Trial[i][x] < Qmin)
			{
				g_Trial[i][x] = Qmin;
			}
			if (g_Trial[i][x] > Qmax)
			{
				g_Trial[i][x] = Qmax;
			}
			parc = parc->nextarc;
		}//while
	}//for
}


///		Search函数

///		寻找全局最优蚂蚁来取得最优路径，同时以约束模型判断此次寻路是否有解
///     @note		  若满足所有条件，即有解则返回最优解；若无解则根据约束模型再次进行寻路直到满足条件输出一个次优解
void CShortestPath::Search() 
{
	const int inf = 100000000;
	char costOrNode = 'a';
	m_cGlobalBestAnt.m_nPathLength = inf;//初始化全局最优蚂蚁的路径长度为INT_MAX

	while (true)
	{
		for (int i = 0; i < N_IT_COUNT; i++) //迭代
		{
			m_cIterationBestAnt.m_nPathLength = inf;//初始化本次迭代的最优解为INT_MAX
			//每只蚂蚁搜索一遍
			for (int j = 0; j < N_ANT_COUNT; j++) {//一次迭代放出所有蚂蚁出去搜索
				m_cAntAry[j].Search();
			}//for
			/*-----------------------------------------------*/
			//保存最佳结果
			switch (costOrNode)//判断用户选择cost优先或Node数优先
			{
			case 'a':
				for (int j = 0; j < N_ANT_COUNT; j++)
				{
					if (m_cAntAry[j].m_finish && m_cAntAry[j].m_nPathLength < m_cGlobalBestAnt.m_nPathLength) {
						m_cGlobalBestAnt = m_cAntAry[j];//根据路径长度（cost）更新全局的最优蚂蚁
					}
					if (m_cAntAry[j].m_finish&&m_cAntAry[j].m_nPathLength < m_cIterationBestAnt.m_nPathLength) {
						m_cIterationBestAnt = m_cAntAry[j];//更新本次迭代的最优蚂蚁
					}
				}//for
				break;
			case 'b':
				for (int j = 0; j < N_ANT_COUNT; j++)
				{
					if (m_cAntAry[j].m_finish && m_cAntAry[j].m_nMovedNodeCount < m_cGlobalBestAnt.m_nMovedNodeCount) {
						m_cGlobalBestAnt = m_cAntAry[j];//根据节点数更新全局的最优蚂蚁
					}
					if (m_cAntAry[j].m_finish&&m_cAntAry[j].m_nMovedNodeCount < m_cIterationBestAnt.m_nMovedNodeCount) {
						m_cIterationBestAnt = m_cAntAry[j];//更新本次迭代的最优蚂蚁
					}
				}//for    
				break;
			default: 
				std::cout << "输入错误,默认a（cost最小）\n"; costOrNode = 'a';  
				break;
			}
			//使用全局最优和迭代最优交替更新的策略
			//每5次迭代使用一次全局最优蚂蚁更新信息素
			if ((i + 1) % 5 == 0)
			{
				UpdateTrial(1);//使用全局最优更新信息素
			}
			else
			{
				UpdateTrial(0);//使用局部最优更新信息素
			}
		}//for

		if_nodeNum = (m_cGlobalBestAnt.m_nMovedNodeCount <= m_cGlobalBestAnt.limit_vertex);  /* 限制点个数限制       1 */
		if_nodeNeed = m_cGlobalBestAnt.influenceSet.empty();														 /* 必经点==要求数       2 */
		if_arcNeed = m_cGlobalBestAnt.mustEdgeSet.empty();														 /* 必经边==要求数       3 */
		if_arcBan = (m_cGlobalBestAnt.arcBanNum == 0);																 /* 走了多少禁止边        4 */

	//    std::cout<<"global: "<<m_cGlobalBestAnt.m_nMovedNodeCount <<" "<<m_cGlobalBestAnt.influenceSet.size()<<" "<<m_cGlobalBestAnt.mustEdgeSet.size()<< " "<<m_cGlobalBestAnt.arcBanNum<<std::endl;
		if (if_nodeNum && if_nodeNeed && if_arcNeed && if_arcBan) /*1,2,3,4*/
		{
			printPath();
			break;
		}
		else if (if_56 == false)//用户选择5,6
		{
			if_56 = true;
			std::cout << "\n无完全符合要求的路径，请选择以下两种次优路径：\n";
			std::cout<<"a：花费最少\tb：经过节点数最少\n";
			std::cin >> costOrNode; 
			if (costOrNode == 'b')
			{
				continue;
			}
		}

		if (if_nodeNeed&&if_arcNeed&&if_arcBan)/*2,3,4 */
		{
			printPath();
			break;
		}
		else if (initMustEdge.size() > 0) /*3 初始化必经边数>2(保留可能的起始点终止点所在的边)*/
		{
			arcObsolete = deleteArc(initMustEdge, arcObsolete); //保存删除的必经边 
			continue;
		}
		else if (if_nodeNum&&if_nodeNeed&&if_arcBan) /*1,2,4*/  /*到此处路径中含有的必经边数==0*/
		{
			printPath();
			break;
		}
		else if (if_nodeNeed&&if_arcBan)/*2,4 */
		{
			printPath();
			break;
		}
		else
		{
			std::cout <<"禁止边="<< m_cGlobalBestAnt.arcBanNum << std::endl;
			std::cout << "无解\n";
			break;
		}
	}//while

}// Search

///		printPath函数

///		输出最优的路径
///     @note      将全局最优蚂蚁所走的路径和所用的花费，进行打印输出 
void CShortestPath::printPath()
{
	std::cout << "\n输出路径为：    ";
	std::cout << m_cGlobalBestAnt.m_nPath[0];
	int i = 1;
	for (; i < m_cGlobalBestAnt.m_nMovedNodeCount; i++)
		if (m_cGlobalBestAnt.m_nPath[i] != end)
			std::cout<< "-->"<<m_cGlobalBestAnt.m_nPath[i];
	std::cout <<"-->"<< m_cGlobalBestAnt.m_nPath[i-1]<< std::endl;
	std::cout << "花费 = " << m_cGlobalBestAnt.m_nPathLength<<std::endl;
}