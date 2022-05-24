#include <algorithm>//STL通用算法
#include <mem.h>
#include <iostream>
#include <map> 
#include <vector>
#include <sstream>
#include <cmath>
#include "memory.h"

MemoryManager::MemoryManager()
{
    Total_physical_m = max_page_number * page_size;
    schedule = "LRU";
    for(int i=0;i<max_page_number;i++)
    {
        p_memory[i]=1; //最初全部有效
    }
    this->clearMemory();
}

int MemoryManager::getTotalMem()
{
    return this->Total_physical_m;
}

int MemoryManager::getAllocatedMem()
{
    return this->physical_size_used;
}

float MemoryManager::getMemRate()
{
    return (float(physical_size_used) / float(Total_physical_m));
}

float MemoryManager::getPageFaultRate()
{
    if(page_embed == 0)
    {
        return 0;
    }
    return (float(page_fault) / float(page_embed));
}

bool MemoryManager::freePage(int pid)
{
    int flag = 0;
    Page_table temp_table;
    for(map<int,Page_table>::iterator i=page_tables.begin(); i!=page_tables.end(); i++)
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
        FILE* fp;
        fp = fopen("memory.bin", "rb");
        for(map<int,vector<int> >::iterator i=page_tables[pid].table.begin(); i!=page_tables[pid].table.end(); i++)
	    {
            if(i->second[2]==1)
            {  
                rewind(fp);//把当前的读写位置回到内存的最开始开始
	            fseek(fp,i->second[0]*page_size, SEEK_SET);//指针应该移动到对应物理块加偏移量的位置
                fwrite("0", page_size, 1, fp);//将空字符串内容写入到内存
                physical_size_used-=page_size;

            }   
	    }
        fclose(fp);
        //删除页表
        page_tables.erase(pid);
    }
    else
    {
        printf("错误! 页表未找到.");
        return false;
    } 
    return true;
}

void MemoryManager::showPage(int pid)
{
    int Occupied_memory = 0;
    printf("total: %dB allocated: %dB free: %dB aid:%d\n",Total_physical_m, physical_size_used,Total_physical_m - physical_size_used,cur_aid);
    Page_table temp_table;
    int flag=0;
    for(map<int,Page_table>::iterator i=page_tables.begin(); i!=page_tables.end(); i++)
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
        for(int i=0;i<max_page_number;i++)
        {
            if(temp_table.table[i][2]==1)
            {
                //printf("Virtual_page #%d  Phisical_page #%d disk_number #%d\n",i, temp_table.table[i][0], temp_table.table[i][1]);
                char buff[page_size];
                readMemPage(temp_table.table[i][0],buff);
                printf("当前物理页中内容的占用内存:%d\n",sizeof(buff));
                Occupied_memory+=sizeof(buff);
            } 
        }       
        printf("进程一共占用内存:%d\n",Occupied_memory);
    }
    else
    {
        printf("error!");
        return;
    }
}     

int MemoryManager::getFirstFree()
{
    for(int i  = 0; i < max_page_number; i ++)
    {
        if(this->p_memory[i] == 1)
        {
            return i;
        }
    }
    return invalid_value;
}
void MemoryManager::Monitor()
{
    printf("memory_rate: %.2f",float(physical_size_used) / float(Total_physical_m));
    printf("page_fault_rate : %.2f",float(page_fault) / float(page_embed));
    cout<<endl; 
}

void MemoryManager::createPageTable(int pid,string filename)
{
    Page_table temp_table;
    temp_table.initializePage(filename);
    page_tables[pid]=temp_table;

}

