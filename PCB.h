<<<<<<< HEAD
#pragma once
=======
﻿#pragma once
>>>>>>> b596dffb48aa89f33ce708c040d68e97361c6456

#include<string>
#include<map>

using namespace std;

enum Status
{
<<<<<<< HEAD
	ready,
	running,
	blocked,
	dead
};

enum Event
{
	std_io,
	dist_io
=======
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
>>>>>>> b596dffb48aa89f33ce708c040d68e97361c6456
};

class PCB
{
	private:
		int pId;
		
<<<<<<< HEAD
		//���̵�����Ϣ
		Status status;
		int priority;
		Event event; 
		
		//���̿�����Ϣ
=======
		//进程调度信息
		Status status;
		int priority;
        IOtype ioType;

        //IO信息
        Operation ope;      //文件操作:读/写
        int activeFile;     //当前正在操作的文件
		
		//进程控制信息
>>>>>>> b596dffb48aa89f33ce708c040d68e97361c6456
		int textStart,textEnd;
		int dataStart,dataEnd;
		int base,top;
		
<<<<<<< HEAD
		//CPU������Ϣ
		int ax,bx,cx,dx;
		int PC;
		
		//�ļ�ϵͳ��Ϣ
		string workDir;
		map<int,int> fileTable; 
	
	//getter��setter	
=======
		//CPU控制信息
		int ax,bx,cx,dx;
		int PC;
		
		//文件系统信息
		string workDir;

        // key是文件描述符，value是文件在系统文件表中的地址
		map<int,int> fileTable; 
	
	//getter和setter	
>>>>>>> b596dffb48aa89f33ce708c040d68e97361c6456
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
		
<<<<<<< HEAD
		Event getEvent()
		{
			return this->event;
		}
		void setEvent(Event _event)
		{
			this->event = _event;
=======
        IOtype getIOtype()
		{
            return this->ioType;
		}
        void setIOtype(IOtype type)
		{
            this->ioType = type;
>>>>>>> b596dffb48aa89f33ce708c040d68e97361c6456
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
<<<<<<< HEAD
		
=======

        int getActiveFile(){return activeFile;}
        void setActiveFile(int file){activeFile = file;}

        Operation getOperation(){return ope;}
        void setOperation(Operation o){ope=o;}
>>>>>>> b596dffb48aa89f33ce708c040d68e97361c6456
}; 
