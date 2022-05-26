#include <algorithm>//STL通用算法
#include <iostream>
#include <map> 
#include <vector>
#include <sstream>
#include <string.h>
#include <cmath>
#include "memory.h"
#include "file_dir.h"

Page_table::Page_table()    
{
    for(int i=0;i<max_page_number;i++)
    {
        this->table[i].push_back(-3);//页表创建初始没有分配物理页
        this->table[i].push_back(-2);//页表创建初始没有分配对应相应的磁盘块号
        this->table[i].push_back(0);//表示磁盘块是否被占用
    }
    this->nValid = 0;
}

void Page_table::initializePage(string f_name)
{
   // qDebug() << "初始化页表-------------";
    // qDebug() << "文件名-------------" << QString::fromStdString(f_name);
    os_file* os_f=File::Open_File(f_name);//先通过文件目录查找到文件的句柄
    // qDebug() << "拿到句柄-------------";
    //将文件句柄对应的磁盘块号与分配的页号对应
    int* blocka = os_f->f_iNode->block_address;

    int j;
    for(j = 0;j<max_page_number && blocka[j] != MAX_BLOCK_NUM + 1;j++)
    {
       if(this->table[j][1]==-2)
       {
            this->table[j][1]=os_f->f_iNode->block_address[j];//通过句柄找到对应的磁盘块号给没有存上的页表对应上
       }
    }  

    for(int i = j; i < max_page_number; i ++)
    {
        this->table[i][1] = disk::first_free();
    }
    File::Close_File(os_f);
}

void Page_table::insertPage(int page_num,int temp_block)//插入一个空页
{
    this->table[page_num].push_back(-3);
    this->table[page_num].push_back(temp_block);
    this->table[page_num].push_back(0);
}

void Page_table::deletePage(int page_num,int temp_block)//删除一个页
{
    map<int,vector<int> >::iterator key = table.find(page_num);
    if(key!=table.end())
 	{
		table.erase(key);
 	}
 	insertPage(page_num,temp_block);
}

int Page_table::findBlockNumber(int address)
{
	int pagenumber,blocknum;
	pagenumber=address/page_size;
    if (pagenumber < (int)table.size() && table[pagenumber][1] >= 0)
    {
        blocknum = table[pagenumber][1];  //查找磁盘块号
        return blocknum;
    }   
    else
        return invalid_value;
}

int Page_table::findPageNumber(int address)
{
    int pagenumber,index;
	pagenumber=address/page_size;
//    qDebug() << "page_numver" << pagenumber;
//    qDebug() << "有效位：" << table[pagenumber][2];
    if (pagenumber < (int)table.size() && table[pagenumber][2] == 1)
    {
        index = table[pagenumber][0];  //查找物理页号
        return index;
    }   
    else
        return invalid_value;
}

int Page_table::findOffset(int address)//计算偏移量
{
    int Deviation;
    Deviation=address%page_size;
    return Deviation;
}

void Page_table::revisePage(int page_num, int frame_num, int valid,int wBlock)//通过缺页调度算法进行页表的更改
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
