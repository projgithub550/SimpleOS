#include <algorithm>//STL通用算法
#include <iostream>
#include <map> 
#include <vector>
#include <sstream>
#include <cmath>
#include "memory.h"

Quick_page_table::Quick_page_table()
{
    for(int i=0;i<q_max_page_number;i++)
    {
        quick_table[i].push_back(-3);//快表创建初始没有分配物理页
        quick_table[i].push_back(-1);//快表创建初始没有分配虚拟页
        quick_table[i].push_back(-1);//在磁盘中的位置
        quick_table[i].push_back(-1);
    }
}

int Quick_page_table::findPageNumber(int pid,int address)
{
	int pagenumber,index;
	pagenumber=address/page_size;
    for (map<int,vector<int> >::iterator iter=quick_table.begin();iter!=quick_table.end();iter++)
    {
        if (pagenumber == iter->second[1] && pid == iter->second[3])
        {
            index = iter->second[0];  //查找物理页
            return index;
        }               
    }
    return invalid_value;
}

int Quick_page_table::findOffset(int address)
{
    int Deviation;
    Deviation=address%page_size;
    return Deviation;
}

void Quick_page_table::insertQuickTable(int wPage,int virtualpagenumber,int wBlock,int pid)
{
    if(count==q_max_page_number)
    {
        map<int,vector<int> >::iterator iter=quick_table.begin();
        quick_table.erase(iter);//把快表的第一项删除
        map<int,vector<int> >::iterator it=quick_table.end();
        quick_table[it->first+1][0]=wPage;
        quick_table[it->first+1][1]=virtualpagenumber;
        quick_table[it->first+1][2]=wBlock;
        quick_table[it->first+1][3]=pid;
    }
    else if(count<q_max_page_number)
    {
        quick_table[count][0]=wPage;
        quick_table[count][1]=virtualpagenumber;
        quick_table[count][2]=wBlock;
        quick_table[count][3]=pid;
        count++;
    }
}