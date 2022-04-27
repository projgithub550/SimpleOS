#include <algorithm>//STL通用算法
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <cmath>
#define NONE -2;
#define invalid_value -1;
#define Valid 1;
using namespace std;

int page_size=4096;
int max_page_number=16;
int q_max_page_number=4;
int Physical_blocks = 32;

class page_table//每一个进程对应一个页表，页表中应该存有进程分配的页号和对应的物理块号
{
    public:
        map<int,vector<int> >table;

        page_table();
        ~page_table();
        void Insert(int page_num);
        void Delete(int page_num);
        int Find_physical_block(string address);
        void revise(int page_num, int frame_num, int valid);
};

page_table::page_table()
{
    for(int i=0;i<max_page_number;i++)
    {
        table[i].push_back(-2);
        table[i].push_back(-1);
    }
}

page_table::~page_table()
{
}

void page_table::Insert(int page_num)
{
    table[page_num].push_back(-2);
    table[page_num].push_back(-1);
}

void page_table::Delete(int page_num)
{
    map<int,vector<int> >::iterator key = table.find(page_num);
    if(key!=table.end())
    {
        table.erase(key);
    }
    table[page_num].push_back(-2);
    table[page_num].push_back(-1);
}

int page_table::Find_physical_block(string address)
{
    int len=address.length();
    int n=0,page,Deviation,index;
    for(int i=0;i<len;++i)
    {//其中注意if的判断两个条件相等的符号
        if(address[i]=='1')
            n+=pow(2,len-1-i);
    }
    page=n/page_size;
    Deviation=n%page_size;
    if (Deviation < table.size()){
        index = table[page][0];  //查找物理块
        return index;
    }
    else
        return NONE;
}

void page_table::revise(int page_num, int frame_num,int valid){
    if(valid == 1){
        table[page_num][1] = Valid;
        table[page_num][0] = frame_num;
    }
    else{
        table[page_num][1] = invalid_value;
        table[page_num][0] = NONE;
    }
}

class quick_page_table
{
    public:
        map<int,int>quick_table;

        quick_page_table();
        ~quick_page_table();
        int Find_physical_block(string address);
};

quick_page_table::quick_page_table()
{
    for(int i=0;i<q_max_page_number;i++)
        quick_table[i]=NONE;
}

quick_page_table::~quick_page_table()
{
}

int quick_page_table::Find_physical_block(string address)
{
    int len=address.length();
    int n=0,page,Deviation,index;
    for(int i=0;i<len;++i)
    {//其中注意if的判断两个条件相等的符号
        if(address[i]=='1')
            n+=pow(2,len-1-i);
    }
    page=n/page_size;
    Deviation=n%page_size;
    if (Deviation < quick_table.size()){
        index = quick_table[page];  //查找物理块
        return index;
    }
    else
        return invalid_value;
}

class MemoryManager
{
    public:
        map<int,vector<int> >vm;//主键是页号，值是一个包含页面大小，页是否是空表，已分配次数的vector结构
        vector<int>schedule_queue;//用于页面调度算法
        map<unsigned int,page_table>page_tables; //map结构：放入进程对应的页表
        map<int,vector<int> >physical_m;
        string schedule;
        int cur_aid = 0;  // 记录分配次数
        int Total_vm = 0;
        int allocated = 0;  //分配的内存
        int Total_physical_m;
        int physical_size_used=0;
        int page_fault = 0;
        int page_access = 0;
        quick_page_table q_page_table;

        int* Resident_set;//驻留集
        int* Work_set;//工作集


        MemoryManager();
        ~MemoryManager();
        int page_allocation(unsigned int pid, unsigned int exe_size,unsigned int );
        bool page_free(unsigned int pid,unsigned int aid);
        void page_show();
        void fifo(unsigned pid,int page_num, page_table temp_table);
        void LRU(unsigned pid,int page_num, page_table temp_table);
        void monitor();
        void disk(string path);
        void page_embed(unsigned int pid,string address);
};

MemoryManager::MemoryManager()
{
    Total_physical_m = Physical_blocks * page_size;
    Total_vm = max_page_number * page_size;
    schedule = "FIFO";
    for(int i=0;i<max_page_number;i++)
    {
        vm[i].push_back(page_size);
        vm[i].push_back(-2);
        vm[i].push_back(0);
    }
    for(int i=0;i<Physical_blocks;i++)
    {
        physical_m[i].push_back(page_size);
        physical_m[i].push_back(-2);
    }
}

MemoryManager::~MemoryManager()
{
}

void MemoryManager::disk(string path)
{

}

