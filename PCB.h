#pragma once

#include<string>
#include<map>

using namespace std;

enum Status
{
	ready,
	running,
	blocked,
	dead
};

enum Event
{
	std_io,
	dist_io
};

class PCB
{
	private:
		int pId;
		
		//进程调度信息
		Status status;
		int priority;
		Event event; 
		
		//进程控制信息
		int textStart,textEnd;
		int dataStart,dataEnd;
		int base,top;
		
		//CPU控制信息
		int ax,bx,cx,dx;
		int PC;
		
		//文件系统信息
		string workDir;
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
		
		Event getEvent()
		{
			return this->event;
		}
		void setEvent(Event _event)
		{
			this->event = _event;
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
		
}; 
