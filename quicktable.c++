#include <algorithm>//STL通用算法
#include <iostream>
#include <map> 
#include <vector>
#include <sstream>
#include <cmath>
#include "memory.h"

quick_page_table::quick_page_table()
{
    for(int i=0;i<q_max_page_number;i++)
    {
        quick_table[i].push_back(-3);//快表创建初始没有分配物理页
        quick_table[i].push_back(-1);//快表创建初始没有分配虚拟页
        quick_table[i].push_back(-1);
    }
}

int quick_page_table::findPagenumber(int pid,int address)
{
	int pagenumber,index;
	pagenumber=address/page_size;
    for (map<int,vector<int> >::iterator iter=quick_table.begin();iter!=quick_table.end();iter++)
    {
        if (pagenumber == iter->second[1] && pid == iter->second[2])
        {
            index = iter->second[0];  //查找物理页
            return index;
        }               
    }
    return invalid_value;
}

int quick_page_table::Find_offest(int pid,int address)
{
    int Deviation;
    Deviation=address%page_size;
    return Deviation;
}