int MemoryManager::page_allocation(unsigned int pid, unsigned int exe_size,unsigned int priority)
{
    unsigned int s = exe_size;
    page_table temp_table;
    int *aid = &cur_aid;
    cur_aid += 1;
    int flag=0;
    for(int j=0;j<max_page_number;j++)
    {
        flag=0;
        if(vm[j][1]==-2)
        {
            vm[j][1]=int(pid);
            for(map<unsigned int,page_table>::iterator i=page_tables.begin(); i!=page_tables.end(); i++)
            {
                if(i->first==pid)
                {
                    flag=1;
                    break;
                }
            }
            if(flag)
                temp_table = page_tables[pid];//暂存变量
            else
                page_tables[pid] = temp_table;
            temp_table.Insert(j);
            if (s >= page_size)
            {
                vm[j][0] = page_size;
                s -= page_size;
                vm[j][2] = *aid;
            }
            else
            {
                vm[j][0] = s;
                s = 0;
                vm[j][2] = *aid;
            }
        }
        if(s == 0)
        {
            allocated += exe_size;
            break;
        }
    }
    if (s > 0)
    {
        page_free(pid, *aid);
        return -1;
    }
    return *aid;
}

bool MemoryManager::page_free(unsigned int pid,unsigned int aid)
{
    int flag = 0;
    page_table temp_table;
    for(int i=0;i<max_page_number;i++)
    {
        if (vm[i][1] == pid && (vm[i][2] == aid || aid == 0))
        {
            flag = 1;
            if(physical_m[i][1]!=-2)//如果物理内存有占用也要释放
            {
                physical_m[i][1]=-2;
                physical_m[i][0]+=vm[i][0];
                for(vector<int>::iterator iter=schedule_queue.begin();iter!=schedule_queue.end();iter++)
                {        //从vector中删除指定的某一个元素
                    if(*iter==i)
                    {
                        schedule_queue.erase(iter);
                        break;
                    }
                }
            }
            temp_table = page_tables[pid];
            temp_table.Delete(i);
            allocated -= vm[i][0];
            vm[i][0] = page_size;
            vm[i][1] = NONE;
            vm[i][2] = 0;
        }
    }
    if (flag == 0)
    {
        printf("错误! 内存未找到.");
        return false;
    }
    return true;
}

void MemoryManager::page_show()
{
    printf("total: %dB allocated: %dB free: %dB\n",Total_vm, allocated,Total_vm - allocated);
    for(int i=0;i<max_page_number;i++)
    {
        if((vm[i][1]!=-2) && (vm[i][2]!=0))
        {
            printf("page #%d  %-4d/%-4d B  pid =%-3d  aid =%-3d\n",i, vm[i][0], page_size,vm[i][1],vm[i][2]);
        }
    }
}

void MemoryManager::monitor()
{
    printf("physical_rate: %.2f",float(physical_size_used) / float(Total_physical_m));
    printf("virtual_rate: %.2f",float(allocated) / float(Total_vm));
    printf("page_fault_rate : %.2f",float(page_fault) / float(page_access));
    cout<<endl;
}

void MemoryManager::page_embed(unsigned int pid,string address)
{
    page_table temp_table;
    page_access += 1;
    int n=0,offset;
    int virtual_pageID;
    int len=address.length();
    for(int i=0;i<len;++i)
    {
        if(address[i]=='1')
            n+=pow(2,len-1-i);
    }
    offset=n%page_size;
    temp_table = page_tables[pid];//得到进程所在的页
    int v_pageID;
    v_pageID = q_page_table.Find_physical_block(address);
    if(v_pageID == -1)
        printf("快表未查询到!");
    v_pageID = temp_table.Find_physical_block(address);
    if (v_pageID== -1 || vm[v_pageID][0] < offset)//没找到或者偏移地址有错误
    {
        printf("ERROR!!!");
        return;
    }
    if (schedule == "LRU")
        LRU(pid,v_pageID, temp_table);
    else if(schedule == "FIFO")
        fifo(pid,v_pageID, temp_table);
}

