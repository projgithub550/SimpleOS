#include <algorithm>//STL通用算法
#include <iostream>
#include <map> 
#include <vector>
#include <sstream>
#include <cmath>
#include "memory.h"

MemoryManager::MemoryManager()
{
    Total_physical_m = (max_page_number-occupancy) * page_size;
    schedule = "FIFO";
    for(int i=occupancy;i<max_page_number;i++)
    {
        Physical_memory[i]=0;
    }
}

bool MemoryManager::page_free(int pid)
{
    int flag = 0;
    page_table temp_table;
    for(map<int,page_table>::iterator i=page_tables.begin(); i!=page_tables.end(); i++)
	{
		if(i->first==pid) 
        {
            flag=1;//表示进程所在页表在内存中
            temp_table=page_tables[pid];
            break;
        }
	}
    if(flag)
    {
        for(map<int,vector<int> >::iterator i=page_tables[pid].table.begin(); i!=page_tables[pid].table.end(); i++)
	    {
            if(i->second[2]==1)
            {
                i->second[2]=0;//回收内存
                FILE* fp;
	            fp = fopen("memory.bin", "rb");
                rewind(fp);//把当前的读写位置回到内存的最开始开始
	            fseek(fp,i->second[0]*page_size, SEEK_SET);//指针应该移动到对应物理块加偏移量的位置
	            fwrite("\0", page_size, 1, fp);//将空字符串内容写入到内存
                physical_size_used-=page_size;
                fclose(fp);
                this->Physical_memory[i->second[0]]=0;//同时对应的物理块标志位反转
                i->second[0]=-3;
            }   
	    }
    }
    else
    {
        printf("错误! 页表未找到.");
        return false;
    } 
    return true;
}

void MemoryManager::page_show(int pid)
{
    printf("total: %dB allocated: %dB free: %dB aid:%d\n",Total_physical_m, physical_size_used,Total_physical_m - physical_size_used,cur_aid);
    page_table temp_table;
    int flag=0;
    for(map<int,page_table>::iterator i=page_tables.begin(); i!=page_tables.end(); i++)
	{
		if(i->first==pid) 
        {
            flag=1;//表示进程所在页表在内存中
            temp_table=i->second;
            break;
        }
	}
    if(flag)
    {
        for(int i=0;i<occupancy;i++)
        {
            if(temp_table.table[i][2]==1)
                printf("Virtual_page #%d  Phisical_page #%d disk_number #%d\n",i, temp_table.table[i][0], temp_table.table[i][1]);
        }       
    }
    else
    {
        printf("error!");
        return;
    }
}     

void MemoryManager::monitor()
{
    printf("memory_rate: %.2f",float(physical_size_used) / float(Total_physical_m));
    printf("page_fault_rate : %.2f",float(page_fault) / float(page_embed));
    cout<<endl; 
}

int MemoryManager::Is_Enter_interrupt(int pid,int address)
{
    int flag = 0;
    int offset;
    int Phisical_pageID;
    page_table temp_table;
    Phisical_pageID = q_page_table.findPagenumber(pid,address);
    if(Phisical_pageID == -1)
        printf("Fast table not found!\n");
    else 
    {
        printf("The page does not exist！\n");
        return Valid;
    }
    for(map<int,page_table>::iterator i=page_tables.begin(); i!=page_tables.end(); i++)
	{
		if(i->first==pid) 
        {
            flag=1;//表示进程所在页表在内存中
            temp_table=page_tables[pid];
            break;
        }
	}
    if(flag)
    {
        page_embed += 1;
        offset=temp_table.Find_offest(pid,address);
        Phisical_pageID = temp_table.findPagenumber(pid,address);
        if (Phisical_pageID == -1)//没找到
        {
            printf("Page table not found!!!\n");
            page_embed -= 1;
            return invalid_value;
        }  
        else
        {
            printf("Page table found!!\n");
            return Valid;
        }
    }
    else
    {
        page_tables[pid]=temp_table;
        cout<<"create page table"<<endl;
    }   
    return 1;
}

