#include <algorithm>//STL通用算法
#include <iostream>
#include <map> 
#include <vector>
#include <sstream>
#include <string.h>
#include <cmath>
#include "memory.h"
//#include "file_dir.h"

page_table::page_table()
{
    for(int i=0;i<occupancy;i++)
    {
        this->table[i].push_back(-3);//页表创建初始没有分配物理页
        this->table[i].push_back(-2);//页表创建初始没有分配对应相应的磁盘块号
        this->table[i].push_back(0);//表示磁盘块是否被占用
    }
}

void page_table::Init_page(string f_name)
{
   os_file* os_f=File::Open_File(f_name);//先通过文件目录查找到文件的句柄
     for(int i=0;i<FBLK_NUM ;i++)//将文件句柄对应的磁盘块号与分配的页号对应
        {
            for(int j=0;j<occupancy/2;j++)
            {
               if(this->table[j][1]==-2)
                {
                     this->table[j][1]=os_f->f_iNode->block_address[i];//通过句柄找到对应的磁盘块号给没有存上的页表对应上
                    break;
               }
            }
       }
     File::Close_File(os_f);
 }

void page_table::Insert(int page_num,int temp_block)//插入一个空页
{
    this->table[page_num].push_back(-3);
    this->table[page_num].push_back(temp_block);
    this->table[page_num].push_back(0);
}

void page_table::Delete(int page_num,int temp_block)//删除一个页
{
    map<int,vector<int> >::iterator key = table.find(page_num);
    if(key!=table.end())
 	{
		table.erase(key);
 	}
 	Insert(page_num,temp_block);
}

int page_table::findblocknumber(int pid,int address)
{
	int pagenumber,blocknum;
	pagenumber=address/page_size;
    if (pagenumber < table.size())
    {
        blocknum = table[pagenumber][1];  //查找磁盘块号
        return blocknum;
    }   
    else
        return invalid_value;
}

int page_table::findPagenumber(int pid,int address)
{
    int pagenumber,index;
	pagenumber=address/page_size;
    if (pagenumber < table.size())
    {
        index = table[pagenumber][0];  //查找物理页号
        return index;
    }   
    else
        return invalid_value;
}

int page_table::Find_offest(int pid,int address)//计算偏移量
{
    int Deviation;
    Deviation=address%page_size;
    return Deviation;
}

void page_table::revise(int page_num, int frame_num, int valid,int wBlock)//通过缺页调度算法进行页表的更改
{
    if(valid == 1)
    {
        table[page_num][2] = Valid;
        table[page_num][1] = wBlock;
		table[page_num][0] = frame_num;
    }     
    else
    {
        table[page_num][2] = 0;
		table[page_num][0] = Physical_blocks_Not_exist;
    }
}