void MemoryManager::fifo(unsigned pid,int page_num, page_table temp_table)
{
    int flag=0,temp;
    if(temp_table.table[page_num][1] != 1)
    {
        flag=0;
        for(int i=0;i<Physical_blocks;i++)
        {
            if(physical_m[i][1] == -2)
            {
                flag=1;
                temp=i;
                break;
            }
        }
        if(flag)
        {
            physical_m[temp][1]=page_num;
            Total_physical_m -= vm[page_num][0];
            physical_m[temp][0] -= vm[page_num][0];
            schedule_queue.push_back(page_num);
            temp_table.revise(page_num,temp,1);  // 修改页表
            page_fault += 1;
        }
        else
        {
            int index;
            for(int i=0;i<Physical_blocks;i++)
            {
                if(physical_m[i][1]==schedule_queue[0])
                {
                    index=i;
                    break;
                }
            }
            //交换调度序列的第一个
            physical_m[index][1] = page_num;
            //改物理内存
            Total_physical_m += vm[schedule_queue[0]][0];
            physical_m[schedule_queue[0]][0] += vm[schedule_queue[0]][0];
            Total_physical_m -= vm[page_num][0];
            physical_m[page_num][0] -= vm[page_num][0];
            page_fault += 1;

            //改页表和队列
            page_table t1;
            t1 = page_tables[pid];
            t1.revise(schedule_queue[0], 0, -1);
            for(vector<int>::iterator iter=schedule_queue.begin();iter!=schedule_queue.end();iter++)
            {        //从vector中删除指定的某一个元素
                if(*iter==schedule_queue[0])
                {
                    schedule_queue.erase(iter);
                    break;
                }
            }
            schedule_queue.push_back(page_num);
            temp_table.revise(page_num, index, 1);
        }
    }
    //加入快表
    if(q_page_table.quick_table.size()==q_max_page_number)
    {
        map<int,int>::iterator iter=q_page_table.quick_table.begin();
        q_page_table.quick_table.erase(iter);
        q_page_table.quick_table[q_max_page_number-1]=page_num;
    }
    else if(q_page_table.quick_table.size()<q_max_page_number)
    {
        q_page_table.quick_table[q_page_table.quick_table.size()-1]=page_num;
    }
}

void MemoryManager::LRU(unsigned pid,int page_num, page_table temp_table)
{
    int flag=0;
    if(temp_table.table[page_num][1] == 1)  //页面在物理内存只改调度顺序
    {
        for(vector<int>::iterator iter=schedule_queue.begin();iter!=schedule_queue.end();iter++)
        {        //从vector中删除指定的某一个元素
            if(*iter==page_num)
            {
                schedule_queue.erase(iter);
                break;
            }
        }
        schedule_queue.push_back(page_num);//放在调度队列尾部（表示最近访问）
    }
    for(int i=0;i<Physical_blocks;i++)
    {
        if(physical_m[i][1] == -2)
        {
           physical_m[i][1] = page_num;
           Total_physical_m -= vm[page_num][0];
           physical_m[i][0] -= vm[page_num][0];
           schedule_queue.push_back(page_num);
           temp_table.revise(page_num,i,1); // 修改页表
           page_fault += 1;
           flag=1;
           break;
        }
    }
    if(!flag)
    {
            int index;
            for(int i=0;i<Physical_blocks;i++)
            {
                if(physical_m[i][1]==schedule_queue[0])
                {
                    index=i;
                    break;
                }
            }
            //交换调度序列的第一个
            physical_m[index][1] = page_num;
            //改物理内存
            Total_physical_m += vm[schedule_queue[0]][0];
            physical_m[schedule_queue[0]][0] += vm[schedule_queue[0]][0];
            Total_physical_m -= vm[page_num][0];
            physical_m[page_num][0] -= vm[page_num][0];
            page_fault += 1;

            //改页表和队列
            page_table t1;
            t1 = page_tables[pid];
            t1.revise(schedule_queue[0], 0, -1);
            for(vector<int>::iterator iter=schedule_queue.begin();iter!=schedule_queue.end();iter++)
            {        //从vector中删除指定的某一个元素
                if(*iter==schedule_queue[0])
                {
                    schedule_queue.erase(iter);
                    break;
                }
            }
            schedule_queue.push_back(page_num);
            temp_table.revise(page_num, index, 1);
    }
    //加入快表
    if(q_page_table.quick_table.size()==q_max_page_number)
    {
        map<int,int>::iterator iter=q_page_table.quick_table.begin();
        q_page_table.quick_table.erase(iter);
        q_page_table.quick_table[q_max_page_number-1]=page_num;
    }
    else if(q_page_table.quick_table.size()<q_max_page_number)
    {
        q_page_table.quick_table[q_page_table.quick_table.size()-1]=page_num;
    }
}

//void MemoryManager::LRU(unsigned pid,int page_num, page_table temp_table)
//{
//}

int main(void)
{
    MemoryManager tt;
    tt.page_allocation(0,200,0);
    tt.page_show();
    tt.monitor();
    tt.page_allocation(1,4200,1);
    tt.page_show();
    tt.page_free(0,1);
    tt.page_show();
    return 0;
}
