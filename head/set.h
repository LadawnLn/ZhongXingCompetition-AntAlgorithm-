#ifndef _SET_H_
#define _SET_H_

///		必经点集合的结构体

///		定义了全局必经点的集合，包括节点数目上限、父节点、查找和删除等操作
typedef struct _unionSet
{
	int maxn;															///< 节点数目上限
	int *pa;																///< 表示x的父节点										
	int (*FindSet)(int x,struct _unionSet *);					///< 查找x属于哪个集合的函数指针	
	void (*UnionSet)(int x, int y,struct _unionSet *);		///< 合并x,y所在的集合的函数指针	
	void (*clear)(struct _unionSet*);							///< 清空_unionSet的函数指针	
	void (*DestroySet)(struct _unionSet *);				///< 销毁_unionSet的函数指针	
}*pUnionFindSet,UnionFindSet;

///		InitUnionSet函数

///		初始化UnionSet（必经点集合）
///     @param p						指向一个_unionSet结构体的指针
///     @param n						集合数目的最大值
void InitUnionSet(pUnionFindSet p, int n);

#endif