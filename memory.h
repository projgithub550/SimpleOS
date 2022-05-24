#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <map> 
#include <vector>
#include <sstream>
#include <cmath>
#include "constant.h"
using namespace std;


class Page_table//每一个进程对应一个页表，页表中存有进程分配的页号和对应的物理块号 
{
    public:
        map<int,vector<int> >table;//主键是虚拟页号，物理页号，在磁盘中的位置，有效位；
        vector<int>schedule_queue;//用于页面调度算法，调度队列
        int nValid; //已经被占用的页表项
        Page_table();
        void initializePage(string f_name);//初始化页表
        void insertPage(int page_num,int temp_block);//插入进程页表项
        void deletePage(int page_num,int temp_block);//删除进程页表项
        int findBlockNumber(int address);//寻找对应的磁盘块号
        int findPageNumber(int address);//寻找对应的物理页
        int findOffset(int address);//寻找偏移量
        void revisePage(int page_num, int frame_num, int valid,int wBlock);
};

class Quick_page_table//快表
{
    public:
        map<int,vector<int> >quick_table;//主键0-14下标，对应的物理块号,，虚拟页号，在磁盘中的位置和进程id
        int count=0;//快表中存的页数
        Quick_page_table();
        int findPageNumber(int pid,int address);//寻找对应的物理页
        int findOffset(int address);//寻找偏移量
        void insertQuickTable(int wPage,int virtualpagenumber,int wBlock,int pid);//加入快表
};

class MemoryManager
{
    public:
        map<int,Page_table>page_tables; //主键进程号，值对应放入进程对应的页表
        Quick_page_table q_page_table;
        int p_memory[max_page_number];//物理页，有效位

        string schedule;
        int cur_aid = 0;  // 记录总分配次数            
        int allocated = 0;  //分配的内存
        int Total_physical_m;
        int physical_size_used=0;
        int page_fault = 0;
        int page_embed = 0;
//        Quick_page_table q_page_table;
//        Page_table p_table;
//        int firstFree;

        MemoryManager();
        bool freePage(int pid);
        void showPage(int pid);
        int getFirstFree();
        void FIFO(int pid,int& wPage,int& wBlock);
        void LRU(int pid,int addr,int& wPage,int& wBlock);
        void Monitor();
        void createPageTable(int pid,string filename);
        int getProcPhyMem(int pid);
        int getProcVirMem(int pid);
        void clearMemory();

        int readMem(int pid,int startAddr,int size,void* buff);//读内存函数（进程号，读的起始地址，读的大小，缓冲区）
        int writeMem(int pid,int startAddr,int size,void* buff);//写内存函数
        void updateScheQue(int pid,int pageNum);//更新调度队列

        int readMemPage(int wPage,char *buff);//读物理页对应的内存块
        int writeMemPage(int wPage,char *buff);//写入物理页对应的内存块


        /*界面接口*/
        int getTotalMem();
        int getAllocatedMem();
        float getMemRate();
        float getPageFaultRate();
};

#endif






