#include <iostream>
#include <map> 
#include <vector>
#include <sstream>
#include <cmath>
using namespace std;


class page_table//每一个进程对应一个页表，页表中存有进程分配的页号和对应的物理块号 
{
    public:
        map<int,vector<int> >table;//主键是虚拟页号，物理页号，在磁盘中的位置，有效位；
        vector<int>schedule_queue;//用于页面调度算法，调度队列
        page_table();
        void Init_page(string f_name);//初始化页表
        void Insert(int page_num,int temp_block);//插入进程页表项
        void Delete(int page_num,int temp_block);//删除进程页表项
        int findblocknumber(int pid,int address);//寻找对应的磁盘块号
        int findPagenumber(int pid,int address);//寻找对应的物理页
        int Find_offest(int pid,int address);//寻找偏移量
        void revise(int page_num, int frame_num, int valid,int wBlock);
};

class quick_page_table//快表
{
    public:
        map<int,vector<int> >quick_table;//主键页号，对应的物理块号,和进程id
        int count=0;//快表中存的页数
        quick_page_table();
        int findPagenumber(int pid,int address);//寻找对应的物理页
        int Find_offest(int pid,int address);//寻找偏移量
};

class MemoryManager
{
    public:
        map<int,page_table> page_tables; //主键进程号，值对应放入进程对应的页表
        map<int,int> Physical_memory;//物理页以及对应的内存

        string schedule;
        int cur_aid = 0;  // 记录分配次数            
        int allocated = 0;  //分配的内存
        int Total_physical_m;
        int physical_size_used=0;
        int page_fault = 0;
        int page_embed = 0;
        quick_page_table q_page_table;
        page_table pTable;

        MemoryManager();
        int page_allocation(int pid,int exe_size,char buff[]);//进程号，大小，内容
        bool page_free(int pid);
        void page_show(int pid);
        void fifo(int pid,int wPage,int wBlock);
        void LRU(int pid,int wPage,int wBlock);
        void monitor();
        int Is_Enter_interrupt(int pid,int address);


        int readMem(int pid,int startAddr,int size,void* buff);//读内存函数（进程号，读的起始地址，读的大小，缓冲区）
        int writeMem(int pid,int startAddr,int size,void* buff);//写内存函数
        int readMemPage(int wPage,char *buff);//读物理页对应的内存块
        int writeMemPage(int wPage,char *buff);//写入物理页对应的内存块
};







