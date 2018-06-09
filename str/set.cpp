#include "set.h"
#include "string.h"

///		_findSet函数

///		返回x的父母
///     @param x						节点编号
///     @param pset				指向一个_unionSet结构体的指针
int _findSet(int x, pUnionFindSet pset)
{
	return (pset->pa)[x];
}

///		_unionset函数

///		将x的父母设置为y
///     @param x						节点编号1
///     @param y						节点编号2
///     @param pset				指向一个_unionSet结构体的指针
void _unionset(int x, int y, pUnionFindSet pset)
{
	pset->pa[x] = y;
}

///		_destroySet函数

///		释放内存
///     @param pset				指向一个_unionSet结构体的指针
void _destroySet(pUnionFindSet pset)
{
	delete [] (pset->pa);
}

///		_clear函数

///		清空数据为 -1
///     @param p						指向一个_unionSet结构体的指针
void _clear(pUnionFindSet pset)
{
	memset(pset->pa, 0xff, (pset->maxn)*sizeof(int));
}

///		InitUnionSet函数

///		初始化UnionSet（必经点集合）
///     @param p						指向一个_unionSet结构体的指针
///     @param n						集合数目的最大值
void InitUnionSet(pUnionFindSet pset, int n)
{
	pset->pa = new int[n];
	pset->maxn = n;
	memset(pset->pa, 0xff, n*sizeof(int));		
	pset->FindSet = _findSet;
	pset->UnionSet = _unionset;
	pset->DestroySet = _destroySet;
	pset->clear = _clear;
}