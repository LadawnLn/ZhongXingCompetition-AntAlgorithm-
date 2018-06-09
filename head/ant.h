#pragma once
#include "map.h"
#include "set.h"
#include <list>
using std::list;

/// 启发因子，信息素的重要程度
#define ALPHA 1.0 
/// 期望因子，节点间距离的重要程度
#define BETA 2   
/// 信息素残留参数
#define ROU  0.8 
/// 蚂蚁数量
#define N_ANT_COUNT 50
/// 迭代次数
#define N_IT_COUNT 200 
/// 最大节点数量
#define N_NODE_COUNT 600 
/// 总的信息素
#define DBQ 100.0		 
/// 一个标志数，10的9次方
#define DB_MAX 1000000000 
/// 两两节点间信息素，就是环境信息素
extern double g_Trial[N_NODE_COUNT][N_NODE_COUNT];

///		rnd函数

///		返回指定范围内的随机浮点数
///     @param dbLow		范围起始数    
///     @param dbUpper  范围终止数
///     @return					返回的此范围内的随机数 
double rnd(double dbLow, double dbUpper);

/// 类名：CAnt		
/// 
/// 类功能：定义单个蚂蚁与寻路相关的成员变量和成员函数
class CAnt
{
public:
	CAnt(void) { };
	~CAnt(void) { };
public:
	/// 蚂蚁走的路径
	int m_nPath[N_NODE_COUNT];		
	/// 蚂蚁走过的路径长度(实际就是指花销值)
	int m_nPathLength;							   
	/// 蚂蚁没去过的节点
	int m_nAllowedNode[N_NODE_COUNT]; 
	/// 当前所在节点编号
	int m_nCurNodeNo;							    
	/// 已经去过的节点数量
	int m_nMovedNodeCount;						
	/// 表示蚂蚁最终是否到达终点,同时通过所有必过点
	bool m_finish;										
	/// 表示能对当前蚂蚁产生吸引力的including set（必经点）的点的集合（一只蚂蚁如果经过了某个set的点，那么这个点就不会再产生影响力，需要将其剔除掉）
	list<int> influenceSet;								
	/// 表示当前蚂蚁的必经边的集合，一旦蚂蚁经过了这个必经边，则需要将其释放掉。
	list<int> mustEdgeSet;							
	/// 表示当前蚂蚁的禁止边的集合。
	list<int> banEdgeSet;								
	/// 表示蚂蚁走的路径的所有节点中，有多少个是including set（必经点）里面的
	int m_nIncluding;									
	/// 表示蚂蚁走的路径的节点个数限制的最大值
	int limit_vertex;                                     
	/// 记录蚂蚁走过的禁止边的个数
	int arcBanNum;									
public:
	///		getInfluence函数

	///		取得指定编号的节点到所有吸引力节点的最小距离
	///     @param num 输入的节点编号    
	///     @return      到吸引力点的最小路径值 
	int getInfluence(int num);		
	///		ChooseNextNode函数

	///		蚂蚁选择下一个节点
	///     @return      蚂蚁选择的下一个路径节点的编号
	///     @note		  蚂蚁根据到各个吸引力点的最小路径计算出转移概率，随后用轮盘随机的方法选择下一个要走的路径节点  
	int ChooseNextNode();	
	///		Init函数

	///		初始化蚂蚁信息
	///     @note		  初始化蚂蚁的成员变量，包括起始点、终止点、必经点集合、必经边集合、禁止点集合等等  
	void Init();		
	///		Move函数

	///		蚂蚁在节点间移动
	///     @return      若蚂蚁可以走到下一个节点则返回1，反之蚂蚁无路可走则返回2
	///     @note		  无路可走说明：必过点没有走完(因为走完了就不会进入这个Move函数)，同时又没有下一条可走的路
	int Move();											
	///		Search函数

	///		蚂蚁搜索路径
	///     @note		  蚂蚁进行路径地搜索，若满足搜索要求则退出搜索并进行路径长度计算，反之则一直处于Move寻路状态
	void Search();										
	///		CalPathLength函数

	///		计算蚂蚁走过的路径长度
	///     @note		  用Dijkstra算法进行与终止点的最短距离相连，并且计算总的路径长度
	void CalPathLength();								
	///		deleteElement函数

	///		用于蚂蚁移动后，删除吸引力集合的元素
	///     @param num   输入的必经点点编号    
	///     @note		  蚂蚁经过一个必经点后，从吸引力集合中删除此节点，表明该必经点将不再起吸引的作用
	void deleteElement(int num);
	///		deleteEdge函数

	///		用于蚂蚁经过必经边后，删除必经边
	///     @param linkid   输入的必经边的LinkID编号    
	///     @note		  蚂蚁经过一个必经边后，从必经边集合中删除此边的编号，表明该必经边将不再起吸引的作用
	void deleteEdge(int linkid);                      
};