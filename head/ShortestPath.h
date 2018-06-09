#pragma once
#include "ant.h"

/// 类名：CShortestPath		
/// 
/// 类功能：所有蚂蚁在规定约束下进行最优路径寻找，包括初始化、更新信息素、寻路函数等 
class CShortestPath
{
public:
	CShortestPath() {};
	~CShortestPath()  {};
public:
	/// 蚂蚁数组
	CAnt	 m_cAntAry[N_ANT_COUNT];
	/// 全局最优蚂蚁
	CAnt	 m_cGlobalBestAnt;
	/// 本次迭代最优蚂蚁
	CAnt	 m_cIterationBestAnt;
	/// 临时蚂蚁
	CAnt	 m_cTempAnt;
	/// 信息素最大值
	double	Qmax;
	/// 信息素最小值
	double	Qmin;
	/// 最大值和最小值的比率
	double	m_dbRate;
	///		InitData函数

	///		初始化蚂蚁的信息素分布
	///     @note		  初始化topo图中所有相连边的信息素大小为10000
	void InitData();
	
	///		Search函数

	///		寻找全局最优蚂蚁来取得最优路径，同时以约束模型判断此次寻路是否有解
	///     @note		  若满足所有条件，即有解则返回最优解；若无解则根据约束模型再次进行寻路直到满足条件输出一个次优解
	void Search();
	
	///		UpdateTrial函数

	///		更新路径的信息素分布
	///     @param flag  表示是否使用全局最优解，1表示使用全局最优解，0表示使用迭代最优解 
	///     @note   使用全局最优和迭代最优交替更新的策略，每5次迭代更新一次信息素
	void UpdateTrial(int flag);
	
	///		printPath函数

	///		输出最优的路径
	///     @note      将全局最优蚂蚁所走的路径和所用的花费，进行打印输出 
	void printPath();
};