void MemoryManager::fifo(int pid,int wPage,int wBlock)//进程号，访问页号,磁盘块号
{
    int flag=0,temp;
    if(this->page_tables[pid].table[wPage][0] == -3)//如果该进程对应的页表中的命中页没有调入内存
    {
        for(int i=occupancy;i<max_page_number;i++)//寻找空闲的物理块
        {
            if(this->Physical_memory[i] == 0)//寻找到空闲的物理块
            {
                flag=1;
                temp=i;
                break;
            } 
        }
        if(flag)
        {
            Physical_memory[temp]=1;//将物理块标志位翻转
            this->page_tables[pid].schedule_queue.push_back(wPage);//在调度队列末尾加入访问页号
            this->page_tables[pid].revise(wPage,temp,1,wBlock);  // 修改页表
            page_fault += 1;//页错误加一
        }
        else//如果没有空闲的物理块，寻找一页换出
        {
            int index,index_b,p_page;
            for(map<int,vector<int> >::iterator i=this->page_tables[pid].table.begin();i!=this->page_tables[pid].table.end();i++)
            {
                if((i->second)[0] == this->page_tables[pid].schedule_queue[0])//找到调度队列第一个物理块对应的页表项
                {
                    index=i->first;//记录要调出的页号
                    p_page=(i->second)[0];//记录物理块号
                    index_b=(i->second)[1];//记录对应的磁盘块号
                    break;
                }
            }
            //页错误加一
            page_fault += 1; 

            //改页表和队列
            this->page_tables[pid].revise(wPage,p_page,1,wBlock);//将该页修改成为调入内存
            this->page_tables[pid].revise(index,0,0,0);//将被调换的页调出
            for(vector<int>::iterator iter=this->page_tables[pid].schedule_queue.begin();iter!=this->page_tables[pid].schedule_queue.end();iter++)
            {        //从vector中删除指定的某一个元素 
                if(*iter==this->page_tables[pid].schedule_queue[0])
                {
                    this->page_tables[pid].schedule_queue.erase(iter);
                    this->page_tables[pid].schedule_queue.push_back(wPage);
                    cout<<"替换成功"<<endl;
                    break;
                }
            }
        }
    }      
    //加入快表
    if(q_page_table.count==q_max_page_number)
    {
        map<int,vector<int> >::iterator iter=q_page_table.quick_table.begin();
        q_page_table.quick_table.erase(iter);//把快表的第一项删除
        map<int,vector<int> >::iterator it=q_page_table.quick_table.end();
        q_page_table.quick_table[it->first+1][0]=temp;
        q_page_table.quick_table[it->first+1][1]=wPage;
        q_page_table.quick_table[it->first+1][2]=pid;
    }
    else if(q_page_table.count<q_max_page_number)
    {
        q_page_table.quick_table[q_page_table.count][0]=temp;
        q_page_table.quick_table[q_page_table.count][1]=wPage;
        q_page_table.quick_table[q_page_table.count][2]=pid;
        q_page_table.count++;
    }
}