void MemoryManager::FIFO(int pid,int& wPage,int& wBlock)//进程号，物理页号,磁盘块号
{
    int virtualpagenumber;
    int isfull=0,wPage_flag=0;
    page_embed+=1;
    for(map<int,vector<int> >::iterator i=this->page_tables[pid].table.begin();i!=this->page_tables[pid].table.end();i++)
    {
        if(i->second[2] == 0)//如果此页处于空闲状态
        {
            isfull=1;
            virtualpagenumber=i->first;
            for(int j=occupancy;j<max_page_number;j++)//寻找空闲的物理页
            {
                if(this->p_memory[j] == 1)//寻找到空闲的物理页
                {
                    wPage_flag=1;
                    wPage=j;
                    break;
                } 
            }
            if(wPage_flag)
            {
                wBlock=-1;
                p_memory[wPage]=0;//将物理页标志位翻转
                this->page_tables[pid].schedule_queue.push_back(wPage);//在调度队列末尾加入访问页号
                this->page_tables[pid].revisePage(i->first,wPage,1,i->second[1]);  // 修改页表
                page_fault += 1;//页错误加一
            }
            else//如果没有空闲的物理页，寻找一页换出
            {
                int index,index_b,p_page;
                for(map<int,vector<int> >::iterator k=this->page_tables[pid].table.begin();k!=this->page_tables[pid].table.end();k++)
                {
                    if((k->second)[0] == this->page_tables[pid].schedule_queue[0])//找到调度队列第一个物理块对应的页表项
                    {
                        index=k->first;//记录要调出的页号
                        wPage=(k->second)[0];//记录物理页号
                        wBlock=(k->second)[1];//记录对应的磁盘块号
                        break;
                    }
                }
                //页错误加一
                page_fault += 1; 

                //改页表和队列
                this->page_tables[pid].revisePage(i->first,wPage,1,i->second[1]);//将该页修改成为调入内存
                this->page_tables[pid].revisePage(index,0,0,0);//将被调换的页调出
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
    }
    if(!isfull)
    {
        int index,index_b,p_page;
        for(map<int,vector<int> >::iterator k=this->page_tables[pid].table.begin();k!=this->page_tables[pid].table.end();k++)
        {
            if((k->second)[0] == this->page_tables[pid].schedule_queue[0])//找到调度队列第一个物理块对应的页表项
            {
                virtualpagenumber=k->first;//记录要调出的页号
                wPage=(k->second)[0];//记录物理块号
                wBlock=(k->second)[1];//记录对应的磁盘块号
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
        q_page_table.quick_table[it->first+1][0]=wPage;
        q_page_table.quick_table[it->first+1][1]=virtualpagenumber;
        q_page_table.quick_table[it->first+1][2]=pid;
    }
    else if(q_page_table.count<q_max_page_number)
    {
        q_page_table.quick_table[q_page_table.count][0]=wPage;
        q_page_table.quick_table[q_page_table.count][1]=virtualpagenumber;
        q_page_table.quick_table[q_page_table.count][2]=pid;
        q_page_table.count++;
    }
}

void MemoryManager::LRU(int pid,int addr,int& wPage,int& wBlock)//进程号，地址，物理页号,磁盘块号
{

    Page_table temp_table=page_tables[pid];
    int page_number=addr/page_size;//找到对应虚拟页号

    //判断有效页表项是否达到occupancy
    //如果没有达到
    if(temp_table.nValid < occupancy)
    {
       int fPage = this->getFirstFree();
       if(fPage != invalid_value)//如果有空闲页,不需要做替换
       {
          //分配一页，把页号赋给wPage,
          wPage = fPage;
          wBlock = -1;
          this->p_memory[wPage] = 0;
          physical_size_used+=page_size;

          //qDebug() << "修改页表";
          //wPage存入页表中VP对应的物理页号中，VP有效位置1，将VP加入调度队列
          temp_table.revisePage(page_number,wPage,1,temp_table.table[page_number][1]);

          //在调度队列末尾加入访问页号
          temp_table.schedule_queue.push_back(page_number);

          page_tables[pid] = temp_table;

          // 修改快表
          q_page_table.insertQuickTable(wPage,page_number,temp_table.table[page_number][1],pid);

         return;
       }
     }

    //如果达到，就选出最近最长时间没有用到的虚拟页号VP1,即栈底元素,删除
    int rPageNum = temp_table.schedule_queue[0];
    temp_table.schedule_queue.erase(temp_table.schedule_queue.begin());

   // 拿到其物理页号和磁盘块号，赋给wPage和wBlock,
    wPage = temp_table.table[rPageNum][0];
    wBlock = temp_table.table[rPageNum][1];

   //VP1的物理页号置空，有效位置0。
    temp_table.revisePage(rPageNum,-3,0,temp_table.table[rPageNum][1]);

   //将wPage存入VP对应的物理页号中，VP有效位置1，将VP加入调度队列
    temp_table.revisePage(rPageNum,wPage,1,temp_table.table[page_number][1]);
    temp_table.schedule_queue.push_back(page_number);

    page_tables[pid] = temp_table;
    q_page_table.insertQuickTable(wPage,page_number,temp_table.table[page_number][1],pid);//加入快表

    return;
}

int MemoryManager::readMem(int pid,int startAddr,int size,void* buff)
{
    page_embed += 1;
    int start_block,offset,page_number;
    page_number = startAddr/page_size;
    start_block=this->page_tables[pid].findPageNumber(startAddr);//查找对应进程的页表找到对应地址的起始块
    offset=this->page_tables[pid].findOffset(startAddr);
    if(start_block == -1)
    {
      //  qDebug() << "缺页";
        page_fault += 1;
        return invalid_value;
    }

    //说明页表项存在，更新调度队列
    this->updateScheQue(pid,page_number);

    FILE* fp;
	fp = fopen("memory.bin", "rb");
    rewind(fp);//把当前的读写位置回到内存的最开始开始

  //  qDebug() << "start_block:" << start_block;
   // qDebug() << "offset:" << offset;
    fseek(fp,start_block*page_size+offset, SEEK_SET);//指针应该移动到对应物理块加偏移量的位置
    // qDebug() << "sssssssssssssssssssssssssss";
	if(fread(buff,size, 1, fp))//将物理内存的内容读出到缓冲区
    {
        //char *b = (char*)buff;
      //  qDebug() << "0是"<< ((char*)buff)[0] << "1是" << ((char*)buff)[1] <<"2是" <<((char*)buff)[2];
      //  qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaa";
        fclose(fp);
        return Valid;
    }
    else
    {
        qDebug()<<"read memory error";
        fclose(fp);
        return invalid_value;
    }
        
}

int MemoryManager::writeMem(int pid,int startAddr,int size,void* buff)
{
     page_embed += 1;
    int start_block,offest;
    int page_number = startAddr/page_size;
    start_block=this->page_tables[pid].findPageNumber(startAddr);//查找对应进程的页表找到对应地址的起始块
    offest=this->page_tables[pid].findOffset(startAddr);

    if(start_block == -1)
    {
        page_fault += 1;
        return invalid_value;
    }

    //说明页表项存在，更新调度队列
    this->updateScheQue(pid,page_number);

    FILE* fp;
    fp = fopen("memory.bin", "wb");
    rewind(fp);//把当前的读写位置回到内存的最开始开始
    fseek(fp,start_block*page_size+offest, SEEK_SET);//指针应该移动到对应物理块加偏移量的位置
    if(fwrite(buff, size, 1, fp))//将内容写入到内存
    {
        fclose(fp);
        return Valid;
    }
    else
    {
        cout<<"write memory error"<<endl;
        return invalid_value;
    } 
}

void MemoryManager::updateScheQue(int pid,int pageNum)
{
    vector<int> que = this->page_tables[pid].schedule_queue;

    vector<int>::iterator dt =  find(que.begin(),que.end(),pageNum);
    que.erase(dt);
    que.push_back(pageNum);
    this->page_tables[pid].schedule_queue = que;
    return;
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
    {
        cout<<"read memory page error"<<endl;
        return invalid_value;
    }
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
    {
        cout<<"write memory page error"<<endl;
        return invalid_value;
    }
}

int MemoryManager::getProcPhyMem(int pid)
{
    int Occupied_Phisical_memory = 0;
    Page_table temp_table;
    int flag=0;
    for(map<int,Page_table>::iterator i=page_tables.begin(); i!=page_tables.end(); i++)
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
            {
                char buff[page_size];
                readMemPage(temp_table.table[i][0],buff);
                Occupied_Phisical_memory+=sizeof(buff);//累加每一页的物理内存
                memset(buff,'0',page_size);
            } 
        }       
        return Occupied_Phisical_memory;
    }
    else
    {
        printf("error!");
        return invalid_value;
    }
}

int MemoryManager::getProcVirMem(int pid)
{
    int pm = this->getProcPhyMem(pid);
    return max_page_number*page_size - pm;
}



void MemoryManager::clearMemory()
{
    fopen("memory.bin","w");//清空文件
	return ;
}

