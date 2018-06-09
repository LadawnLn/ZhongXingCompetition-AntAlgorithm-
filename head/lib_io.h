#ifndef __LIB_IO_H__
#define __LIB_IO_H__

///		read_file函数

///		从一个csv文件读取topo图，并将结果存入静态内存
///     @param buff			指向存储topo图的数组指针，即用一个二维数组存储图信息
///		@param spec		表示二维数组的最大行数，即图的边数
///		@param filename  指向要读取的csv文件路径
///     @return					若读取成功，返回二维数组的行数；若读取失败，返回0
int read_file(char ** const buff, const unsigned int spec, const char * const filename);


///		release_buff函数

///		释放读取topo图时建立的动态内存空间
///     @param buff							指向存储topo图的数组指针
///		@param valid_item_num		表示允许释放的最大行数，即图的边数
void release_buff(char ** const buff, const int valid_item_num);

#endif

