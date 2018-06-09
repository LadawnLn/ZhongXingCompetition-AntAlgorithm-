//////////////////////////////////////////////////////////////////////////  
///     COPYRIGHT NOTICE  
///     Copyright (c) 2017, 西南交通大学        
///     All rights reserved.  
///   
/// @author           刘邦国、李炬  
/// @date              2017年5月9日  
///  
//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <time.h>
#include <vector>
#include "lib_io.h"
#include "map.h"
#include "ShortestPath.h"
/// 原图，即根据文件信息构造出来的图
Graph G;						
/// 要求经过的点的个数
int includeNodeNum;		
/// 要求经过的边的个数
int includeEdgeNum;       
/// 禁止经过的边的个数
int banEdgeNum;			
/// topo图边数
int edge_num;				
/// 存放必经点
std::vector<int> demand_file;			
/// 存放起点、终点节点
int VertexStart,VertexEnd;				
/// 存放必经边
std::vector<int> mustedge_file;		
/// 存放禁止点
std::vector<int> ban_file;
/// 存放限制点个数
int LimitNum;      
/// 弗洛伊德入参
int minDistance[600][600];
int pre[600][600];

int main(int argc, char *argv[])
{
	///存放图信息
    char *topo[5000];    
	///指向图文件
    char *topo_file;		 
	 ///输入的必经点的个数
	int demand_num;   
	 ///输入的必经边的个数
	int mustedge_num;
	///输入的禁止边的个数
	int ban_num;          

	///初始化随机种子
	time_t tm;
	time(&tm);
	srand(time(NULL));
	
	///初始化图的信息
	topo_file = "topo.csv";
	///读取topo图，并返回边的数目
	edge_num = read_file(topo, 5000, topo_file);		
	///根据文件初始化图G的拓扑信息
	InitMap(topo);													

	///输入必经点和首尾点
	demand_num = InputDemand(demand_file,&VertexStart,&VertexEnd);								
	///初始化必过点要求
	includeNodeNum = InitDemandSet(demand_file,demand_num, VertexStart, VertexEnd);		
	///输入必经边
	mustedge_num	= InputMustEdge(mustedge_file);															
	///初始化必过边要求
	includeEdgeNum = InitMustEdge(mustedge_file,mustedge_num);										
	///输入禁止边
	ban_num = InputBanEdge(ban_file,&LimitNum);																
	///初始化禁止边要求
	banEdgeNum = InitBanEdge(ban_file,ban_num,LimitNum);												

	/*测试程序运行时间*/
	clock_t Time_start,Time_end;
	double Duration_Time ;
	Time_start = clock();

 	///蚁群算法的核心函数
	FloydWarshall(minDistance, pre);	 
	CShortestPath csp;
	csp.InitData();
	csp.Search();

	///释放堆内存
	release_buff(topo, edge_num);
	DestroyMap(&G);
	includingSet.DestroySet(&includingSet);

	/*统计程序运行时间*/
	Time_end  = clock();
	Duration_Time = ((double)(Time_end-Time_start))/CLK_TCK;
//	std::cout <<"the used time is: "<<Duration_Time<<std::endl;

	system("pause");
	return 0;
}

