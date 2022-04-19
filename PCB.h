#pragma once

#include<string>
#include<map>

using namespace std;

enum Status
{
    st_ready,
    st_running,
    st_blocked,
    st_dead
};

enum IOtype
{
    type_stdio,
    type_diskio
};

enum Operation {
    op_read,
    op_write
};

class PCB
{
	private:
		int pId;
		
		//进程调度信息
		Status status;
		int priority;
        IOtype ioType;

        //IO信息
        Operation ope;      //文件操作:读/写
        int activeFile;     //当前正在操作的文件
		
		//进程控制信息
		int textStart,textEnd;
		int dataStart,dataEnd;
		int base,top;
		
		//CPU控制信息
		int ax,bx,cx,dx;
		int PC;
		
		//文件系统信息
		string workDir;

        // key是文件描述符，value是文件在系统文件表中的地址
		map<int,int> fileTable; 
	
	//getter和setter	
	public:
		int getPId()
		{
			return this->pId;
		} 
		void setPId(int id)
		{
			this->pId = id;
		}
		
		Status getStatus()
		{
			return this->status;
		}
		void setStatus(Status new_status)
		{
			this->status = new_status;
		}
		
		int getPriority()
		{
			return this->priority;
		}
		void setPriority(int _priority)
		{
			this->priority = _priority;
		}
		
        IOtype getIOtype()
		{
            return this->ioType;
		}
        void setIOtype(IOtype type)
		{
            this->ioType = type;
		}
		
		int getTextStart()
		{
			return this->textStart;
		}
		void setTextStart(int start)
		{
			this->textStart = start;
		}
		
		int getTextEnd()
		{
			return this->textEnd;
		}
		void setTextEnd(int end)
		{
			this->textEnd = end;
		}
		
		int getDataStart()
		{
			return this->dataStart;
		}
		void setDataStart(int start)
		{
			this->dataStart = start;
		}
		
		int getDataEnd()
		{
			return this->dataEnd;
		}
		void setDataEnd(int end)
		{
			this->dataEnd = end;
		}

        int getActiveFile(){return activeFile;}
        void setActiveFile(int file){activeFile = file;}

        Operation getOperation(){return ope;}
        void setOperation(Operation o){ope=o;}
}; 