void MemoryManager::LRU(int pid,int wPage,int wBlock)
{
    int flag=0,temp;
    if(this->page_tables[pid].table[wPage][0] != -3)  //页面在物理内存只改调度顺序    
    {
        for(vector<int>::iterator iter=this->page_tables[pid].schedule_queue.begin();iter!=this->page_tables[pid].schedule_queue.end();iter++)
        {        //从vector中删除指定的某一个元素 
            if(*iter==wPage)
            {
                this->page_tables[pid].schedule_queue.erase(iter);
                break;
            }
        }
        this->page_tables[pid].schedule_queue.push_back(wPage);//放在调度队列尾部（表示最近访问）
    }             
    for(int i=occupancy;i<max_page_number;i++)//寻找空闲的物理块
    {
        if(this->Physical_memory[i] == 0)//寻找到空闲的物理块
        {
            flag=1;
            temp=i;
            break;
        } 
    }
    if(flag)
    {
        Physical_memory[temp]=1;//将物理块标志位翻转
        this->page_tables[pid].schedule_queue.push_back(wPage);//在调度队列末尾加入访问页号
        this->page_tables[pid].revise(wPage,temp,1,wBlock);  // 修改页表
        page_fault += 1;//页错误加一
    }
    else//如果没有空闲的物理块，寻找一页换出
    {
        int index,index_b,p_page;
        for(map<int,vector<int> >::iterator i=this->page_tables[pid].table.begin();i!=this->page_tables[pid].table.end();i++)
        {
            if((i->second)[0] == this->page_tables[pid].schedule_queue[0])//找到调度队列第一个物理块对应的页表项
            {
                index=i->first;//记录要调出的页号
                p_page=(i->second)[0];//记录分配的物理块号
                index_b=(i->second)[1];//记录对应的磁盘块号
                break;
            }
        }
        //页错误加一
        page_fault += 1; 

        //改页表和队列
        this->page_tables[pid].revise(wPage,p_page,1,wBlock);//将该页修改成为调入内存
        this->page_tables[pid].revise(index,0,0,0);//将被调换的页调出
        for(vector<int>::iterator iter=this->page_tables[pid].schedule_queue.begin();iter!=this->page_tables[pid].schedule_queue.end();iter++)
        {        //从vector中删除指定的某一个元素 
            if(*iter==this->page_tables[pid].schedule_queue[0])
            {
                this->page_tables[pid].schedule_queue.erase(iter);
                this->page_tables[pid].schedule_queue.push_back(wPage);
                cout<<"替换成功"<<endl;
                break;
            }
        }
    }
    //加入快表
    if(q_page_table.count==q_max_page_number)
    {
        map<int,vector<int> >::iterator iter=q_page_table.quick_table.begin();
        q_page_table.quick_table.erase(iter);//把快表的第一项删除
        map<int,vector<int> >::iterator it=q_page_table.quick_table.end();
        q_page_table.quick_table[it->first+1][0]=temp;
        q_page_table.quick_table[it->first+1][1]=wPage;
        q_page_table.quick_table[it->first+1][2]=pid;
    }
    else if(q_page_table.count<q_max_page_number)
    {
        q_page_table.quick_table[q_page_table.count][0]=temp;
        q_page_table.quick_table[q_page_table.count][1]=wPage;
        q_page_table.quick_table[q_page_table.count][2]=pid;
        q_page_table.count++;
    }
}

int MemoryManager::readMem(int pid,int startAddr,int size,void* buff)
{
    int start_block,offest;
    start_block=this->page_tables[pid].findPagenumber(pid,startAddr);//查找对应进程的页表找到对应地址的起始块
    offest=this->page_tables[pid].Find_offest(pid,startAddr);
    FILE* fp;
	fp = fopen("memory.bin", "rb");
    rewind(fp);//把当前的读写位置回到内存的最开始开始
	fseek(fp,start_block*page_size+offest, SEEK_SET);//指针应该移动到对应物理块加偏移量的位置
	if(fread(buff, page_size, 1, fp))//将物理内存的内容读出到缓冲区
    {
        fclose(fp);
        return Valid;
    }
    else
        return invalid_value;
}

int MemoryManager::writeMem(int pid,int startAddr,int size,void* buff)
{
    int start_block,offest;
    physical_size_used+=page_size;
    cur_aid+=1;
    start_block=this->page_tables[pid].findPagenumber(pid,startAddr);//查找对应进程的页表找到对应地址的起始块
    offest=this->page_tables[pid].Find_offest(pid,startAddr);
    FILE* fp;
	fp = fopen("memory.bin", "wb");
    rewind(fp);//把当前的读写位置回到内存的最开始开始
	fseek(fp,start_block*page_size+offest, SEEK_SET);//指针应该移动到对应物理块加偏移量的位置
	if(fwrite(buff, page_size, 1, fp))//将内容写入到内存
    {
        fclose(fp);
        return Valid;
    }
    else
        return invalid_value;
}

int MemoryManager::readMemPage(int wPage,char *buff)
{
    FILE* fp;
	fp = fopen("memory.bin", "rb");
    rewind(fp);//把当前的读写位置的最开始
	fseek(fp,wPage*page_size, SEEK_SET);//指针应该移动到对应物理块加偏移量的位置
	if(fread(buff, page_size, 1, fp))//将物理内存的内容读出到缓冲区
    {
        fclose(fp);
        return Valid;
    }
    else
        return invalid_value;
}

int MemoryManager::writeMemPage(int wPage,char *buff)
{
    FILE* fp;
    physical_size_used+=page_size;
    cur_aid+=1;
	fp = fopen("memory.bin", "wb");
    rewind(fp);//把当前的读写位置回到内存的最开始开始
	fseek(fp,wPage*page_size, SEEK_SET);//指针应该移动到对应物理块加偏移量的位置
	if(fwrite(buff, page_size, 1, fp))//将内容写入到内存
    {
        fclose(fp);
        return Valid;
    }
    else
        return invalid_value;
}

