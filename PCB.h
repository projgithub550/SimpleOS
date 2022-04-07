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
		
		//���̵�����Ϣ
		Status status;
		int priority;
		Event event; 
		
		//���̿�����Ϣ
		int textStart,textEnd;
		int dataStart,dataEnd;
		int base,top;
		
		//CPU������Ϣ
		int ax,bx,cx,dx;
		int PC;
		
		//�ļ�ϵͳ��Ϣ
		string workDir;
		map<int,int> fileTable; 
	
	//getter��setter	
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
