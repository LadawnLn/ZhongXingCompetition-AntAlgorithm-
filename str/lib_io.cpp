#include <string.h>
#include <assert.h>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>
#include <signal.h>
#include "lib_io.h"

#define MAX_LINE_LEN 4000

///		read_file函数

///		从一个csv文件读取topo图，并将结果存入静态内存
///     @param buff			指向存储topo图的数组指针，即用一个二维数组存储图信息
///		@param spec		表示二维数组的最大行数，即图的边数
///		@param filename  指向要读取的csv文件路径
///     @return					若读取成功，返回二维数组的行数；若读取失败，返回0
int read_file(char ** const buff, const unsigned int spec, const char * const filename)
{
	FILE *fp = fopen(filename, "r");
	char line[MAX_LINE_LEN + 2];
	unsigned int cnt = 0;	//统计行数
    if (fp == NULL)
    {
        std::cout<<"Fail to open file "<<filename<<", "<<strerror(errno)<<".\n";
        return 0;
    }
    std::cout<<"Open file "<<filename<<" is OK.\n";
    while ((cnt < spec) && !feof(fp))
    {
        line[0] = 0;
        fgets(line, MAX_LINE_LEN + 2, fp);
        if (line[0] == 0)   continue;
        buff[cnt] =  new char [MAX_LINE_LEN + 2] ;
        strncpy(buff[cnt], line, MAX_LINE_LEN + 2 - 1);
        buff[cnt][4001] = 0;
        cnt++;
    }
    fclose(fp);
    std::cout<<"There are "<< cnt << " lines in file "<<filename<< " .\n\n";
    return cnt;
}

///		release_buff函数

///		释放读取topo图时建立的动态内存空间
///     @param buff							指向存储topo图的数组指针
///		@param valid_item_num		表示允许释放的最大行数，即图的边数
void release_buff(char ** const buff, const int valid_item_num)
{
	int i = 0;
    for (; i < valid_item_num; i++)
        delete [] buff[i];
}